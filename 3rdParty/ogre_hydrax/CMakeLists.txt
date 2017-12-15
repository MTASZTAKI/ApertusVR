cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(Hydrax)

if(UNIX AND NOT APPLE)
    # Newer ld versions need this linked explicitly
    find_package(Threads REQUIRED)
    add_definitions(${CMAKE_THREAD_LIBS_INIT})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed ${CMAKE_THREAD_LIBS_INIT}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed ${CMAKE_THREAD_LIBS_INIT}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-as-needed ${CMAKE_THREAD_LIBS_INIT}")
endif(UNIX AND NOT APPLE)

if(APPLE)
    # Newer ld versions need this linked explicitly
    find_package(Threads REQUIRED)
    add_definitions(${CMAKE_THREAD_LIBS_INIT})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl ${CMAKE_THREAD_LIBS_INIT}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl ${CMAKE_THREAD_LIBS_INIT}")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl ${CMAKE_THREAD_LIBS_INIT}")

    find_package(CG REQUIRED)

    # Resources (Only used by OSX)
    set(MAC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Mac")
    set(RUNPATH "${CMAKE_CURRENT_SOURCE_DIR}/RunPath")
endif(APPLE)

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

set(HEADER_FILES
    include/Hydrax.h
	include/HydraxCfgFileManager.h
	include/HydraxDecalsManager.h
	include/HydraxEnums.h
	include/HydraxGodRaysManager.h
	include/HydraxGPUNormalMapManager.h
	include/HydraxHelp.h
	include/HydraxImage.h
	include/HydraxMaterialManager.h
	include/HydraxMesh.h
	include/HydraxPrerequisites.h
	include/HydraxRttManager.h
    include/HydraxTextureManager.h
    include/Noise/HydraxNoise.h
    include/Noise/Perlin/HydraxPerlin.h
    include/Noise/FFT/HydraxFFT.h
    include/Noise/Real/HydraxReal.h
    include/Noise/Real/HydraxWave.h
    include/Noise/Real/HydraxPressurePoint.h
    include/Modules/HydraxModule.h
    include/Modules/ProjectedGrid/HydraxProjectedGrid.h
    include/Modules/RadialGrid/HydraxRadialGrid.h
    include/Modules/SimpleGrid/HydraxSimpleGrid.h

)
set(SOURCE_FILES
    src/Hydrax.cpp
	src/HydraxCfgFileManager.cpp
	src/HydraxDecalsManager.cpp
	src/HydraxEnums.cpp
	src/HydraxGodRaysManager.cpp
	src/HydraxGPUNormalMapManager.cpp
	src/HydraxHelp.cpp
	src/HydraxImage.cpp
	src/HydraxMaterialManager.cpp
	src/HydraxMesh.cpp
	src/HydraxPrerequisites.cpp
	src/HydraxRttManager.cpp
	src/HydraxTextureManager.cpp
    src/Noise/HydraxNoise.cpp
    src/Noise/Perlin/HydraxPerlin.cpp
    src/Noise/FFT/HydraxFFT.cpp
    src/Noise/Real/HydraxReal.cpp
    src/Noise/Real/HydraxWave.cpp
    src/Noise/Real/HydraxPressurePoint.cpp
    src/Modules/HydraxModule.cpp
    src/Modules/ProjectedGrid/HydraxProjectedGrid.cpp
    src/Modules/RadialGrid/HydraxRadialGrid.cpp
    src/Modules/SimpleGrid/HydraxSimpleGrid.cpp
)


SET(HYDRAX_VERSION 0)
SET(HYDRAX_VERSION_MAJOR 1)

include_directories (include)
# Terrain Scene Manager
#include_directories ( ${OGITOR_SM_INCLUDES} )

add_library(Hydrax SHARED ${HEADER_FILES} ${SOURCE_FILES}  )
SET(LIBRARY_OUTPUT_PATH srcdemo/${CMAKE_BUILD_TYPE})

set_target_properties(PROPERTIES
        INSTALL_RPATH ${OGRE_INSTALL_DIR}/lib/OGRE  )

set_target_properties(Hydrax PROPERTIES VERSION ${HYDRAX_VERSION} SOVERSION ${HYDRAX_VERSION_MAJOR})

set_target_properties(Hydrax PROPERTIES
COMPILE_DEFINITIONS HYDRAX_LIB
)

IF(MSVC_IDE)
	add_custom_command( TARGET Hydrax PRE_BUILD
		COMMAND if not exist ..\\..\\..\\..\\..\\RunPath\\bin mkdir ..\\..\\..\\..\\..\\RunPath\\bin )
	add_custom_command( TARGET Hydrax POST_BUILD
		COMMAND copy \"$(TargetPath)\" ..\\..\\..\\..\\..\\RunPath\\bin )
ENDIF(MSVC_IDE)

set_target_properties(Hydrax PROPERTIES SOLUTION_FOLDER Dependencies)

target_link_libraries(Hydrax MY_OGRE)

IF(WIN32)
	install(TARGETS Hydrax
		RUNTIME DESTINATION bin
		CONFIGURATIONS Debug Release RelWithDebInfo MinSizeRel)
ENDIF(WIN32)

IF(UNIX AND NOT APPLE)

    set_target_properties( Hydrax PROPERTIES OUTPUT_NAME "Hydrax_Ogitor" VERSION
    ${OGITOR_VERSION_STRING} SOVERSION ${abi_version} )

    if(OGITOR_DIST)
        install(TARGETS Hydrax
            LIBRARY DESTINATION lib
            CONFIGURATIONS Release RelWithDebInfo MinSizeRel)
    endif(OGITOR_DIST)

ENDIF(UNIX AND NOT APPLE)


IF (APPLE)
        set_target_properties(Hydrax PROPERTIES OUTPUT_NAME Hydrax)
        # Set the INSTALL_PATH so that Frameworks can be local
        set_target_properties(Hydrax PROPERTIES BUILD_WITH_INSTALL_RPATH 1 INSTALL_NAME_DIR "@executable_path/../Plugins/Ogitor")
ENDIF(APPLE)
