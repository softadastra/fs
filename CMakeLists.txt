cmake_minimum_required(VERSION 3.20)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(softadastra_fs
        VERSION 0.1.0
        DESCRIPTION "Softadastra FS - Filesystem observation layer"
        LANGUAGES CXX
    )
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ========================
# Standalone fallback options
# ========================
if(NOT DEFINED SOFTADASTRA_WARNINGS_AS_ERRORS)
    option(SOFTADASTRA_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
endif()

if(SOFTADASTRA_UMBRELLA_BUILD)
    set(SOFTADASTRA_FS_BUILD_TESTS ${SOFTADASTRA_BUILD_TESTS})
else()
    option(SOFTADASTRA_FS_BUILD_TESTS "Build fs tests" OFF)
endif()

option(SOFTADASTRA_FS_BUILD_EXAMPLES "Build fs examples" OFF)

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

# macOS only
if(APPLE)
    target_link_libraries(softadastra_fs
        PRIVATE
            "-framework CoreServices"
    )
endif()

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
    if(SOFTADASTRA_WARNINGS_AS_ERRORS)
        target_compile_options(softadastra_fs PRIVATE /WX)
    endif()
else()
    target_compile_options(softadastra_fs PRIVATE -Wall -Wextra -Wpedantic)
    if(SOFTADASTRA_WARNINGS_AS_ERRORS)
        target_compile_options(softadastra_fs PRIVATE -Werror)
    endif()
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
    VERSION 0.1.0
    SOVERSION 0
    EXPORT_NAME fs
)

# ========================
# Install
# ========================
install(
    TARGETS softadastra_fs
    EXPORT softadastraFsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT softadastraFsTargets
    FILE softadastraFsTargets.cmake
    NAMESPACE softadastra::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/softadastra_fs"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/softadastra_fsConfigVersion.cmake"
    VERSION 0.1.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/softadastra_fsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/softadastra_fsConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/softadastra_fs"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/softadastra_fsConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/softadastra_fsConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/softadastra_fs"
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
message(STATUS "softadastra::fs configured (0.1.0)")
message(STATUS "Core target:     ${SOFTADASTRA_FS_CORE_TARGET}")
message(STATUS "Include dir:     ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests:     ${SOFTADASTRA_FS_BUILD_TESTS}")
message(STATUS "Build examples:  ${SOFTADASTRA_FS_BUILD_EXAMPLES}")
message(STATUS "Warnings=error:  ${SOFTADASTRA_WARNINGS_AS_ERRORS}")
message(STATUS "Build type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix:  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "------------------------------------------------------")
