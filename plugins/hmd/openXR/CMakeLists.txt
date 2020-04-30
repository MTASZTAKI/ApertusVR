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
	apeOpenXRPlugin.h
	)

set (SOURCES 
	apeOpenXRPlugin.cpp
	)
	
set(OPENXR_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/openxr)
add_library(OPENXR_LIB STATIC IMPORTED)
if (WIN32)
	set_property(TARGET OPENXR_LIB PROPERTY IMPORTED_LOCATION_DEBUG ${OPENXR_HOME}/lib/win64/openxr.lib)
	set_property(TARGET OPENXR_LIB PROPERTY IMPORTED_LOCATION_RELEASE ${OPENXR_HOME}/lib/win64/openxr.lib)
endif ()	

include_directories(
	${OPENXR_HOME}/headers
	)

add_library (apeOpenXRPlugin SHARED ${SOURCES} ${HEADERS})

target_link_libraries (apeOpenXRPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeUserInputMacro OPENXR_LIB)

set_property (TARGET apeOpenXRPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeOpenXRPlugin PROPERTY FOLDER "Plugins/HMD")

if (WIN32)
	#file(COPY ${OPENXR_HOME}/bin/win64/openxr.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	#file(COPY ${OPENXR_HOME}/bin/win64/openxr.pdb DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	#file(COPY ${OPENXR_HOME}/bin/win64/openxr.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	#file(COPY ${OPENXR_HOME}/bin/win64/openxr.pdb DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()


