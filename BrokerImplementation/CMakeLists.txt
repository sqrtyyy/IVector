cmake_minimum_required(VERSION 3.0)
project(AntonovBroker)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DBUILD_INTERFACES)

include_directories(include)
file(GLOB BROKER Broker/*.cpp Broker/*.h)
file(GLOB HFILES include/*.h)

add_library(${PROJECT_NAME} SHARED ${BROKER} ${HFILES} )