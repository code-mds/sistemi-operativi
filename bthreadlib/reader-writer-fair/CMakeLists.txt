cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 99)

#set(GCC_COVERAGE_COMPILE_FLAGS "-m32 -mno-accumulate-outgoing-args -c -g -Wa,-ahl=main_assembly.s32 -fverbose-asm")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

project(reader-writer-fair)

set(SOURCE_FILES ../tqueue.c ../bthread.c ../tmutex.c ../tcondition.c ../tsemaphore.c readerwriterfair.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
