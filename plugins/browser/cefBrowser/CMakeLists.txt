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

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

#cef
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

if (${APE_PLUGIN_CefBrowser_VideoCodecsSupport_FromSource})
	if (MSVC15)
		if (CEF_PLATFORM EQUAL "windows64")
			message (STATUS "Try to get and build cef, this require at least 200GB free space and around 48 hours @i7 CPU")
			execute_process ( 
					COMMAND 
					set CEF_USE_GN=1
					set GN_ARGUMENTS=--ide=vs2017 --sln=cef --filters=//cef/*
					set GN_DEFINES=proprietary_codecs=true ffmpeg_branding="Chrome"
					set CEF_ARCHIVE_FORMAT=tar.bz2
					python ${CMAKE_CURRENT_SOURCE_DIR}\3rdParty\cef_source\automate-git.py --download-dir=${CMAKE_BINARY_DIR}\cef_source --branch=trunk --x64-build
					RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
					)		
			reportError (output if error)
			message (STATUS "Cef build succesfully finished")	
		endif()
	endif()
endif ()

if (${APE_PLUGIN_CefBrowser_VideoCodecsSupport})
	set(CEF_VERSION "3.3113.1650.g6da2bbf")
	set(CEF_DOWNLOAD_SERVER "http://srv.mvv.sztaki.mta.hu/ape/3rdPartiesBin/cef_ffmpeg/")
else()
	set(CEF_VERSION "3.3071.1643.ga83b8e9")
	set(CEF_DOWNLOAD_SERVER "http://opensource.spotify.com/cefbuilds/")
endif()	

# Specify the CEF distribution version.

set(CEF_DISTRIBUTION "cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")
if (NOT EXISTS ${CMAKE_BINARY_DIR}/${CEF_DISTRIBUTION}/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CEF_DISTRIBUTION}/build)
endif()
set(CEF_BUILD_DIR ${CMAKE_BINARY_DIR}/${CEF_DISTRIBUTION}/build)
set(CEF_HOME ${CEF_BUILD_DIR}/${CEF_DISTRIBUTION})
set(CEF_DLLWRAPPER_HOME ${CEF_HOME}/libcef_dll)
set(CEF_DOWNLOAD_DIR ${CEF_BUILD_DIR})

set(CEF_READY FALSE)
if(EXISTS ${CEF_HOME}/Release/libcef.lib)
	set(CEF_READY TRUE)
	message (STATUS "libcef is installed" )
else ()
	set(CEF_READY FALSE)
	message (STATUS "libcef is not installed" )
endif ()
if (NOT CEF_READY)
# The location where we expect the extracted binary distribution.
set(CEF_ROOT "${CEF_DOWNLOAD_DIR}/${CEF_DISTRIBUTION}" CACHE INTERNAL "CEF_ROOT")
# Download and/or extract the binary distribution if necessary.
if(NOT IS_DIRECTORY "${CEF_ROOT}")
set(CEF_DOWNLOAD_FILENAME "${CEF_DISTRIBUTION}.tar.bz2")
set(CEF_DOWNLOAD_PATH "${CEF_DOWNLOAD_DIR}/${CEF_DOWNLOAD_FILENAME}")
if(NOT EXISTS "${CEF_DOWNLOAD_PATH}")
	set(CEF_DOWNLOAD_URL "${CEF_DOWNLOAD_SERVER}${CEF_DOWNLOAD_FILENAME}")
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
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_BUILD_DIR}/cmake")
#build cef_dll_wrapper
foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
	message (STATUS "Configuring cef_dll_wrapper for ${CONF}, please wait.....")
	execute_process ( 
			COMMAND ${CMAKE_COMMAND} 
			-G ${CMAKE_GENERATOR} 
			${CEF_HOME}
			WORKING_DIRECTORY ${CEF_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
			)		
	reportError (output if error)
	message (STATUS "Building cef_dll_wrapper for ${CONF}, please wait.....")
	execute_process ( 
		COMMAND ${CMAKE_COMMAND} --build ${CEF_BUILD_DIR} --config ${CONF} --target libcef_dll_wrapper
		WORKING_DIRECTORY ${CEF_BUILD_DIR}
		RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
	reportError (output if error)
	#message (STATUS "Building cefsimple for ${CONF}, please wait.....")
	#execute_process ( 
	#	COMMAND ${CMAKE_COMMAND} --build CEF_BUILD_DIR --config ${CONF} --target cefsimple
	#	WORKING_DIRECTORY ${CEF_BUILD_DIR}
	#	RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
	#reportError (output if error)
endforeach ()
endif ()
#use cef
add_library(MY_CEF_LIBS STATIC IMPORTED)
set_property(TARGET MY_CEF_LIBS PROPERTY IMPORTED_LOCATION_DEBUG ${CEF_HOME}/Debug/libcef.lib)
set_property(TARGET MY_CEF_LIBS PROPERTY IMPORTED_LOCATION_RELEASE ${CEF_HOME}/Release/libcef.lib)
#use cef_dll_wrapper
add_library(MY_CEF_DLLWRAPPER_LIBS STATIC IMPORTED)
set_property(TARGET MY_CEF_DLLWRAPPER_LIBS PROPERTY IMPORTED_LOCATION_DEBUG ${CEF_BUILD_DIR}/libcef_dll_wrapper/Debug/libcef_dll_wrapper.lib)
set_property(TARGET MY_CEF_DLLWRAPPER_LIBS PROPERTY IMPORTED_LOCATION_RELEASE ${CEF_BUILD_DIR}/libcef_dll_wrapper/Release/libcef_dll_wrapper.lib)
# apeCefBrowserPlugin
set (HEADERS
	apeCefBrowserPlugin.h
	apeCefClientImpl.h
	apeCefRenderHandlerImpl.h
	apeCefLifeSpanHandlerImpl.h
	apeCefKeyboardHandlerImpl.h
	)
set (SOURCES
	apeCefBrowserPlugin.cpp
	apeCefClientImpl.cpp
	apeCefRenderHandlerImpl.cpp
	apeCefLifeSpanHandlerImpl.cpp
	apeCefKeyboardHandlerImpl.cpp
	)
include_directories(
	${CEF_HOME}
	${CEF_HOME}/include
	)

add_library (apeCefBrowserPlugin SHARED ${SOURCES} ${HEADERS})
if (WIN32)
	target_link_libraries (apeCefBrowserPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeCoreConfig MY_CEF_LIBS MY_CEF_DLLWRAPPER_LIBS comctl32.lib rpcrt4.lib shlwapi.lib ws2_32.lib)
endif()
set_property (TARGET apeCefBrowserPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET apeCefBrowserPlugin PROPERTY FOLDER "Plugins/Browser/CefBrowser")
if (WIN32)
	file(COPY ${CEF_HOME}/debug/chrome_elf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/d3dcompiler_43.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/d3dcompiler_47.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/libcef.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/libEGL.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/widevinecdmadapter.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/natives_blob.bin DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/debug/snapshot_blob.bin DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/locales DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/cef.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/cef_100_percent.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/cef_200_percent.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/cef_extensions.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/devtools_resources.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/resources/icudtl.dat DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${CEF_HOME}/release/chrome_elf.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/d3dcompiler_43.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/d3dcompiler_47.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/libcef.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/libEGL.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/widevinecdmadapter.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/natives_blob.bin DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/release/snapshot_blob.bin DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/locales DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/cef.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/cef_100_percent.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/cef_200_percent.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/cef_extensions.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/devtools_resources.pak DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${CEF_HOME}/resources/icudtl.dat DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()

add_subdirectory (cefSubProcessApp)




