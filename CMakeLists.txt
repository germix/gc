cmake_minimum_required(VERSION 3.8)

project(gc)

set(SRC_LIB "src/gc.c")
set(SRC_TEST "test/test.c")

add_library(gclib STATIC ${SRC_LIB})

include_directories("src")
add_executable(test ${SRC_TEST})

target_link_libraries(test gclib)
