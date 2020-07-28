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

# ogre21deps log begin
# OGREDEPS_BUILD_SDL2 set to FALSE
# ogre21deps log begin

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre21/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre21/build)
endif()
if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre21/build/Dependencies/ogredeps)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre21/build/Dependencies/ogre21deps)
endif()
set (OGRE21_BUILD_DIR ${CMAKE_BINARY_DIR}/ogre21/build)

set(PATH_OGRE21DEPS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre21/Dependencies)
set(PATH_OGRE21DEPS_OUT ${OGRE21_BUILD_DIR}/Dependencies/ogre21deps)
set(PATH_OGRE21 ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre21)
set(OGRE21_READY FALSE)

#find ogre21
message (STATUS "try to find Ogre" )
if(EXISTS ${OGRE21_BUILD_DIR}/bin/Release/OgreMain.dll)
	set(OGRE21_READY TRUE)
	message (STATUS "Ogre21 is installed" )
else ()
	set(OGRE21_READY FALSE)
	message (STATUS "Ogre21 is not installed" )
endif ()

#try ogre21
if (NOT OGRE21_READY)
#build ogre21deps
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre21 dependencies for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND}
			-OGREDEPS_BUILD_SDL2:BOOL=FALSE			
			-G ${CMAKE_GENERATOR}
			${PATH_OGRE21DEPS}
			WORKING_DIRECTORY ${PATH_OGRE21DEPS_OUT}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
			)
		reportError (output if error)
		message (STATUS "Building ogre21 dependencies for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${PATH_OGRE21DEPS_OUT} --config ${CONF} --target install
			WORKING_DIRECTORY ${PATH_OGRE21DEPS_OUT}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
#build ogre21
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre21 for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-DOGRE_DEPENDENCIES_DIR:PATH=${PATH_OGRE21DEPS_OUT}/ogre21deps
				-DOGRE_BUILD_SAMPLES:BOOL=FALSE
				-DOGRE_INSTALL_SAMPLES=OFF
				-G ${CMAKE_GENERATOR} 
				${PATH_OGRE21}
				WORKING_DIRECTORY ${OGRE21_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre21 for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OGRE21_BUILD_DIR} --config ${CONF} --target install
			WORKING_DIRECTORY ${OGRE21_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()

add_library(MY_OGRE21 STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE21 PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE21_BUILD_DIR}/sdk/lib/Debug/OgreMain_d.lib)
	set_property(TARGET MY_OGRE21 PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE21_BUILD_DIR}/sdk/lib/Release/OgreMain.lib)
endif ()

add_library(MY_OGRE21_PBS STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE21_PBS PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE21_BUILD_DIR}/sdk/lib/Debug/OgreHlmsPbs_d.lib)
	set_property(TARGET MY_OGRE21_PBS PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE21_BUILD_DIR}/sdk/lib/Release/OgreHlmsPbs.lib)
endif ()

add_library(MY_OGRE21_UNLIT STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE21_UNLIT PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE21_BUILD_DIR}/sdk/lib/Debug/OgreHlmsUnlit_d.lib)
	set_property(TARGET MY_OGRE21_UNLIT PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE21_BUILD_DIR}/sdk/lib/Release/OgreHlmsUnlit.lib)
endif ()

#find ogre_gltf
set (ENV{OGRE_HOME} ${PATH_OGRE21})
set (ENV{OGRE_SOURCE} ${PATH_OGRE21})
set (ENV{OGRE_SDK} ${OGRE21_BUILD_DIR}/sdk)
set (ENV{OGRE_BUILD} ${OGRE21_BUILD_DIR})
set (ENV{OGRE_DEPENDENCIES_DIR} ${PATH_OGRE21DEPS_OUT}/ogre21deps)
set (ENV{OGRE_HlmsCommon_INCLUDE_DIRS} ${OGRE21_BUILD_DIR}/sdk/include/OGRE/Hlms/Common)
set (ENV{OGRE_HlmsPbs_INCLUDE_DIRS} ${OGRE21_BUILD_DIR}/sdk/include/OGRE/Hlms/Pbs) 
message (STATUS "try to find Ogre_GLTF" )
set(PATH_OGRE_GLTF ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_gltf)
set(OGRE_GLTF_READY FALSE)
if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre_gltf/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre_gltf/build)
endif()
set (OGRE_GLTF_BUILD_DIR ${CMAKE_BINARY_DIR}/ogre_gltf/build)
if(EXISTS ${OGRE_GLTF_BUILD_DIR}/release/Ogre_glTF.dll)
	set(OGRE_GLTF_READY TRUE)
	message (STATUS "Ogre_GLTF is installed" )
else ()
	set(OGRE_GLTF_READY FALSE)
	message (STATUS "Ogre_GLTF is not installed" )
endif ()
#try ogre_gltf
if (NOT OGRE_GLTF_READY)
#build ogre_gltf
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre_gltf for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				${PATH_OGRE_GLTF}
				WORKING_DIRECTORY ${OGRE_GLTF_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_gltf for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OGRE_GLTF_BUILD_DIR} --config ${CONF} --target Ogre_glTF
			WORKING_DIRECTORY ${OGRE_GLTF_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
add_library(MY_OGRE_GLTF STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE_GLTF PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE_GLTF_BUILD_DIR}/Debug/Ogre_glTF_d.lib)
	set_property(TARGET MY_OGRE_GLTF PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE_GLTF_BUILD_DIR}/Release/Ogre_glTF.lib)
endif ()
# apeOgre21RenderPlugin
set (HEADERS
	apeOgre21RenderPlugin.h
	apeOgre21RenderPluginConfigs.h
	apeOgre21Conversions.h
	apeOgre21Utilities.h
	)
set (SOURCES
	apeOgre21RenderPlugin.cpp
	)
include_directories(
	$ENV{OGRE_BUILD}/include
	$ENV{OGRE_SDK}/include/OGRE
	$ENV{OGRE_SDK}/include/OGRE/RenderSystems/Direct3D11
	$ENV{OGRE_SDK}/include/OGRE/RenderSystems/GL
	$ENV{OGRE_SDK}/include/OGRE/Hlms/Common
	$ENV{OGRE_SDK}/include/OGRE/Hlms/Pbs
	$ENV{OGRE_SDK}/include/OGRE/Hlms/Unlit
	${PATH_OGRE21}/OgreMain/include
	${PATH_OGRE_GLTF}/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)
add_library (apeOgre21RenderPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (apeOgre21RenderPlugin apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro MY_OGRE21 MY_OGRE21_PBS MY_OGRE21_UNLIT MY_OGRE_GLTF  )

set_property (TARGET apeOgre21RenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeOgre21RenderPlugin PROPERTY FOLDER "Plugins/Render")
if (WIN32)
	file(COPY ${OGRE21_BUILD_DIR}/bin/debug/OgreMain_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE21_BUILD_DIR}/bin/debug/RenderSystem_GL3Plus_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE21_BUILD_DIR}/bin/debug/RenderSystem_Direct3D11_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE21_BUILD_DIR}/bin/debug/OgreHlmsPbs_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE21_BUILD_DIR}/bin/debug/OgreHlmsUnlit_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_GLTF_BUILD_DIR}/debug/Ogre_glTF_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE21_BUILD_DIR}/bin/release/OgreMain.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE21_BUILD_DIR}/bin/release/RenderSystem_GL3Plus.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE21_BUILD_DIR}/bin/release/RenderSystem_Direct3D11.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE21_BUILD_DIR}/bin/release/OgreHlmsPbs.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE21_BUILD_DIR}/bin/release/OgreHlmsUnlit.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_GLTF_BUILD_DIR}/release/Ogre_glTF.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()


