cmake_minimum_required(VERSION 3.20)
project(rand VERSION 0.1.0 LANGUAGES C CXX)

# Generate LUTs
find_package(Python3 REQUIRED)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/rand/rand_lut.hpp
    COMMAND ${CMAKE_COMMAND} -E echo "Generating LUTs from rand.hpp..."
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py ${CMAKE_CURRENT_SOURCE_DIR}/include/rand/rand.hpp
    COMMENT "Generating lookup tables"
)
add_custom_target(generate_rand_luts DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/rand/rand_lut.hpp)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*.cpp source/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS RELATIVE include/*.h include/*.hpp)

add_library(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${HEADERS}>
    PRIVATE ${SOURCES}
)
add_dependencies(${PROJECT_NAME} generate_rand_luts)

target_include_directories(${PROJECT_NAME} PUBLIC include)

