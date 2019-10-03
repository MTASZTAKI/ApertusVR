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

set (LEAP_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/leapMotion)
set (LEAP_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

#find leap
if(EXISTS ${LEAP_HOME}/lib/x64/Leap.dll)
	set(LEAP_READY TRUE)
	message (STATUS "leap is installed" )
else ()
	set(LEAP_READY FALSE)
	message (STATUS "leap is not installed" )
endif ()

#use leap
add_library(MY_LEAP STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_LEAP PROPERTY IMPORTED_LOCATION_DEBUG ${LEAP_HOME}/lib/x64/Leap.lib)
	set_property(TARGET MY_LEAP PROPERTY IMPORTED_LOCATION_RELEASE ${LEAP_HOME}/lib/x64/Leap.lib)
endif ()

#apeLeapMotionPlugin
set (HEADERS
	apeLeapMotionPlugin.h
	)

set (SOURCES 
	apeLeapMotionPlugin.cpp
	)

include_directories(
	  
	${LEAP_HOME}/include
	)

# Add the dinamic library target to the project
add_library (apeLeapMotionPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (apeLeapMotionPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeCoreConfig MY_LEAP)
	
# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apeLeapMotionPlugin PROPERTY FOLDER "Plugins/Track/Hand")

if (WIN32)
	file(COPY ${LEAP_HOME}/lib/x64/Leap.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${LEAP_HOME}/lib/x64/Leap.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()



