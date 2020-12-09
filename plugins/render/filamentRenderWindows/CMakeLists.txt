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

set(FILAMENT_DOWNLOAD_URL "https://github.com/google/filament/releases/download/v1.9.9/filament-v1.9.9-windows.tgz")
set(FILAMENT_PREBUILD_DIR ${CMAKE_BINARY_DIR}/filamentWindows/build)
set(FILAMENT_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/filamentWindows/filament-v1.9.9-windows.tgz)
if (NOT EXISTS ${FILAMENT_PREBUILD_DIR})
		file(MAKE_DIRECTORY ${FILAMENT_PREBUILD_DIR})
endif ()

set(FILAMENT_READY FALSE)
if(EXISTS ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filament.lib)
	set(FILAMENT_READY TRUE)
	message (STATUS "Filament is installed" )
else ()
	set(FILAMENT_READY FALSE)
	message (STATUS "Filament is not installed" )
endif ()
if (NOT FILAMENT_READY)
	if(NOT EXISTS "${FILAMENT_PREBUILD_ZIP}")
		message(STATUS "Downloading ${FILAMENT_DOWNLOAD_URL}...")
		file(
			DOWNLOAD "${FILAMENT_DOWNLOAD_URL}" "${FILAMENT_PREBUILD_ZIP}"
			SHOW_PROGRESS
		)
	endif()
	message(STATUS "Extracting ${FILAMENT_PREBUILD_ZIP}...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${FILAMENT_PREBUILD_ZIP}"
		WORKING_DIRECTORY ${FILAMENT_PREBUILD_DIR}
	)
endif ()
add_library(MY_FILAMENT STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_FILAMENT PROPERTY IMPORTED_LOCATION_DEBUG ${FILAMENT_PREBUILD_DIR}/lib/x86_64/mdd/filament.lib)
	set_property(TARGET MY_FILAMENT PROPERTY IMPORTED_LOCATION_RELEASE ${FILAMENT_PREBUILD_DIR}/lib/x86_64/md/filament.lib)
endif ()

set (HEADERS
	apeFilamentWindowsRenderPlugin.h
	)
set (SOURCES
	apeFilamentWindowsRenderPlugin.cpp
	)
include_directories(
	${FILAMENT_PREBUILD_DIR}/include
	)
	
add_library (apeFilamentWindowsRenderPlugin SHARED ${SOURCES} ${HEADERS})

target_link_libraries (apeFilamentWindowsRenderPlugin apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro MY_FILAMENT )

set_property (TARGET apeFilamentWindowsRenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeFilamentWindowsRenderPlugin PROPERTY FOLDER "Plugins/Render")


