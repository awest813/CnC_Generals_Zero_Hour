# WWMath – portable maths library.
# All source files are included; inline-assembly paths are already guarded
# behind _MSC_VER && _M_IX86 with standard C fallbacks.

file(GLOB WWMATH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB WWMATH_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl")

add_library(WWMath STATIC ${WWMATH_SOURCES} ${WWMATH_HEADERS})

target_include_directories(WWMath PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GENERALS_INCLUDE_DIRS}
)

target_link_libraries(WWMath PUBLIC WWLib)
