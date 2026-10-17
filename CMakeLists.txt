cmake_minimum_required(VERSION 3.20)
project(curvy)
find_package(SDL2 REQUIRED)
# find_package(SDL2_image REQUIRED)

add_executable(curvy curvy.cpp)
# target_include_directories(cmakeee PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(curvy PRIVATE SDL2::SDL2)# SDL2_image::SDL2_image)
