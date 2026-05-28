# WWMath – portable maths library – Zero Hour tree.

file(GLOB WWMATH_MD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB WWMATH_MD_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl")

add_library(WWMathMD STATIC ${WWMATH_MD_SOURCES} ${WWMATH_MD_HEADERS})

target_include_directories(WWMathMD PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GENERALSMD_INCLUDE_DIRS}
)

target_link_libraries(WWMathMD PUBLIC WWLibMD)
