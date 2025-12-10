/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * KernelProvider - async kernel package discovery using libpamac-cpp.
 */

#pragma once

#include "../Types.hpp"
#include "Kernel.hpp"

#include <pamac/database.hpp>

#include <functional>
#include <optional>
#include <string>

namespace mcp::kernel {

enum class KernelError {
    DatabaseNotInitialized,
    ParseError,
    NotFound
};

template<typename T>
using KernelResult = Result<T, KernelError>;

using ProgressCallback = std::function<void(int current, int total, const std::string& kernel_name)>;

/**
 * KernelProvider - discovers and provides kernel information.
 * 
 * Uses pamac::Database to query kernel packages and parses
 * version information from package names.
 * 
 * Usage:
 *   KernelProvider provider;
 *   auto kernels = co_await provider.get_kernels();
 *   if (kernels) {
 *       for (const auto& k : *kernels) { ... }
 *   }
 */
class KernelProvider {
public:
    KernelProvider();

    [[nodiscard]] Task<KernelResult<KernelVector>> get_kernels(ProgressCallback progress = nullptr) const;

    [[nodiscard]] Task<KernelResult<Kernel>> get_kernel(const std::string& package_name) const;

    [[nodiscard]] Task<KernelResult<Kernel>> get_running_kernel() const;

private:
    [[nodiscard]] static std::optional<Kernel> parse_kernel(const pamac::AlpmPackagePtr& pkg);

    [[nodiscard]] static std::optional<KernelVersion> parse_version(const std::string& name);

    [[nodiscard]] static KernelFlags detect_flags(const pamac::AlpmPackagePtr& pkg,
                                                   const std::string& running_version);

    [[nodiscard]] static std::string get_running_kernel_version();

    void populate_kernel_metadata(Kernel& kernel) const;

    [[nodiscard]] std::vector<std::string> get_extra_modules(const std::string& package_name) const;
};

} // namespace mcp::kernel
