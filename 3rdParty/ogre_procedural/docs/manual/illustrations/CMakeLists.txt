project(OgreProceduralIllustrations)

set( HDRS
		../../../library/include/ProceduralStableHeaders.h
		include/Illustrations.h
	)

set( SRCS
		../../../library/src/ProceduralPrecompiledHeaders.cpp
		src/Illustrations.cpp
	)

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(FREETYPE_FOUND)
  if(OgreProcedural_STATIC)
    target_link_libraries(OgreProcedural ${FREETYPE_LIBRARIES})
  endif()
  add_definitions(-DPROCEDURAL_USE_FREETYPE)
endif ()

procedural_add_executable(OgreProceduralIllustrations WIN32 ${HDRS} ${SRCS})

file(WRITE ${OgreProcedural_ILLUSTRATIONS_PATH}/temp.txt "This file should be deleted before installation. It is only here to force the creation of the image folder")
file(REMOVE ${OgreProcedural_ILLUSTRATIONS_PATH}/temp.txt)

set_target_properties(OgreProceduralIllustrations PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(OgreProceduralIllustrations ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OgreProcedural_LIBRARIES})

procedural_create_vcproj_userfile(OgreProceduralIllustrations)

# specify a precompiled header to use
use_precompiled_header(OgreProceduralIllustrations 
  "../../../library/include/ProceduralStableHeaders.h"
  "../../../library/src/ProceduralPrecompiledHeaders.cpp"
)

get_target_property(Illustrations_EXE OgreProceduralIllustrations LOCATION)
get_filename_component(Illustrations_DIR "${Illustrations_EXE}" PATH)
add_custom_command(TARGET OgreProceduralIllustrations POST_BUILD COMMAND OgreProceduralIllustrations ARGS "${OgreProcedural_ILLUSTRATIONS_PATH}" WORKING_DIRECTORY ${Illustrations_DIR})
