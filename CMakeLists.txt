cmake_minimum_required(VERSION 2.8)
project(FREESTARS)
find_package(Qt4 REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(MersenneTwister)
add_subdirectory(tinyxml)
add_subdirectory(Server)
add_subdirectory(QtClient)
