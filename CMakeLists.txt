cmake_minimum_required(VERSION 3.30)
project(step_test LANGUAGES CXX C)

file(GLOB LIB_SOURCES "src/*.cpp" "src/*.c")
add_library(step_mtr STATIC ${LIB_SOURCES})
target_include_directories(step_mtr PRIVATE ${PROJECT_SOURCE_DIR}/include)


add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/examples/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC 
"${PROJECT_SOURCE_DIR}/include/")

target_link_libraries(${PROJECT_NAME} step_mtr pthread)


