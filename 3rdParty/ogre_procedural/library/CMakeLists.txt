cmake_minimum_required(VERSION 2.6)

project(OgreProceduralMain)

include(PrecompiledHeader)
include(ProceduralConfigTargets)

set( HDRS
	${OgreProcedural_BINARY_DIR}/include/ProceduralPlatform.h
	include/Procedural.h
	include/ProceduralMeshGenerator.h
	include/ProceduralBoxGenerator.h
	include/ProceduralCapsuleGenerator.h
	include/ProceduralConeGenerator.h
	include/ProceduralCylinderGenerator.h
	include/ProceduralIcoSphereGenerator.h
	include/ProceduralPlaneGenerator.h
	include/ProceduralSphereGenerator.h	
	include/ProceduralRoundedBoxGenerator.h
	include/ProceduralTorusGenerator.h
	include/ProceduralTorusKnotGenerator.h
	include/ProceduralTubeGenerator.h
	include/ProceduralUtils.h
	include/ProceduralExtruder.h
	include/ProceduralLathe.h
	include/ProceduralShape.h	
	include/ProceduralShapeGenerators.h
	include/ProceduralShapeGeneratorsBase.h
	include/ProceduralPath.h
	include/ProceduralPathGenerators.h
	include/ProceduralPathGeneratorsBase.h
	include/ProceduralTrack.h
	include/ProceduralSplines.h
	include/ProceduralTriangulator.h
	include/ProceduralTriangleBuffer.h
	include/ProceduralStableHeaders.h
	include/ProceduralMultiShape.h
	include/ProceduralMultiShapeGenerators.h	
	include/ProceduralGeometryHelpers.h
	include/ProceduralBoolean.h
	include/ProceduralSpringGenerator.h
	include/ProceduralSVG.h
	include/ProceduralDebugRendering.h
	include/ProceduralTextureGenerator.h
	include/ProceduralTextureModifiers.h
	include/ProceduralNoise.h
	include/ProceduralMeshModifiers.h
	include/ProceduralTextureBuffer.h
	include/ProceduralPrismGenerator.h
	)

set( SRCS
		src/ProceduralBoxGenerator.cpp
		src/ProceduralCapsuleGenerator.cpp
		src/ProceduralConeGenerator.cpp
		src/ProceduralCylinderGenerator.cpp
		src/ProceduralIcoSphereGenerator.cpp
		src/ProceduralPlaneGenerator.cpp		
		src/ProceduralRoundedBoxGenerator.cpp
		src/ProceduralSphereGenerator.cpp
		src/ProceduralTorusGenerator.cpp
		src/ProceduralTorusKnotGenerator.cpp
		src/ProceduralTubeGenerator.cpp		
		src/ProceduralUtils.cpp
		src/ProceduralShape.cpp		
		src/ProceduralShapeGenerators.cpp
		src/ProceduralPath.cpp
		src/ProceduralPathGenerators.cpp
		src/ProceduralTrack.cpp
		src/ProceduralExtruder.cpp
		src/ProceduralLathe.cpp
		src/ProceduralTriangulator.cpp
		src/ProceduralPrecompiledHeaders.cpp
		src/ProceduralMultiShape.cpp
		src/ProceduralMultiShapeGenerators.cpp
		src/ProceduralGeometryHelpers.cpp
		src/ProceduralBoolean.cpp
		src/ProceduralSpringGenerator.cpp
		src/ProceduralSVG.cpp
		src/ProceduralDebugRendering.cpp
		src/ProceduralTextureGenerator.cpp
		src/ProceduralTextureModifiers.cpp
		src/ProceduralNoise.cpp
		src/ProceduralMeshModifiers.cpp
		src/ProceduralTextureBuffer.cpp		
		src/ProceduralTriangleBuffer.cpp
		src/ProceduralPrismGenerator.cpp
	)

include_directories(SYSTEM ${OGRE_INCLUDE_DIRS}
)

# should we build static libs?
if (OgreProcedural_STATIC)
  set(OgreProcedural_LIB_TYPE STATIC)
  set(OgreProcedural_CONFIG_LIB_TYPE "PROCEDURAL_STATIC")
else ()
  set(OgreProcedural_LIB_TYPE SHARED)
  set(OgreProcedural_CONFIG_LIB_TYPE "PROCEDURAL_SHARED")
endif ()
add_definitions("-D${OgreProcedural_CONFIG_LIB_TYPE}")

option(OgreProcedural_HARDENEDBUILDFLAGS "Hardened build flags" FALSE)

if (OgreProcedural_HARDENEDBUILDFLAGS)
	if(MSVC) # Sort the odd stuff out
		add_definitions("/WX /EHsc")
	else() # Clang/GCC/Mingw
		add_definitions("-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-parameter -pedantic -std=c++0x")
	endif()
endif()

procedural_add_library(OgreProcedural ${OgreProcedural_LIB_TYPE} ${HDRS} ${SRCS})

if(FREETYPE_FOUND)
  include_directories(OgreProcedural ${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(OgreProcedural ${FREETYPE_LIBRARIES})
  set(OgreProcedural_CONFIG_FREETYPE "PROCEDURAL_USE_FREETYPE")
else()
  set(OgreProcedural_CONFIG_FREETYPE "PROCEDURAL_NO_FREETYPE")
endif ()

set_target_properties(OgreProcedural PROPERTIES DEBUG_POSTFIX _d)

if (NOT OgreProcedural_STATIC)
	if (MINGW)
		set_target_properties(OgreProcedural PROPERTIES PREFIX "")
	endif()
endif()

configure_file(${OgreProcedural_TEMPLATES_DIR}/ProceduralPlatform.h.in ${OgreProcedural_BINARY_DIR}/include/ProceduralPlatform.h)

target_link_libraries(OgreProcedural ${OGRE_LIBRARIES} ${OIS_LIBRARIES})

procedural_install_target(OgreProcedural)
		
install(FILES ${HDRS} DESTINATION include/OgreProcedural)
if(MSVC)
	if(OgreProcedural_INSTALL_PDB)
		if(OgreProcedural_STATIC)
			install(FILES ${OgreProcedural_BINARY_DIR}/lib/debug/OgreProcedural_d.pdb DESTINATION lib/debug CONFIGURATIONS Debug)
		else()
			install(FILES ${OgreProcedural_BINARY_DIR}/bin/debug/OgreProcedural_d.pdb DESTINATION bin/debug CONFIGURATIONS Debug)
		endif()
	endif()
endif()

# specify a precompiled header to use
use_precompiled_header(OgreProcedural 
  "${CMAKE_CURRENT_SOURCE_DIR}/include/ProceduralStableHeaders.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ProceduralPrecompiledHeaders.cpp"
)
