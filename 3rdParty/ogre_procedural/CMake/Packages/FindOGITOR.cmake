######################################################################
# OGITOR BUILD SYSTEM
# Welcome to the CMake build system for OGITOR.
# This is the main file where we prepare the general build environment
# and provide build configuration options.
######################################################################

# - Try to find Ogitor
# Once done, this will define
#
#  OGITOR_FOUND - system has Ogitor
#  OGITOR_INCLUDE_DIRS - the Ogitor include directories
#  OGITOR_LIBRARIES - link these to use Ogitor

include(FindPkgMacros)
include(PreprocessorUtils)
findpkg_begin(OGITOR)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(OGITOR_HOME)

# construct search paths
set(OGITOR_PREFIX_PATH ${OGITOR_HOME} ${ENV_OGITOR_HOME} /usr/local /usr/local/include /usr/local/lib /usr/include /usr/lib /usr/local/include/ogitor /usr/include/ogitor /usr/lib/ogitor /usr/local/lib/ogitor)
create_search_paths(OGITOR)
# redo search if prefix path changed
clear_if_changed(OGITOR_PREFIX_PATH
  OGITOR_LIBRARY_REL
  OGITOR_LIBRARY_DBG
  OGITOR_INCLUDE_DIR
  OGITOR_LIBRARIES
  )

set(OGITOR_LIBRARY_NAMES Ogitor)
get_debug_names(OGITOR_LIBRARY_NAMES)

use_pkgconfig(OGITOR_PKGC OGITOR)

findpkg_framework(OGITOR)

find_path(OGITOR_INCLUDE_DIR NAMES Ogitors.h HINTS ${OGITOR_INC_SEARCH_PATH} ${OGITOR_PKGC_INCLUDE_DIRS} PATH_SUFFIXES Ogitor)
find_library(OGITOR_LIBRARY_REL NAMES ${OGITOR_LIBRARY_NAMES} HINTS ${OGITOR_LIB_SEARCH_PATH} ${OGITOR_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" release relwithdebinfo minsizerel)
find_library(OGITOR_LIBRARY_DBG NAMES ${OGITOR_LIBRARY_NAMES_DBG} HINTS ${OGITOR_LIB_SEARCH_PATH} ${OGITOR_PKGC_LIBRARY_DIRS} PATH_SUFFIXES "" debug)
make_library_set(OGITOR_LIBRARY)

if(OGITOR_INCLUDE_DIR AND OGITOR_LIBRARY_REL AND OGITOR_LIBRARY_DBG)
	set(OGITOR_FOUND TRUE)
endif(OGITOR_INCLUDE_DIR AND OGITOR_LIBRARY_REL AND OGITOR_LIBRARY_DBG)
set(OGITOR_LIBRARIES optimized ${OGITOR_LIBRARY_REL} debug ${OGITOR_LIBRARY_DBG})
set(OGITOR_LIBRARIES ${OGITOR_LIBRARIES} CACHE STRING "Ogitor libraries")
set(OGITOR_INCLUDE_DIRS ${OGITOR_INCLUDE_DIR})

findpkg_finish(OGITOR)

add_parent_dir(OGITOR_INCLUDE_DIRS OGITOR_INCLUDE_DIR)
set(OGITOR_DEPENDENCY_INCLUDE_DIR "${OGITOR_INCLUDE_DIRS}/dependencies/include" CACHE STRING "Ogitor dependency include dir")
set(OGITOR_DEPENDENCY_LIB_DIR "${OGITOR_INCLUDE_DIRS}/dependencies/lib" CACHE STRING "Ogitor dependency library dir")

# Dependencies
set(OGITOR_DEPENDENCIES_INCLUDES
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/PagedGeometry"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Caelum-0.5.0/main"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/HYDRAX/Hydrax/src/Hydrax"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/SkyX"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/MeshMagick"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript"
    #"${OGITOR_DEPENDENCY_INCLUDE_DIR}/lua/src"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/autowrapper"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/scriptbuilder"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/scriptstdstring"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/scriptmath"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/scriptarray"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/quaternion"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/vector3"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/Angelscript/add_on/debugger"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/OFS"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/GenericTextEditor"
    "${OGITOR_DEPENDENCY_INCLUDE_DIR}/GenericImageEditor"
    )
	set(OGITOR_DEPENDENCIES_INCLUDES ${OGITOR_DEPENDENCIES_INCLUDES} CACHE STRING "Ogitor dependency includes")
message(STATUS "${OGITOR_DEPENDENCIES_INCLUDES} OGITOR_DEPENDENCIES_INCLUDES.")
message(STATUS "${OGITOR_DEPENDENCY_INCLUDE_DIR} OGITOR_DEPENDENCY_INCLUDE_DIR.")
message(STATUS "${OGITOR_DEPENDENCY_LIB_DIR} OGITOR_DEPENDENCY_LIB_DIR.")
