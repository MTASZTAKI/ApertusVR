find_package(OGITOR REQUIRED)

set( HDRS
		include/Plugin.h
	)

set( SRCS
		src/Plugin.cpp
	)

include_directories( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${OGRE_Paging_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${OGITOR_INCLUDE_DIRS}
)

procedural_add_library(OgProcedural SHARED ${HDRS} ${SRCS})

set_target_properties(OgProcedural PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(OgProcedural ${OGRE_LIBRARIES} ${OIS_LIBRARIES} ${OgreProcedural_LIBRARIES} ${OGITOR_LIBRARIES})