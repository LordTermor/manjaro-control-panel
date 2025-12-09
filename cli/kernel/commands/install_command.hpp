/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "command.hpp"
#include "common/output.hpp"
#include "common/progress_bar.hpp"

#include "kernel/KernelProvider.hpp"

#include <pamac/database.hpp>
#include <pamac/transaction.hpp>

#include <fmt/core.h>

#include <glib.h>

#include <iostream>
#include <string>
#include <vector>

namespace mcp::cli::kernel {

using namespace mcp::kernel;

class InstallCommand : public Command {
    std::string m_package_name;
    bool m_download_only;
    bool m_no_confirm;
    
    ProgressBar m_download_bar{"Downloading", 35};
    ProgressBar m_action_bar{"Installing", 35};
    bool m_download_active = false;
    bool m_action_active = false;

public:
    explicit InstallCommand(std::string package_name, bool download_only = false, bool no_confirm = false)
        : m_package_name(std::move(package_name))
        , m_download_only(download_only)
        , m_no_confirm(no_confirm)
    {}

    [[nodiscard]] int execute() override {
        KernelProvider provider;
        auto kernel_result = provider.get_kernel(m_package_name);

        if (!kernel_result) {
            switch (kernel_result.error()) {
                case KernelError::DatabaseNotInitialized:
                    out().error("Failed to initialize package database.");
                    break;
                case KernelError::NotFound:
                    out().error(fmt::format("Kernel '{}' not found.", m_package_name));
                    out().info("Use 'mcp-kernel list' to see available kernels.");
                    break;
                case KernelError::ParseError:
                    out().error("Failed to parse kernel information.");
                    break;
            }
            return 1;
        }

        const auto& kernel = *kernel_result;

        if (kernel.is_installed()) {
            out().info(fmt::format("Kernel '{}' is already installed (version {}).",
                                   m_package_name, kernel.installed_version));
            return 0;
        }

        auto db_result = pamac::Database::instance();
        if (!db_result) {
            out().error("Database not initialized.");
            return 1;
        }
        auto& db = db_result.value();

        pamac::Transaction txn(db);

        setup_transaction_signals(txn);

        txn.set_download_only(m_download_only);

        txn.add_pkg_to_install(m_package_name);

        std::string headers_pkg = m_package_name + "-headers";
        auto headers_result = provider.get_kernel(headers_pkg);
        if (headers_result && !headers_result->is_installed()) {
            out().info(fmt::format("Also installing headers package: {}", headers_pkg));
            txn.add_pkg_to_install(headers_pkg);
        }

        out().header("Installing Kernel");
        fmt::print("Package: {}\n", m_package_name);
        fmt::print("Version: {}\n", kernel.available_version);
        if (!m_no_confirm && !confirm_installation()) {
            out().info("Installation cancelled.");
            return 0;
        }

        fmt::print("\n");
        out().info("Requesting authorization...");
        
        bool auth_result = false;
        bool auth_done = false;
        pamac_transaction_get_authorization_async(
            txn.c_ptr(),
            +[](GObject* /*obj*/, GAsyncResult* res, gpointer user_data) {
                auto* data = static_cast<std::pair<bool*, bool*>*>(user_data);
                *data->first = pamac_transaction_get_authorization_finish(
                    PAMAC_TRANSACTION(g_async_result_get_source_object(res)), res) != FALSE;
                *data->second = true;
            },
            new std::pair<bool*, bool*>(&auth_result, &auth_done));
        
        while (!auth_done) {
            g_main_context_iteration(nullptr, TRUE);
        }

        if (!auth_result) {
            out().error("Failed to get authorization.");
            return 1;
        }

        out().info("Starting transaction...");
        
        bool run_result = false;
        bool run_done = false;
        pamac_transaction_run_async(
            txn.c_ptr(),
            +[](GObject* /*obj*/, GAsyncResult* res, gpointer user_data) {
                auto* data = static_cast<std::pair<bool*, bool*>*>(user_data);
                *data->first = pamac_transaction_run_finish(
                    PAMAC_TRANSACTION(g_async_result_get_source_object(res)), res) != FALSE;
                *data->second = true;
            },
            new std::pair<bool*, bool*>(&run_result, &run_done));
        
        while (!run_done) {
            g_main_context_iteration(nullptr, TRUE);
        }

        txn.remove_authorization();

        if (!run_result) {
            out().error("Transaction failed.");
            return 1;
        }

        fmt::print("\n");
        if (m_download_only) {
            out().success(fmt::format("Kernel '{}' downloaded successfully.", m_package_name));
        } else {
            out().success(fmt::format("Kernel '{}' installed successfully.", m_package_name));
            out().info("Reboot to use the new kernel.");
        }

        return 0;
    }

private:
    void on_action(const std::string& action) {
        finish_action();
        finish_download();
        out().info(action);
    }

    void on_action_progress(const std::string& action, const std::string& status, double progress) {
        if (!m_action_active) {
            m_action_bar.set_label(action);
            m_action_bar.reset();
            m_action_active = true;
        }
        m_action_bar.update(progress, status);
        
        if (progress >= 1.0) {
            finish_action();
        }
    }

    void on_download_progress(const std::string& /*action*/, const std::string& status, double progress) {
        if (!m_download_active) {
            m_download_bar.reset();
            m_download_active = true;
        }
        m_download_bar.update(progress, status);
        
        if (progress >= 1.0) {
            finish_download();
        }
    }

    void on_error(const std::string& message, const std::vector<std::string>& details) {
        if (m_action_active) {
            m_action_bar.fail(message);
            m_action_active = false;
        } else if (m_download_active) {
            m_download_bar.fail(message);
            m_download_active = false;
        } else {
            out().error(message);
        }
        for (const auto& detail : details) {
            fmt::print(stderr, "  {}\n", detail);
        }
    }

    void on_warning(const std::string& warning) {
        clear_progress();
        fmt::print(stderr, "{} {}\n",
            fmt::styled(symbol::warning, fmt::fg(fmt::color::yellow)),
            warning);
    }

    void on_script_output(const std::string& output) {
        clear_progress();
        fmt::print("{}", output);
    }

    void on_hook_progress(const std::string& action, const std::string& target,
                          const std::string& /*percent_str*/, double progress) {
        on_action_progress(action, target, progress);
    }

    void on_start_downloading() {
        finish_action();
        out().info("Downloading packages...");
    }

    void on_start_preparing() {
        finish_download();
        out().info("Preparing transaction...");
    }

    void finish_download(std::string_view message = "") {
        if (m_download_active) {
            m_download_bar.finish(message);
            m_download_active = false;
        }
    }

    void finish_action(std::string_view message = "") {
        if (m_action_active) {
            m_action_bar.finish(message);
            m_action_active = false;
        }
    }

    void clear_progress() {
        if (m_download_active) m_download_bar.clear();
        if (m_action_active) m_action_bar.clear();
    }

    void setup_transaction_signals(pamac::Transaction& txn) {
        txn.signal_emit_action.connect([this](const std::string& a) { on_action(a); });
        txn.signal_emit_action_progress.connect([this](const std::string& a, const std::string& s, double p) { on_action_progress(a, s, p); });
        txn.signal_emit_download_progress.connect([this](const std::string& a, const std::string& s, double p) { on_download_progress(a, s, p); });
        txn.signal_emit_error.connect([this](const std::string& m, const std::vector<std::string>& d) { on_error(m, d); });
        txn.signal_emit_warning.connect([this](const std::string& w) { on_warning(w); });
        txn.signal_emit_script_output.connect([this](const std::string& o) { on_script_output(o); });
        txn.signal_emit_hook_progress.connect([this](const std::string& a, const std::string& t, const std::string& ps, double p) { on_hook_progress(a, t, ps, p); });
        txn.signal_start_downloading.connect([this]() { on_start_downloading(); });
        txn.signal_start_preparing.connect([this]() { on_start_preparing(); });
        txn.signal_start_waiting.connect([]() { out().info("Waiting..."); });
        txn.signal_stop_downloading.connect([this]() { finish_download(); });
    }

    bool confirm_installation() const {
        fmt::print("\nProceed with installation? [Y/n] ");
        std::fflush(stdout);

        std::string response;
        std::getline(std::cin, response);

        return response.empty() || response == "y" || response == "Y" ||
               response == "yes" || response == "Yes" || response == "YES";
    }
};

} // namespace mcp::cli::kernel
