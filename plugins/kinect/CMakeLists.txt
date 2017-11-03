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

set (KINECT_HOME ${CMAKE_SOURCE_DIR}/3rdParty/kinect)
set (KINECT_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(KINECT_PLATFORM "macosx64")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(KINECT_PLATFORM "linux64")
  else()
    set(KINECT_PLATFORM "linux32")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(KINECT_PLATFORM "windows64")
  else()
    set(KINECT_PLATFORM "windows32")
  endif()
endif()
#find kinect
if (KINECT_PLATFORM EQUAL "windows64")
	if(EXISTS ${KINECT_HOME}/v2.0_1409/Lib/x64/Kinect20.lib)
		set(KINECT_READY TRUE)
		message (STATUS "leap is installed" )
	else ()
		set(KINECT_READY FALSE)
		message (STATUS "leap is not installed" )
	endif ()
endif ()
#use leap
add_library(MY_KINECT STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_KINECT PROPERTY IMPORTED_LOCATION_DEBUG ${KINECT_HOME}/v2.0_1409/Lib/x64/Kinect20.lib)
	set_property(TARGET MY_KINECT PROPERTY IMPORTED_LOCATION_RELEASE ${KINECT_HOME}/v2.0_1409/Lib/x64/Kinect20.lib)
endif ()

#ApeKinectPlugin
set (HEADERS
	ApeKinectPlugin.h
	resource.h
	stdafx.h
	)

set (SOURCES 
	ApeKinectPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${KINECT_HOME}/v2.0_1409/inc
	)

# Add the dinamic library target to the project
add_library (ApeKinectPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (ApeKinectPlugin ApePluginManager ApeScene ApeEventManager ApeSystemConfig ApeMainWindow MY_KINECT)
	
# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApeKinectPlugin PROPERTY FOLDER "Plugins")




