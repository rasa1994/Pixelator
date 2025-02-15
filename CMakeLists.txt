cmake_minimum_required(VERSION 3.29)

enable_testing()

set(VCPKG_ROOT "VCPKG_ROOT" CACHE STRING "VCPKG_ROOT")

if (NOT DEFINED VCPKG_ROOT)
  message(FATAL_ERROR "MISSING ROOT FOR VCPKG setting to C:/vcpkg")
  set(VCPKG_ROOT "C:/vcpkg")
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project("Pixelator")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include ("cmake/LoadPPL.cmake")
message(STATUS "Path to lib: ${processppl_SOURCE_DIR}")
add_executable(Pixelator Source.cpp)
target_link_libraries(Pixelator PUBLIC imageextracter processppl)
target_link_directories(Pixelator PUBLIC ${CMAKE_SOURCE_DIR}/ImageExtracter ${processppl_SOURCE_DIR})

include_directories(${CMAKE_TOOLCHAIN_FILE})
add_subdirectory(ImageExtracter)