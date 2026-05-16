cmake_minimum_required(VERSION 3.10.0)
project(ChickGoGo VERSION 0.1.0 LANGUAGES C CXX)

# 添加SFML的CMake模块路径
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};D:/SFML64/lib/cmake/SFML")

# 生成 compile_commands.json 供 clangd 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(./include)
aux_source_directory(./src srcs)

add_executable(ChickGoGo main.cpp ${srcs}
	"src/string.cpp"
	"src/ResourceManager.cpp"
	)

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)
target_link_libraries(ChickGoGo PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
