/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * KernelProvider - async kernel package discovery using libpamac-cpp.
 * Provides both synchronous and coroutine-based interfaces.
 */

#pragma once

#include "Kernel.hpp"

#include <pamac/database.hpp>

#include <coro/task.hpp>

#include <expected>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace mcp::kernel {

enum class KernelError {
    DatabaseNotInitialized,
    ParseError,
    NotFound
};

template<typename T>
using KernelResult = std::expected<T, KernelError>;

using ProgressCallback = std::function<void(int current, int total, const std::string& kernel_name)>;

/**
 * KernelProvider - discovers and provides kernel information.
 * 
 * Uses pamac::Database to query kernel packages and parses
 * version information from package names.
 * 
 * Usage (sync):
 *   KernelProvider provider;
 *   auto kernels = provider.get_kernels();
 *   if (kernels) {
 *       for (const auto& k : *kernels) { ... }
 *   }
 * 
 * Usage (async/coroutine):
 *   auto task = provider.get_kernels_async();
 *   auto kernels = task.get();  // Pumps GLib main loop
 */
class KernelProvider {
public:
    KernelProvider();

    [[nodiscard]] KernelResult<std::vector<Kernel>> get_kernels(ProgressCallback progress = nullptr) const;

    [[nodiscard]] coro::task<KernelResult<std::vector<Kernel>>> get_kernels_async(ProgressCallback progress = nullptr) const;

    [[nodiscard]] KernelResult<Kernel> get_kernel(const std::string& package_name) const;

    [[nodiscard]] KernelResult<Kernel> get_running_kernel() const;

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
