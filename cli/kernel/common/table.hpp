/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "output.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace mcp::cli {

/**
 * Column alignment options.
 */
enum class Align { Left, Right, Center };

/**
 * Table column definition.
 */
struct Column {
    std::string header;
    size_t width = 0;        // 0 = auto-calculate
    Align align = Align::Left;
};

/**
 * Table cell with optional styling.
 */
struct Cell {
    std::string text;
    std::optional<fmt::text_style> style;
    
    Cell() = default;
    Cell(std::string t) : text(std::move(t)) {}
    Cell(std::string t, fmt::text_style s) : text(std::move(t)), style(s) {}
    Cell(std::string_view t) : text(t) {}
    Cell(std::string_view t, fmt::text_style s) : text(t), style(s) {}
    Cell(const char* t) : text(t) {}
    Cell(const char* t, fmt::text_style s) : text(t), style(s) {}
};

/**
 * Table row - vector of cells with optional prefix.
 */
struct Row {
    std::vector<Cell> cells;
    std::string prefix;  // e.g., "▶ " for running kernel
    
    Row() = default;
    Row(std::vector<Cell> c) : cells(std::move(c)) {}
    Row(std::vector<Cell> c, std::string p) : cells(std::move(c)), prefix(std::move(p)) {}
};

/**
 * Simple table builder for formatted terminal output.
 * 
 * Usage:
 *   Table table;
 *   table.add_column("NAME", Align::Left);
 *   table.add_column("VERSION", Align::Right);
 *   table.add_row({{"linux66", fmt::fg(fmt::color::green)}, "6.6.117-1"});
 *   table.print();
 */
class Table {
    std::vector<Column> m_columns;
    std::vector<Row> m_rows;
    size_t m_prefix_width = 2;

public:
    /// Add a column definition
    Table& add_column(std::string header, Align align = Align::Left, size_t width = 0) {
        m_columns.push_back({std::move(header), width, align});
        return *this;
    }

    /// Add a data row
    Table& add_row(std::vector<Cell> cells, std::string prefix = "") {
        m_rows.push_back({std::move(cells), std::move(prefix)});
        return *this;
    }

    /// Add a separator row
    Table& add_separator() {
        m_rows.push_back({});
        return *this;
    }

    /// Set prefix column width
    Table& set_prefix_width(size_t width) {
        m_prefix_width = width;
        return *this;
    }

    /// Calculate column widths based on content
    void auto_size() {
        for (size_t i = 0; i < m_columns.size(); ++i) {
            if (m_columns[i].width == 0) {
                m_columns[i].width = m_columns[i].header.size();
                for (const auto& row : m_rows) {
                    if (i < row.cells.size()) {
                        m_columns[i].width = std::max(m_columns[i].width, 
                                                       row.cells[i].text.size());
                    }
                }
            }
        }
    }

    /// Print the table header
    void print_header() const {
        fmt::print("{}", std::string(m_prefix_width, ' '));
        
        for (size_t i = 0; i < m_columns.size(); ++i) {
            const auto& col = m_columns[i];
            
            if (out().color_enabled()) {
                fmt::print("{}", fmt::styled(format_cell(col.header, col.width, col.align),
                                            fmt::emphasis::faint));
            } else {
                fmt::print("{}", format_cell(col.header, col.width, col.align));
            }
            
            if (i < m_columns.size() - 1) {
                fmt::print("  ");
            }
        }
        fmt::print("\n");
        
        print_separator();
    }

    /// Print separator line
    void print_separator() const {
        size_t total = m_prefix_width;
        for (size_t i = 0; i < m_columns.size(); ++i) {
            total += m_columns[i].width;
            if (i < m_columns.size() - 1) {
                total += 2;
            }
        }
        
        // Build separator string with repeated Unicode dash
        std::string sep;
        sep.reserve(total * 3); // UTF-8 encoding of ─ is 3 bytes
        for (size_t i = 0; i < total; ++i) {
            sep += "\u2500";
        }
        
        if (out().color_enabled()) {
            fmt::print("{}\n", fmt::styled(sep, fmt::emphasis::faint));
        } else {
            fmt::print("{}\n", sep);
        }
    }

    /// Print all rows
    void print_rows() const {
        for (const auto& row : m_rows) {
            if (row.cells.empty()) {
                print_separator();
                continue;
            }

            // Print prefix
            if (!row.prefix.empty()) {
                fmt::print("{}", row.prefix);
                // Assume prefix contains 2 visible characters (e.g., "▶ ")
                if (row.prefix.size() < m_prefix_width) {
                    fmt::print("{}", std::string(m_prefix_width - 2, ' '));
                }
            } else {
                fmt::print("{}", std::string(m_prefix_width, ' '));
            }

            // Print cells
            for (size_t i = 0; i < m_columns.size(); ++i) {
                if (i < row.cells.size()) {
                    const auto& cell = row.cells[i];
                    auto formatted = format_cell(cell.text, m_columns[i].width, m_columns[i].align);
                    
                    if (cell.style && out().color_enabled()) {
                        fmt::print("{}", fmt::styled(formatted, *cell.style));
                    } else {
                        fmt::print("{}", formatted);
                    }
                } else {
                    fmt::print("{}", std::string(m_columns[i].width, ' '));
                }
                
                if (i < m_columns.size() - 1) {
                    fmt::print("  ");
                }
            }
            fmt::print("\n");
        }
    }

    /// Print complete table
    void print() const {
        print_header();
        print_rows();
    }

private:
    /// Format a cell value with alignment using fmt's native formatting
    [[nodiscard]] static std::string format_cell(std::string_view text, size_t width, Align align) {
        switch (align) {
            case Align::Left:
                return fmt::format("{:<{}}", text, width);
            case Align::Right:
                return fmt::format("{:>{}}", text, width);
            case Align::Center:
                return fmt::format("{:^{}}", text, width);
        }
        return std::string(text);
    }
};

} // namespace mcp::cli
