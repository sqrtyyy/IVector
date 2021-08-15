cmake_minimum_required(VERSION 3.0)
project(UI_MATH)

include_directories(include)
file(GLOB SRC src/*.cpp src/*.h test/*.cpp)

add_executable(${PROJECT_NAME} ${SRC} )