/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Terminal progress bar with Unicode block characters.
 * Provides npm-style visual progress indication with braille patterns
 * for partial blocks and filled blocks for completed sections.
 */

#pragma once

#include "colors.hpp"
#include "output.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <string_view>
#include <unistd.h>

namespace mcp::cli {

// 8 sub-block states for smooth progress animation
namespace progress_chars {

inline constexpr std::string_view filled = "█";

inline constexpr std::string_view blocks[] = {
    " ",  // 0/8 - empty
    "▏",  // 1/8
    "▎",  // 2/8
    "▍",  // 3/8
    "▌",  // 4/8
    "▋",  // 5/8
    "▊",  // 6/8
    "▉",  // 7/8
};

inline constexpr std::string_view braille_spinner[] = {
    "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
};

inline constexpr std::string_view empty = "░";

} // namespace progress_chars

struct ProgressPalette {
    virtual ~ProgressPalette() = default;
    
    virtual void print_label(std::string_view label) const = 0;
    virtual void print_filled_bar(std::string_view bar) const = 0;
    virtual void print_empty_bar(std::string_view bar) const = 0;
    virtual void print_spinner(std::string_view spinner) const = 0;
    virtual void print_status(std::string_view status) const = 0;
    virtual void print_eta(std::string_view eta) const = 0;
    virtual void print_success(std::string_view sym, std::string_view msg) const = 0;
    virtual void print_error(std::string_view sym, std::string_view msg) const = 0;
};

struct ColorfulPalette final : ProgressPalette {
    void print_label(std::string_view label) const override {
        fmt::print("{} ", fmt::styled(label, fmt::fg(fmt::color::cyan)));
    }
    
    void print_filled_bar(std::string_view bar) const override {
        fmt::print("{}", fmt::styled(bar, fmt::fg(fmt::color::green)));
    }
    
    void print_empty_bar(std::string_view bar) const override {
        fmt::print("{}", fmt::styled(bar, fmt::fg(fmt::color::gray)));
    }
    
    void print_spinner(std::string_view spinner) const override {
        fmt::print("{}", fmt::styled(spinner, fmt::fg(fmt::color::yellow)));
    }
    
    void print_status(std::string_view status) const override {
        fmt::print(" {}", fmt::styled(status, fmt::emphasis::faint));
    }
    
    void print_eta(std::string_view eta) const override {
        fmt::print("{}", fmt::styled(eta, fmt::emphasis::faint));
    }
    
    void print_success(std::string_view sym, std::string_view msg) const override {
        fmt::print("{} {}\n",
            fmt::styled(sym, fmt::fg(fmt::color::green)),
            msg);
    }
    
    void print_error(std::string_view sym, std::string_view msg) const override {
        fmt::print("{} {}\n",
            fmt::styled(sym, fmt::fg(fmt::color::red)),
            msg);
    }
};

struct BwPalette final : ProgressPalette {
    void print_label(std::string_view label) const override {
        fmt::print("{} ", label);
    }
    
    void print_filled_bar(std::string_view bar) const override {
        fmt::print("{}", bar);
    }
    
    void print_empty_bar(std::string_view bar) const override {
        fmt::print("{}", bar);
    }
    
    void print_spinner(std::string_view spinner) const override {
        fmt::print("{}", spinner);
    }
    
    void print_status(std::string_view status) const override {
        fmt::print(" {}", status);
    }
    
    void print_eta(std::string_view eta) const override {
        fmt::print("{}", eta);
    }
    
    void print_success(std::string_view sym, std::string_view msg) const override {
        fmt::print("{} {}\n", sym, msg);
    }
    
    void print_error(std::string_view sym, std::string_view msg) const override {
        fmt::print("{} {}\n", sym, msg);
    }
};

inline std::unique_ptr<ProgressPalette> make_palette(bool use_color) {
    if (use_color) {
        return std::make_unique<ColorfulPalette>();
    }
    return std::make_unique<BwPalette>();
}

inline std::unique_ptr<ProgressPalette> make_palette() {
    return make_palette(isatty(fileno(stdout)) != 0);
}

class ProgressBar {
    std::string m_label;
    std::string m_status;
    int m_width;
    double m_progress = 0.0;
    bool m_show_percentage = true;
    bool m_show_eta = false;
    bool m_finished = false;
    
    std::unique_ptr<ProgressPalette> m_palette;
    std::chrono::steady_clock::time_point m_start_time;
    int m_spinner_frame = 0;

public:
    explicit ProgressBar(std::string label = "", int width = 30)
        : m_label(std::move(label))
        , m_width(width)
        , m_palette(make_palette())
        , m_start_time(std::chrono::steady_clock::now())
    {}

    ProgressBar& set_label(std::string label) {
        m_label = std::move(label);
        return *this;
    }

    ProgressBar& set_width(int width) {
        m_width = width;
        return *this;
    }

    ProgressBar& set_palette(std::unique_ptr<ProgressPalette> palette) {
        m_palette = std::move(palette);
        return *this;
    }

    ProgressBar& set_color(bool enabled) {
        m_palette = make_palette(enabled);
        return *this;
    }

    ProgressBar& set_show_percentage(bool show) {
        m_show_percentage = show;
        return *this;
    }

    ProgressBar& set_show_eta(bool show) {
        m_show_eta = show;
        return *this;
    }

    void reset() {
        m_progress = 0.0;
        m_status.clear();
        m_finished = false;
        m_start_time = std::chrono::steady_clock::now();
        m_spinner_frame = 0;
    }

    void update(double progress, std::string_view status = "") {
        m_progress = std::clamp(progress, 0.0, 1.0);
        m_status = status;
        render();
    }

    void update(size_t current, size_t total, std::string_view status = "") {
        if (total > 0) {
            update(static_cast<double>(current) / static_cast<double>(total), status);
        }
    }

    void spin(std::string_view status = "") {
        m_status = status;
        m_spinner_frame = (m_spinner_frame + 1) % 10;
        render_spinner();
    }

    void finish(std::string_view message = "") {
        if (m_finished) return;
        m_finished = true;
        m_progress = 1.0;
        
        fmt::print("\r\033[K");
        
        if (!message.empty()) {
            m_palette->print_success(symbol::check, message);
        } else {
            render();
            fmt::print("\n");
        }
        std::fflush(stdout);
    }

    void fail(std::string_view message = "") {
        if (m_finished) return;
        m_finished = true;
        
        fmt::print("\r\033[K");
        m_palette->print_error(symbol::cross, message.empty() ? "Failed" : message);
        std::fflush(stdout);
    }

    void clear() {
        fmt::print("\r\033[K");
        std::fflush(stdout);
    }

private:
    void render() {
        if (m_finished) return;
        
        double filled_blocks = m_progress * m_width;
        int full_blocks = static_cast<int>(filled_blocks);
        int partial_index = static_cast<int>((filled_blocks - full_blocks) * 8);
        int empty_blocks = m_width - full_blocks - (partial_index > 0 ? 1 : 0);
        
        // Build filled part
        std::string filled_part;
        filled_part.reserve(m_width * 4);
        for (int i = 0; i < full_blocks; ++i) {
            filled_part += progress_chars::filled;
        }
        if (partial_index > 0 && full_blocks < m_width) {
            filled_part += progress_chars::blocks[partial_index];
        }
        
        // Build empty part
        std::string empty_part;
        empty_part.reserve(m_width * 4);
        for (int i = 0; i < empty_blocks; ++i) {
            empty_part += progress_chars::empty;
        }
        
        int percent = static_cast<int>(std::round(m_progress * 100));
        
        // Calculate ETA
        std::string eta_str;
        if (m_show_eta && m_progress > 0.01) {
            auto elapsed = std::chrono::steady_clock::now() - m_start_time;
            auto elapsed_secs = std::chrono::duration<double>(elapsed).count();
            double remaining_secs = (elapsed_secs / m_progress) * (1.0 - m_progress);
            
            if (remaining_secs < 60) {
                eta_str = fmt::format(" ETA: {:.0f}s", remaining_secs);
            } else if (remaining_secs < 3600) {
                eta_str = fmt::format(" ETA: {:.0f}m{:.0f}s", 
                    std::floor(remaining_secs / 60), 
                    std::fmod(remaining_secs, 60));
            } else {
                eta_str = fmt::format(" ETA: {:.0f}h{:.0f}m",
                    std::floor(remaining_secs / 3600),
                    std::fmod(remaining_secs / 60, 60));
            }
        }
        
        // Truncate status
        std::string status_display = m_status;
        const size_t max_status_len = 25;
        if (status_display.length() > max_status_len) {
            status_display = "..." + status_display.substr(status_display.length() - max_status_len + 3);
        }
        
        // Render using palette
        fmt::print("\r\033[K");
        
        if (!m_label.empty()) {
            m_palette->print_label(m_label);
        }
        
        m_palette->print_filled_bar(filled_part);
        m_palette->print_empty_bar(empty_part);
        
        if (m_show_percentage) {
            fmt::print(" {:3d}%", percent);
        }
        
        if (!status_display.empty()) {
            m_palette->print_status(status_display);
        }
        
        if (!eta_str.empty()) {
            m_palette->print_eta(eta_str);
        }
        
        std::fflush(stdout);
    }
    
    void render_spinner() {
        if (m_finished) return;
        
        fmt::print("\r\033[K");
        
        if (!m_label.empty()) {
            m_palette->print_label(m_label);
        }
        
        m_palette->print_spinner(progress_chars::braille_spinner[m_spinner_frame]);
        
        if (!m_status.empty()) {
            fmt::print(" {}", m_status);
        }
        
        std::fflush(stdout);
    }
};

} // namespace mcp::cli
