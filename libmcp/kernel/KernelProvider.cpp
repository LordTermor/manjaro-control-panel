/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-FileCopyrightText: 2025 Daniil Ludwig <eightbyte81@gmail.com>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "KernelProvider.hpp"

#include <coro/sync_wait.hpp>
#include <coro/task.hpp>

#include <algorithm>
#include <array>
#include <regex>

#ifdef __linux__
#include <sys/utsname.h>
#endif

namespace mcp::kernel {

namespace {

constexpr std::string_view c_kernel_glob = "linux[0-9]*";

constexpr std::array c_official_repos = {
    std::string_view{"core"},
    std::string_view{"extra"},
    std::string_view{"multilib"},
};

bool is_official_repo(const std::string& repo)
{
    return std::ranges::find(c_official_repos, repo) != c_official_repos.end();
}

bool is_lts_version(int major, int minor)
{
    static const std::vector<std::pair<int, int>> lts_versions = {
        {4, 14}, {4, 19},
        {5, 4}, {5, 10}, {5, 15},
        {6, 1}, {6, 6}, {6, 12},
        {6,18}
    };
    
    return std::ranges::find(lts_versions, std::make_pair(major, minor)) != lts_versions.end();
}

bool is_rt_kernel(const std::string& name)
{
    return name.contains("-rt");
}

bool is_experimental_kernel(const std::string& name)
{
    return name.contains("-rc") || name.contains("-git");
}

} // namespace

KernelProvider::KernelProvider()
{
    pamac::Database::initialize("/etc/pamac.conf");
    pamac::Database::instance().value().get();    
}

std::string KernelProvider::get_running_kernel_version()
{
#ifdef __linux__
    struct utsname buf;
    if (uname(&buf) == 0) {
        return std::string(buf.release);
    }
#endif
    return {};
}

std::optional<KernelVersion> KernelProvider::parse_version(const std::string& name)
{
    // Pattern: linux[MAJOR][MINOR] or linux-VERSION
    // Examples: linux66, linux610, linux-lts, linux515-rt

    static const std::regex version_regex(R"(linux(\d)(\d+)(?:-.*)?$)");

    std::smatch match;
    if (std::regex_match(name, match, version_regex)) {
        KernelVersion version;

        version.major = std::stoi(match[1].str());
        version.minor = std::stoi(match[2].str());

        return version;
    }

// Handle special kernels like linux-lts, linux-zen
if (name.starts_with("linux-") || name == "linux") {
        // These don't have encoded version, need to get from package version
        return KernelVersion{};
    }

    return std::nullopt;
}

KernelFlags KernelProvider::detect_flags(const pamac::AlpmPackagePtr& pkg,
                                          const std::string& running_version)
{
    KernelFlags flags{};

    const auto& name = pkg->name();

    // LTS detection is done via version, not name
    flags.real_time = is_rt_kernel(name);
    flags.experimental = is_experimental_kernel(name);
    flags.installed = pkg->is_installed();

    if (!running_version.empty() && flags.installed) {
        auto installed_ver = pkg->installed_version();
        if (installed_ver && running_version.contains(*installed_ver)) {
            flags.in_use = true;
        }
    }

    flags.recommended = false;

    return flags;
}

std::optional<Kernel> KernelProvider::parse_kernel(const pamac::AlpmPackagePtr& pkg)
{
    const auto& name = pkg->name();

    if (name.contains("-headers") || name.contains("-docs") ||
        name.contains("-api-headers")) {
        return std::nullopt;
    }

    // Filter out driver packages: allow no dashes or only one dash if ends with -rt
    // Valid: linux66, linux610, linux515-rt
    // Invalid: linux66-nvidia, linux66-zfs, linux515-rt-nvidia
    auto dash_count = std::ranges::count(name, '-');
    if (dash_count > 1 || (dash_count == 1 && !name.ends_with("-rt"))) {
        return std::nullopt;
    }

    auto version_opt = parse_version(name);
    if (!version_opt) {
        return std::nullopt;
    }

    auto running = get_running_kernel_version();

    Kernel kernel;
    kernel.package_name = name;
    kernel.version = *version_opt;
    kernel.flags = detect_flags(pkg, running);

    if (auto repo = pkg->repo()) {
        kernel.repo = *repo;
        kernel.flags.not_supported = !is_official_repo(kernel.repo);
    } else {
        kernel.flags.not_supported = true;
    }

    if (auto installed = pkg->installed_version()) {
        kernel.installed_version = *installed;
    }

    kernel.available_version = pkg->version();

    // Extract patch version from installed version if available, otherwise from available version
    // Format: MAJOR.MINOR.PATCH-REL (e.g., "6.6.10-1")
    static const std::regex patch_regex(R"(\d+\.\d+\.(\d+)(?:-.*)?$)");
    std::smatch match;
    
    const std::string& version_to_parse = kernel.installed_version.empty() 
        ? kernel.available_version 
        : kernel.installed_version;
    
    if (std::regex_search(version_to_parse, match, patch_regex)) {
        kernel.version.patch = match[1].str();
    }

    // If version wasn't parsed from name, try from package version
    if (kernel.version.major == 0) {
        static const std::regex ver_regex(R"((\d+)\.(\d+)\.(\d+))");
        std::smatch match;
        if (std::regex_search(kernel.available_version, match, ver_regex)) {
            kernel.version.major = std::stoi(match[1].str());
            kernel.version.minor = std::stoi(match[2].str());
            kernel.version.patch = match[3].str();
        }
    }

    return kernel;
}

void KernelProvider::populate_kernel_metadata(Kernel& kernel) const
{
    kernel.extra_modules = get_extra_modules(kernel.package_name);
    
    kernel.changelog_url = "https://kernelnewbies.org/Linux_" + 
                           std::to_string(kernel.version.major) + "." + 
                           std::to_string(kernel.version.minor);
}

KernelResult<std::vector<Kernel>> KernelProvider::get_kernels(ProgressCallback progress) const
{
    auto packages = pamac::Database::instance().value().get().get_sync_pkgs_by_glob(std::string(c_kernel_glob));

    std::vector<Kernel> kernels;
    kernels.reserve(packages.size());

    for (const auto& pkg : packages) {
        if (auto kernel = parse_kernel(pkg)) {
            kernels.push_back(std::move(*kernel));
        }
    }

    // Sort by version (newest first)
    std::ranges::sort(kernels, std::greater{});

    // Set LTS flags based on version and find latest non-in-use LTS
    Kernel* latest_lts = nullptr;
    for (auto& kernel : kernels) {
        kernel.flags.lts = is_lts_version(kernel.version.major, kernel.version.minor);
        
        // Track latest LTS that's not real-time
        if (kernel.flags.lts && !kernel.flags.real_time) {
            if (!latest_lts || kernel.version > latest_lts->version) {
                latest_lts = &kernel;
            }
        }
    }

    // Set recommended to latest LTS
    if (latest_lts) {
        latest_lts->flags.recommended = true;
    }

    // Populate metadata with progress reporting
    int current = 0;
    int total = static_cast<int>(kernels.size());
    
    for (auto& kernel : kernels) {
        if (progress) {
            progress(current, total, kernel.package_name);
        }
        populate_kernel_metadata(kernel);
        ++current;
    }
    
    if (progress) {
        progress(total, total, "");
    }

    return kernels;
}

coro::task<KernelResult<std::vector<Kernel>>> KernelProvider::get_kernels_async(ProgressCallback progress) const
{
    auto packages = co_await pamac::Database::instance().value().get().search_pkgs_async("linux");

    std::vector<Kernel> kernels;

    for (const auto& pkg : packages) {
        const auto& name = pkg->name();
        if (!name.starts_with("linux") || name.contains("-headers")) {
            continue;
        }

        if (auto kernel = parse_kernel(pkg)) {
            kernels.push_back(std::move(*kernel));
        }
    }

    // Sort by version (newest first)
    std::ranges::sort(kernels, std::greater{});

    // Set LTS flags based on version and find latest non-in-use LTS
    Kernel* latest_lts = nullptr;
    for (auto& kernel : kernels) {
        kernel.flags.lts = is_lts_version(kernel.version.major, kernel.version.minor);
        
        // Track latest LTS that's not real-time
        if (kernel.flags.lts && !kernel.flags.real_time) {
            if (!latest_lts || kernel.version > latest_lts->version) {
                latest_lts = &kernel;
            }
        }
    }

    // Set recommended to latest LTS
    if (latest_lts) {
        latest_lts->flags.recommended = true;
    }

    // Populate metadata with progress reporting
    int current = 0;
    int total = static_cast<int>(kernels.size());
    
    for (auto& kernel : kernels) {
        if (progress) {
            progress(current, total, kernel.package_name);
        }
        
        kernel.extra_modules = get_extra_modules(kernel.package_name);
        
        kernel.changelog_url = "https://kernelnewbies.org/Linux_" + 
                               std::to_string(kernel.version.major) + "." + 
                               std::to_string(kernel.version.minor);
        
        ++current;
    }
    
    if (progress) {
        progress(total, total, "");
    }

    co_return kernels;
}

coro::task<KernelResult<Kernel>> KernelProvider::get_kernel_async(const std::string& package_name) const
{
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        co_return std::unexpected(KernelError::DatabaseNotInitialized);
    }
    
    auto& db = db_result.value().get();
    auto pkg = db.get_pkg(package_name);

    if (!pkg) {
        co_return std::unexpected(KernelError::NotFound);
    }

    auto kernel = parse_kernel(pkg);
    if (!kernel) {
        co_return std::unexpected(KernelError::ParseError);
    }
    
    populate_kernel_metadata(*kernel);

    co_return *kernel;
}

KernelResult<Kernel> KernelProvider::get_kernel(const std::string& package_name) const
{
    auto pkg = pamac::Database::instance().value().get().get_pkg(package_name);

    if (!pkg) {
        return std::unexpected(KernelError::NotFound);
    }

    auto kernel = parse_kernel(pkg);
    if (!kernel) {
        return std::unexpected(KernelError::ParseError);
    }

    return *kernel;
}

coro::task<KernelResult<Kernel>> KernelProvider::get_running_kernel_async() const
{
    auto running = get_running_kernel_version();
    if (running.empty()) {
        co_return std::unexpected(KernelError::NotFound);
    }

    // Extract kernel package name from running version
    // Format: 6.6.10-1-MANJARO -> linux66
    static const std::regex ver_regex(R"((\d+)\.(\d+)\.)");
    std::smatch match;

    if (!std::regex_search(running, match, ver_regex)) {
        co_return std::unexpected(KernelError::ParseError);
    }

    std::string package_name = "linux" + match[1].str() + match[2].str();

    auto result = co_await get_kernel_async(package_name);
    if (result) {
        result->flags.in_use = true;
    }

    co_return result;
}

KernelResult<Kernel> KernelProvider::get_running_kernel() const
{
    auto running = get_running_kernel_version();
    if (running.empty()) {
        return std::unexpected(KernelError::NotFound);
    }

    // Extract kernel package name from running version
    // Format: 6.6.10-1-MANJARO -> linux66
    static const std::regex ver_regex(R"((\d+)\.(\d+)\.)");
    std::smatch match;

    if (!std::regex_search(running, match, ver_regex)) {
        return std::unexpected(KernelError::ParseError);
    }

    std::string package_name = "linux" + match[1].str() + match[2].str();

    auto result = get_kernel(package_name);
    if (result) {
        result->flags.in_use = true;
    }

    return result;
}

std::vector<std::string> KernelProvider::get_extra_modules(const std::string& package_name) const
{
    std::vector<std::string> modules;
    
    auto db_result = pamac::Database::instance();
    if (!db_result) {
        return modules;
    }
    
    auto& db = db_result.value().get();
    
    // Get currently installed extra modules on the running kernel
    // to determine what user actually uses
    auto running_kernel_result = get_running_kernel();
    std::vector<std::string> installed_module_types;
    
    if (running_kernel_result) {
        const auto& running_pkg = running_kernel_result->package_name;
        std::string glob_pattern = running_pkg + "-*";
        auto running_packages = db.get_installed_pkgs_by_glob(glob_pattern);
        
        for (const auto& pkg : running_packages) {
            const auto& name = pkg->name();
            
            if (name == running_pkg + "-headers" || 
                name.ends_with("-docs") || 
                name.ends_with("-api-headers")) {
                continue;
            }
            
            // Extract module type (e.g., "nvidia", "virtualbox", "zfs")
            // from "linux66-nvidia" -> "nvidia"
            auto module_start = running_pkg.length() + 1; // +1 for the dash
            if (module_start < name.length()) {
                installed_module_types.push_back(name.substr(module_start));
            }
        }
    }
    
    if (installed_module_types.empty()) {
        return modules;
    }
    
    std::string glob_pattern = package_name + "-*";
    auto packages = db.get_sync_pkgs_by_glob(glob_pattern);
    
    for (const auto& pkg : packages) {
        const auto& name = pkg->name();
        
        if (name == package_name || 
            name == package_name + "-headers" || 
            name.ends_with("-docs") || 
            name.ends_with("-api-headers")) {
            continue;
        }
        
        auto module_start = package_name.length() + 1;
        if (module_start >= name.length()) {
            continue;
        }
        
        std::string module_type = name.substr(module_start);
        
        // Only include if this module type is installed on running kernel
        if (std::ranges::find(installed_module_types, module_type) != installed_module_types.end()) {
            modules.push_back(name);
        }
    }
    
    return modules;
}

} // namespace mcp::kernel
