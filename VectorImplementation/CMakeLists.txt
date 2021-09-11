cmake_minimum_required(VERSION 3.0)
project(AntonovVector)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_definitions(-DBUILD_INTERFACES)

file(GLOB VECTOR Vector/*.cpp Vector/*.h)
file(GLOB HFILES include/*.h)
file(GLOB HELPERS include/*.h)

add_library(${PROJECT_NAME} SHARED ${VECTOR} ${HELPERS} ${HFILES})
