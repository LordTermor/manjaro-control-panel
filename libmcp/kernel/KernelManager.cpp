/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "KernelManager.hpp"

#include <pamac/database.hpp>

namespace mcp::kernel {

KernelManager::KernelManager(pamac::Database& database)
    : m_provider()
    , database_(database)
    , transaction_(database)
{
}

void KernelManager::reset_transaction()
{
    // TODO: Add reset method to pamac::Transaction or create new manager instance
}

void KernelManager::on_transaction_success()
{
    // Refresh file database to update package information after installation/removal
    database_.refresh_tmp_files_dbs();
}

bool KernelManager::has_pending_updates(pamac::Database& db) const
{
    auto updates = db.get_updates();
    return updates.has_updates();
}

KernelManagerResult<void> 
KernelManager::add_to_install(const std::string& package_name, bool with_headers)
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        return std::unexpected(KernelManagerError::InvalidOperation);
    }
    
    auto& db = db_result.value().get();
    
    // Check for pending updates before installation
    if (has_pending_updates(db)) {
        return std::unexpected(KernelManagerError::UpdatesPending);
    }
    
    // Verify kernel package exists
    auto kernel_pkg = db.get_sync_pkg(package_name);
    if (!kernel_pkg) {
        return std::unexpected(KernelManagerError::KernelNotFound);
    }
    
    // Add kernel package to install
    transaction_.add_pkg_to_install(package_name);
    
    // Add headers package if requested and available
    if (with_headers) {
        std::string headers_name = get_headers_package(package_name);
        auto headers_pkg = db.get_sync_pkg(headers_name);
        if (headers_pkg) {
            transaction_.add_pkg_to_install(headers_name);
        }
    }
    
    return {};
}

KernelManagerResult<void> 
KernelManager::add_to_remove(const std::string& package_name, bool with_headers, bool force)
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        return std::unexpected(KernelManagerError::InvalidOperation);
    }
    
    auto& db = db_result.value().get();
    
    // Verify kernel package is installed
    auto kernel_pkg = db.get_installed_pkg(package_name);
    if (!kernel_pkg) {
        return std::unexpected(KernelManagerError::KernelNotFound);
    }
    
    // Check if kernel is in use (unless forced)
    if (!force && !verify_safe_to_remove(package_name)) {
        return std::unexpected(KernelManagerError::KernelInUse);
    }
    
    // Add kernel package to remove
    transaction_.add_pkg_to_remove(package_name);
    
    // Add headers package if requested and installed
    if (with_headers) {
        std::string headers_name = get_headers_package(package_name);
        auto headers_pkg = db.get_installed_pkg(headers_name);
        if (headers_pkg) {
            transaction_.add_pkg_to_remove(headers_name);
        }
    }
    
    return {};
}

void KernelManager::add_to_upgrade(bool force_refresh)
{
    transaction_.add_pkgs_to_upgrade(force_refresh);
}

std::string KernelManager::get_headers_package(const std::string& package_name)
{
    return package_name + "-headers";
}

bool KernelManager::verify_safe_to_remove(const std::string& package_name) const
{
    auto kernel_result = m_provider.get_kernel(package_name);
    
    if (!kernel_result) {
        return false;
    }
    
    const auto& kernel = kernel_result.value();
    return !kernel.is_in_use();
}

std::vector<std::string> 
KernelManager::build_agent_command(
    const std::string& operation,
    const std::vector<std::string>& packages,
    bool force,
    bool refresh)
{
    std::vector<std::string> cmd = {"mcp-transaction-agent", operation};
    
    // Add flags
    if (force && operation == "remove") {
        cmd.push_back("--force");
    }
    if (refresh && operation == "upgrade") {
        cmd.push_back("--refresh");
    }
    
    // Add packages
    for (const auto& pkg : packages) {
        cmd.push_back(pkg);
    }
    
    return cmd;
}

} // namespace mcp::kernel
