cmake_minimum_required(VERSION 3.20)

project(softadastra_fs
    VERSION 0.1.0
    DESCRIPTION "Softadastra FS - Filesystem observation layer"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# ========================
# Options
# ========================
option(SOFTADASTRA_FS_BUILD_TESTS "Build fs tests" OFF)
option(SOFTADASTRA_FS_BUILD_EXAMPLES "Build fs examples" OFF)

# ========================
# Global settings
# ========================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ========================
# Dependencies
# ========================
if(NOT SOFTADASTRA_UMBRELLA_BUILD)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
        include("${CMAKE_CURRENT_SOURCE_DIR}/.vix/vix_deps.cmake")
    endif()
endif()

if(NOT TARGET softadastra::core AND NOT TARGET softadastra_core)
    find_package(softadastra_core CONFIG QUIET)
    find_package(core CONFIG QUIET)
endif()

set(SOFTADASTRA_FS_CORE_TARGET "")
if(TARGET softadastra::core)
    set(SOFTADASTRA_FS_CORE_TARGET softadastra::core)
elseif(TARGET softadastra_core)
    set(SOFTADASTRA_FS_CORE_TARGET softadastra_core)
endif()

if(SOFTADASTRA_FS_CORE_TARGET STREQUAL "")
    message(FATAL_ERROR
        "softadastra core target not found.\n"
        "Install dependencies with Vix:\n"
        "  vix add @softadastra/core\n"
        "  vix install"
    )
endif()

# ========================
# Sources (generic)
# ========================
file(GLOB_RECURSE SOFTADASTRA_FS_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ========================
# Platform sources
# ========================
set(SOFTADASTRA_FS_PLATFORM_SOURCES)

if(UNIX AND NOT APPLE)
    file(GLOB_RECURSE SOFTADASTRA_FS_LINUX_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/*.cpp"
    )
    list(APPEND SOFTADASTRA_FS_PLATFORM_SOURCES ${SOFTADASTRA_FS_LINUX_SOURCES})
endif()

if(APPLE)
    file(GLOB_RECURSE SOFTADASTRA_FS_MAC_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/mac/*.cpp"
    )
    list(APPEND SOFTADASTRA_FS_PLATFORM_SOURCES ${SOFTADASTRA_FS_MAC_SOURCES})
endif()

if(WIN32)
    file(GLOB_RECURSE SOFTADASTRA_FS_WINDOWS_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/platform/windows/*.cpp"
    )
    list(APPEND SOFTADASTRA_FS_PLATFORM_SOURCES ${SOFTADASTRA_FS_WINDOWS_SOURCES})
endif()

# ========================
# Library
# ========================
add_library(softadastra_fs
    ${SOFTADASTRA_FS_SOURCES}
    ${SOFTADASTRA_FS_PLATFORM_SOURCES}
)

add_library(softadastra::fs ALIAS softadastra_fs)

target_compile_features(softadastra_fs PUBLIC cxx_std_20)

target_link_libraries(softadastra_fs
    PUBLIC
        ${SOFTADASTRA_FS_CORE_TARGET}
)

# ========================
# Include directories
# ========================
target_include_directories(softadastra_fs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
# Platform definitions
# ========================
if(UNIX AND NOT APPLE)
    target_compile_definitions(softadastra_fs PRIVATE SOFTADASTRA_FS_LINUX)
elseif(APPLE)
    target_compile_definitions(softadastra_fs PRIVATE SOFTADASTRA_FS_MAC)
elseif(WIN32)
    target_compile_definitions(softadastra_fs PRIVATE SOFTADASTRA_FS_WINDOWS)
endif()

# ========================
# Target properties
# ========================
set_target_properties(softadastra_fs PROPERTIES
    OUTPUT_NAME softadastra_fs
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME fs
)

# ========================
# Examples
# ========================
if(SOFTADASTRA_FS_BUILD_EXAMPLES AND NOT SOFTADASTRA_UMBRELLA_BUILD)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
        add_subdirectory(examples)
    else()
        message(STATUS "[fs] examples/ missing CMakeLists.txt -> skipping")
    endif()
endif()

# ========================
# Tests
# ========================
if(SOFTADASTRA_FS_BUILD_TESTS)
    include(CTest)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        add_subdirectory(tests)
    else()
        message(STATUS "[fs] tests/ missing CMakeLists.txt -> skipping")
    endif()
endif()

# ========================
# Summary
# ========================
message(STATUS "------------------------------------------------------")
message(STATUS "softadastra::fs configured (${PROJECT_VERSION})")
message(STATUS "Core target:     ${SOFTADASTRA_FS_CORE_TARGET}")
message(STATUS "Include dir:     ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:     ${SOFTADASTRA_FS_BUILD_TESTS}")
message(STATUS "Build examples:  ${SOFTADASTRA_FS_BUILD_EXAMPLES}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
