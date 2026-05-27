cmake_minimum_required(VERSION 3.25)

project(CnCGenerals
    VERSION 1.0.0
    DESCRIPTION "Command & Conquer Generals / Zero Hour"
    LANGUAGES C CXX
)

# ── C++ Standard ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Platform Detection ────────────────────────────────────────────────────────
if(WIN32)
    set(PLATFORM_WIN32 ON)
    add_compile_definitions(PLATFORM_WIN32=1 _WINDOWS WIN32)
elseif(APPLE)
    set(PLATFORM_MACOS ON)
    add_compile_definitions(PLATFORM_MACOS=1)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for macOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS version")
elseif(UNIX)
    set(PLATFORM_LINUX ON)
    add_compile_definitions(PLATFORM_LINUX=1 _UNIX)
endif()

# ── Build Options ─────────────────────────────────────────────────────────────
option(BUILD_ZEROHOUR "Build Zero Hour (GeneralsMD) in addition to Generals" ON)
option(BUILD_TOOLS "Build editor tools (WorldBuilder, GUIEdit, etc.)" OFF)

# ── Compiler Warnings ────────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-sign-compare
        -Wno-unused-variable
        -Wno-reorder
    )
endif()

# ── Subdirectories ────────────────────────────────────────────────────────────
# These will be added as the port progresses:
# add_subdirectory(Generals/Code)
# if(BUILD_ZEROHOUR)
#     add_subdirectory(GeneralsMD/Code)
# endif()

message(STATUS "")
message(STATUS "=== CnC Generals Build Configuration ===")
message(STATUS "  Platform:       ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})")
message(STATUS "  C++ Standard:   ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type:     ${CMAKE_BUILD_TYPE}")
if(APPLE)
    message(STATUS "  Architectures:  ${CMAKE_OSX_ARCHITECTURES}")
    message(STATUS "  macOS Target:   ${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()
message(STATUS "  Zero Hour:      ${BUILD_ZEROHOUR}")
message(STATUS "  Tools:          ${BUILD_TOOLS}")
message(STATUS "=========================================")
message(STATUS "")
