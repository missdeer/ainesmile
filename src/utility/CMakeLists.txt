cmake_minimum_required(VERSION 3.15)
project(utility VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB
	utility_src
	./*.cpp
	./*.hpp
  ./*.h
)

add_library(utility STATIC ${utility_src})

target_include_directories (utility PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
