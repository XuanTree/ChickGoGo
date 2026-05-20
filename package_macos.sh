#!/usr/bin/env bash
# ============================================================
# ChickGoGo macOS 一键打包脚本
# 依赖: cmake, clang++, make, git, Xcode Command Line Tools
#
# 安装 Xcode Command Line Tools:
#   xcode-select --install
#
# 或者安装 Homebrew 依赖:
#   brew install cmake make
# ============================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/out/build/macOS-Release"

echo "========================================"
echo "  ChickGoGo macOS 一键打包脚本"
echo "========================================"
echo ""

# --------------------------------------------------
# 步骤1: 配置 CMake
# --------------------------------------------------
echo "[1/4] 配置 CMake (SFML 3.1.0, Release)..."
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
echo "[完成] CMake 配置成功"
echo ""

# --------------------------------------------------
# 步骤2: 编译（CMake 自动生成 .app 包）
# --------------------------------------------------
echo "[2/4] 编译项目..."
cmake --build "${BUILD_DIR}" -j "$(sysctl -n hw.logicalcpu)"
echo "[完成] 编译成功"
echo ""

# --------------------------------------------------
# 步骤3: 完善 .app 包（添加资源、图标等）
# --------------------------------------------------
echo "[3/4] 完善 .app 应用包..."

APP_DIR="${BUILD_DIR}/ChickGoGo.app"

# CMake 的 MACOSX_BUNDLE 已自动生成 .app 骨架，
# 但 assets 资源需要手动复制到 Resources 目录
echo "  [资源] 复制资源文件到 .app 包内..."
mkdir -p "${APP_DIR}/Contents/Resources/assets/fonts"
mkdir -p "${APP_DIR}/Contents/Resources/assets/music"
mkdir -p "${APP_DIR}/Contents/Resources/assets/sounds"
mkdir -p "${APP_DIR}/Contents/Resources/assets/sprites"

cp "${SCRIPT_DIR}/assets/fonts/"* "${APP_DIR}/Contents/Resources/assets/fonts/"
cp "${SCRIPT_DIR}/assets/music/"* "${APP_DIR}/Contents/Resources/assets/music/"
cp "${SCRIPT_DIR}/assets/sounds/"* "${APP_DIR}/Contents/Resources/assets/sounds/"
cp "${SCRIPT_DIR}/assets/sprites/"* "${APP_DIR}/Contents/Resources/assets/sprites/"

# 复制许可证
cp "${SCRIPT_DIR}/LICENSE" "${APP_DIR}/Contents/Resources/"

# 创建应用图标（从 assets/sprites/chick.png 生成 .icns）
ICONS_DIR="${APP_DIR}/Contents/Resources"
ICONSET="${ICONS_DIR}/ChickGoGo.iconset"
if command -v iconutil &> /dev/null && command -v sips &> /dev/null; then
    echo "  [图标] 从 chick.png 生成应用图标..."
    mkdir -p "${ICONSET}"
    sips -z 16 16 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_16x16.png" &> /dev/null || true
    sips -z 32 32 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_16x16@2x.png" &> /dev/null || true
    sips -z 32 32 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_32x32.png" &> /dev/null || true
    sips -z 64 64 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_32x32@2x.png" &> /dev/null || true
    sips -z 128 128 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_128x128.png" &> /dev/null || true
    sips -z 256 256 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_128x128@2x.png" &> /dev/null || true
    sips -z 256 256 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_256x256.png" &> /dev/null || true
    sips -z 512 512 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_256x256@2x.png" &> /dev/null || true
    sips -z 512 512 "${SCRIPT_DIR}/assets/sprites/chick.png" --out "${ICONSET}/icon_512x512.png" &> /dev/null || true
    iconutil -c icns "${ICONSET}" -o "${ICONS_DIR}/ChickGoGo.icns" &> /dev/null || true
    rm -rf "${ICONSET}"
    if [ -f "${ICONS_DIR}/ChickGoGo.icns" ]; then
        echo "  [图标] 图标生成成功"
    fi
else
    echo "  [图标] 跳过图标生成（需要 iconutil + sips，请安装 Xcode Command Line Tools）"
    echo "         应用将使用 macOS 默认图标"
fi

echo "[完成] .app 应用包构建完成"
echo ""
echo "  ${APP_DIR}"
echo "  双击 ChickGoGo.app 即可运行（无需解压）"
echo ""

# --------------------------------------------------
# 步骤4: 打包（可选，生成 DMG/zip/tgz 用于分发）
# --------------------------------------------------
echo "[4/4] 生成安装包（用于分发）..."

PACKAGE_NAME="ChickGoGo-1.0.0-macOS"
STAGE_DIR="${BUILD_DIR}/${PACKAGE_NAME}"

# 创建 DMG 构建目录
DMG_STAGE="${BUILD_DIR}/dmg_stage"
rm -rf "${DMG_STAGE}"
mkdir -p "${DMG_STAGE}"

# 将 .app 复制到 DMG 暂存目录，并创建 Applications 别名
cp -R "${APP_DIR}" "${DMG_STAGE}/"
ln -s /Applications "${DMG_STAGE}/Applications"

cd "${BUILD_DIR}"

# 生成 .tar.gz（通用）
rm -f "${PACKAGE_NAME}.tar.gz"
tar czf "${PACKAGE_NAME}.tar.gz" "${PACKAGE_NAME}" 2>/dev/null || true
echo "  [TGZ] ${PACKAGE_NAME}.tar.gz"

# 生成 .zip（通用）
rm -f "${PACKAGE_NAME}.zip"
zip -r "${PACKAGE_NAME}.zip" "${PACKAGE_NAME}" > /dev/null 2>&1 || true
echo "  [ZIP] ${PACKAGE_NAME}.zip"

# 生成 .dmg（macOS 原生安装包）
DMG_NAME="${PACKAGE_NAME}.dmg"
rm -f "${DMG_NAME}"

if command -v hdiutil &> /dev/null; then
    hdiutil create -volname "ChickGoGo" \
        -srcfolder "${DMG_STAGE}" \
        -ov -format UDZO \
        "${DMG_NAME}" 2>/dev/null || true
    echo "  [DMG] ${DMG_NAME}"
else
    echo "  [跳过] hdiutil 不可用，跳过 DMG 生成"
fi

# 清理暂存目录
rm -rf "${STAGE_DIR}" 2>/dev/null || true
rm -rf "${DMG_STAGE}" 2>/dev/null || true

echo "[完成] 安装包生成成功"
echo ""

# --------------------------------------------------
# 显示结果
# --------------------------------------------------
echo "========================================"
echo "  打包完成!"
echo "  输出目录: ${BUILD_DIR}"
echo "========================================"
echo ""
echo "直接运行:"
echo "  ${APP_DIR}"
echo "  双击 ChickGoGo.app 即可运行"
echo ""
echo "分发文件:"
echo "  ${PACKAGE_NAME}.tar.gz  (通用)"
echo "  ${PACKAGE_NAME}.zip     (通用)"
if [ -f "${DMG_NAME}" ]; then
    echo "  ${DMG_NAME}            (macOS DMG)"
fi
echo ""
echo "注意: 所有 SFML 依赖已静态链接，无需额外安装"
echo "      最低系统版本: macOS 13.0 (Ventura)"
echo ""

cd "${SCRIPT_DIR}"
