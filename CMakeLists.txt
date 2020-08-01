cmake_minimum_required(VERSION 3.11)

# TBD should be compatible to old MSVC CMake versions, but maybe a higher policy is still required
if(${CMAKE_VERSION} VERSION_LESS 3.15)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.15)
endif()

project(wmelite VERSION 1.0
        DESCRIPTION "WMELite game engine"
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(libecho)

