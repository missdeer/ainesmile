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

find_package(Boost 1.79.0)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    set(Boost_USE_STATIC_LIBS ON)

    set(Boost_USE_MULTITHREADED ON)
    if(MSVC)
      set(Boost_USE_STATIC_RUNTIME ON)
    endif(MSVC)
endif(Boost_FOUND)

add_library(utility STATIC ${utility_src})

target_include_directories (utility PUBLIC 
    ${CMAKE_CURRENT_BINARY_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
