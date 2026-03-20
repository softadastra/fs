cmake_minimum_required(VERSION 3.20)

project(softadastra_fs
    VERSION 0.1.0
    DESCRIPTION "Softadastra FS - Filesystem observation layer"
    LANGUAGES CXX
)

# ========================
# Options
# ========================
option(SOFTADASTRA_FS_BUILD_TESTS "Build fs tests" ON)
option(SOFTADASTRA_FS_BUILD_EXAMPLES "Build fs examples" ON)

# ========================
# Library
# ========================
add_library(softadastra_fs)

# C++ standard
target_compile_features(softadastra_fs PUBLIC cxx_std_20)

# ========================
# Dependencies
# ========================
if(NOT TARGET softadastra_core)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core build_core)
endif()

target_link_libraries(softadastra_fs
    PUBLIC
        softadastra_core
)

# ========================
# Include directories
# ========================
target_include_directories(softadastra_fs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ========================
# Sources (generic)
# ========================
file(GLOB_RECURSE FS_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# ========================
# Platform sources
# ========================
set(PLATFORM_SOURCES)

if(UNIX AND NOT APPLE)
    file(GLOB_RECURSE LINUX_SOURCES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/*.cpp
    )
    list(APPEND PLATFORM_SOURCES ${LINUX_SOURCES})
endif()

if(APPLE)
    file(GLOB_RECURSE MAC_SOURCES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/mac/*.cpp
    )
    list(APPEND PLATFORM_SOURCES ${MAC_SOURCES})
endif()

if(WIN32)
    file(GLOB_RECURSE WINDOWS_SOURCES
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/windows/*.cpp
    )
    list(APPEND PLATFORM_SOURCES ${WINDOWS_SOURCES})
endif()

# ========================
# Attach sources
# ========================
target_sources(softadastra_fs
    PRIVATE
        ${FS_SOURCES}
        ${PLATFORM_SOURCES}
)

# ========================
# Compiler options
# ========================
if(MSVC)
    target_compile_options(softadastra_fs PRIVATE /W4 /permissive-)
else()
    target_compile_options(softadastra_fs PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ========================
# Linux specific (inotify)
# ========================
if(UNIX AND NOT APPLE)
    target_compile_definitions(softadastra_fs PRIVATE SOFTADASTRA_FS_LINUX)
endif()

# ========================
# Position Independent Code
# ========================
set_target_properties(softadastra_fs PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# ========================
# Namespacing
# ========================
add_library(softadastra::fs ALIAS softadastra_fs)

# ========================
# Examples
# ========================
if(SOFTADASTRA_FS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ========================
# Tests
# ========================
if(SOFTADASTRA_FS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
