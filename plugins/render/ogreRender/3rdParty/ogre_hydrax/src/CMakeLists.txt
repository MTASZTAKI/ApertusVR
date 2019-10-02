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


set(HEADER_FILES
    CfgFileManager.h
    GPUNormalMapManager.h
    MaterialManager.h  TextureManager.h
    DecalsManager.h
    Help.h
    Mesh.h
    Enums.h
    Hydrax.h
    Prerequisites.h
    GodRaysManager.h
    Image.h
    RttManager.h
    Noise/Noise.h
    Noise/Perlin/Perlin.h
    Noise/FFT/FFT.h
    Noise/Real/Real.h
    Noise/Real/Wave.h
    Noise/Real/PressurePoint.h
    Modules/Module.h
    Modules/ProjectedGrid/ProjectedGrid.h
    Modules/RadialGrid/RadialGrid.h
    Modules/SimpleGrid/SimpleGrid.h

)
set(SOURCE_FILES
    CfgFileManager.cpp
    Help.cpp
    Prerequisites.cpp
    DecalsManager.cpp
    Hydrax.cpp
    RttManager.cpp
    Enums.cpp
    Image.cpp
    TextureManager.cpp
    GodRaysManager.cpp
    MaterialManager.cpp
    GPUNormalMapManager.cpp
    Mesh.cpp
    Noise/Noise.cpp
    Noise/Perlin/Perlin.cpp
    Noise/FFT/FFT.cpp
    Noise/Real/Real.cpp
    Noise/Real/Wave.cpp
    Noise/Real/PressurePoint.cpp
    Modules/Module.cpp
    Modules/ProjectedGrid/ProjectedGrid.cpp
    Modules/RadialGrid/RadialGrid.cpp
    Modules/SimpleGrid/SimpleGrid.cpp

)


SET(HYDRAX_VERSION 0)
SET(HYDRAX_VERSION_MAJOR 1)

add_library(MY_OGRE STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE PROPERTY IMPORTED_LOCATION_DEBUG $ENV{OGRE_SDK}/lib/Debug/OgreMain_d.lib)
	set_property(TARGET MY_OGRE PROPERTY IMPORTED_LOCATION_RELEASE $ENV{OGRE_SDK}/lib/Release/OgreMain.lib)
endif ()

include_directories(
	$ENV{OGRE_SDK}/include/ogre
	$ENV{OGRE_BUILD}/include
	$ENV{OGRE_HYDRAX_HOME}
	)
# Terrain Scene Manager
#include_directories ( ${OGITOR_SM_INCLUDES} )

add_library(Hydrax SHARED ${HEADER_FILES} ${SOURCE_FILES}  )
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BUILD_TYPE})

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
