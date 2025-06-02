cmake_minimum_required(VERSION 3.20)
project(DOSSound VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*.cpp source/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS RELATIVE include/*.h include/*.hpp)

add_library(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${HEADERS}>
    PRIVATE ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
