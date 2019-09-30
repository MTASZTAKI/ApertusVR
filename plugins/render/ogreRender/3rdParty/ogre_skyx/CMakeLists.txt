#-------------------------------------------------------------------
# This file is part of the CMake build system for SkyX
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

############################################################
# SkyX core library
############################################################

PROJECT(SkyX)

# define header and source files for the library
set (SKYX_HEADER_FILES
  src/AtmosphereManager.h
  src/BasicController.h
  src/CloudsManager.h
  src/ColorGradient.h
  src/Controller.h
  src/GPUManager.h
  src/MeshManager.h
  src/MoonManager.h
  src/Prerequisites.h
  src/SCfgFileManager.h
  src/SkyX.h
  src/VCloudsManager.h
)

set (SKYX_SOURCE_FILES
  src/AtmosphereManager.cpp
  src/BasicController.cpp
  src/CloudsManager.cpp
  src/ColorGradient.cpp
  src/GPUManager.cpp
  src/MeshManager.cpp
  src/MoonManager.cpp
  src/SCfgFileManager.cpp
  src/SkyX.cpp
  src/VCloudsManager.cpp
)

set (VCLOUDS_HEADER_FILES
  src/VClouds/DataManager.h
  src/VClouds/Ellipsoid.h
  src/VClouds/FastFakeRandom.h
  src/VClouds/GeometryBlock.h
  src/VClouds/GeometryManager.h
  src/VClouds/Lightning.h
  src/VClouds/LightningManager.h
  src/VClouds/VClouds.h
)

set (VCLOUDS_SOURCE_FILES
  src/VClouds/DataManager.cpp
  src/VClouds/Ellipsoid.cpp
  src/VClouds/FastFakeRandom.cpp
  src/VClouds/GeometryBlock.cpp
  src/VClouds/GeometryManager.cpp
  src/VClouds/Lightning.cpp
  src/VClouds/LightningManager.cpp
  src/VClouds/VClouds.cpp
)

# Create files groups for IDE.
source_group(Header\ Files\\VClouds FILES ${VCLOUDS_HEADER_FILES})
source_group(Source\ Files\\VClouds FILES ${VCLOUDS_SOURCE_FILES})

# Add include directory
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	)

# Add needed definitions and nedmalloc include dir
add_definitions(-DSKYX_LIB -D_USRDLL)

# Add static flag if needed
#if(SKYX_STATIC)
#  add_definitions(-DSKYX_STATIC_LIB)
#endif()

#set ogre
set (OGRE_HOME ../ogre)
include_directories(
	${OGRE_HOME}/ogremain/include
	${OGRE_HOME}/include)
#use ogre
add_library(MY_OGRE STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE_HOME}/lib/Debug/OgreMain_d.lib)
	set_property(TARGET MY_OGRE PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE_HOME}/lib/Release/OgreMain.lib)
endif ()

if (UNIX)
  set(PLATFORM_LIBS dl)
endif()
set(LIBRARIES
  ${PLATFORM_LIBS}
)

# setup SkyX target
add_library(SkyX SHARED
  ${SKYX_LIB_TYPE}
  ${SKYX_HEADER_FILES}
  ${SKYX_SOURCE_FILES}
  ${VCLOUDS_HEADER_FILES}
  ${VCLOUDS_SOURCE_FILES}
)
set_target_properties(SkyX PROPERTIES VERSION ${SKYX_VERSION} SOVERSION ${SKYX_VERSION_MAJOR})
target_link_libraries(SkyX ${LIBRARIES} MY_OGRE)

# Configure SkyX
#skyx_config_lib(SkyX)

###############################################################
# install targets
###############################################################
#install(FILES ${SKYX_HEADER_FILES}
#  DESTINATION include/SKYX
#)
#install(FILES ${VCLOUDS_HEADER_FILES}
#  DESTINATION include/SKYX/VClouds
#)
