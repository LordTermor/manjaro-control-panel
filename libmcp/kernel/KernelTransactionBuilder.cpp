/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Builds validated transaction agent command lines for kernel operations.
 */

#include "KernelTransactionBuilder.hpp"

#include <pamac/database.hpp>

namespace mcp::kernel {

coro::task<bool> KernelTransactionBuilder::has_pending_updates_async()
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        co_return false;
    }
    
    auto& db = db_result.value().get();
    auto updates = co_await db.get_updates_async();
    co_return updates.has_updates();
}

coro::task<std::expected<AgentCommand, TransactionError>>
KernelTransactionBuilder::install(
    const std::string& package_name,
    bool with_headers,
    bool with_extra_modules)
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        co_return std::unexpected(TransactionError::InvalidOperation);
    }
    
    auto& db = db_result.value().get();
    
    if (co_await has_pending_updates_async()) {
        co_return std::unexpected(TransactionError::UpdatesPending);
    }
    
    KernelProvider provider;
    auto kernel_result = co_await provider.get_kernel_async(package_name);
    if (!kernel_result) {
        co_return std::unexpected(TransactionError::KernelNotFound);
    }
    
    const auto& kernel = kernel_result.value();
    
    AgentCommand cmd;
    cmd.operation = "install";
    cmd.packages.push_back(package_name);
    
    if (with_headers) {
        std::string headers_name = get_headers_package(package_name);
        auto headers_pkg = db.get_sync_pkg(headers_name);
        if (headers_pkg) {
            cmd.packages.push_back(headers_name);
        }
    }
    
    if (with_extra_modules) {
        for (const auto& module : kernel.extra_modules) {
            cmd.packages.push_back(module);
        }
    }
    
    co_return cmd;
}

coro::task<std::expected<AgentCommand, TransactionError>>
KernelTransactionBuilder::remove(
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
    auto kernel_result = co_await provider.get_kernel_async(package_name);
    if (!kernel_result) {
        co_return std::unexpected(TransactionError::KernelNotFound);
    }
    
    const auto& kernel = kernel_result.value();
    
    if (!kernel.is_installed()) {
        co_return std::unexpected(TransactionError::KernelNotFound);
    }
    
    if (!force && !co_await is_safe_to_remove_async(package_name)) {
        co_return std::unexpected(TransactionError::KernelInUse);
    }
    
    AgentCommand cmd;
    cmd.operation = "remove";
    cmd.force = force;
    cmd.packages.push_back(package_name);
    
    if (with_headers) {
        std::string headers_name = get_headers_package(package_name);
        auto headers_pkg = db.get_installed_pkg(headers_name);
        if (headers_pkg) {
            cmd.packages.push_back(headers_name);
        }
    }
    
    if (with_extra_modules) {
        for (const auto& module : kernel.extra_modules) {
            auto module_pkg = db.get_installed_pkg(module);
            if (module_pkg) {
                cmd.packages.push_back(module);
            }
        }
    }
    
    co_return cmd;
}

coro::task<std::expected<AgentCommand, TransactionError>>
KernelTransactionBuilder::upgrade(bool force_refresh)
{
    AgentCommand cmd;
    cmd.operation = "upgrade";
    cmd.refresh = force_refresh;
    co_return cmd;
}

std::string KernelTransactionBuilder::get_headers_package(const std::string& package_name)
{
    return package_name + "-headers";
}

coro::task<bool> KernelTransactionBuilder::is_safe_to_remove_async(const std::string& package_name)
{
    KernelProvider provider;
    auto kernel_result = co_await provider.get_kernel_async(package_name);
    
    if (!kernel_result) {
        co_return false;
    }
    
    const auto& kernel = kernel_result.value();
    co_return !kernel.is_in_use();
}

} // namespace mcp::kernel
