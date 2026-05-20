#!/usr/bin/env bash
# ============================================================
# ChickGoGo Linux 一键打包脚本
# 依赖: cmake, g++/clang++, make, git, libx11-dev, libgl-dev
#
# Ubuntu/Debian 安装依赖:
#   sudo apt install cmake g++ make git libx11-dev libgl-dev
#
# Fedora/RHEL:
#   sudo dnf install cmake gcc-c++ make git libX11-devel mesa-libGL-devel
#
# Arch Linux:
#   sudo pacman -S cmake gcc make git libx11 mesa
# ============================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/out/build/Linux-Release"

echo "========================================"
echo "  ChickGoGo Linux 一键打包脚本"
echo "========================================"
echo ""

# --------------------------------------------------
# 步骤1: 配置 CMake
# --------------------------------------------------
echo "[1/4] 配置 CMake (SFML 3.1.0, Release)..."
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release
echo "[完成] CMake 配置成功"
echo ""

# --------------------------------------------------
# 步骤2: 编译（自动生成可运行包）
# --------------------------------------------------
echo "[2/4] 编译项目..."
cmake --build "${BUILD_DIR}" -j "$(nproc)"
echo "[完成] 编译成功"
echo ""

# --------------------------------------------------
# 步骤3: 完善可运行包（添加启动脚本、桌面文件等）
# --------------------------------------------------
echo "[3/4] 完善可运行包..."

# 查找 CMake 自动生成的可运行包目录
RUNNABLE_DIR=$(ls -d "${BUILD_DIR}/ChickGoGo-"*"-Linux" 2>/dev/null | head -1)
if [ -z "${RUNNABLE_DIR}" ]; then
    echo "[错误] 未找到可运行包目录!"
    exit 1
fi

PACKAGE_NAME=$(basename "${RUNNABLE_DIR}")

# 创建启动脚本（方便从终端启动）
cat > "${RUNNABLE_DIR}/run.sh" << 'RUNEOF'
#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "${SCRIPT_DIR}"
./ChickGoGo
RUNEOF
chmod +x "${RUNNABLE_DIR}/run.sh"

# 创建 .desktop 文件（方便在桌面环境中显示）
cat > "${RUNNABLE_DIR}/ChickGoGo.desktop" << DESKTOPFEOF
[Desktop Entry]
Name=ChickGoGo
Comment=A fun jumping game built with SFML 3
Exec=${RUNNABLE_DIR}/ChickGoGo
Path=${RUNNABLE_DIR}
Icon=${RUNNABLE_DIR}/assets/sprites/chick.png
Terminal=false
Type=Application
Categories=Game;
DESKTOPFEOF

# 创建安装脚本（方便安装到系统）
cat > "${RUNNABLE_DIR}/install.sh" << 'INSTALLEOF'
#!/usr/bin/env bash
# ============================================================
# ChickGoGo Linux 安装脚本
# 将游戏安装到 /opt/chickgogo 并为所有用户创建桌面入口
# ============================================================
set -e

INSTALL_DIR="/opt/chickgogo"
DESKTOP_FILE="/usr/share/applications/ChickGoGo.desktop"

echo "正在安装 ChickGoGo 到 ${INSTALL_DIR}..."

# 检查是否为 root
if [ "$EUID" -ne 0 ]; then
    echo "请使用 sudo 运行此安装脚本:"
    echo "  sudo ./install.sh"
    exit 1
fi

# 创建安装目录
mkdir -p "${INSTALL_DIR}"

# 复制所有文件
cp -r "$(dirname "$0")"/* "${INSTALL_DIR}/"
chmod +x "${INSTALL_DIR}/ChickGoGo"
chmod +x "${INSTALL_DIR}/run.sh"

# 安装桌面入口
cat > "${DESKTOP_FILE}" << EOF
[Desktop Entry]
Name=ChickGoGo
Comment=A fun jumping game built with SFML 3
Exec=${INSTALL_DIR}/ChickGoGo
Path=${INSTALL_DIR}
Icon=${INSTALL_DIR}/assets/sprites/chick.png
Terminal=false
Type=Application
Categories=Game;
EOF

# 更新桌面数据库
if command -v update-desktop-database &> /dev/null; then
    update-desktop-database /usr/share/applications/ &> /dev/null || true
fi

echo ""
echo "安装完成！"
echo "可以在应用菜单中找到 ChickGoGo，或直接运行:"
echo "  ${INSTALL_DIR}/ChickGoGo"
echo ""
echo "卸载方法:"
echo "  sudo rm -rf ${INSTALL_DIR} ${DESKTOP_FILE}"
INSTALLEOF
chmod +x "${RUNNABLE_DIR}/install.sh"

echo "[完成] 可运行包完善完成"
echo ""
echo "  ${RUNNABLE_DIR}/"
echo "  ├── ChickGoGo          (可执行文件)"
echo "  ├── run.sh             (启动脚本)"
echo "  ├── install.sh         (系统安装脚本)"
echo "  ├── ChickGoGo.desktop  (桌面快捷方式)"
echo "  ├── LICENSE"
echo "  └── assets/"
echo ""

# --------------------------------------------------
# 步骤4: 打包（可选，生成 tar.gz/zip 用于分发）
# --------------------------------------------------
echo "[4/4] 生成安装包（用于分发）..."

cd "${BUILD_DIR}"

# 生成 TGZ
rm -f "${PACKAGE_NAME}.tar.gz"
tar czf "${PACKAGE_NAME}.tar.gz" "${PACKAGE_NAME}"
echo "  [TGZ] ${PACKAGE_NAME}.tar.gz"

# 生成 ZIP（跨平台兼容）
rm -f "${PACKAGE_NAME}.zip"
zip -r "${PACKAGE_NAME}.zip" "${PACKAGE_NAME}" > /dev/null
echo "  [ZIP] ${PACKAGE_NAME}.zip"

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
echo "  ${RUNNABLE_DIR}/ChickGoGo"
echo ""
echo "分发文件:"
echo "  ${PACKAGE_NAME}.tar.gz  (通用)"
echo "  ${PACKAGE_NAME}.zip     (通用)"
echo ""
echo "注意: 所有 SFML 依赖已静态链接，无需额外安装"
echo "      如果遇到 libX11 错误，请安装:"
echo "        sudo apt install libx11-dev libgl-dev  # Debian/Ubuntu"
echo "        sudo dnf install libX11-devel mesa-libGL-devel  # Fedora"
echo ""

cd "${SCRIPT_DIR}"
