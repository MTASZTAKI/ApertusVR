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

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

#cef
# Specify the CEF distribution version.
set(CEF_VERSION "3.3071.1643.ga83b8e9")
# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(CEF_PLATFORM "macosx64")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "linux64")
  else()
    set(CEF_PLATFORM "linux32")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "windows64")
  else()
    set(CEF_PLATFORM "windows32")
  endif()
endif()
set(CEF_DISTRIBUTION "cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")
set(CEF_HOME ${CMAKE_SOURCE_DIR}/3rdParty/${CEF_DISTRIBUTION})
set(CEF_DLLWRAPPER_HOME ${CMAKE_SOURCE_DIR}/3rdParty/${CEF_DISTRIBUTION}/libcef_dll)
set(CEF_DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/3rdParty/)
# The location where we expect the extracted binary distribution.
set(CEF_ROOT "${CEF_DOWNLOAD_DIR}/${CEF_DISTRIBUTION}" CACHE INTERNAL "CEF_ROOT")
# Download and/or extract the binary distribution if necessary.
if(NOT IS_DIRECTORY "${CEF_ROOT}")
set(CEF_DOWNLOAD_FILENAME "${CEF_DISTRIBUTION}.tar.bz2")
set(CEF_DOWNLOAD_PATH "${CEF_DOWNLOAD_DIR}/${CEF_DOWNLOAD_FILENAME}")
if(NOT EXISTS "${CEF_DOWNLOAD_PATH}")
	set(CEF_DOWNLOAD_URL "http://opensource.spotify.com/cefbuilds/${CEF_DOWNLOAD_FILENAME}")
	# Download the SHA1 hash for the binary distribution.
	message(STATUS "Downloading ${CEF_DOWNLOAD_PATH}.sha1...")
	file(DOWNLOAD "${CEF_DOWNLOAD_URL}.sha1" "${CEF_DOWNLOAD_PATH}.sha1")
	file(READ "${CEF_DOWNLOAD_PATH}.sha1" CEF_SHA1)
	# Download the binary distribution and verify the hash.
	message(STATUS "Downloading ${CEF_DOWNLOAD_PATH}...")
	file(
	DOWNLOAD "${CEF_DOWNLOAD_URL}" "${CEF_DOWNLOAD_PATH}"
	EXPECTED_HASH SHA1=${CEF_SHA1}
	SHOW_PROGRESS
	)
endif()
# Extract the binary distribution.
message(STATUS "Extracting ${CEF_DOWNLOAD_PATH}...")
execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar xzf "${CEF_DOWNLOAD_DIR}/${CEF_DOWNLOAD_FILENAME}"
  WORKING_DIRECTORY ${CEF_DOWNLOAD_DIR}
  )
endif()
if (WIN32)
	message (STATUS "try to modifying cef build MT build option to MD" )
	FILE(READ ${CEF_HOME}/cmake/cef_variables.cmake cef_variables_content)
	string(REGEX REPLACE "/MT" "/MD" modified_cef_variables_content "${cef_variables_content}" )
    FILE(WRITE ${CEF_HOME}/cmake/cef_variables.cmake ${modified_cef_variables_content})
	message (STATUS "modifying cef build option ended" )
endif()
# Add the CEF binary distribution's cmake/ directory to the module path.
message (STATUS "try to find cef" )
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_HOME}/cmake")
# Load the CEF configuration (executes FindCEF.cmake).
find_package(CEF QUIET)
#use cef
add_library(MY_CEF_LIBS STATIC IMPORTED)
set_property(TARGET MY_CEF_LIBS PROPERTY IMPORTED_LOCATION_DEBUG ${CEF_HOME}/Debug/libcef.lib)
set_property(TARGET MY_CEF_LIBS PROPERTY IMPORTED_LOCATION_RELEASE ${CEF_HOME}/Release/libcef.lib)
#build cef_dll_wrapper
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	message (STATUS "Configuring cef_dll_wrapper for ${CONF}, please wait.....")
	execute_process ( 
			COMMAND ${CMAKE_COMMAND} 
			-G ${CMAKE_GENERATOR} 
			.
			WORKING_DIRECTORY ${CEF_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
			)		
	reportError (output if error)
	message (STATUS "Building cef_dll_wrapper for ${CONF}, please wait.....")
	execute_process ( 
		COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target libcef_dll_wrapper
		WORKING_DIRECTORY ${CEF_HOME}
		RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
	reportError (output if error)
endforeach ()
#use cef_dll_wrapper
add_library(MY_CEF_DLLWRAPPER_LIBS STATIC IMPORTED)
set_property(TARGET MY_CEF_DLLWRAPPER_LIBS PROPERTY IMPORTED_LOCATION_DEBUG ${CEF_HOME}/libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib)
set_property(TARGET MY_CEF_DLLWRAPPER_LIBS PROPERTY IMPORTED_LOCATION_RELEASE ${CEF_HOME}/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib)
# ApeCefBrowserPlugin
set (HEADERS
	ApeCefBrowserPlugin.h
	ApeCefClientImpl.h
	ApeCefRenderHandlerImpl.h
	)
set (SOURCES
	ApeCefBrowserPlugin.cpp
	ApeCefClientImpl.cpp
	ApeCefRenderHandlerImpl.cpp
	)
include_directories(
	${PROJECT_SOURCE_DIR}/common/include 
	${CEF_HOME}
	${CEF_HOME}/include
	)

add_library (ApeCefBrowserPlugin SHARED ${SOURCES} ${HEADERS})
if (WIN32)
	target_link_libraries (ApeCefBrowserPlugin ApePluginManager ApeScene ApeEventManager ApeSystemConfig ApeMainWindow MY_CEF_LIBS MY_CEF_DLLWRAPPER_LIBS comctl32.lib rpcrt4.lib shlwapi.lib ws2_32.lib)
endif()
set_property (TARGET ApeCefBrowserPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET ApeCefBrowserPlugin PROPERTY FOLDER "Plugins")


