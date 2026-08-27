#主配置文件，负责整个项目的构建配置
cmake_minimum_required(VERSION 3.16)

# 是否作为顶层项目独立构建；被宿主工程(如 lyqtRpc)作为子目录引入时只构建 lczmuduo 静态库，
# 不重复 project()/全局 C++ 标准/构建类型/输出目录，避免覆盖宿主的这些全局配置。
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(LCZMUDUO_TOP_LEVEL ON)
else()
    set(LCZMUDUO_TOP_LEVEL OFF)
endif()

if(LCZMUDUO_TOP_LEVEL)
    project(lczmuduo)

    # 构建选项
    option(BUILD_APPS "Build example and server applications" ON)
    option(BUILD_TESTS "Build unit tests (requires network for GTest download)" OFF)

    # C++ 标准
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # 位置无关代码（静态库需要）
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # 生成 compile_commands.json（IDE/LSP 需要）
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # 编译警告（仅 GCC/Clang）
    add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall> $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>)

    # 构建类型默认值
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    endif()
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fno-omit-frame-pointer")

    # 输出目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()

# 线程库
find_package(Threads REQUIRED)

# 加载子目录（静态库 lczmuduo）
add_subdirectory(src)

# 示例/测试/安装只在独立构建时进行
if(LCZMUDUO_TOP_LEVEL)
    if(BUILD_APPS)
        add_subdirectory(example)
        add_subdirectory(net)
    endif()
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()

    # 安装头文件（同时匹配 .hpp 和 .h）
    install(DIRECTORY include/
        DESTINATION include/lczmuduo
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
    )

    # 安装路径前缀
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Install path prefix" FORCE)
    endif()
endif()
