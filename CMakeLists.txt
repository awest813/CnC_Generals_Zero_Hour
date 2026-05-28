cmake_minimum_required(VERSION 3.25)
project(CnC_Generals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(BUILD_GENERALS    "Build the Generals (original) tree"    ON)
option(BUILD_GENERALSMD  "Build the Generals Zero Hour tree"     OFF)

# ---------------------------------------------------------------------------
# Compiler configuration
# ---------------------------------------------------------------------------

if(MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-sign-compare
        -Wno-reorder
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-unknown-pragmas
        -Wno-narrowing
        -Wno-delete-incomplete
        -Wno-class-memaccess
    )
endif()

# ---------------------------------------------------------------------------
# Per-tree targets
# ---------------------------------------------------------------------------

if(BUILD_GENERALS)
    add_subdirectory(Generals/Code)
endif()

if(BUILD_GENERALSMD)
    add_subdirectory(GeneralsMD/Code)
endif()
