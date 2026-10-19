cmake_minimum_required(VERSION 3.28)
project(curvy CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(curvy curvy.cpp)

find_package(SDL2 REQUIRED)
target_link_libraries(curvy PRIVATE SDL2::SDL2)

find_package(glm REQUIRED)
target_link_libraries(curvy PRIVATE glm::glm)
target_compile_definitions(curvy PRIVATE GLM_FORCE_SIZE_T_LENGTH)
