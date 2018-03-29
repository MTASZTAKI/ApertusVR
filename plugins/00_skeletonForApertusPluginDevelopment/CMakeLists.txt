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


# ApeMyPlugin

if (WIN32)
	set (HEADERS
	ApeMyPlugin.h
	)
set (SOURCES
	ApeMyPlugin.cpp
	)
include_directories(
	${PROJECT_SOURCE_DIR}/common/include		#Apertus factory header files from yourDevFolder\ApertusVR\common\include\
	${YOUR3RDPARTY_INCLUDE_DIR}					#3rdParty extension from yourDevFolder\ApertusVR\3rdParty\your3rdParty\
	${RAPIDJSON_INCLUDE_DIR}
	)

add_library (ApeMyPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ApeMyPlugin ApePluginManager ApeScene ApeEventManager ApeSystemConfig ${YOUR3RDPARTY_INCLUDE_DIR})

set_property (TARGET ApeOgreRenderPlugin PROPERTY FOLDER "Plugins")
if (WIN32)
	# Copying the ogre bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreMain_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreMain_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	

endif ()

if (APPLE).......endif()

if (UNIX).......endif()







