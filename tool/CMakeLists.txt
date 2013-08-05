
project(configure_generator)
cmake_minimum_required(VERSION 2.6)

if(WIN32)
  set(EXT ".exe")
endif(WIN32)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(Boost_USE_STATIC_LIBS ${BUILD_STATIC})


set(Boost_USE_MULTITHREADED ON)
if(MSVC)
  set(Boost_USE_STATIC_RUNTIME ON)
endif(MSVC)
find_package(Boost 1.49.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

include_directories(${PROJECT_SOURCE_DIR}/../3rdparty/rapidxml-1.13)

if(MSVC)
  add_definitions("/wd4244 /wd4996")
endif(MSVC)

add_executable (npp2ainesmile npp2ainesmile.cpp )
add_executable (npp2langmap   npp2langmap.cpp )
add_executable (gen_cfg gen_cfg.cpp)

