
project(configure_generator)
cmake_minimum_required(VERSION 3.15)

find_package(OpenSSL)

include_directories(${PROJECT_SOURCE_DIR}/../3rdparty/rapidxml-1.13)

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

if(MSVC)
    add_definitions("/wd4244 /wd4996")
endif(MSVC)

add_executable (npp2ainesmile npp2ainesmile.cpp )
add_executable (npp2langmap   npp2langmap.cpp )
add_executable (gen_cfg gen_cfg.cpp)
add_executable (keygen keygen.cpp)

target_link_libraries (keygen OpenSSL::SSL OpenSSL::Crypto)
