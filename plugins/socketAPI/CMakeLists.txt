#MIT License
#
#Copyright (c) 2016 MTA SZTAKI
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

set (UWS_HOME ${CMAKE_SOURCE_DIR}/3rdParty/uWebSockets)

set(UWS_DLL_NAME "uWS.dll")
set(UWS_DLL_PATH_DEBUG "${UWS_HOME}/x64/Debug/${UWS_DLL_NAME}")
set(UWS_DLL_PATH_RELEASE "${UWS_HOME}/x64/Release/${UWS_DLL_NAME}")

set (UWS_READY FALSE)

function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

if(EXISTS ${UWS_DLL_PATH_RELEASE})
	set(UWS_READY TRUE)
	message (STATUS "uws is installed" )
else()
	set(UWS_READY FALSE)
	message (STATUS "uws is not installed" )
endif()
if (NOT UWS_READY)
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring uws for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${UWS_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building uws for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${UWS_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()

add_library(MY_UWS SHARED IMPORTED)
if (WIN32)
	set_property(TARGET MY_UWS PROPERTY IMPORTED_LOCATION_DEBUG ${UWS_DLL_PATH_DEBUG})
	set_property(TARGET MY_UWS PROPERTY IMPORTED_LOCATION_RELEASE ${UWS_DLL_PATH_RELEASE})
endif ()

set (HEADERS
	ApeSocketAPIPlugin.h
	)

set (SOURCES 
	ApeSocketAPIPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${NODE_SOURCE_DIR}/deps/zlib
	${NODE_SOURCE_DIR}/deps/openssl/openssl/include
	${UWS_HOME}/src
	)

add_library(ApeSocketAPIPlugin SHARED ${SOURCES} ${HEADERS})
LINK_DIRECTORIES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
target_link_libraries(ApeSocketAPIPlugin ApePluginManager ApeScene ApeEventManager MY_UWS)

set_property(TARGET ApeSocketAPIPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property(TARGET ApeSocketAPIPlugin PROPERTY FOLDER "Plugins")

if (WIN32)
	# if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/${UWS_DLL_NAME})
	# 	file(COPY ${UWS_DLL_PATH_DEBUG} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	# endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/${UWS_DLL_NAME})
		file(COPY ${UWS_DLL_PATH_RELEASE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()
