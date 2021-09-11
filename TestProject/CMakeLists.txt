cmake_minimum_required(VERSION 3.0)
project(Tests)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

file(GLOB TESTS test/*.cpp)
file(GLOB HFILES include/*.h)

add_executable(${PROJECT_NAME} ${TESTS} ${HFILES} )

target_link_directories(${PROJECT_NAME} PUBLIC libs)
target_link_libraries(${PROJECT_NAME} AntonovCompact AntonovLogger AntonovProblem AntonovSet AntonovVector)