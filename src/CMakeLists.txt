cmake_minimum_required(VERSION 3.15)
project(src VERSION 1.0.0)

if(NOT DEFINED CMAKE_OSX_ARCHITECTURES)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

add_subdirectory(ainesmile)
# add_subdirectory(utility)
