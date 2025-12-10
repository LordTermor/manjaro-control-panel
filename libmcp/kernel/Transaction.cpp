/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Kernel transaction builders - validates operations and produces agent commands.
 */

#include "Transaction.hpp"
#include "KernelProvider.hpp"

#include <pamac/database.hpp>

namespace mcp::kernel {

namespace {

Task<bool> has_pending_updates()
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        co_return false;
    }
    
    auto& db = db_result.value().get();
    auto updates = co_await db.get_updates_async();
    co_return updates.has_updates();
}

Task<bool> is_safe_to_remove(const std::string& package_name)
{
    KernelProvider provider;
    auto kernel_result = co_await provider.get_kernel(package_name);
    
    if (!kernel_result) {
        co_return false;
    }
    
    co_return !kernel_result->is_in_use();
}

std::string get_headers_package(const std::string& package_name)
{
    return package_name + "-headers";
}

} // namespace

Task<CommandResult>
build_install(
    const std::string& package_name,
    bool with_headers,
    bool with_extra_modules)
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        co_return std::unexpected(TransactionError::InvalidOperation);
    }
    
    auto& db = db_result.value().get();
    
    if (co_await has_pending_updates()) {
        co_return std::unexpected(TransactionError::UpdatesPending);
    }
    
    KernelProvider provider;
    auto kernel_result = co_await provider.get_kernel(package_name);
    if (!kernel_result) {
        co_return std::unexpected(TransactionError::KernelNotFound);
    }
    
    const auto& kernel = kernel_result.value();
    
    std::vector<std::string> packages;
    packages.push_back(package_name);
    
    if (with_headers) {
        std::string headers_name = get_headers_package(package_name);
        auto headers_pkg = db.get_sync_pkg(headers_name);
        if (headers_pkg) {
            packages.push_back(headers_name);
        }
    }
    
    if (with_extra_modules) {
        for (const auto& module : kernel.extra_modules) {
            packages.push_back(module);
        }
    }
    
    co_return agent::make_install(std::move(packages));
}

Task<CommandResult>
build_remove(
    const std::string& package_name,
    bool with_headers,
    bool with_extra_modules,
    bool force)
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        co_return std::unexpected(TransactionError::InvalidOperation);
    }
    
    auto& db = db_result.value().get();
    
    KernelProvider provider;
    auto kernel_result = co_await provider.get_kernel(package_name);
    if (!kernel_result) {
        co_return std::unexpected(TransactionError::KernelNotFound);
    }
    
    const auto& kernel = kernel_result.value();
    
    if (!kernel.is_installed()) {
        co_return std::unexpected(TransactionError::KernelNotFound);
    }
    
    if (!force && !co_await is_safe_to_remove(package_name)) {
        co_return std::unexpected(TransactionError::KernelInUse);
    }
    
    std::vector<std::string> packages;
    packages.push_back(package_name);
    
    if (with_headers) {
        std::string headers_name = get_headers_package(package_name);
        auto headers_pkg = db.get_installed_pkg(headers_name);
        if (headers_pkg) {
            packages.push_back(headers_name);
        }
    }
    
    if (with_extra_modules) {
        for (const auto& module : kernel.extra_modules) {
            auto module_pkg = db.get_installed_pkg(module);
            if (module_pkg) {
                packages.push_back(module);
            }
        }
    }
    
    co_return agent::make_remove(std::move(packages), force);
}

Task<CommandResult>
build_upgrade(bool force_refresh)
{
    co_return agent::make_upgrade(force_refresh);
}

} // namespace mcp::kernel
