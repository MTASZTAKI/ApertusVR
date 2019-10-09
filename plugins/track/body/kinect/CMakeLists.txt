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

set (KINECT_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/kinect)
set (KINECT_READY FALSE)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

#find kinect
if(EXISTS ${KINECT_HOME}/v2.0_1409/Lib/x64/Kinect20.lib)
	set(KINECT_READY TRUE)
	message (STATUS "kinect is installed" )
else ()
	set(KINECT_READY FALSE)
	message (STATUS "kinect is not installed" )
endif ()

#use kinect
add_library(MY_KINECT STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_KINECT PROPERTY IMPORTED_LOCATION_DEBUG ${KINECT_HOME}/v2.0_1409/Lib/x64/Kinect20.lib)
	set_property(TARGET MY_KINECT PROPERTY IMPORTED_LOCATION_RELEASE ${KINECT_HOME}/v2.0_1409/Lib/x64/Kinect20.lib)
endif ()

#apeKinectPlugin
set (HEADERS
	apeKinectPlugin.h
	resource.h
	stdafx.h
	)

set (SOURCES 
	apeKinectPlugin.cpp
	)

include_directories(
	${KINECT_HOME}/v2.0_1409/inc
	)

# Add the dinamic library target to the project
add_library (apeKinectPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (apeKinectPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeCoreConfig apeUserInputMacro MY_KINECT)
	
# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apeKinectPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET apeKinectPlugin PROPERTY FOLDER "Plugins/Track/Body")

file(COPY ${KINECT_HOME}/v2.0_1409/Bin/Kinect20.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
file(COPY ${KINECT_HOME}/v2.0_1409/Bin/Kinect20.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
