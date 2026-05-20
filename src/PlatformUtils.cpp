#include "PlatformUtils.hpp"
#include <iostream>

// ============================================================
// 跨平台：获取可执行文件所在目录
// ============================================================
#if defined(_WIN32)
#include <windows.h>
static fs::path getExeDirectory() {
    wchar_t path[MAX_PATH];
    if (GetModuleFileNameW(nullptr, path, MAX_PATH) == 0) {
        std::cerr << "[PlatformUtils] Failed to get exe path" << std::endl;
        return fs::current_path();
    }
    return fs::path(path).parent_path();
}
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
static fs::path getExeDirectory() {
    char path[4096];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) != 0) {
        std::cerr << "[PlatformUtils] Failed to get exe path" << std::endl;
        return fs::current_path();
    }
    return fs::path(path).parent_path();
}
#else // Linux
#include <unistd.h>
#include <limits.h>
static fs::path getExeDirectory() {
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len == -1) {
        std::cerr << "[PlatformUtils] Failed to get exe path" << std::endl;
        return fs::current_path();
    }
    path[len] = '\0';
    return fs::path(path).parent_path();
}
#endif

// ============================================================
// 跨平台：切换工作目录
// ============================================================
#if defined(_WIN32)
static bool changeDirectory(const fs::path& dir) {
    return SetCurrentDirectoryW(dir.c_str()) != 0;
}
#else
#include <unistd.h>
static bool changeDirectory(const fs::path& dir) {
    return chdir(dir.c_str()) == 0;
}
#endif

// ============================================================
// 查找包含 assets/ 目录的根目录
// 策略：从 exe 所在目录开始，逐级向上查找，直到找到 assets/ 为止
// ============================================================
bool findAndSetAssetsDirectory() {
    fs::path exeDir = getExeDirectory();

    // ---- macOS .app 包特殊处理 ----
    // .app 包结构:
    //   ChickGoGo.app/Contents/MacOS/ChickGoGo  (exe)
    //   ChickGoGo.app/Contents/Resources/assets/ (资源)
    // 所以需要额外检查 ../Resources/assets/ 相对于 exe 目录
#if defined(__APPLE__)
    {
        fs::path bundleAssets = exeDir / "../Resources/assets";
        if (fs::exists(bundleAssets) && fs::is_directory(bundleAssets)) {
            // 切换到 .app 包的 Resources 目录的父目录（即 .app 包所在目录）
            // 这样 assets/ 路径就能正确解析
            fs::path bundleRoot = exeDir / "../Resources";
            changeDirectory(fs::weakly_canonical(bundleRoot));
            return true;
        }
    }
#endif

    // ---- 通用策略：逐级向上查找 ----
    fs::path searchDir = exeDir;
    while (true) {
        fs::path assetsPath = searchDir / "assets";
        if (fs::exists(assetsPath) && fs::is_directory(assetsPath)) {
            // 找到 assets/，切换到该目录
            changeDirectory(searchDir);
            return true;
        }
        // 向父目录查找
        fs::path parent = searchDir.parent_path();
        if (parent == searchDir) {
            // 已到根目录，未找到
            std::cerr << "[PlatformUtils] Could not find assets/ directory from: "
                      << fs::absolute(exeDir) << std::endl;
            // 回退到 exe 目录（ZIP 包场景）
            changeDirectory(exeDir);
            return false;
        }
        searchDir = parent;
    }
}
