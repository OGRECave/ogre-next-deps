cmake_minimum_required(VERSION 2.4.4...4.0)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(zlib)

set(VERSION "1.3.1")

# include(CheckTypeSize)
# include(CheckFunctionExists)
# include(CheckIncludeFile)
# include(CheckCSourceCompiles)
# enable_testing()

# check_include_file(sys/types.h HAVE_SYS_TYPES_H)
# check_include_file(stdint.h    HAVE_STDINT_H)
# check_include_file(stddef.h    HAVE_STDDEF_H)

# #
# # Check to see if we have large file support
# #
# set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
# # We add these other definitions here because CheckTypeSize.cmake
# # in CMake 2.4.x does not automatically do so and we want
# # compatibility with CMake 2.4.x.
# if(HAVE_SYS_TYPES_H)
# 	list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
# endif()
# if(HAVE_STDINT_H)
# 	list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
# endif()
# if(HAVE_STDDEF_H)
# 	list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
# endif()
# check_type_size(off64_t OFF64_T)
# if(HAVE_OFF64_T)
#    add_definitions(-D_LARGEFILE64_SOURCE=1)
# endif()
# set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
# check_function_exists(fseeko HAVE_FSEEKO)
# if(NOT HAVE_FSEEKO)
# 	add_definitions(-DNO_FSEEKO)
# endif()

#
# Check for unistd.h
#
# check_include_file(unistd.h Z_HAVE_UNISTD_H)

#============================================================================
# zlib
#============================================================================

set(ZLIB_PUBLIC_HDRS
	zconf.h
	zlib.h
)
set(ZLIB_PRIVATE_HDRS
	crc32.h
	deflate.h
	gzguts.h
	inffast.h
	inffixed.h
	inflate.h
	inftrees.h
	trees.h
	zutil.h
)
set(ZLIB_SRCS
	adler32.c
	compress.c
	crc32.c
	deflate.c
	gzclose.c
	gzlib.c
	gzread.c
	gzwrite.c
	inflate.c
	infback.c
	inftrees.c
	inffast.c
	trees.c
	uncompr.c
	zutil.c
)

add_library(zlib STATIC ${ZLIB_SRCS} ${ZLIB_DLL_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
install_dep(zlib include zlib.h zconf.h)
if (OGRE_PROJECT_FOLDERS)
	set_property(TARGET zlib PROPERTY FOLDER Dependencies)
endif ()

if (APPLE)
 set_target_properties(zlib PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")

 if(OGRE_BUILD_PLATFORM_APPLE_IOS)
  set_target_properties(zlib PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
 endif()
endif()

set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" CACHE PATH "" FORCE)
set(ZLIB_LIBRARY_DBG "zlib" CACHE STRING "" FORCE)
set(ZLIB_LIBRARY_REL "zlib" CACHE STRING "" FORCE)
mark_as_advanced(ZLIB_INCLUDE_DIR ZLIB_LIBRARY_DBG ZLIB_LIBRARY_REL)
