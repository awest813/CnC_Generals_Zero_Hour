set(WWMATH_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/wwmath.cpp
)

add_library(generals_wwmath STATIC ${WWMATH_SOURCES})

target_include_directories(generals_wwmath
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWLib
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWDebug
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../GameEngine/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWSaveLoad
)

target_compile_definitions(generals_wwmath PUBLIC BUILDING_WWMATH=1)
