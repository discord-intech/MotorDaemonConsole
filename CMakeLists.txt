cmake_minimum_required(VERSION 2.4)
project(MotorDaemonConsole)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

set(SOURCE_FILES main.c)
add_executable(MotorDaemonConsole ${SOURCE_FILES})