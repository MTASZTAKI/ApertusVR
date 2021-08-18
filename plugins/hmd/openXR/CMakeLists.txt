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

if (NOT EXISTS ${CMAKE_BINARY_DIR}/openxr/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/openxr/build)
endif()
set (OPENXR_BUILD_DIR ${CMAKE_BINARY_DIR}/openxr/build)
set (OPENXR_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/openxr)
set (OPENXR_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

#find openxr
if(EXISTS ${OPENXR_BUILD_DIR}/src/loader/Debug/openxr_loader.lib AND EXISTS ${OPENXR_BUILD_DIR}/src/loader/Release/openxr_loader.lib)
	set(OPENXR_READY TRUE)
	message (STATUS "openxr is installed" )
else ()
	set(OPENXR_READY FALSE)
	message (STATUS "openxr is not installed" )
endif ()
if (NOT OPENXR_READY)
#build openxr
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring openxr for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				${OPENXR_HOME}
				WORKING_DIRECTORY ${OPENXR_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building openxr for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OPENXR_BUILD_DIR} --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${OPENXR_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use openxr
add_library(MY_OPENXR STATIC IMPORTED)
add_library(MY_OPENXR_GFXWRAPPER STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OPENXR PROPERTY IMPORTED_LOCATION_DEBUG ${OPENXR_BUILD_DIR}/src/loader/Debug/openxr_loader.lib)
	set_property(TARGET MY_OPENXR PROPERTY IMPORTED_LOCATION_RELEASE ${OPENXR_BUILD_DIR}/src/loader/Release/openxr_loader.lib)
	set_property(TARGET MY_OPENXR_GFXWRAPPER PROPERTY IMPORTED_LOCATION_DEBUG ${OPENXR_HOME}/lib/debug/openxr-gfxwrapper.lib)
	set_property(TARGET MY_OPENXR_GFXWRAPPER PROPERTY IMPORTED_LOCATION_RELEASE ${OPENXR_HOME}/lib/release/openxr-gfxwrapper.lib)
endif ()

# Add all header and cpp files in the directory to the project
set (HEADERS
	apeOpenXRPlugin.h
	)

set (SOURCES 
	apeOpenXRPlugin.cpp
	)	

include_directories(
	${OPENXR_HOME}/include
	${OPENXR_BUILD_DIR}/src
	${CMAKE_SOURCE_DIR}/plugins/virtualLearningFactory/sceneLoader/3rdParty
	)

find_package(OpenGL REQUIRED)
add_library (apeOpenXRPlugin SHARED ${SOURCES} ${HEADERS})

if (WIN32)
	target_link_libraries (apeOpenXRPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeUserInputMacro MY_OPENXR MY_OPENXR_GFXWRAPPER d3d11 d3dcompiler dxgi ${OPENGL_LIBRARIES})
else()
	target_link_libraries (apeOpenXRPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeUserInputMacro MY_OPENXR MY_OPENXR_GFXWRAPPER)
endif()

set_property (TARGET apeOpenXRPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

if (WIN32)
	set_property (TARGET apeOpenXRPlugin PROPERTY COMPILE_DEFINITIONS
	XR_USE_PLATFORM_WIN32
	XR_USE_GRAPHICS_API_D3D11
	XR_USE_GRAPHICS_API_OPENGL
	)
endif()

set_property (TARGET apeOpenXRPlugin PROPERTY FOLDER "Plugins/HMD")



