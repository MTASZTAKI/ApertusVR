cmake_minimum_required(VERSION 2.6)

project(OgreProcedural)

set(CMAKE_MODULE_PATH 
  "${OgreProcedural_SOURCE_DIR}/CMake"
  "${OgreProcedural_SOURCE_DIR}/CMake/Utils"
  "${OgreProcedural_SOURCE_DIR}/CMake/Packages"
)

include(ProceduralAddTargets)

set(OgreProcedural_VERSION_MAJOR 0)
set(OgreProcedural_VERSION_MINOR 3)
set(OgreProcedural_VERSION_PATCH 0)
set(OgreProcedural_VERSION_DASH_SEPARATED "0.3.0 unstable")

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(CMAKE_DEBUG_POSTFIX "_d")

if (WIN32 OR APPLE)
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
	  "${OgreProcedural_BINARY_DIR}/sdk" CACHE PATH "PROCEDURAL install prefix" FORCE
    )
  endif (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
endif(WIN32 OR APPLE)

if (IS_DIRECTORY "${OgreProcedural_SOURCE_DIR}/Dependencies")
	message(STATUS "Found a directory named Dependencies...")
	set(OgreProcedural_DEPENDENCIES_DIR "${OgreProcedural_SOURCE_DIR}/Dependencies")
	if (IS_DIRECTORY "${OgreProcedural_DEPENDENCIES_DIR}/doxygen")
		message(STATUS "Found doxygen")
		set(DOXYGEN_DIR "${OgreProcedural_DEPENDENCIES_DIR}/doxygen")
	endif()
	file(GLOB GRAPHVIZ_POSSIBLE_PATH "${OgreProcedural_DEPENDENCIES_DIR}/Graphviz*")
	if (EXISTS "${GRAPHVIZ_POSSIBLE_PATH}")
		message(STATUS "Found Graphviz")
		set(DOXYGEN_DOT_PATH "${GRAPHVIZ_POSSIBLE_PATH}/bin")
	endif()
	if (EXISTS "${OgreProcedural_DEPENDENCIES_DIR}/lua")
		message(STATUS "Found lua")
		set(LUA_DIR "${OgreProcedural_DEPENDENCIES_DIR}/lua")
	endif()
	file(GLOB SWIG_POSSIBLE_PATH "${OgreProcedural_DEPENDENCIES_DIR}/swig*")
	if (EXISTS "${SWIG_POSSIBLE_PATH}")
		message(STATUS "Found Swig")
		set(SWIG_DIR ${SWIG_POSSIBLE_PATH})
	endif()	
endif()

if (NOT DEFINED BOOST_ROOT AND NOT EXISTS "$ENV{BOOST_ROOT}")
	message(STATUS "BOOST_ROOT not defined. Searching for it in ogre subdir")
	file(GLOB BOOST_POSSIBLE_PATH "${OGRE_HOME}/boost*")
	if (EXISTS "${BOOST_POSSIBLE_PATH}")
		message(STATUS "setting BOOST_ROOT=${BOOST_POSSIBLE_PATH}")
		set (BOOST_ROOT "${BOOST_POSSIBLE_PATH}" CACHE PATH "boost root")
	endif()
endif()

# Find Boost
# Prefer static linking in all cases
if (WIN32 OR APPLE)
	set(Boost_USE_STATIC_LIBS TRUE)
else ()
	# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
	set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
endif ()
if (APPLE AND OGRE_BUILD_PLATFORM_APPLE_IOS)
    set(Boost_USE_MULTITHREADED OFF)
endif()
set(Boost_ADDITIONAL_VERSIONS "1.58" "1.58.0" "1.57" "1.57.0" "1.56" "1.56.0" "1.55" "1.55.0" "1.54" "1.54.0" "1.53" "1.53.0" "1.52" "1.52.0" "1.51" "1.51.0" "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.47" "1.47.0" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40")

find_package(OGRE 1.10 QUIET)

# Unity build options
# A Unity build includes all sources files in just a few actual compilation units
# to potentially speed up the compilation.
option(OgreProcedural_UNITY_BUILD "Enable unity build for OgreProcedural." FALSE)
set(OgreProcedural_UNITY_FILES_PER_UNIT "50" CACHE STRING "Number of files per compilation unit in Unity build.")

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast /MP /Zm256")
  # Enable intrinsics on MSVC in debug mode
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi")
  if (CMAKE_CL_64)
    # Visual Studio bails out on debug builds in 64bit mode unless
	# this flag is set...
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
  endif ()
  if (OgreProcedural_UNITY_BUILD)
    # object files can get large with Unity builds
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  endif ()
  if (MSVC_VERSION GREATER 1600 OR MSVC_VERSION EQUAL 1600)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  endif ()
endif ()

find_package(Doxygen)

option(OgreProcedural_BUILD_SAMPLES "Build OgreProcedural samples." FALSE)
option(OgreProcedural_BUILD_TESTS "Build OgreProcedural test cases." FALSE)
option(OgreProcedural_BUILD_SCRIPT_INTERPRETER "Build a lua test bench to prototype stuff with OgreProcedural." FALSE)
option(OgreProcedural_BUILD_OGITOR_PLUGIN "Build Ogitor Plugin." FALSE)
option(OgreProcedural_BUILD_DOCS "Build documentation (if Doxygen is installed)." FALSE)
option(OgreProcedural_INSTALL_DOCS "Install documentation." FALSE)
if(MSVC)
	option(OgreProcedural_INSTALL_PDB "Install debug files." FALSE)
endif ()
option(OgreProcedural_STATIC "Static build." FALSE)
# option(OgreProcedural_ILLUSTRATIONS "Build illustrations for manual" FALSE)
set(OgreProcedural_ILLUSTRATIONS_PATH "${OgreProcedural_BINARY_DIR}/image")

# Set docs target directory
if (WIN32 OR APPLE)
  set(OgreProcedural_DOCS_PATH "docs")
elseif (UNIX)
  set(OgreProcedural_DOCS_PATH "share/OgreProcedural/docs")
endif ()

# Specify build paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OgreProcedural_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OgreProcedural_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OgreProcedural_BINARY_DIR}/bin")

set(OgreProcedural_TEMPLATES_DIR "${OgreProcedural_SOURCE_DIR}/CMake/Templates")
set(OgreProcedural_WORK_DIR ${OgreProcedural_BINARY_DIR})
set(OgreProcedural_LIB_DIRECTORY "lib${LIB_SUFFIX}" CACHE STRING "Install path for libraries, e.g. 'lib64' on some 64-bit Linux distros.")

# Configure version file for use by scripts
configure_file("${OgreProcedural_TEMPLATES_DIR}/version.txt.in" "${OgreProcedural_BINARY_DIR}/version.txt" @ONLY)

include_directories("${OgreProcedural_SOURCE_DIR}/library/include" "${OgreProcedural_BINARY_DIR}/include")

find_package(Freetype)

add_subdirectory(library)

# definitions for samples
set(OgreProcedural_LIBRARIES OgreProcedural) 

# CMake macros
include(ProceduralMacros)
include(ProceduralConfigTargets)

if (OgreProcedural_ILLUSTRATIONS OR OgreProcedural_BUILD_SAMPLES OR OgreProcedural_BUILD_TESTS)
	prepare_demo_dirs()
endif()

if (OgreProcedural_BUILD_SAMPLES)	
	add_subdirectory(samples)
endif()

if (OgreProcedural_BUILD_TESTS)
	add_subdirectory(tests)	
endif()

if (OgreProcedural_BUILD_SCRIPT_INTERPRETER)
	add_subdirectory(tools/ScriptInterpreter)
endif()

# documentation
if(OgreProcedural_BUILD_DOCS)
	add_subdirectory(docs)
endif()

# Install CMake modules
add_subdirectory(CMake)

# and CPack
# cpack
set(CPACK_PACKAGE_DESCRIPTION "OgreProcedural")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Ogre-Procedural is an Ogre3D based lib enabling developpers to quickly create geometric primitives, such as cubes, spheres, cylinders...")
set(CPACK_PACKAGE_NAME "ogre-procedural")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "ois ogre")
set(CPACK_PACKAGE_CONTACT "webmaster@ogreprocedural.org")
set(CPACK_PACKAGE_VENDOR "webmaster@ogreprocedural.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/MIT.txt")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
# TODO: add the version variable at the top in here
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "OgreProcedural-${OgreProcedural_VERSION_DASH_SEPARATED}")

SET(CPACK_GENERATOR ZIP)
IF(UNIX)
  SET(CPACK_GENERATOR ${CPACK_GENERATOR};STGZ;TGZ)
ENDIF(UNIX)
IF(LINUX)
  SET(CPACK_GENERATOR ${CPACK_GENERATOR};DEB;RPM)
ENDIF(LINUX)
IF(MSVC)
  SET(CPACK_GENERATOR ${CPACK_GENERATOR};NSIS)
ENDIF(MSVC)
IF(APPLE)
  SET(CPACK_GENERATOR ${CPACK_GENERATOR};PackageMaker)
ENDIF(APPLE)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${OgreProcedural_VERSION_DASH_SEPARATED}-${CMAKE_SYSTEM_PROCESSOR}")

# some NSIS stuff
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  #SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} OgreProcedural")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\http://www.ogre3d.org/tikiwiki/Ogre+Procedural+Geometry+Library")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\http://www.ogre3d.org/tikiwiki/Ogre+Procedural+Geometry+Library")
  SET(CPACK_NSIS_CONTACT "webmaster@ogreprocedural.org")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  #SET(CPACK_STRIP_FILES "bin/MyExecutable")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
INCLUDE(CPack)

if (OgreProcedural_INSTALL_DOCS)
  # other doc files
  set(DOC_FILES Contributors.txt MIT.txt ReadMe.txt)
  install(FILES ${DOC_FILES} DESTINATION ${OgreProcedural_DOCS_PATH})
endif()