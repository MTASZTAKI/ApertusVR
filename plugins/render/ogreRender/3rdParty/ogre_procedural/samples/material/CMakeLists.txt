cmake_minimum_required(VERSION 2.6)

project(Sample_Material)

set( HDRS
		../common/include/BaseApplication.h
		include/Material.h
	)

set( SRCS
		../common/src/BaseApplication.cpp
		src/Material.cpp
	)

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	../common/include
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

procedural_add_executable(Sample_Material WIN32 ${HDRS} ${SRCS})

set_target_properties(Sample_Material PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(Sample_Material ${OgreProcedural_LIBRARIES} ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARY})

if ((${OGRE_VERSION} VERSION_EQUAL "1.9.0") OR (${OGRE_VERSION} VERSION_GREATER "1.9.0"))
	include_directories(${OGRE_Overlay_INCLUDE_DIR})
	target_link_libraries(Sample_Material ${OGRE_Overlay_LIBRARIES})
endif()

procedural_create_vcproj_userfile(Sample_Material)