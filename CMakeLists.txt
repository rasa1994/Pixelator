cmake_minimum_required(VERSION 3.29)

project("Pixelator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Pixelator Source.cpp)
target_link_libraries(Pixelator PUBLIC imageextracter)
target_link_directories(Pixelator PUBLIC ${CMAKE_SOURCE_DIR}/ImageExtracter)

add_subdirectory(ImageExtracter)