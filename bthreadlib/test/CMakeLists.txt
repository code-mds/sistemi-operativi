cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 99)

#set(GCC_COVERAGE_COMPILE_FLAGS "-m32 -mno-accumulate-outgoing-args -c -g -Wa,-ahl=main_assembly.s32 -fverbose-asm")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

project(bthreadlib_test)

set(SOURCE_FILES test_main.c test_bthread.c test_tqueue.c ../tqueue.c ../bthread.c)

add_executable(bthreadlib_test ${SOURCE_FILES})
