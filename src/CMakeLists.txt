cmake_minimum_required(VERSION 3.15)
project(src VERSION 1.0.0)

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

add_compile_definitions(STRSAFE_NO_DEPRECATE)

add_subdirectory(ainesmile)
# add_subdirectory(utility)
