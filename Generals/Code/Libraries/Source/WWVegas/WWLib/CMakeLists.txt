set(WWLIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/crc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gcd_lcm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nstrdup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/strtok_r.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/trim.cpp
)

add_library(generals_wwlib STATIC ${WWLIB_SOURCES})

target_include_directories(generals_wwlib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWMath
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWDebug
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../GameEngine/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWSaveLoad
)

target_link_libraries(generals_wwlib PUBLIC generals_wwmath)
