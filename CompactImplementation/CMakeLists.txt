cmake_minimum_required(VERSION 3.0)
project(AntonovCompact)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DBUILD_INTERFACES)

include_directories(include)
file(GLOB COMPACT Compact/*.cpp Compact/*.h)
file(GLOB HFILES include/*.h)
file(GLOB HELPERS Helpers/*.cpp Helpers/*.h)

add_library(${PROJECT_NAME} SHARED ${COMPACT} ${HELPERS} )