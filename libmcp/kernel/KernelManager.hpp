/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * KernelManager - manages kernel installation/removal using libpamac transactions.
 * Provides both synchronous and coroutine-based interfaces for kernel operations.
 */

#pragma once

#include "KernelProvider.hpp"

#include <pamac/database.hpp>
#include <pamac/transaction.hpp>

#include <coro/task.hpp>

#include <expected>
#include <string>

namespace mcp::kernel {

enum class KernelManagerError {
    KernelNotFound,
    KernelInUse,
    InvalidOperation,
    UpdatesPending  // System has pending updates - upgrade required before kernel operations
};

template<typename T>
using KernelManagerResult = std::expected<T, KernelManagerError>;

/**
 * KernelManager - manages kernel installation, removal, and upgrades.
 * 
 * Two operational modes:
 * 1. Direct transaction (legacy): Build and execute pamac transaction in-process
 * 2. Agent mode (recommended): Spawn mcp-transaction-agent process
 * 
 * Agent mode allows transactions to continue even if calling app closes.
 * 
 * Usage (Direct):
 *   auto db = pamac::Database::instance();
 *   KernelManager manager(*db);
 *   
 *   if (auto result = manager.add_to_install("linux66"); result) {
 *       auto& txn = manager.transaction();
 *       txn.signal_emit_action.connect([](auto msg) { ... });
 *       co_await txn.run_async();
 *   }
 * 
 * Usage (Agent):
 *   KernelManager manager;
 *   
 *   auto cmd = manager.build_agent_command("install", {"linux66", "linux66-headers"});
 *   // Launch: mcp-transaction-agent install linux66 linux66-headers
 *   // Monitor process, check exit code for success/failure
 */
class KernelManager {
public:
    explicit KernelManager(pamac::Database& database);

    // === Transaction building (direct mode) ===

    // Checks for pending system updates before allowing installation
    [[nodiscard]] KernelManagerResult<void> 
    add_to_install(const std::string& package_name, bool with_headers = false);

    // Prevents removal of currently running kernel unless forced
    [[nodiscard]] KernelManagerResult<void> 
    add_to_remove(const std::string& package_name, bool with_headers = false, bool force = false);

    void add_to_upgrade(bool force_refresh = false);

    [[nodiscard]] pamac::Transaction& transaction() { return transaction_; }

    void reset_transaction();

    /**
     * Called after successful transaction to refresh file database.
     * Should be called after co_await txn.run_async() returns true.
     */
    void on_transaction_success();

    // === Agent mode helpers ===
    
    /**
     * Build command for spawning mcp-transaction-agent.
     * Returns: {executable_path, arg1, arg2, ...}
     * 
     * @param operation - "install", "remove", or "upgrade"
     * @param packages - package names
     * @param force - force mode for remove operations
     * @param refresh - force refresh for upgrade
     */
    [[nodiscard]] static std::vector<std::string> 
    build_agent_command(
        const std::string& operation,
        const std::vector<std::string>& packages,
        bool force = false,
        bool refresh = false
    );

private:
    [[nodiscard]] bool has_pending_updates(pamac::Database& db) const;

    [[nodiscard]] bool verify_safe_to_remove(const std::string& package_name) const;

    [[nodiscard]] static std::string get_headers_package(const std::string& package_name);

    KernelProvider m_provider;
    pamac::Database& database_;
    pamac::Transaction transaction_;
};

} // namespace mcp::kernel
