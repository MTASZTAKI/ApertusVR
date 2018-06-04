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

# build Boost
# execute_process(
# 	COMMAND cmd /c "3rdParty/boost/1.67.0/bootstrap.bat"
# 	RESULT_VARIABLE BOOST_BUILD_RESULT
# )
# message("Boost: build exit code: (${BOOST_BUILD_RESULT})")
# if (${BOOST_BUILD_RESULT} EQUAL 0)
# 	message("Boost: build ok")
# else ()
# 	message(FATAL_ERROR "Boost: build error")
# endif ()

set(Boost_USE_STATIC_LIBS ON)
set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/3rdParty/boost/1.67.0")
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")

set(BOOST_LIB_SYSTEM "${BOOST_LIBRARYDIR}/libboost_system-vc141-mt-x64-1_67.lib")
set(BOOST_LIB_DATETIME "${BOOST_LIBRARYDIR}/libboost_date_time-vc141-mt-x64-1_67.lib")
set(BOOST_LIB_REGEX "${BOOST_LIBRARYDIR}/libboost_regex-vc141-mt-x64-1_67.lib")

set(BOOST_LIB_SYSTEM_D "${BOOST_LIBRARYDIR}/libboost_system-vc141-mt-gd-x64-1_67.lib")
set(BOOST_LIB_DATETIME_D "${BOOST_LIBRARYDIR}/libboost_date_time-vc141-mt-gd-x64-1_67.lib")
set(BOOST_LIB_REGEX_D "${BOOST_LIBRARYDIR}/libboost_regex-vc141-mt-gd-x64-1_67.lib")

# Add all header and cpp files in the directory to the project
set (HEADERS
	ApeWebserverPlugin.h
	)

set (SOURCES
	ApeWebserverPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${BOOST_ROOT}
	${PROJECT_SOURCE_DIR}/3rdParty/crow/include
	)

add_library (ApeWebserverPlugin SHARED ${SOURCES} ${HEADERS})

#find_package(Boost 1.67.0 REQUIRED COMPONENTS system date_time regex)
#if(Boost_FOUND)
#	include_directories(${Boost_INCLUDE_DIR})
#	link_directories(${Boost_LIBRARY_DIRS})
	# Link with dependency libraries
	#target_link_libraries (ApeWebserverPlugin ApePluginManager ApeScene ApeEventManager ApeLogManager Boost::system Boost::date_time Boost::regex)
#else ()
	#message(FATAL_ERROR "Boost: missing libraries.")
#endif()

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

target_link_libraries (ApeWebserverPlugin ApePluginManager ApeScene ApeEventManager ApeLogManager MY_BOOST_SYSTEM MY_BOOST_DATETIME MY_BOOST_REGEX)

set_property (TARGET ApeWebserverPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET ApeWebserverPlugin PROPERTY FOLDER "Plugins")
