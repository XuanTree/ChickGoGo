#ifndef PLATFORM_UTILS_HPP
#define PLATFORM_UTILS_HPP

#include <filesystem>
namespace fs = std::filesystem;

// 查找并切换到包含 assets/ 目录的根目录
// 返回 true 表示成功找到并切换，false 表示未找到（但仍会切换到 exe 所在目录兜底）
bool findAndSetAssetsDirectory();

#endif // !PLATFORM_UTILS_HPP
