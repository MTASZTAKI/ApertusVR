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

set(Boost_USE_STATIC_LIBS ON)

set(BOOST_VERSION "1.67.0")
message(STATUS "Boost: BOOST_VERSION: ${BOOST_VERSION}")

set(BOOST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/boost/${BOOST_VERSION}")
message(STATUS "Boost: BOOST_ROOT: ${BOOST_ROOT}")
if (NOT EXISTS ${BOOST_ROOT})
	message(FATAL_ERROR "Boost: BOOST_ROOT: ${BOOST_ROOT} does not exist!")
endif ()

if (WIN32)
	set(BOOST_BUILD_SYSTEM_PATH "${BOOST_ROOT}/b2.exe")
else ()
	set(BOOST_BUILD_SYSTEM_PATH "${BOOST_ROOT}/b2")
endif ()
message(STATUS "Boost: BOOST_BUILD_SYSTEM_PATH: ${BOOST_BUILD_SYSTEM_PATH}")

set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")
message(STATUS "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")

	set(BOOST_BUILD_SYSTEM_READY FALSE)
	set(BOOST_BUILD_READY FALSE)

	if (NOT EXISTS ${BOOST_BUILD_SYSTEM_PATH})
		# build Boost's build system ( bjam / b2 )
		execute_process(
			COMMAND cmd /c "cd ${BOOST_ROOT} && bootstrap.bat"
			RESULT_VARIABLE BOOST_BUILD_SYSTEM_RESULT
		)
		message("Boost: build exit code: (${BOOST_BUILD_SYSTEM_RESULT})")
		if (${BOOST_BUILD_SYSTEM_RESULT} EQUAL 0)
			message("Boost: BOOST_BUILD_SYSTEM_RESULT: ok")
			set(BOOST_BUILD_SYSTEM_READY TRUE)
		else ()
			message(FATAL_ERROR "Boost: BOOST_BUILD_SYSTEM_RESULT: error")
		endif ()
	else ()
		set(BOOST_BUILD_SYSTEM_READY TRUE)
	endif ()

	if (${BOOST_BUILD_SYSTEM_READY})
		# build Boost

		if (MSVC_VERSION EQUAL 1900) # VS 14.0 (v140 toolset)
			set(BOOST_BUILD_TOOLSET "msvc-14.0")
		elseif (MSVC_VERSION GREATER_EQUAL 1910) # VS 15.0 (v141 toolset)
			set(BOOST_BUILD_TOOLSET "msvc-14.1")
		endif ()

		execute_process(
			COMMAND cmd /c "cd ${BOOST_ROOT} && b2 -j4 --toolset=${BOOST_BUILD_TOOLSET} architecture=x86 address-model=64 --with-system --with-date_time --with-regex"
			RESULT_VARIABLE BOOST_BUILD_RESULT
		)
		message("Boost: build exit code: (${BOOST_BUILD_RESULT})")
		if (${BOOST_BUILD_RESULT} EQUAL 0)
			message("Boost: BOOST_BUILD_RESULT: ok")
			set(BOOST_BUILD_READY TRUE)
		else ()
			message(FATAL_ERROR "Boost: BOOST_BUILD_RESULT: error")
		endif ()
	else ()
		message(FATAL_ERROR "Boost: BOOST_BUILD_SYSTEM_READY: false, exiting!")
	endif ()



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
	${BOOST_ROOT}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/crow/include
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
