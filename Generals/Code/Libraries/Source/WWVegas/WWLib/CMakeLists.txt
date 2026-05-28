file(GLOB WWLIB_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.c")

if(NOT WIN32)
    list(REMOVE_ITEM WWLIB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ddraw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dsurface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/win.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mutex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/registry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/LaunchWeb.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/verchk.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/cpudetect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/refcount.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/stimer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/systimer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/keyboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msgloop.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/rawfile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mixfile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mpu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ini.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/widestring.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/wwstring.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mpmath.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/srandom.cpp
    )
endif()

add_library(generals_wwlib STATIC ${WWLIB_SOURCES})

target_include_directories(generals_wwlib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../WWMath
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../Include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../
)

target_link_libraries(generals_wwlib PUBLIC generals_wwmath)
