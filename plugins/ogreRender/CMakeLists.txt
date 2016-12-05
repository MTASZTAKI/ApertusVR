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
# ogre patch log begin

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

set(PATH_OGREDEPS ${CMAKE_SOURCE_DIR}/3rdParty/ogre/Dependencies)
set(PATH_OGREDEPS_OUT ${CMAKE_SOURCE_DIR}/3rdParty/ogre/Dependencies/ogredeps)
set(PATH_OGRE ${CMAKE_SOURCE_DIR}/3rdParty/ogre)
set(OGRE_READY FALSE)

#find ogre
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
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${PATH_OGRE};${PATH_OGRE}/CMake/;${PATH_OGRE}/lib/OGRE/cmake;${PATH_OGRE}/CMake/Packages; ${PATH_OGRE}/CMake/Utils)
find_package (OGRE 1.10 QUIET)
if (${OGRE_FOUND})
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
# ApeOgreRenderPlugin
set (HEADERS
	ApeOgreRenderPlugin.h
	ApeOgreRenderPluginConfigs.h
	ApeOgreMovableText.h
	ApeOgreConversions.h
	)
set (SOURCES
	ApeOgreRenderPlugin.cpp
	ApeOgreMovableText.cpp
	)
include_directories(
	${PROJECT_SOURCE_DIR}/common/include 
	${OGRE_INCLUDE_DIRS}
	${OGRE_Overlay_INCLUDE_DIRS}
	${OGRE_HLMS_INCLUDE_DIRS}
	${OGRE_MeshLodGenerator_INCLUDE_DIRS}
	${OGRE_Plugin_ParticleFX_INCLUDE_DIRS}
	${OGRE_RenderSystem_Direct3D11_INCLUDE_DIRS}
	${OGRE_RenderSystem_GL_INCLUDE_DIRS}
	${OGRE_RTShaderSystem_INCLUDE_DIRS}
	${RAPIDJSON_INCLUDE_DIR}
	)
add_library (ApeOgreRenderPlugin SHARED ${SOURCES} ${HEADERS})
target_link_libraries (ApeOgreRenderPlugin ApePluginManager ApeScene ApeEventManager ApeSystemConfig ApeMainWindow ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES} ${OGRE_HLMS_LIBRARIES} ${OGRE_MeshLodGenerator_LIBRARIES}
${OGRE_Plugin_ParticleFX_LIBRARIES} ${OGRE_RenderSystem_Direct3D11_LIBRARIES} ${OGRE_RenderSystem_GL_LIBRARIES} ${OGRE_RTShaderSystem_LIBRARIES})
set_property (TARGET ApeOgreRenderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET ApeOgreRenderPlugin PROPERTY FOLDER "Plugins")
if (WIN32)
	# todo are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the ogre bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/OgreMain_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreMain_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OgreOverlay_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreOverlay_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OgreOverlay_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreOverlay_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OgreRTShaderSystem_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/OgreRTShaderSystem_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RenderSystem_GL_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/RenderSystem_GL_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RenderSystem_Direct3D11_d.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/debug/RenderSystem_Direct3D11_d.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/OgreMain.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreMain.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OgreOverlay.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreOverlay.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OgreOverlay.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreOverlay.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/OgreRTShaderSystem.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/OgreRTShaderSystem.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RenderSystem_GL.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/RenderSystem_GL.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RenderSystem_Direct3D11.dll)
		file(COPY ${OGRE_BINARY_DIR}/bin/release/RenderSystem_Direct3D11.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()


