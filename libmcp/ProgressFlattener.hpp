/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <sigc++/signal.h>

#include <string>

namespace pamac {
class Transaction;
}

namespace mcp {

/*
 * Flattens multiple pamac transaction progress signals into unified 0-100% progress.
 * 
 * pamac emits separate progress for different phases (download, action, hooks, scripts),
 * where each can reach 100% independently. This class combines them into a single
 * progress value that reaches 100% only when transaction truly completes.
 * 
 * Phase weights (based on typical transaction behavior):
 * - Download:  30% (0-30%)   - Cancellable phase
 * - Prepare:    5% (30-35%)  - Database preparation
 * - Actions:   45% (35-80%)  - Install/remove/upgrade operations
 * - Hooks:     10% (80-90%)  - Post-transaction hooks
 * - Scripts:   10% (90-100%) - Post-install scripts
 */
class ProgressFlattener {
public:
    ProgressFlattener() = default;

    // Connect to pamac::Transaction signals
    void connect_to_transaction(pamac::Transaction& txn);

    // Get current unified progress (0.0 - 1.0)
    [[nodiscard]] double progress() const { return m_total_progress; }

    // Signals
    // Emitted when unified progress changes (value 0.0 - 1.0)
    sigc::signal<void(double)> signal_progress_changed;

    // Emitted when phase changes with phase name
    sigc::signal<void(const std::string&)> signal_phase_changed;

    // Emitted when meaningful details change (deduplicated)
    sigc::signal<void(const std::string&)> signal_details_changed;

private:
    enum class Phase {
        Download,  // Downloading packages
        Prepare,   // Preparing transaction
        Actions,   // Installing/removing/upgrading
        Hooks,     // Running hooks
        Scripts    // Running scripts
    };

    // Phase tracking
    Phase m_current_phase{Phase::Download};
    
    // Per-phase progress (0.0 - 1.0)
    double m_download_progress{0.0};
    double m_action_progress{0.0};
    double m_hook_progress{0.0};
    
    // Unified total progress
    double m_total_progress{0.0};

    // Detail deduplication tracking
    std::string m_last_emitted_details;
    std::string m_last_action_details;
    std::string m_last_hook_details;
    int m_detail_repeat_count{0};

    // Phase weights
    static constexpr double WEIGHT_DOWNLOAD = 0.30;
    static constexpr double WEIGHT_PREPARE = 0.05;
    static constexpr double WEIGHT_ACTIONS = 0.45;
    static constexpr double WEIGHT_HOOKS = 0.10;
    static constexpr double WEIGHT_SCRIPTS = 0.10;

    // Phase transition handlers
    void on_start_downloading();
    void on_stop_downloading();
    void on_start_preparing();
    void on_stop_preparing();

    // Progress update handlers
    void on_action_progress(const std::string& action, const std::string& status, double progress);
    void on_download_progress(const std::string& action, const std::string& status, double progress);
    void on_hook_progress(const std::string& action, const std::string& details,
                       const std::string& status, double progress);

    // Calculate total progress based on current phase and progress
    void update_total_progress();
    
    // Set current phase and emit signal
    void set_phase(Phase phase);
    
    // Get phase name for UI display
    [[nodiscard]] static std::string phase_name(Phase phase);

    // Detail flattening helpers
    void emit_details(const std::string& details);
    [[nodiscard]] bool is_repetitive_detail(const std::string& details) const;
    [[nodiscard]] std::string extract_meaningful_detail(const std::string& action, const std::string& details) const;
};

} // namespace mcp
