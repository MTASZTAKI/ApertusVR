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

# dx9
if (WIN32 AND NOT EXISTS ${CMAKE_SOURCE_DIR}/3rdParty/directx9/DXSDK_Jun10.exe)
    message("DirectX9 SDK: Starting download, please wait...")
	file(DOWNLOAD https://download.microsoft.com/download/A/E/7/AE743F1F-632B-4809-87A9-AA1BB3458E31/DXSDK_Jun10.exe
	${CMAKE_SOURCE_DIR}/3rdParty/directx9/DXSDK_Jun10.exe SHOW_PROGRESS)
	message("DirectX9 SDK: Starting installer...")
	set(DX9SDK_SETUP_PATH "${CMAKE_SOURCE_DIR}/3rdParty/directx9/DXSDK_Jun10.exe")
	execute_process(
		COMMAND cmd /c ${DX9SDK_SETUP_PATH}
		RESULT_VARIABLE DX9SDK_SETUP_RESULT
	)
	if (${DX9SDK_SETUP_RESULT} EQUAL 0)
		message("DirectX9 SDK: Installation succeeded, exit code (${DX9SDK_SETUP_RESULT})")
	else ()
		message("DirectX9 SDK: Installation failed, exit code (${DX9SDK_SETUP_RESULT})")
	endif ()
endif ()

set(PATH_OGREDEPS ${CMAKE_SOURCE_DIR}/3rdParty/ogre/Dependencies)
set(PATH_OGREDEPS_OUT ${CMAKE_SOURCE_DIR}/3rdParty/ogre/Dependencies/ogredeps)
set(PATH_OGRE ${CMAKE_SOURCE_DIR}/3rdParty/ogre)
set(OGRE_READY FALSE)
set(PATH_OGRE_PROCEDURAL ${CMAKE_SOURCE_DIR}/3rdParty/ogre_procedural)
set(OGRE_PROCEDURAL_READY FALSE)

#find ogre
message (STATUS "try to find Ogre" )
set (OGRE_STATIC OFF)
set (ENV{OGREDEPS_BUILD_SDL2} FALSE)
set (ENV{OGRE_BUILD_RENDERSYSTEM_D3D9} FALSE)
set (ENV{FREETYPE_FT2BUILD_INCLUDE_DIR} ${PATH_OGREDEPS_OUT}/include/freetype)
set (ENV{OGRE_HOME} ${PATH_OGRE})
set (ENV{OGRE_SOURCE} ${PATH_OGRE})
set (ENV{OGRE_BUILD} ${PATH_OGRE})
set (ENV{OGRE_DEPENDENCIES_DIR} ${PATH_OGREDEPS_OUT})
set (ENV{Cg_HOME} ${PATH_OGREDEPS_OUT})
set (ENV{FREEIMAGE_HOME} ${PATH_OGREDEPS_OUT})
set (ENV{FREETYPE_HOME} ${PATH_OGREDEPS_OUT})
set (ENV{ZLIB_HOME} ${PATH_OGREDEPS_OUT})
set (ENV{ZZIP_HOME} ${PATH_OGREDEPS_OUT})
set (STORED_CMSD ${CMAKE_SOURCE_DIR})
set (STORED_CIP ${CMAKE_INSTALL_PREFIX})
set (CMAKE_SOURCE_DIR ${PATH_OGRE})
set (CMAKE_INSTALL_PREFIX ${PATH_OGRE})
add_subdirectory(${PATH_OGRE} ${PATH_OGRE} EXCLUDE_FROM_ALL)
set (CMAKE_SOURCE_DIR ${STORED_CMSD})
set (CMAKE_INSTALL_PREFIX ${STORED_CIP})
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${PATH_OGRE};${PATH_OGRE}/CMake/;${PATH_OGRE}/lib/OGRE/cmake;${PATH_OGRE}/CMake/Packages;${PATH_OGRE}/CMake/Utils)
find_package (OGRE 1.10 QUIET)
if (OGRE_FOUND)
	set(OGRE_READY TRUE)
	message (STATUS "ogre is installed" )
else ()
	set(OGRE_READY FALSE)
	message (STATUS "ogre is not installed")
endif ()
#try ogre
if (NOT OGRE_READY)
#build ogredeps
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre dependencies for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} 
			-G ${CMAKE_GENERATOR}
			.
			WORKING_DIRECTORY ${PATH_OGREDEPS}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
			)
		reportError (output if error)
		message (STATUS "Building ogre dependencies for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target install
			WORKING_DIRECTORY ${PATH_OGREDEPS}
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
				.
				WORKING_DIRECTORY ${PATH_OGRE}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target install
			WORKING_DIRECTORY ${PATH_OGRE}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#find ogre_procedural
message (STATUS "try to find Ogre_procedural" )
set (ENV{OgreProcedural_HOME} ${PATH_OGRE_PROCEDURAL})
set (STORED_CMSD ${CMAKE_SOURCE_DIR})
set (STORED_CIP ${CMAKE_INSTALL_PREFIX})
set (CMAKE_SOURCE_DIR ${PATH_OGRE_PROCEDURAL})
set (CMAKE_INSTALL_PREFIX ${PATH_OGRE_PROCEDURAL})
add_subdirectory(${PATH_OGRE_PROCEDURAL} ${PATH_OGRE_PROCEDURAL} EXCLUDE_FROM_ALL)
set (CMAKE_SOURCE_DIR ${STORED_CMSD})
set (CMAKE_INSTALL_PREFIX ${STORED_CIP})
set (CMAKE_MODULE_PATH ${PATH_OGRE_PROCEDURAL}/CMake/Utils;${PATH_OGRE_PROCEDURAL}/CMake;${PATH_OGRE_PROCEDURAL}/CMake/Packages)
find_package (OgreProcedural QUIET)
if (OgreProcedural_FOUND)
	set(OGRE_PROCEDURAL_READY TRUE)
	message (STATUS "ogre_procedural is installed" )
else ()
	set(OGRE_PROCEDURAL_READY FALSE)
	message (STATUS "ogre_procedural is not installed")
endif ()
#try ogre_procedural
if (NOT OGRE_PROCEDURAL_READY)
#build ogre_procedural
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre_procedural for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${PATH_OGRE_PROCEDURAL}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_procedural for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target install
			WORKING_DIRECTORY ${PATH_OGRE_PROCEDURAL}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#find ogre_hydrax
option(APE_PLUGIN_OgreRender_HydraxNew "Use OgreRenderPluginHydraxNew?" ON )
if (${APE_PLUGIN_OgreRender_HydraxNew})
	set (HYDRAX_HOME ${CMAKE_SOURCE_DIR}/3rdParty/ogre_hydrax_new)
else ()
	set (HYDRAX_HOME ${CMAKE_SOURCE_DIR}/3rdParty/ogre_hydrax)
endif ()
set (HYDRAX_READY FALSE)
if(EXISTS ${HYDRAX_HOME}/srcdemo/Debug/Hydrax.dll AND EXISTS ${HYDRAX_HOME}/srcdemo/Release/Hydrax.dll)
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
				WORKING_DIRECTORY ${HYDRAX_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_hydrax for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${HYDRAX_HOME} --config ${CONF} --target Hydrax
			WORKING_DIRECTORY ${HYDRAX_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use ogre_hydrax
add_library(MY_HYDRAX STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_HYDRAX PROPERTY IMPORTED_LOCATION_DEBUG ${HYDRAX_HOME}/srcdemo/Debug/Hydrax.lib)
	set_property(TARGET MY_HYDRAX PROPERTY IMPORTED_LOCATION_RELEASE ${HYDRAX_HOME}/srcdemo/Release/Hydrax.lib)
endif ()
#find ogre_skyx
set (SKYX_HOME ${CMAKE_SOURCE_DIR}/3rdParty/ogre_skyx)
set (SKYX_READY FALSE)
if(EXISTS ${SKYX_HOME}/Debug/Skyx.dll AND EXISTS ${SKYX_HOME}/Release/Skyx.dll)
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
				WORKING_DIRECTORY ${SKYX_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre_skyx for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${SKYX_HOME} --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${SKYX_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use ogre_skyx
add_library(MY_SKYX STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_SKYX PROPERTY IMPORTED_LOCATION_DEBUG ${SKYX_HOME}/Debug/Skyx.lib)
	set_property(TARGET MY_SKYX PROPERTY IMPORTED_LOCATION_RELEASE ${SKYX_HOME}/Release/Skyx.lib)
endif ()
# ApeOgreRenderPlugin
set (HEADERS
	ApeOgreRenderPlugin.h
	ApeOgreRenderPluginConfigs.h
	ApeOgreMovableText.h
	ApeOgrePointCloud.h
	ApeOgreShaderGeneratorResolver.h
	ApeOgreConversions.h
	ApeOgreUtilities.h
	)
set (SOURCES
	ApeOgreRenderPlugin.cpp
	ApeOgreMovableText.cpp
	ApeOgrePointCloud.cpp
	)
include_directories(
	${PROJECT_SOURCE_DIR}/common/include
	${PROJECT_SOURCE_DIR}/macros/userInput 
	${OGRE_INCLUDE_DIRS}
	${OGRE_Overlay_INCLUDE_DIRS}
	${OGRE_HLMS_INCLUDE_DIRS}
	${OGRE_MeshLodGenerator_INCLUDE_DIRS}
	${OGRE_Plugin_ParticleFX_INCLUDE_DIRS}
	${OGRE_RenderSystem_Direct3D9_INCLUDE_DIRS}
	${OGRE_RenderSystem_Direct3D11_INCLUDE_DIRS}
	${OGRE_RenderSystem_GL_INCLUDE_DIRS}
	${OGRE_RTShaderSystem_INCLUDE_DIRS}
	${OGRE_CgProgramManager_INCLUDE_DIRS}
	${OgreProcedural_INCLUDE_DIRS}
	${HYDRAX_HOME}/include
	${HYDRAX_HOME}/include/Modules
	${HYDRAX_HOME}/include/Noise
	${HYDRAX_HOME}/src
	${SKYX_HOME}/src
	${SKYX_HOME}/src/VClouds
	${RAPIDJSON_INCLUDE_DIR}
	)
add_library (ApeOgreRenderPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ApeOgreRenderPlugin ApePluginManager ApeSceneManager ApeEventManager ApeSystemConfig ApeMainWindow ApeUserInputMacro ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${OGRE_HLMS_LIBRARIES} ${OGRE_MeshLodGenerator_LIBRARIES}
${OGRE_Plugin_ParticleFX_LIBRARIES} ${OGRE_RenderSystem_Direct3D9_LIBRARIES} ${OGRE_RenderSystem_Direct3D11_LIBRARIES} ${OGRE_RenderSystem_GL_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARIES} ${OGRE_CgProgramManager_LIBRARIES} ${OgreProcedural_LIBRARIES} MY_HYDRAX MY_SKYX )
if (${APE_PLUGIN_OgreRender_HydraxNew})
	set_property (TARGET ApeOgreRenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}" HYDRAX_NEW)
else ()
	set_property (TARGET ApeOgreRenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
endif()

set_property (TARGET ApeOgreRenderPlugin PROPERTY FOLDER "Plugins")
if (WIN32)
	# todo_ApeOgreRenderPlugin are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the ogre bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreMain_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreMain_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreOverlay_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreOverlay_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreHLMS_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreHLMS_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreMeshLodGenerator_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreMeshLodGenerator_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/RenderSystem_GL_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/RenderSystem_GL_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/RenderSystem_Direct3D9_d.dll AND EXISTS ${OGRE_BINARY_DIR}/bin/debug/RenderSystem_Direct3D9_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/RenderSystem_Direct3D9_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/RenderSystem_Direct3D11_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/RenderSystem_Direct3D11_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreRTShaderSystem_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreRTShaderSystem_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/Plugin_CgProgramManager_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/Plugin_CgProgramManager_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/cg.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/cg.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreProcedural_d.dll)
		file(COPY ${OgreProcedural_BINARY_DIR}/bin/debug/OgreProcedural_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/Hydrax.dll)
		file(COPY ${HYDRAX_HOME}/srcdemo/Debug/Hydrax.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/Skyx.dll)
		file(COPY ${SKYX_HOME}/Debug/Skyx.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreMain.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreMain.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreOverlay.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreOverlay.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreHLMS.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreHLMS.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreMeshLodGenerator.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreMeshLodGenerator.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/RenderSystem_GL.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/RenderSystem_GL.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/RenderSystem_Direct3D9.dll AND EXISTS ${OGRE_BINARY_DIR}/bin/release/RenderSystem_Direct3D9.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/RenderSystem_Direct3D9.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/RenderSystem_Direct3D11.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/RenderSystem_Direct3D11.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreRTShaderSystem.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreRTShaderSystem.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/Plugin_CgProgramManager.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/Plugin_CgProgramManager.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
        if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/cg.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/cg.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreProcedural.dll)
		file(COPY ${OgreProcedural_BINARY_DIR}/bin/release/OgreProcedural.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/Hydrax.dll)
		file(COPY ${HYDRAX_HOME}/srcdemo/release/Hydrax.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/Skyx.dll)
		file(COPY ${SKYX_HOME}/release/Skyx.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()


