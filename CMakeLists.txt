if (APPLE AND NOT ANDROID)
  cmake_minimum_required(VERSION 2.8.8)
  SET(CMAKE_SIZEOF_VOID_P 4)
else()
  cmake_minimum_required(VERSION 2.6)
endif ()

project(OGREDEPS)
set(CMAKE_MODULE_PATH "${OGREDEPS_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})

if (WIN32)
  # look for DirectX which is needed by OIS
  find_package(DirectX REQUIRED)
  add_definitions(-DWINVER=0x0500)
endif ()

if (APPLE AND OGRE_BUILD_PLATFORM_APPLE_IOS)
  # Set build variables
  set(CMAKE_OSX_SYSROOT iphoneos)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "")
  set(XCODE_ATTRIBUTE_SDKROOT iphoneos)
  set(CMAKE_OSX_ARCHITECTURES ${ARCHS_STANDARD_32_BIT})
elseif (APPLE AND NOT OGRE_BUILD_PLATFORM_APPLE_IOS)
  # Set 10.5 as the base SDK by default
  set(CMAKE_OSX_DEPLOYMENT_TARGET 10.6)

  if (NOT CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES ${ARCHS_STANDARD_32_64_BIT})
  endif()
endif ()

if (ANDROID)
  SET(TargetPlatform "Android")
  SET(OGRE_PLATFORM OGRE_PLATFORM_ANDROID)
endif()

add_subdirectory(src)
