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

# Add all header and cpp files in the directory to the project
set (HEADERS
	ApeOculusDK2Plugin.h
	)

set (SOURCES 
	ApeOculusDK2Plugin.cpp
	)
	
set(OCULUS_HOME ${CMAKE_SOURCE_DIR}/3rdParty/oculus)
add_library(OCULUS_LIB STATIC IMPORTED)
if (WIN32)
	set_property(TARGET OCULUS_LIB PROPERTY IMPORTED_LOCATION_DEBUG ${OCULUS_HOME}/LibOVR/Lib/x64/VS2015/libovr64d.lib)
	set_property(TARGET OCULUS_LIB PROPERTY IMPORTED_LOCATION_RELEASE ${OCULUS_HOME}/LibOVR/Lib/x64/VS2015/libovr64.lib)
endif ()	

include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	 ${PROJECT_SOURCE_DIR}/macros/userInput 
	${OCULUS_HOME}/LibOVR/Include
	)

add_library (ApeOculusDK2Plugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
if (WIN32)
target_link_libraries (ApeOculusDK2Plugin ApePluginManager ApeSceneManager ApeEventManager ApeLogManager ApeMainWindow ApeUserInputMacro OCULUS_LIB winmm.lib)
endif ()

set_property (TARGET ApeOculusDK2Plugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET ApeOculusDK2Plugin PROPERTY FOLDER "Plugins")


