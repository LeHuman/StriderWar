cmake_minimum_required(VERSION 3.20)
project(DOSMath VERSION 0.1.0 LANGUAGES C CXX)

# Generate LUTs
find_package(Python3 REQUIRED)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated_luts.hpp
    COMMAND ${CMAKE_COMMAND} -E echo "Generating LUTs from fixed_math.hpp..."
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py ${CMAKE_CURRENT_SOURCE_DIR}/include/dos/math/fixed/fixed_math.hpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py ${CMAKE_CURRENT_SOURCE_DIR}/include/dos/math/fixed/fixed.hpp
    COMMENT "Generating lookup tables"
)
add_custom_target(generate_luts DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated_luts.hpp)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*.cpp source/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS RELATIVE include/*.h include/*.hpp)

add_library(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${HEADERS}>
    PRIVATE ${SOURCES}
)
add_dependencies(${PROJECT_NAME} generate_luts)

target_include_directories(${PROJECT_NAME} PUBLIC include)

