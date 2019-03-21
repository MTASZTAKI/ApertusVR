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

set (LEAP_HOME ${CMAKE_SOURCE_DIR}/3rdParty/leapMotion)
set (LEAP_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(LEAP_PLATFORM "macosx64")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(LEAP_PLATFORM "linux64")
  else()
    set(LEAP_PLATFORM "linux32")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(LEAP_PLATFORM "windows64")
  else()
    set(LEAP_PLATFORM "windows32")
  endif()
endif()
#find leap
if (LEAP_PLATFORM EQUAL "windows64")
	if(EXISTS ${LEAP_HOME}/lib/x64/Leap.dll)
		set(LEAP_READY TRUE)
		message (STATUS "leap is installed" )
	else ()
		set(LEAP_READY FALSE)
		message (STATUS "leap is not installed" )
	endif ()
endif ()
#use leap
add_library(MY_LEAP STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_LEAP PROPERTY IMPORTED_LOCATION_DEBUG ${LEAP_HOME}/lib/x64/Leap.lib)
	set_property(TARGET MY_LEAP PROPERTY IMPORTED_LOCATION_RELEASE ${LEAP_HOME}/lib/x64/Leap.lib)
endif ()

#ApeLeapMotionPlugin
set (HEADERS
	ApeLeapMotionPlugin.h
	)

set (SOURCES 
	ApeLeapMotionPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${LEAP_HOME}/include
	)

# Add the dinamic library target to the project
add_library (ApeLeapMotionPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (ApeLeapMotionPlugin ApePluginManager ApeSceneManager ApeEventManager ApeLogManager ApeSystemConfig ApeMainWindow MY_LEAP)
	
# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApeLeapMotionPlugin PROPERTY FOLDER "Plugins")

if (WIN32)
	# todo_ApeLeapMotionPlugin are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the leap bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/Leap.dll)
		file(COPY ${LEAP_HOME}/lib/x64/Leap.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/Leap.dll)
		file(COPY ${LEAP_HOME}/lib/x64/Leap.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()



