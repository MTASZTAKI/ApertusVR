#MIT License
#
#Copyright (c) 2018 MTA SZTAKI
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.

set(SDL2_DOWNLOAD_URL "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/sdl2Windows/sdl2.zip")
set(SDL2_PREBUILD_DIR ${CMAKE_BINARY_DIR}/SDL2Windows/build)
set(SDL2_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/SDL2Windows/sdl2.zip)
if (NOT EXISTS ${SDL2_PREBUILD_DIR})
		file(MAKE_DIRECTORY ${SDL2_PREBUILD_DIR})
endif ()

set(SDL2_READY FALSE)
if(EXISTS ${SDL2_PREBUILD_DIR}/sdl2/lib/debug/sdl2.lib)
	set(SDL2_READY TRUE)
	message (STATUS "SDL2 is installed" )
else ()
	set(SDL2_READY FALSE)
	message (STATUS "SDL2 is not installed" )
endif ()
if (NOT SDL2_READY)
	if(NOT EXISTS "${SDL2_PREBUILD_ZIP}")
		message(STATUS "Downloading ${SDL2_DOWNLOAD_URL}...")
		file(
			DOWNLOAD "${SDL2_DOWNLOAD_URL}" "${SDL2_PREBUILD_ZIP}"
			SHOW_PROGRESS
		)
	endif()
	message(STATUS "Extracting ${SDL2_PREBUILD_ZIP}...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${SDL2_PREBUILD_ZIP}"
		WORKING_DIRECTORY ${SDL2_PREBUILD_DIR}
	)
endif ()

add_library(MY_SDL2 STATIC IMPORTED)
set_property(TARGET MY_SDL2 PROPERTY IMPORTED_LOCATION_DEBUG ${SDL2_PREBUILD_DIR}/sdl2/lib/release/sdl2.lib)
set_property(TARGET MY_SDL2 PROPERTY IMPORTED_LOCATION_RELEASE ${SDL2_PREBUILD_DIR}/sdl2/lib/debug/sdl2.lib)


# apeSdlWindowPlugin
set (HEADERS
    apeSdlWindowPluginConfigs.h
	apeSdlWindowPlugin.h
	)
set (SOURCES
	apeSdlWindowPlugin.cpp
	)
include_directories(
	${SDL2_PREBUILD_DIR}/sdl2/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)
add_library (apeSdlWindowPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (apeSdlWindowPlugin apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro MY_SDL2)

set_property (TARGET apeSdlWindowPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeSdlWindowPlugin PROPERTY FOLDER "Plugins/Render")

