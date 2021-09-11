cmake_minimum_required(VERSION 3.0)
project(AntonovLogger)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_definitions(-DBUILD_INTERFACES)

file(GLOB LOGGER Logger/*.cpp Logger/*.h)
file(GLOB HFILES include/*.h)
file(GLOB HELPERS Helpers/*.cpp Helpers/*.h)

add_library(${PROJECT_NAME} SHARED ${LOGGER} ${HELPERS} )