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

# ogre patch log begin
# Dependencies folder was copied to the ogre root folder
# FindDirectX11.cmake (3rdParty/ogre/CMake/Packages/FindDirectX11.cmake) was patched to find DirectX11 on Windows 8.1
# ogre patch log begin


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

set(PATH_OGRE21DEPS ${CMAKE_SOURCE_DIR}/3rdParty/ogre21/Dependencies)
set(PATH_OGRE21DEPS_OUT ${CMAKE_SOURCE_DIR}/3rdParty/ogre21/Dependencies)
set(PATH_OGRE21 ${CMAKE_SOURCE_DIR}/3rdParty/ogre21)	
set(OGRE21_READY FALSE)


#find ogre21
message (STATUS "try to find Ogre" )
set (OGRE_STATIC OFF)
set (ENV{OGREDEPS_BUILD_SDL2} FALSE)
set (ENV{OGRE_BUILD_RENDERSYSTEM_D3D9} FALSE)
set (ENV{FREETYPE_FT2BUILD_INCLUDE_DIR} ${PATH_OGRE21DEPS_OUT}/include/freetype)
set (ENV{OGRE_HOME} ${PATH_OGRE21})
set (ENV{OGRE_SOURCE} ${PATH_OGRE21})
set (ENV{OGRE_BUILD} ${PATH_OGRE21})
set (ENV{OGRE_DEPENDENCIES_DIR} ${PATH_OGRE21DEPS_OUT})
set (ENV{Cg_HOME} ${PATH_OGRE21DEPS_OUT})
set (ENV{FREEIMAGE_HOME} ${PATH_OGRE21DEPS_OUT})
set (ENV{FREETYPE_HOME} ${PATH_OGRE21DEPS_OUT})
set (ENV{ZLIB_HOME} ${PATH_OGRE21DEPS_OUT})
set (ENV{ZZIP_HOME} ${PATH_OGRE21DEPS_OUT})
set (STORED_CMSD ${CMAKE_SOURCE_DIR})
set (STORED_CIP ${CMAKE_INSTALL_PREFIX})
set (CMAKE_SOURCE_DIR ${PATH_OGRE21})
set (CMAKE_INSTALL_PREFIX ${PATH_OGRE21})
add_subdirectory(${PATH_OGRE21} ${PATH_OGRE21} EXCLUDE_FROM_ALL)
set (CMAKE_SOURCE_DIR ${STORED_CMSD})
set (CMAKE_INSTALL_PREFIX ${STORED_CIP})
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${PATH_OGRE21};${PATH_OGRE21}/CMake/;${PATH_OGRE21}/lib/OGRE/cmake;${PATH_OGRE21}/CMake/Packages;${PATH_OGRE21}/CMake/Utils)
find_package (OGRE 2.10 QUIET)
if (OGRE_FOUND)
	set(OGRE21_READY TRUE)
	message (STATUS "ogre2.1 is installed" )
else ()
	set(OGRE21_READY FALSE)
	message (STATUS "ogre2.1 is not installed")
endif ()


#try ogre2.1
if (NOT OGRE21_READY)


#build ogre21deps
#	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
#		message (STATUS "Configuring ogre dependencies for ${CONF}, please wait.....")
#		execute_process ( 
#			COMMAND ${CMAKE_COMMAND}
#			-DOGREDEPS_BUILD_SDL2:BOOL=OFF
#			-G ${CMAKE_GENERATOR}
#			.
#			WORKING_DIRECTORY ${PATH_OGRE21DEPS}
#			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
#			)
#		reportError (output if error)
#		message (STATUS "Building ogre dependencies for ${CONF}, please wait.....")
#		execute_process ( 
#			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target install
#			WORKING_DIRECTORY ${PATH_OGRE21DEPS}
#			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
#		reportError (output if error)
#	endforeach ()


#build ogre2.1
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ogre for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-DOGRE_DEPENDENCIES_DIR:PATH=${PATH_OGRE21DEPS_OUT}
				-DOGRE_BUILD_SAMPLES:BOOL=FALSE
				-DOGRE_INSTALL_SAMPLES=OFF
				-DOGREDEPS_BUILD_SDL2:BOOL=OFF
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${PATH_OGRE21}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ogre2.1 for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target install
			WORKING_DIRECTORY ${PATH_OGRE21}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()



# ApeOgre21RenderPlugin
set (HEADERS
	ApeOgre21Conversions.h
	ApeOgre21RenderPlugin.h
	ApeOgre21RenderPluginConfigs.h
	)
set (SOURCES
	ApeOgre21RenderPlugin.cpp
	)
include_directories(
	
	${PROJECT_SOURCE_DIR}/common/include 
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
	${RAPIDJSON_INCLUDE_DIR}
	)
add_library (ApeOgre21RenderPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ApeOgre21RenderPlugin ApePluginManager ApeScene ApeEventManager ApeSystemConfig ApeMainWindow ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${OGRE_HLMS_LIBRARIES} ${OGRE_MeshLodGenerator_LIBRARIES}
${OGRE_Plugin_ParticleFX_LIBRARIES} ${OGRE_RenderSystem_Direct3D9_LIBRARIES} ${OGRE_RenderSystem_Direct3D11_LIBRARIES} ${OGRE_RenderSystem_GL_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARIES} ${OGRE_CgProgramManager_LIBRARIES} ${OgreProcedural_LIBRARIES})


set_property (TARGET ApeOgre21RenderPlugin PROPERTY FOLDER "Plugins")
set_property (TARGET ApeOgre21RenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
if (WIN32)
	# todo are there any solution for doing this platform idependent way? maybe ogre install? sdk?
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
	
endif ()


