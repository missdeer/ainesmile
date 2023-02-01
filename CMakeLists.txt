cmake_minimum_required(VERSION 3.15)
project(AinesmileEditor VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILECOMMANDS ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_TYPE Debug)
    add_compile_definitions(LOGS_ENABLED)
ELSE(CMAKE_BUILD_TYPE MATCHES Debug)
    set(BUILD_TYPE Release)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

add_subdirectory(3rdparty)
add_subdirectory(src)
