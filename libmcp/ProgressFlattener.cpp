/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "ProgressFlattener.hpp"

#include <pamac/transaction.hpp>

#include <cmath>
#include <regex>
#include <string>

namespace mcp {

void ProgressFlattener::connect_to_transaction(pamac::Transaction& txn)
{
    // Phase transition signals
    txn.signal_start_downloading.connect([this]() { on_start_downloading(); });
    txn.signal_stop_downloading.connect([this]() { on_stop_downloading(); });
    txn.signal_start_preparing.connect([this]() { on_start_preparing(); });
    txn.signal_stop_preparing.connect([this]() { on_stop_preparing(); });

    // Progress signals
    txn.signal_emit_action_progress.connect(
        [this](const std::string& action, const std::string& status, double progress) {
            on_action_progress(action, status, progress);
        });

    txn.signal_emit_download_progress.connect(
        [this](const std::string& action, const std::string& status, double progress) {
            on_download_progress(action, status, progress);
        });

    txn.signal_emit_hook_progress.connect(
        [this](const std::string& action, const std::string& details,
               const std::string& status, double progress) {
            on_hook_progress(action, details, status, progress);
        });
}

void ProgressFlattener::on_start_downloading()
{
    set_phase(Phase::Download);
    m_download_progress = 0.0;
    update_total_progress();
}

void ProgressFlattener::on_stop_downloading()
{
    m_download_progress = 1.0;
    update_total_progress();
    // Transition to prepare will happen via start_preparing signal
}

void ProgressFlattener::on_start_preparing()
{
    set_phase(Phase::Prepare);
    update_total_progress();
}

void ProgressFlattener::on_stop_preparing()
{
    // Move to Actions phase
    set_phase(Phase::Actions);
    m_action_progress = 0.0;
    update_total_progress();
}

void ProgressFlattener::on_action_progress(
    const std::string& action,
    const std::string& status,
    double progress)
{
    (void)action;

    // Extract and emit meaningful details (package name, operation)
    std::string details = extract_meaningful_detail(action, status);
    if (!details.empty() && details != m_last_action_details) {
        emit_details(details);
        m_last_action_details = details;
    }

    // action_progress reaches 100% when starting scripts, not at completion
    // We track this but don't transition phases here - wait for hook signals
    m_action_progress = progress;
    
    if (m_current_phase == Phase::Actions) {
        update_total_progress();
    }
}

void ProgressFlattener::on_download_progress(
    const std::string& action,
    const std::string& status,
    double progress)
{
    (void)action;
    (void)status;

    m_download_progress = progress;
    
    if (m_current_phase == Phase::Download) {
        update_total_progress();
    }
}

void ProgressFlattener::on_hook_progress(
    const std::string& action,
    const std::string& details,
    const std::string& status,
    double progress)
{
    (void)status;

    // First hook emission transitions us from Actions to Hooks phase
    if (m_current_phase == Phase::Actions) {
        set_phase(Phase::Hooks);
        m_hook_progress = 0.0;
    }
    
    // Extract meaningful hook details (hook name, not "1/2" repetitions)
    std::string hook_details = extract_meaningful_detail(action, details);
    if (!hook_details.empty() && hook_details != m_last_hook_details) {
        emit_details(hook_details);
        m_last_hook_details = hook_details;
    }
    
    m_hook_progress = progress;
    
    if (m_current_phase == Phase::Hooks) {
        update_total_progress();
        
        // When hooks complete, transition to Scripts phase
        if (progress >= 0.99) {
            set_phase(Phase::Scripts);
        }
    }
}

void ProgressFlattener::update_total_progress()
{
    double new_progress = 0.0;

    switch (m_current_phase) {
    case Phase::Download:
        new_progress = WEIGHT_DOWNLOAD * m_download_progress;
        break;

    case Phase::Prepare:
        // Download complete, preparing
        new_progress = WEIGHT_DOWNLOAD + WEIGHT_PREPARE * 0.5; // Arbitrary prepare progress
        break;

    case Phase::Actions:
        // Download + Prepare complete, actions in progress
        new_progress = WEIGHT_DOWNLOAD + WEIGHT_PREPARE + (WEIGHT_ACTIONS * m_action_progress);
        break;

    case Phase::Hooks:
        // Download + Prepare + Actions complete, hooks in progress
        new_progress = WEIGHT_DOWNLOAD + WEIGHT_PREPARE + WEIGHT_ACTIONS +
                     (WEIGHT_HOOKS * m_hook_progress);
        break;

    case Phase::Scripts:
        // Everything else complete, scripts running
        // Script progress is implicit - we just show 90-100% range
        new_progress = WEIGHT_DOWNLOAD + WEIGHT_PREPARE + WEIGHT_ACTIONS + WEIGHT_HOOKS +
                     WEIGHT_SCRIPTS * 0.5; // Show 95% during scripts
        break;
    }

    // Clamp to [0.0, 1.0] and only emit if changed
    new_progress = std::clamp(new_progress, 0.0, 1.0);
    
    if (std::abs(new_progress - m_total_progress) > 0.001) {
        m_total_progress = new_progress;
        signal_progress_changed.emit(m_total_progress);
    }
}

void ProgressFlattener::set_phase(Phase phase)
{
    if (m_current_phase != phase) {
        m_current_phase = phase;
        signal_phase_changed.emit(phase_name(phase));
    }
}

std::string ProgressFlattener::phase_name(Phase phase)
{
    using namespace std::string_literals;

    switch (phase) {
    case Phase::Download: return "Downloading"s;
    case Phase::Prepare:  return "Preparing"s;
    case Phase::Actions:  return "Processing"s;
    case Phase::Hooks:    return "Running hooks"s;
    case Phase::Scripts:  return "Running scripts"s;
    }

    return "Unknown"s;
}

void ProgressFlattener::emit_details(const std::string& details)
{
    if (is_repetitive_detail(details)) {
        m_detail_repeat_count++;
        // Only emit every 10th repetition to reduce spam
        if (m_detail_repeat_count % 10 != 0) {
            return;
        }
    } else {
        m_detail_repeat_count = 0;
    }

    if (details != m_last_emitted_details) {
        m_last_emitted_details = details;
        signal_details_changed.emit(details);
    }
}

bool ProgressFlattener::is_repetitive_detail(const std::string& details) const
{
    // Detect patterns like "1/2", "2/2", "1/50", etc.
    static std::regex repetitive_pattern(R"(^\s*\d+/\d+\s*$)");
    return std::regex_match(details, repetitive_pattern);
}

std::string ProgressFlattener::extract_meaningful_detail(
    const std::string& action,
    const std::string& details) const
{
    // If details are just "1/2" style counters, ignore them
    if (is_repetitive_detail(details)) {
        return {};
    }

    // If details are empty, use action
    if (details.empty()) {
        return action;
    }

    // Return the actual meaningful detail
    return details;
}

} // namespace mcp
