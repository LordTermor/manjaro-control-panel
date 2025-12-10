/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Common type aliases for libmcp.
 */

#pragma once

#include <coro/io_scheduler.hpp>
#include <coro/task.hpp>

#include <expected>
#include <memory>

namespace mcp {

template<typename T>
using Task = coro::task<T>;

template<typename T, typename E>
using Result = std::expected<T, E>;

template<typename T, typename E>
using ResultTask = Task<Result<T, E>>;

/**
 * Global I/O scheduler for async operations.
 * Used by providers for expensive I/O operations with thread pool backing.
 */
inline coro::io_scheduler& io_scheduler()
{
    static auto scheduler = coro::io_scheduler::make_unique(
        coro::io_scheduler::options{
            .pool = coro::thread_pool::options{.thread_count = std::thread::hardware_concurrency()},
            .execution_strategy = coro::io_scheduler::execution_strategy_t::process_tasks_on_thread_pool
        }
    );
    return *scheduler;
}

} // namespace mcp
