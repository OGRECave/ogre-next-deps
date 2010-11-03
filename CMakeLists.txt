cmake_minimum_required(VERSION 2.6)
project(OGREDEPS)
set(CMAKE_MODULE_PATH "${OGREDEPS_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

if (WIN32)
  # look for DirectX which is needed by OIS
  find_package(DirectX REQUIRED)
endif ()

add_subdirectory(src)
