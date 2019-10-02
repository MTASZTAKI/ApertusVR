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

# ogre patch log begin
# Dependencies folder was copied to the ogre root folder
# FindDirectX11.cmake (3rdParty/ogre/CMake/Packages/FindDirectX11.cmake) was patched to find DirectX11 on Windows 8.1
# ogre patch log begin

# ogre_procedural patch log begin
# CMakeLists.txt was modified in the ogre_procedural root folder
# FindOGRE.cmake was modified in the ogre_procedural root folder/Cmake/Packages folder
# ogre_procedural patch log begin

# ogre_skyx patch log begin
# CMakeLists.txt was modified in the ogre_skyx folder
# ogre_skyx patch log begin

# ogre_hydrax patch log begin
# CMakeLists.txt was modified in the ogre_hydrax folder
# ogre_hydrax patch log begin

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre/build)
endif()
if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre/build/Dependencies/ogredeps)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre/build/Dependencies/ogredeps)
endif()
set (OGRE_BUILD_DIR ${CMAKE_BINARY_DIR}/ogre/build)

set(PATH_OGREDEPS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre/Dependencies)
set(PATH_OGREDEPS_OUT ${OGRE_BUILD_DIR}/Dependencies/ogredeps)
set(PATH_OGRE ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre)
set(OGRE_READY FALSE)

#find ogre
message (STATUS "try to find Ogre" )
if(EXISTS ${OGRE_BUILD_DIR}/bin/Release/OgreMain.dll)
	set(OGRE_READY TRUE)
	message (STATUS "Ogre is installed" )
else ()
	set(OGRE_READY FALSE)
	message (STATUS "Ogre is not installed" )
endif ()

#try ogre
if (NOT OGRE_READY)
#build ogredeps
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre dependencies for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} 
			-G ${CMAKE_GENERATOR}
			${PATH_OGREDEPS}
			WORKING_DIRECTORY ${PATH_OGREDEPS_OUT}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
			)
		reportError (output if error)
		message (STATUS "Building ogre dependencies for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${PATH_OGREDEPS_OUT} --config ${CONF} --target install
			WORKING_DIRECTORY ${PATH_OGREDEPS_OUT}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
#build ogre
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-DOGRE_DEPENDENCIES_DIR:PATH=${PATH_OGREDEPS_OUT}
				-DOGRE_BUILD_SAMPLES:BOOL=FALSE
				-DOGRE_INSTALL_SAMPLES=OFF
				-G ${CMAKE_GENERATOR} 
				${PATH_OGRE}
				WORKING_DIRECTORY ${OGRE_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OGRE_BUILD_DIR} --config ${CONF} --target install
			WORKING_DIRECTORY ${OGRE_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
add_library(MY_OGRE STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OGRE PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE_BUILD_DIR}/bin/Debug/OgreMain_d.lib)
	set_property(TARGET MY_OGRE PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE_BUILD_DIR}/bin/Release/OgreMain.lib)
endif ()

#find ogre_procedural
set (ENV{OGRE_HOME} ${PATH_OGRE})
set (ENV{OGRE_SOURCE} ${PATH_OGRE})
set (ENV{OGRE_SDK} ${OGRE_BUILD_DIR}/sdk)
set (ENV{OGRE_BUILD} ${OGRE_BUILD_DIR})
set (ENV{OGRE_DEPENDENCIES_DIR} ${PATH_OGREDEPS_OUT})
message (STATUS "try to find Ogre_procedural" )
set(PATH_OGRE_PROCEDURAL ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_procedural)
set(OGRE_PROCEDURAL_READY FALSE)
if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre_procedural/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre_procedural/build)
endif()
set (OGRE_PROCEDURAL_BUILD_DIR ${CMAKE_BINARY_DIR}/ogre_procedural/build)
if(EXISTS ${OGRE_PROCEDURAL_BUILD_DIR}/bin/release/OgreProcedural.dll)
	set(OGRE_PROCEDURAL_READY TRUE)
	message (STATUS "OgreProcedural is installed" )
else ()
	set(OGRE_PROCEDURAL_READY FALSE)
	message (STATUS "OgreProcedural is not installed" )
endif ()
#try ogre_procedural
if (NOT OGRE_PROCEDURAL_READY)
#build ogre_procedural
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre_procedural for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				${PATH_OGRE_PROCEDURAL}
				WORKING_DIRECTORY ${OGRE_PROCEDURAL_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_procedural for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OGRE_PROCEDURAL_BUILD_DIR} --config ${CONF} --target install
			WORKING_DIRECTORY ${OGRE_PROCEDURAL_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
add_library(MY_PROCEDURAL STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_PROCEDURAL PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE_PROCEDURAL_BUILD_DIR}/bin/Debug/OgreProcedural_d.lib)
	set_property(TARGET MY_PROCEDURAL PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE_PROCEDURAL_BUILD_DIR}/bin/Release/OgreProcedural.lib)
endif ()

#find ogre_hydrax
set (ENV{OGRE_HYDRAX_HOME} ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_hydrax/src)
if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre_hydrax/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre_hydrax/build)
endif()
set (OGRE_HYDRAX_BUILD_DIR ${CMAKE_BINARY_DIR}/ogre_hydrax/build)
#option(APE_PLUGIN_OgreRender_HydraxNew "Use OgreRenderPluginHydraxNew?" ON )
#if (${APE_PLUGIN_OgreRender_HydraxNew})
#	set (HYDRAX_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_hydrax_new)
#else ()
	set (HYDRAX_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_hydrax/src)
	#set (HYDRAX_BIN_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_hydrax)
#endif ()
set (HYDRAX_READY FALSE)
if(EXISTS ${OGRE_HYDRAX_BUILD_DIR}/Debug/Hydrax.dll AND EXISTS ${OGRE_HYDRAX_BUILD_DIR}/Release/Hydrax.dll)
	set(HYDRAX_READY TRUE)
	message (STATUS "ogre_hydrax is installed" )
else ()
	set(HYDRAX_READY FALSE)
	message (STATUS "ogre_hydrax is not installed" )
endif ()
if (NOT HYDRAX_READY)
#build ogre_hydrax
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre_hydrax for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				${HYDRAX_HOME}
				WORKING_DIRECTORY ${OGRE_HYDRAX_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_hydrax for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OGRE_HYDRAX_BUILD_DIR} --config ${CONF} --target Hydrax
			WORKING_DIRECTORY ${OGRE_HYDRAX_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use ogre_hydrax
add_library(MY_HYDRAX STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_HYDRAX PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE_HYDRAX_BUILD_DIR}/Debug/Hydrax.lib)
	set_property(TARGET MY_HYDRAX PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE_HYDRAX_BUILD_DIR}/Release/Hydrax.lib)
endif ()

#find ogre_skyx
if (NOT EXISTS ${CMAKE_BINARY_DIR}/ogre_sky/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ogre_sky/build)
endif()
set (OGRE_SKY_BUILD_DIR ${CMAKE_BINARY_DIR}/ogre_sky/build)
set (SKYX_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ogre_skyx)
set (SKYX_READY FALSE)
if(EXISTS ${OGRE_SKY_BUILD_DIR}/Debug/Skyx.dll AND EXISTS ${OGRE_SKY_BUILD_DIR}/Release/Skyx.dll)
	set(SKYX_READY TRUE)
	message (STATUS "ogre_skyx is installed" )
else ()
	set(SKYX_READY FALSE)
	message (STATUS "ogre_skyx is not installed" )
endif ()
if (NOT SKYX_READY)
#build ogre_skyx
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre_skyx for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				${SKYX_HOME}
				WORKING_DIRECTORY ${OGRE_SKY_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_skyx for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OGRE_SKY_BUILD_DIR} --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${OGRE_SKY_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use ogre_skyx
add_library(MY_SKYX STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_SKYX PROPERTY IMPORTED_LOCATION_DEBUG ${OGRE_SKY_BUILD_DIR}/Debug/Skyx.lib)
	set_property(TARGET MY_SKYX PROPERTY IMPORTED_LOCATION_RELEASE ${OGRE_SKY_BUILD_DIR}/Release/Skyx.lib)
endif ()
# apeOgreRenderPlugin
set (HEADERS
	apeOgreRenderPlugin.h
	apeOgreRenderPluginConfigs.h
	apeOgreMovableText.h
	apeOgrePointCloud.h
	apeOgreShaderGeneratorResolver.h
	apeOgreConversions.h
	apeOgreUtilities.h
	)
set (SOURCES
	apeOgreRenderPlugin.cpp
	apeOgreMovableText.cpp
	apeOgrePointCloud.cpp
	)
include_directories(
	${PATH_OGRE}/include
	${PATH_OGRE_PROCEDURAL}/include
	${HYDRAX_HOME}
	${HYDRAX_HOME}/Modules
	${HYDRAX_HOME}/Noise
	${SKYX_HOME}/src
	${SKYX_HOME}/src/VClouds
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)
add_library (apeOgreRenderPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (apeOgreRenderPlugin apePluginManager apeSceneManager apeEventManager apeCoreConfig apeUserInputMacro MY_OGRE MY_PROCEDURAL MY_HYDRAX MY_SKYX )
#if (${APE_PLUGIN_OgreRender_HydraxNew})
#	set_property (TARGET apeOgreRenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}" HYDRAX_NEW)
#else ()
	set_property (TARGET apeOgreRenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
#endif()

set_property (TARGET apeOgreRenderPlugin PROPERTY FOLDER "Plugins/Render")
if (WIN32)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/OgreMain_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/OgreOverlay_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/OgreHLMS_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/OgreMeshLodGenerator_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/RenderSystem_GL_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/RenderSystem_Direct3D9_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/RenderSystem_Direct3D11_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/OgreRTShaderSystem_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/Plugin_CgProgramManager_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/debug/cg.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_PROCEDURAL_BUILD_DIR}/bin/debug/OgreProcedural_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_HYDRAX_BUILD_DIR}/Debug/Hydrax.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_SKY_BUILD_DIR}/Debug/Skyx.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/OgreMain.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/OgreOverlay.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/OgreHLMS.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/OgreMeshLodGenerator.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/RenderSystem_GL.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/RenderSystem_Direct3D9.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/RenderSystem_Direct3D11.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/OgreRTShaderSystem.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/Plugin_CgProgramManager.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_BUILD_DIR}/bin/release/cg.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_PROCEDURAL_BUILD_DIR}/bin/release/OgreProcedural.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_HYDRAX_BUILD_DIR}/release/Hydrax.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	file(COPY ${OGRE_SKY_BUILD_DIR}/release/Skyx.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()


