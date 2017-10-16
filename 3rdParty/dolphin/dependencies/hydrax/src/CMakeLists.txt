cmake_minimum_required(VERSION 2.8)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
cmake_policy(SET CMP0003 NEW)

project(Hydrax)

set (OGRE_HOME "E:/Projects/Ogre/dependencies/ogre110/bin/sdk" )

if(WIN32)
   set(CMAKE_MODULE_PATH "${OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
   set(OGRE_SAMPLES_INCLUDEPATH
      ${OGRE_HOME}/Samples/include
   )
endif(WIN32)
 
if(UNIX)
   if(EXISTS "/usr/local/lib/OGRE/cmake")

     set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
     set(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local

   elseif(EXISTS "/usr/lib/OGRE/cmake")

     set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
     set(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one

   else ()
     message(SEND_ERROR "Failed to find module path.")
   endif(EXISTS "/usr/local/lib/OGRE")
endif(UNIX)
 

set(CMAKE_DEBUG_POSTFIX "_d")

find_package(OGRE REQUIRED)

# Find Boost
# Prefer static linking in all cases

set ( BOOST_HOME "E:/Projects/ogre/dependencies/boost" )

if (NOT OGRE_BUILD_PLATFORM_IPHONE)
    if (WIN32 OR APPLE)
        set(Boost_USE_STATIC_LIBS TRUE)
    else ()
        # Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
        set(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
    endif ()

    if (APPLE AND OGRE_BUILD_PLATFORM_APPLE_IOS)
        set(Boost_USE_MULTITHREADED OFF)
    endif()

    set(Boost_ADDITIONAL_VERSIONS "1.57" "1.57.0" "1.56" "1.56.0" "1.55" "1.55.0" "1.54" "1.54.0" "1.53" "1.53.0" "1.52" "1.52.0" "1.51" "1.51.0" "1.50" "1.50.0" "1.49" "1.49.0" "1.48" "1.48.0" "1.47" "1.47.0" "1.46" "1.46.0" "1.45" "1.45.0" "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40")

    # Components that need linking (NB does not include header-only components like bind)
    set(OGRE_BOOST_COMPONENTS thread date_time system regex filesystem)
    
    set(BOOST_ROOT E:/Projects/Ogre/dependencies/boost)
    
    find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)

    if (NOT Boost_FOUND)
        # Try again with the other type of libs
        if(Boost_USE_STATIC_LIBS)
            set(Boost_USE_STATIC_LIBS OFF)
        else()
            set(Boost_USE_STATIC_LIBS ON)
        endif()

        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    endif()

    if(Boost_FOUND AND Boost_VERSION GREATER 104900)
        if(Boost_VERSION GREATER 105300)
            set(OGRE_BOOST_COMPONENTS thread date_time system atomic chrono regex filesystem)
        else()
            set(OGRE_BOOST_COMPONENTS thread date_time system chrono regex filesystem)
        endif()
        find_package(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
    endif()

    if(Boost_FOUND AND NOT WIN32)
      list(REMOVE_DUPLICATES Boost_LIBRARIES)
    endif()

    if (Boost_FOUND)
        set(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
        set(OGRE_INCLUDE_DIRS ${OGRE_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
    endif (Boost_FOUND)
endif(NOT OGRE_BUILD_PLATFORM_IPHONE)

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

if(NOT ((${OGRE_VERSION} VERSION_EQUAL "1.10.0") OR (${OGRE_VERSION} VERSION_GREATER "1.10.0")))
    message(SEND_ERROR "You need Ogre 1.10 'Xalafu' to build this.")
endif()

link_directories ("E:/Projects/Ogre/dependencies/boost/stage/lib")

include_directories(${OGRE_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

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

include_directories (.)
# Terrain Scene Manager
#include_directories ( ${OGITOR_SM_INCLUDES} )

add_library(Hydrax SHARED ${HEADER_FILES} ${SOURCE_FILES}  )
SET(LIBRARY_OUTPUT_PATH  ../srcdemo/${CMAKE_BUILD_TYPE})

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

target_link_libraries(Hydrax  ${OGRE_LIBRARIES} ${Boost_SYSTEM_LIBRARY})

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
