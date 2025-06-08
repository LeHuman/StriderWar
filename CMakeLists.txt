cmake_minimum_required(VERSION 3.20)
project(DOSTest VERSION 0.1.0 LANGUAGES C CXX)

# Enable export of compile commands (useful for tooling)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the target system as DOS
set(CMAKE_SYSTEM_NAME DOS)

# -bt=dos : Compile for DOS
# -mm : Medium Memory model
# -0 : 8086 or higher
# -d0 : No debugging info
# -zq : operate quietly
# -fr=nul : error file name
# -xd : no exception handling
# -oahi+nsx : relax alias checking, enable repeated optimizations, (C++) -oi with max inlining depth, numerically unstable floating-point, optimize for space, maximum optimization (-obmiler -s)
# -s : remove stack overflow checks
# -ei : force enums to be type int
# -zp8 : struct packing align 8
# -fpi87 : fpi87 inline 80x87
add_compile_options(-bt=dos -mm -0 -d0 -zq -fr=nul -xd -onashi+x -s -oh -oi+ -ei -zp8 -fpi87 )

set(CMAKE_C_FLAGS "-za99")
set(CMAKE_CXX_FLAGS "-ze -za0x")

# Define macros specific to the target environment
add_compile_definitions(_M_I86 __DOS__ __MEDIUM__)

file(GLOB_RECURSE SOURCES source/*.cpp source/*.c)

add_subdirectory(libraries)

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} DOSGraphics DOSInput DOSMath DOSSound)

include_directories(include)
