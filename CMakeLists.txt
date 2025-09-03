cmake_minimum_required(VERSION 3.30)
# set(CMAKE_VERBOSE_MAKEFILE ON)
project(step_test LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES "src/*.cpp" "src/*.c")

add_executable(${PROJECT_NAME} 
main.cpp
${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
"${PROJECT_SOURCE_DIR}/include/")

target_link_libraries(${PROJECT_NAME} pthread)

