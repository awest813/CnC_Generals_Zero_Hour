if(WIN32)
    set(PLATFORM_IMPL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32/Win32PlatformConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32/Win32CriticalSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Win32/Win32Thread.cpp
    )
else()
    set(PLATFORM_IMPL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mac/MacPlatformConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mac/MacCriticalSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mac/MacThread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Mac/MacLocalFileSystem.cpp
    )
endif()

add_library(generals_platform STATIC ${PLATFORM_IMPL_SOURCES})

target_include_directories(generals_platform
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(generals_platform PUBLIC cxx_std_17)
