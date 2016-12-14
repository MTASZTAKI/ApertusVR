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

cmake_minimum_required (VERSION 3.5)
project (ApertusVR)

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Semicolon separated list" FORCE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# APE Project variables
set (APE_SOURCE_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "Source root folder" )
set (APE_OUTPUT_DIR_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
set (APE_OUTPUT_DIR_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)

# python
find_package(PythonInterp)
message("Python: version: ${PYTHON_VERSION_STRING}")
if (NOT PYTHONINTERP_FOUND)
	if (WIN32)

		message("Python: Starting installer...")

		set(PYTHON_SETUP_PATH "${CMAKE_SOURCE_DIR}/3rdParty/python/python-2.7.12.msi")

		execute_process(
			COMMAND cmd /c ${PYTHON_SETUP_PATH}
			RESULT_VARIABLE PYTHON_SETUP_RESULT
		)
		if (${PYTHON_SETUP_RESULT} EQUAL 0)
			message("Python: Installation succeeded, exit code (${PYTHON_SETUP_RESULT})")
		else ()
			message("Python: Installation failed, exit code (${PYTHON_SETUP_RESULT})")
		endif ()

		find_package(PythonInterp)
		message("Python: version: ${PYTHON_VERSION_STRING}")
		if (NOT PYTHONINTERP_FOUND)
			message(FATAL_ERROR "Python: Could not find python")
		endif ()

	endif ()
endif ()

#set-up rapidjson
set(RAPIDJSON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/3rdParty/rapidjson/include")	

# raknet patch log begin
# VariableListDeltaTracker.h was changed
# ReplicaManager3.cpp was changed
# 3rdParty/raknet/Lib/LibStatic/CMakeLists.txt was changed
# 3rdParty/raknet/Source/CCRakNetSlidingWindow.cpp was changed
# raknet patch log end

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
set (RAKNET_SRC_DIR ${CMAKE_SOURCE_DIR}/3rdParty/raknet)
set (RAKNET_HOME ${RAKNET_SRC_DIR})
set (RAKNET_BUILD_DIR ${RAKNET_SRC_DIR}/build)
set (RAKNET_INSTALL_DIR ${RAKNET_SRC_DIR}/install)
set (RAKNET_READY FALSE)
#find raknet
if(EXISTS ${RAKNET_HOME}/Lib/LibStatic/Debug/RakNetLibStatic.lib AND EXISTS ${RAKNET_HOME}/Lib/LibStatic/Release/RakNetLibStatic.lib)
	set(RAKNET_READY TRUE)
	message (STATUS "raknet is installed" )
else ()
	set(RAKNET_READY FALSE)
	message (STATUS "raknet is not installed" )
endif ()
if (NOT RAKNET_READY)
#build raknet
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring raknet for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} -Wno-dev
				-DCMAKE_CONFIGURATION_TYPES:STRING=${CONF}
				-DCMAKE_BUILD_TYPE:STRING=${CONF}		
				-DCMAKE_COMPILE_OPTIONS:STRING=${EXTRA_COMPILE_OPTIONS}
				-DCMAKE_INSTALL_PREFIX:PATH=${RAKNET_INSTALL_DIR}
				-DRAKNET_ENABLE_DLL=OFF
				-DRAKNET_ENABLE_SAMPLES=OFF	
				-DRAKNET_GENERATE_INCLUDE_ONLY_DIR=ON
				-DCMAKE_OSX_ARCHITECTURES=${DS_OSX_ARCHITECTURE}
				-G ${CMAKE_GENERATOR} 
				${RAKNET_SRC_DIR}
				WORKING_DIRECTORY ${RAKNET_SRC_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building raknet for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${RAKNET_SRC_DIR} --config ${CONF} --target RakNetLibStatic
			WORKING_DIRECTORY ${RAKNET_SRC_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use raknet
add_library(RAKNET STATIC IMPORTED)
if (WIN32)
	set_property(TARGET RAKNET PROPERTY IMPORTED_LOCATION_DEBUG ${RAKNET_HOME}/Lib/LibStatic/Debug/RakNetLibStatic.lib)
	set_property(TARGET RAKNET PROPERTY IMPORTED_LOCATION_RELEASE ${RAKNET_HOME}/Lib/LibStatic/Release/RakNetLibStatic.lib)
	set(RAKNET_INCLUDE_DIR ${RAKNET_HOME}/Source)
endif ()


# cUrl
set (CURL_READY FALSE)

set(CURL_SOURCE_DIR ${APE_SOURCE_DIR}/3rdParty/curl)
set(CURL_INCLUDE_DIR ${CURL_SOURCE_DIR}/include/)

set(CURL_DLL_FILENAME libcurl.dll)
set(CURL_LIB_FILENAME libcurl_imp.lib)

set(CURL_DLL_PATH_DEBUG ${CURL_SOURCE_DIR}/lib/Debug/${CURL_DLL_FILENAME})
set(CURL_DLL_PATH_RELEASE ${CURL_SOURCE_DIR}/lib/Release/${CURL_DLL_FILENAME})
set(CURL_DLL_OUTPUT_PATH_DEBUG ${APE_OUTPUT_DIR_DEBUG}/${CURL_DLL_FILENAME})
set(CURL_DLL_OUTPUT_PATH_RELEASE ${APE_OUTPUT_DIR_RELEASE}/${CURL_DLL_FILENAME})

set(CURL_LIB_PATH_DEBUG ${CURL_SOURCE_DIR}/lib/Debug/${CURL_LIB_FILENAME})
set(CURL_LIB_PATH_RELEASE ${CURL_SOURCE_DIR}/lib/Release/${CURL_LIB_FILENAME})
set(CURL_LIB_OUTPUT_PATH_DEBUG ${APE_OUTPUT_DIR_DEBUG}/${CURL_LIB_FILENAME})
set(CURL_LIB_OUTPUT_PATH_RELEASE ${APE_OUTPUT_DIR_RELEASE}/${CURL_LIB_FILENAME})

if (EXISTS ${CURL_LIB_PATH_DEBUG} AND EXISTS ${CURL_DLL_PATH_RELEASE})
	set(CURL_READY TRUE)
	message(STATUS "curl is installed" )
else ()
	set(CURL_READY FALSE)
	message(STATUS "curl is not installed" )
endif ()
if (NOT CURL_READY)
#build curl
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message(STATUS "Configuring curl for ${CONF}, please wait.....")
		execute_process(
				COMMAND ${CMAKE_COMMAND}
				-G ${CMAKE_GENERATOR}
				.
				WORKING_DIRECTORY ${CURL_SOURCE_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)
		reportError(output if error)
		message(STATUS "Building curl for ${CONF}, please wait.....")
		execute_process(
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${CURL_SOURCE_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError(output if error)
	endforeach ()
endif ()
#use curl
add_library(MY_CURL STATIC IMPORTED)
if (WIN32)
    set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_DEBUG ${CURL_LIB_PATH_DEBUG})
    set_property(TARGET MY_CURL PROPERTY IMPORTED_LOCATION_RELEASE ${CURL_LIB_PATH_RELEASE})
endif ()

if (NOT EXISTS ${CURL_DLL_PATH_DEBUG})
	message(FATAL_ERROR "Could not find ${CURL_DLL_PATH_DEBUG}")
endif ()
if (NOT EXISTS ${CURL_DLL_OUTPUT_PATH_DEBUG})
	file(COPY ${CURL_DLL_PATH_DEBUG} DESTINATION ${APE_OUTPUT_DIR_DEBUG})
else ()
	message(STATUS "curl: ${CURL_DLL_OUTPUT_PATH_DEBUG} already exists. Skipping copy.")
endif ()

if (NOT EXISTS ${CURL_DLL_PATH_RELEASE})
	message(FATAL_ERROR "Could not find ${CURL_DLL_PATH_RELEASE}")
endif ()
if (NOT EXISTS ${CURL_DLL_OUTPUT_PATH_RELASE})
	file(COPY ${CURL_DLL_PATH_RELEASE} DESTINATION ${APE_OUTPUT_DIR_RELEASE})
else ()
	message(STATUS "curl: ${CURL_DLL_OUTPUT_PATH_RELEASE} already exists. Skipping copy.")
endif ()

if (NOT EXISTS ${CURL_LIB_PATH_DEBUG})
	message(FATAL_ERROR "Could not find ${CURL_LIB_PATH_DEBUG}")
endif ()
if (NOT EXISTS ${CURL_LIB_OUTPUT_PATH_DEBUG})
	file(COPY ${CURL_LIB_PATH_DEBUG} DESTINATION ${APE_OUTPUT_DIR_DEBUG})
else ()
	message(STATUS "curl: ${CURL_LIB_OUTPUT_PATH_DEBUG} already exists. Skipping copy.")
endif ()

if (NOT EXISTS ${CURL_LIB_PATH_RELEASE})
	message(FATAL_ERROR "Could not find ${CURL_LIB_PATH_RELEASE}")
endif ()
if (NOT EXISTS ${CURL_LIB_OUTPUT_PATH_RELEASE})
	file(COPY ${CURL_LIB_PATH_RELEASE} DESTINATION ${APE_OUTPUT_DIR_RELEASE})
else ()
	message(STATUS "curl: ${CURL_LIB_OUTPUT_PATH_RELEASE} already exists. Skipping copy.")
endif ()


#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)



set (APE_BIN_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" CACHE PATH "Bin folder" )

include_directories( "common/include" )

# on *nixes we need this to use C++11
if (NOT WIN32)
	if (APPLE)
        	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
        	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	else()
        	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif ()
endif()

add_subdirectory (common)
add_subdirectory (core)
add_subdirectory (plugins)
add_subdirectory (samples)
add_subdirectory (servers)
