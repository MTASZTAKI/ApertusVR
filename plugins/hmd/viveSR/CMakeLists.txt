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

set(VIVESR_READY FALSE)
set(VIVESR_DOWNLOAD_URL "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/viveSR/SRWorks_v0.9.7.1_Native_C_Library.zip")
set(VIVESR_PREBUILD_DIR ${CMAKE_BINARY_DIR}/VIVESR/build)
set(VIVESR_PREBUILD_ZIP ${CMAKE_BINARY_DIR}/VIVESR/SRWorks_v0.9.7.1_Native_C_Library.zip)

if (NOT EXISTS ${VIVESR_PREBUILD_DIR})
		file(MAKE_DIRECTORY ${VIVESR_PREBUILD_DIR})
endif ()

if(EXISTS ${VIVESR_PREBUILD_DIR}/C-Sample/libs/ViveSRWorks_Client.lib)
	set(VIVESR_READY TRUE)
	message (STATUS "VIVESR is installed" )
else ()
	set(VIVESR_READY FALSE)
	message (STATUS "VIVESR is not installed" )
endif ()

if (NOT VIVESR_READY)
	if(NOT EXISTS "${VIVESR_PREBUILD_ZIP}")
		message(STATUS "Downloading ${VIVESR_DOWNLOAD_URL}...")
		file(
			DOWNLOAD "${VIVESR_DOWNLOAD_URL}" "${VIVESR_PREBUILD_ZIP}"
			SHOW_PROGRESS
		)
	endif()
	message(STATUS "Extracting ${VIVESR_PREBUILD_ZIP}...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xzf "${VIVESR_PREBUILD_ZIP}"
		WORKING_DIRECTORY ${VIVESR_PREBUILD_DIR}
	)
endif ()

add_library(MY_VIVESR STATIC IMPORTED)
set_property(TARGET MY_VIVESR PROPERTY IMPORTED_LOCATION_DEBUG ${VIVESR_PREBUILD_DIR}/C-Sample/libs/ViveSRWorks_Client.lib)
set_property(TARGET MY_VIVESR PROPERTY IMPORTED_LOCATION_RELEASE ${VIVESR_PREBUILD_DIR}/C-Sample/libs/ViveSRWorks_Client.lib)


set (HEADERS
			apeViveSRPlugin.h
	)
set (SOURCES
	apeViveSRPlugin.cpp
	)
	include_directories(
			${VIVESR_PREBUILD_DIR}/C-Sample/headers
	)

add_library (apeViveSRPlugin SHARED ${SOURCES} ${HEADERS})


target_link_libraries (apeViveSRPlugin apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro MY_VIVESR)

set_property (TARGET apeViveSRPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeViveSRPlugin PROPERTY FOLDER "Plugins/HMD")

file(COPY ${VIVESR_PREBUILD_DIR}/C-Sample/x64/Release/ViveSRWorks_Client.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
file(COPY ${VIVESR_PREBUILD_DIR}/C-Sample/x64/Release/ViveSRWorks_Client.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)

file(COPY ${VIVESR_PREBUILD_DIR}/C-Sample/x64/Release/SRWorks_Log.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
file(COPY ${VIVESR_PREBUILD_DIR}/C-Sample/x64/Release/SRWorks_Log.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)

file(COPY ${VIVESR_PREBUILD_DIR}/C-Sample/x64/Release/ViveSR_CommunicationRules.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
file(COPY ${VIVESR_PREBUILD_DIR}/C-Sample/x64/Release/ViveSR_CommunicationRules.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)


