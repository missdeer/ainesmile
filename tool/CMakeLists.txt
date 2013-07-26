
project(notepadpp_to_boost_property_tree)
cmake_minimum_required(VERSION 2.8)

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


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # uninstall target
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
  set(prefix "${CMAKE_INSTALL_PREFIX}")
  set(exec_prefix "${CMAKE_INSTALL_PREFIX}")
  set(bindir "${exec_prefix}/bin")
  set(libdir "${LIB_INSTALL_DIR}")
  set(pkgdatadir "${prefix}${RIME_DATA_DIR}")
  set(includedir "${CMAKE_INSTALL_PREFIX}/include")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable (npp2ainesmile npp2ainesmile.cpp )
