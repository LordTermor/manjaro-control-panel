/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/Config.hpp"
#include "StringUtils.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <ranges>
#include <unordered_map>

/*
 * MHWDCONFIG file parser and device matching logic.
 * Handles config parsing with external file references, pattern matching,
 * and dependency/conflict checking.
 */

#define CFG_HANDLER [](Config& cfg, const std::string& val)

namespace rg = std::ranges;

namespace mcp::mhwd {

namespace {

using namespace string_utils;

// Read external file referenced by ">filename" syntax
std::string read_external_file(const std::filesystem::path& file_path, const std::filesystem::path& base_path)
{
    std::filesystem::path full_path = file_path.is_absolute() ? file_path : base_path / file_path;

    std::ifstream file(full_path);
    if (!file.is_open()) {
        return "";
    }

    std::string result;
    std::string line;

    while (std::getline(file, line)) {
        const auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        line = trim(line);
        if (!line.empty()) {
            result += " " + line;
        }
    }

    return trim(result);
}

std::optional<std::string> preprocess_line(std::string line)
{
    const auto comment_pos = line.find('#');
    if (comment_pos != std::string::npos) {
        line = line.substr(0, comment_pos);
    }

    line = trim(line);
    return line.empty() ? std::nullopt : std::optional{line};
}

std::optional<std::pair<std::string, std::string>> parse_key_value(const std::string& line, const std::filesystem::path& base_path)
{
    const auto equals_pos = line.find('=');
    if (equals_pos == std::string::npos) {
        return std::nullopt;
    }

    std::string key = to_lower(trim(line.substr(0, equals_pos)));
    std::string value = trim(trim_quotes(trim(line.substr(equals_pos + 1))));

    // Handle external file references
    if (value.starts_with('>') && value.size() > 1) {
        value = read_external_file(value.substr(1), base_path);
    }

    return std::pair{std::move(key), std::move(value)};
}

// Finalize patterns by adding wildcard defaults
void finalize_patterns(std::vector<HardwarePattern>& patterns)
{
    for (auto& pattern : patterns) {
        if (pattern.class_ids.empty()) {
            pattern.class_ids.push_back("*");
        }
        if (pattern.vendor_ids.empty()) {
            pattern.vendor_ids.push_back("*");
        }
        if (pattern.device_ids.empty()) {
            pattern.device_ids.push_back("*");
        }
    }
}

} // namespace

std::expected<Config, ParseError> Config::from_file(const std::filesystem::path& path, BusType type)
{
    if (!std::filesystem::exists(path)) {
        return std::unexpected(ParseError{"File does not exist", path});
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return std::unexpected(ParseError{"Cannot open file", path});
    }

    Config config;
    config.bus_type_ = type;
    config.config_path_ = path;
    config.base_path_ = path.parent_path();

    if (config.patterns_.empty()) {
        config.patterns_.push_back(HardwarePattern{});
    }

    using KeyHandler = std::function<void(Config&, const std::string&)>;
    static const std::unordered_map<std::string, KeyHandler> key_handlers = {
        {"name", CFG_HANDLER {
            cfg.metadata_.name = to_lower(val);
        }},
        {"version", CFG_HANDLER {
            cfg.metadata_.version = val;
        }},
        {"info", CFG_HANDLER {
            cfg.metadata_.info = val;
        }},
        {"priority", CFG_HANDLER {
            cfg.metadata_.priority = std::stoi(val);
        }},
        {"freedriver", CFG_HANDLER {
            cfg.metadata_.free_driver = (to_lower(val) == "true");
        }},
        {"classids", CFG_HANDLER {
            if (!cfg.patterns_.back().class_ids.empty()) {
                cfg.patterns_.push_back(HardwarePattern{});
            }
            cfg.patterns_.back().class_ids = split_values(val);
        }},
        {"vendorids", CFG_HANDLER {
            if (!cfg.patterns_.back().vendor_ids.empty()) {
                cfg.patterns_.push_back(HardwarePattern{});
            }
            cfg.patterns_.back().vendor_ids = split_values(val);
        }},
        {"deviceids", CFG_HANDLER {
            if (!cfg.patterns_.back().device_ids.empty()) {
                cfg.patterns_.push_back(HardwarePattern{});
            }
            cfg.patterns_.back().device_ids = split_values(val);
        }},
        {"blacklistedclassids", CFG_HANDLER {
            cfg.patterns_.back().blacklisted_class_ids = split_values(val);
        }},
        {"blacklistedvendorids", CFG_HANDLER {
            cfg.patterns_.back().blacklisted_vendor_ids = split_values(val);
        }},
        {"blacklisteddeviceids", CFG_HANDLER {
            cfg.patterns_.back().blacklisted_device_ids = split_values(val);
        }},
        {"mhwddepends", CFG_HANDLER {
            cfg.dependencies_ = split_values(val);
        }},
        {"mhwdconflicts", CFG_HANDLER {
            cfg.conflicts_ = split_values(val);
        }}
    };

    std::string line;
    while (std::getline(file, line)) {
        auto processed = preprocess_line(std::move(line));
        if (!processed) {
            continue;
        }

        auto kv = parse_key_value(*processed, config.base_path_);
        if (!kv) {
            continue;
        }

        if (auto it = key_handlers.find(kv->first); it != key_handlers.end()) {
            it->second(config, kv->second);
        }
    }

    if (config.metadata_.name.empty()) {
        return std::unexpected(ParseError{"Config name is required", path});
    }

    finalize_patterns(config.patterns_);

    return config;
}

bool Config::matches_devices(const std::vector<Device>& devices) const
{
    return rg::all_of(patterns_, [&devices](const auto& pattern) {
        return rg::any_of(devices, [&pattern](const auto& device) {
            return device.matches(pattern);
        });
    });
}

bool Config::depends_on(const std::string& config_name) const
{
    return rg::contains(dependencies_, config_name);
}

bool Config::conflicts_with(const std::string& config_name) const
{
    return rg::contains(conflicts_, config_name);
}

} // namespace mcp::mhwd
