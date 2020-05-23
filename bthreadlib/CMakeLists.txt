cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 99)

#set(GCC_COVERAGE_COMPILE_FLAGS "-m32 -mno-accumulate-outgoing-args -c -g -Wa,-ahl=main_assembly.s32 -fverbose-asm")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

project(bthreadlib)
set(SOURCE_FILES tqueue.c bthread.c bthread_private.h tmutex.c tsemaphore.c tcondition.c tbarrier.c)
add_library(bthreadlib ${SOURCE_FILES})


