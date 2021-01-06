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

set (BOOST_READY FALSE)
if(EXISTS ${CMAKE_BINARY_DIR}/boost/lib/libboost_regex-vc141-mt-x64-1_67.lib)
	set(BOOST_READY TRUE)
	message (STATUS "boost is installed" )
else ()
	set(BOOST_READY FALSE)
	message (STATUS "boost is not installed" )
endif ()

if (NOT BOOST_READY)
	set(BOOST_DOWNLOAD_URL "http://srv.mvv.sztaki.hu/Ape/3rdPartiesBin/boost.tar.bz2")
	set(BOOST_DOWNLOAD_PATH ${CMAKE_BINARY_DIR})

	if (NOT EXISTS "${BOOST_DOWNLOAD_PATH}/boost.tar.bz2")
		message(STATUS "Downloading Boost...")
		file(
		DOWNLOAD "${BOOST_DOWNLOAD_URL}" "${BOOST_DOWNLOAD_PATH}/boost.tar.bz2"
		RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
		)		
		reportError (output if error)
	endif()
	
	if (NOT EXISTS "${BOOST_DOWNLOAD_PATH}/Boost")
		file(MAKE_DIRECTORY "${BOOST_DOWNLOAD_PATH}/Boost")
	endif()

	message(STATUS "Extracting Boost to " "${BOOST_DOWNLOAD_PATH}/Boost")
	execute_process(
	  COMMAND ${CMAKE_COMMAND} -E tar xzf "${BOOST_DOWNLOAD_PATH}/boost.tar.bz2"
	  WORKING_DIRECTORY "${BOOST_DOWNLOAD_PATH}/Boost"
	  RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
	)		
	reportError (output if error)
endif ()

set(BOOST_LIBRARYDIR "${CMAKE_BINARY_DIR}/boost/lib")
set(BOOST_LIB_SYSTEM "${BOOST_LIBRARYDIR}/libboost_system-vc141-mt-x64-1_67.lib")
set(BOOST_LIB_DATETIME "${BOOST_LIBRARYDIR}/libboost_date_time-vc141-mt-x64-1_67.lib")
set(BOOST_LIB_REGEX "${BOOST_LIBRARYDIR}/libboost_regex-vc141-mt-x64-1_67.lib")

set(BOOST_LIB_SYSTEM_D "${BOOST_LIBRARYDIR}/libboost_system-vc141-mt-gd-x64-1_67.lib")
set(BOOST_LIB_DATETIME_D "${BOOST_LIBRARYDIR}/libboost_date_time-vc141-mt-gd-x64-1_67.lib")
set(BOOST_LIB_REGEX_D "${BOOST_LIBRARYDIR}/libboost_regex-vc141-mt-gd-x64-1_67.lib")

# Add all header and cpp files in the directory to the project
set (HEADERS
	apeWebSocketServerPlugin.h
	)

set (SOURCES
	apeWebSocketServerPlugin.cpp
	)

include_directories(
	${CMAKE_BINARY_DIR}/Boost/include/boost-1_67
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/crow/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)

add_library (apeWebSocketServerPlugin SHARED ${SOURCES} ${HEADERS})

add_library(MY_BOOST_SYSTEM STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_BOOST_SYSTEM PROPERTY IMPORTED_LOCATION_DEBUG ${BOOST_LIB_SYSTEM_D})
	set_property(TARGET MY_BOOST_SYSTEM PROPERTY IMPORTED_LOCATION_RELEASE ${BOOST_LIB_SYSTEM})
endif ()

add_library(MY_BOOST_DATETIME STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_BOOST_DATETIME PROPERTY IMPORTED_LOCATION_DEBUG ${BOOST_LIB_DATETIME_D})
	set_property(TARGET MY_BOOST_DATETIME PROPERTY IMPORTED_LOCATION_RELEASE ${BOOST_LIB_DATETIME})
endif ()

add_library(MY_BOOST_REGEX STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_BOOST_REGEX PROPERTY IMPORTED_LOCATION_DEBUG ${BOOST_LIB_REGEX_D})
	set_property(TARGET MY_BOOST_REGEX PROPERTY IMPORTED_LOCATION_RELEASE ${BOOST_LIB_REGEX})
endif ()

target_link_libraries (apeWebSocketServerPlugin apePluginManager apeSceneManager apeEventManager apeLogManager MY_BOOST_SYSTEM MY_BOOST_DATETIME MY_BOOST_REGEX)

set_property (TARGET apeWebSocketServerPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeWebSocketServerPlugin PROPERTY FOLDER "Plugins/LanguageAPI")

set_property(TARGET apeWebSocketServerPlugin PROPERTY CXX_STANDARD 14)
set_property(TARGET apeWebSocketServerPlugin PROPERTY CXX_STANDARD_REQUIRED ON)
