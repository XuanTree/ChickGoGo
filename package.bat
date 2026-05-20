@echo off
chcp 65001 >nul
title ChickGoGo 打包工具

echo ========================================
echo   ChickGoGo 一键打包脚本
echo ========================================
echo.

:: 设置路径（去掉尾部反斜杠）
set SOURCE_DIR=%~dp0
set SOURCE_DIR=%SOURCE_DIR:~0,-1%
set BUILD_DIR=%SOURCE_DIR%\out\build\mingw

:: ========================================
:: 步骤1: 配置 CMake
:: ========================================
echo [1/4] 配置 CMake (SFML 3.1.0, Release)...
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" ^
    -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER=D:/MinGW64/bin/gcc.exe ^
    -DCMAKE_CXX_COMPILER=D:/MinGW64/bin/g++.exe ^
    -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo [错误] CMake 配置失败!
    pause
    exit /b 1
)
echo [完成] CMake 配置成功
echo.

:: ========================================
:: 步骤2: 编译（自动生成可运行包）
:: ========================================
echo [2/4] 编译项目...
cmake --build "%BUILD_DIR%" -- -j
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 编译失败!
    pause
    exit /b 1
)
echo [完成] 编译成功
echo.

:: ========================================
:: 步骤3: 打包（可选，生成 ZIP/TGZ 用于分发）
:: ========================================
echo [3/4] 生成安装包（用于分发）...
cd /d "%BUILD_DIR%"

:: 查找 CMake 自动生成的可运行包目录
for /d %%i in (ChickGoGo-*-Windows) do set RUNNABLE_DIR=%%i
if not defined RUNNABLE_DIR (
    echo [错误] 未找到可运行包目录!
    pause
    exit /b 1
)

:: 删除旧的包
if exist "%RUNNABLE_DIR%.zip" del "%RUNNABLE_DIR%.zip"
if exist "%RUNNABLE_DIR%.tar.gz" del "%RUNNABLE_DIR%.tar.gz"

:: 打包 ZIP
cmake -E tar cf "%RUNNABLE_DIR%.zip" --format=zip "%RUNNABLE_DIR%"
if %ERRORLEVEL% NEQ 0 (
    echo [错误] ZIP 打包失败!
    pause
    exit /b 1
)
echo   [ZIP] %RUNNABLE_DIR%.zip

:: 打包 TGZ（跨平台兼容）
cmake -E tar cf "%RUNNABLE_DIR%.tar.gz" --format=gnutar "%RUNNABLE_DIR%"
if %ERRORLEVEL% NEQ 0 (
    echo [警告] TGZ 打包失败（非致命）
) else (
    echo   [TGZ] %RUNNABLE_DIR%.tar.gz
)

echo [完成] 安装包生成成功
echo.

:: ========================================
:: 显示结果
:: ========================================
echo ========================================
echo   打包完成!
echo   输出目录: %BUILD_DIR%
echo ========================================
echo.
echo 直接运行:
echo   %BUILD_DIR%\%RUNNABLE_DIR%\ChickGoGo.exe
echo.
echo 分发文件:
echo   %BUILD_DIR%\%RUNNABLE_DIR%.zip
echo   %BUILD_DIR%\%RUNNABLE_DIR%.tar.gz
echo.
echo 提示: 将 ZIP 解压后直接运行 ChickGoGo.exe 即可
echo       （所有 SFML 依赖已静态链接，无需额外安装）
echo.

cd /d "%SOURCE_DIR%"
pause
