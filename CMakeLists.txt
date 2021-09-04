cmake_minimum_required(VERSION 3.0)
project(UI_MATH)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
file(GLOB LOGGER src/Logger/*.cpp src/Logger/*.h)
file(GLOB VECTOR src/Vector/*.cpp src/Vector/*.h)
file(GLOB SET src/Set/*.cpp src/Set/*.h)
file(GLOB COMPACT src/Compact/*.cpp src/Compact/*.h)
file(GLOB HELPERS src/Helpers/*.cpp src/Helpers/*.h)
file(GLOB TESTS test/*.cpp)
file(GLOB HFILES include/*.h)

add_executable(${PROJECT_NAME} ${LOGGER} ${VECTOR} ${SET} ${COMPACT} ${TESTS} ${HELPERS} ${HFILES} )