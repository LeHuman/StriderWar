cmake_minimum_required(VERSION 3.20)
project(DOSTest VERSION 0.1.0 LANGUAGES C CXX)

# Enable export of compile commands (useful for tooling)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the target system as DOS
set(CMAKE_SYSTEM_NAME DOS)

# Set the memory model to medium
add_compile_options(-bt=dos -mm -0 -d0 -zq -fr=nul -xd -ox -s)
set(CMAKE_C_FLAGS "-za99")
set(CMAKE_CXX_FLAGS "-ze -za0x")

# Define macros specific to the target environment
add_compile_definitions(_M_I86 __DOS__ __MEDIUM__)

file(GLOB_RECURSE SOURCES source/*.cpp source/*.c)

# Generate LUTs
find_package(Python3 REQUIRED)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated_luts.hpp
    COMMAND ${CMAKE_COMMAND} -E echo "Generating LUTs from fixed_math.hpp..."
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_luts.py ${CMAKE_CURRENT_SOURCE_DIR}/include/fixed_math.hpp
    COMMENT "Generating lookup tables"
)
add_custom_target(generate_luts DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated_luts.hpp)


# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} generate_luts)

include_directories(include)
# include_directories("C:\\WATCOM\\h")
