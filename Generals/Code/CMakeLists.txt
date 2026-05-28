# Generals/Code – top-level CMake for the Generals source tree.
# Only core libraries are built for now; GameEngine, GameEngineDevice, Main,
# and tool targets will be added once the core compiles cleanly.

# ---------------------------------------------------------------------------
# Common include directories shared by all targets in this tree
# ---------------------------------------------------------------------------
set(GENERALS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Include/Lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWLib
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWDebug
    ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas/WWSaveLoad
    ${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Include
)

# ---------------------------------------------------------------------------
# Core library sub-projects
# ---------------------------------------------------------------------------
add_subdirectory(Libraries/Source/WWVegas/WWMath)
add_subdirectory(Libraries/Source/WWVegas/WWLib)
