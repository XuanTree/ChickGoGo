cmake_minimum_required(VERSION 3.14)
project(ChickGoGo
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "ChickGoGo - A fun jumping game built with SFML 3"
    HOMEPAGE_URL "https://github.com/xuantree/ChickGoGo"
)

# ============================================================
# C++ Standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# C++ Standard
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# SFML 3 - FetchContent 自动拉取
# ============================================================
set(SFML_BUILD_GRAPHICS ON)
set(SFML_BUILD_WINDOW ON)
set(SFML_BUILD_SYSTEM ON)
set(SFML_BUILD_AUDIO ON)
set(SFML_BUILD_NETWORK OFF)

# 静态链接 SFML（方便分发，无需带 DLL）
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sfml)

# ============================================================
# 源文件
# ============================================================
set(HEADERS
    include/Entity.hpp
    include/Game.hpp
    include/PlatformUtils.hpp
    include/ResourceManager.hpp
    include/string.hpp
    include/States/State.hpp
    include/States/GameState.hpp
    include/States/MenuState.hpp
    include/States/PauseState.hpp
)

set(SOURCES
    main.cpp
    src/Game.cpp
    src/PlatformUtils.cpp
    src/ResourceManager.cpp
    src/string.cpp
    src/Entities/Chick.cpp
    src/Entities/Obstacle.cpp
    src/States/GameState.cpp
    src/States/MenuState.cpp
    src/States/PauseState.cpp
)

# ============================================================
# 可执行目标
# ============================================================
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

# ============================================================
# 平台特定属性
# ============================================================

# Windows GUI 应用程序（无控制台黑框）
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# macOS .app bundle（双击直接运行，无终端）
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.xuantree.ChickGoGo"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/assets/Info.plist"
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# MinGW 静态链接运行时库（Windows 分发无需带 DLL）
if(MINGW)
    # 完全静态链接 MinGW 运行时库（libstdc++、libgcc、libwinpthread 等）
    # 这样 exe 可以直接在未安装 MSYS2/MinGW 的 Windows 上运行
    # 注意：Windows 系统库（opengl32, gdi32, winmm 等）不受影响，
    # 它们链接的是系统 DLL 的导入库（.a），-static 不会改变它们
    target_link_options(${PROJECT_NAME} PRIVATE
        -static
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Windows 需要 sfml-main 来提供 WinMain 入口点
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        sfml-main
    )
endif()

# 平台特定依赖
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        winmm
        opengl32
        gdi32
    )
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        pthread
        X11
        rt
        dl
    )
endif()

# SFML 静态链接宏
target_compile_definitions(${PROJECT_NAME} PRIVATE SFML_STATIC)

# ============================================================
# 构建后：自动生成可运行包（exe + assets 放在一起）
# ============================================================

# 可运行包的输出目录
set(RUNNABLE_DIR "${CMAKE_BINARY_DIR}/ChickGoGo-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")

if(WIN32)
    # Windows：复制 exe + assets 到可运行目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "  正在生成可运行包..."
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${RUNNABLE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/fonts"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/music"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/sounds"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/sprites"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/ChickGoGo.exe" "${RUNNABLE_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/fonts/unifont.otf" "${RUNNABLE_DIR}/assets/fonts/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/music/bgm.mp3" "${RUNNABLE_DIR}/assets/music/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sounds/hit.wav" "${RUNNABLE_DIR}/assets/sounds/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sounds/jump.wav" "${RUNNABLE_DIR}/assets/sounds/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sprites/chick.png" "${RUNNABLE_DIR}/assets/sprites/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sprites/icon.png" "${RUNNABLE_DIR}/assets/sprites/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sprites/spike.png" "${RUNNABLE_DIR}/assets/sprites/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE" "${RUNNABLE_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "  Windows 可运行包已生成:"
        COMMAND ${CMAKE_COMMAND} -E echo "    ${RUNNABLE_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E echo "    ├── ChickGoGo.exe"
        COMMAND ${CMAKE_COMMAND} -E echo "    ├── assets/"
        COMMAND ${CMAKE_COMMAND} -E echo "    └── LICENSE"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "  直接运行 ${RUNNABLE_DIR}/ChickGoGo.exe 即可"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""
    )
elseif(APPLE)
    # macOS：构建后提示 .app 位置（CMake 的 MACOSX_BUNDLE 已自动生成）
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "  macOS .app 包已生成:"
        COMMAND ${CMAKE_COMMAND} -E echo "    ${CMAKE_BINARY_DIR}/ChickGoGo.app"
        COMMAND ${CMAKE_COMMAND} -E echo "  双击 ChickGoGo.app 即可运行"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""
    )
elseif(UNIX)
    # Linux：复制可执行文件 + assets 到可运行目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "  正在生成可运行包..."
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${RUNNABLE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/fonts"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/music"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/sounds"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RUNNABLE_DIR}/assets/sprites"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/ChickGoGo" "${RUNNABLE_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/fonts/unifont.otf" "${RUNNABLE_DIR}/assets/fonts/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/music/bgm.mp3" "${RUNNABLE_DIR}/assets/music/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sounds/hit.wav" "${RUNNABLE_DIR}/assets/sounds/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sounds/jump.wav" "${RUNNABLE_DIR}/assets/sounds/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sprites/chick.png" "${RUNNABLE_DIR}/assets/sprites/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sprites/icon.png" "${RUNNABLE_DIR}/assets/sprites/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/assets/sprites/spike.png" "${RUNNABLE_DIR}/assets/sprites/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE" "${RUNNABLE_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "  Linux 可运行包已生成:"
        COMMAND ${CMAKE_COMMAND} -E echo "    ${RUNNABLE_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E echo "    ├── ChickGoGo"
        COMMAND ${CMAKE_COMMAND} -E echo "    ├── assets/"
        COMMAND ${CMAKE_COMMAND} -E echo "    └── LICENSE"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "  直接运行 ${RUNNABLE_DIR}/ChickGoGo 即可"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""
    )
endif()

# ============================================================
# CPack 打包配置
# ============================================================
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "XuanTree")
set(CPACK_PACKAGE_CONTACT "your.email@example.com")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# 使用 MONOLITHIC 模式打包所有安装的文件
set(CPACK_MONOLITHIC_INSTALL ON)

# --- 通用：生成存档包（ZIP/TGZ） ---
set(CPACK_GENERATOR "ZIP;TGZ")

# --- Windows 专用：NSIS 安装包 ---
if(WIN32)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "ChickGoGo")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_MENU_LINKS "." "ChickGoGo")
endif()

# --- Linux 专用：DEB/RPM ---
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "${CPACK_GENERATOR};DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
endif()

# --- macOS 专用：DMG ---
if(APPLE)
    set(CPACK_GENERATOR "${CPACK_GENERATOR};DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "ChickGoGo")
    set(CPACK_DMG_FORMAT "UDBZ")
endif()

include(CPack)
