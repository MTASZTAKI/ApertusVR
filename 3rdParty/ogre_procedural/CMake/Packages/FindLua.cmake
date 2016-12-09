#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find LUA
# Once done, this will define
#
#  LUA_FOUND - system has LUA
#  LUA_INCLUDE_DIRS - the LUA include directories 
#  LUA_LIBRARIES - link these to use LUA
#  LUA_VERSION - version of LUA

include(FindPkgMacros)
findpkg_begin(LUA)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(LUA_HOME)

# construct search paths
set(LUA_PREFIX_PATH ${LUA_HOME} ${ENV_LUA_HOME} ${LUA_DIR})
create_search_paths(LUA)
# redo search if prefix path changed
clear_if_changed(LUA_PREFIX_PATH
  LUA_LIBRARY_FWK
  LUA_LIBRARY_REL
  LUA_LIBRARY_DBG
  LUA_INCLUDE_DIR
)

set(LUA_LIBRARY_NAMES lua52 lua5.2 lua-5.2 lua51 lua5.1 lua-5.1 lua liblua)
get_debug_names(LUA_LIBRARY_NAMES)

use_pkgconfig(LUA_PKGC lua)

# prefer static library over framework 
set(CMAKE_FIND_FRAMEWORK "LAST")

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
findpkg_framework(LUA)
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_path(LUA_INCLUDE_DIR NAMES lua.h HINTS ${LUA_INC_SEARCH_PATH} ${LUA_PKGC_INCLUDE_DIRS} PATH_SUFFIXES lua)

find_library(LUA_LIBRARY_REL NAMES ${LUA_LIBRARY_NAMES} HINTS ${LUA_LIB_SEARCH_PATH} ${LUA_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" release relwithdebinfo minsizerel)
find_library(LUA_LIBRARY_DBG NAMES ${LUA_LIBRARY_NAMES_DBG} HINTS ${LUA_LIB_SEARCH_PATH} ${LUA_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" debug)

make_library_set(LUA_LIBRARY)

if (LUA_FOUND)
  # include the math library for Unix
  if (UNIX AND NOT APPLE)
    find_library(LUA_MATH_LIBRARY m)
    set(LUA_LIBRARIES "${LUA_LIBRARY};${LUA_MATH_LIBRARY}" CACHE STRING "Lua Libraries")
  # For Windows and Mac, don't need to explicitly include the math library
  else ()
    set(LUA_LIBRARIES "${LUA_LIBRARY}" CACHE STRING "Lua Libraries")
  endif ()
endif ()

findpkg_finish(LUA)
mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES LUA_LIBRARY LUA_MATH_LIBRARY)

if (LUA_FOUND)
  file(STRINGS ${LUA_INCLUDE_DIR}/lua.h LUA_TEMP_VERSION_CONTENT REGEX "^#define LUA_VERSION_[A-Z]+[ \t]+\"[0-9]+\"")
  string(REGEX REPLACE ".*#define LUA_VERSION_MAJOR[ \t]+\"([0-9]+)\".*" "\\1" LUA_VERSION_MAJOR ${LUA_TEMP_VERSION_CONTENT})
  string(REGEX REPLACE ".*#define LUA_VERSION_MINOR[ \t]+\"([0-9]+)\".*" "\\1" LUA_VERSION_MINOR ${LUA_TEMP_VERSION_CONTENT})
  string(REGEX REPLACE ".*#define LUA_VERSION_RELEASE[ \t]+\"([0-9]+)\".*" "\\1" LUA_VERSION_RELEASE ${LUA_TEMP_VERSION_CONTENT})
  set(LUA_VERSION "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_RELEASE}")
  pkg_message(OGRE "Found Lua ${LUA_VERSION}")
endif ()

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")
