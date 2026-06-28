cmake_minimum_required(VERSION 3.16)
project(FREESTARS)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets )

set(CMAKE_BUILD_TYPE Debug)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(MersenneTwister)
add_subdirectory(tinyxml)
add_subdirectory(Server)
add_subdirectory(QtClient)
