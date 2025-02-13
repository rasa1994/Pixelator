cmake_minimum_required(VERSION 3.29)

project("Pixelator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include ("cmake/LoadPPL.cmake")
message(STATUS "Path to lib: ${processppl_SOURCE_DIR}")
add_executable(Pixelator Source.cpp)
target_link_libraries(Pixelator PUBLIC imageextracter processppl)
target_link_directories(Pixelator PUBLIC ${CMAKE_SOURCE_DIR}/ImageExtracter ${processppl_SOURCE_DIR})

add_subdirectory(ImageExtracter)