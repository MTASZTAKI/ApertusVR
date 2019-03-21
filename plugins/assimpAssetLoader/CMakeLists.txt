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

# assimp patch log begin
# In /3rdParty/assimp/code/X3DImporter.cpp std::transforms are commented out because of operator != problem
# assimp patch log begin

set (ASSIMP_HOME ${CMAKE_SOURCE_DIR}/3rdParty/assimp)
set (ASSIMP_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
#find assimp
if(EXISTS ${ASSIMP_HOME}/lib/Debug/assimp-vc140-mt.lib AND EXISTS ${ASSIMP_HOME}/lib/Release/assimp-vc140-mt.lib)
	set(ASSIMP_READY TRUE)
	message (STATUS "assimp is installed" )
else ()
	set(ASSIMP_READY FALSE)
	message (STATUS "assimp is not installed" )
endif ()
if (NOT ASSIMP_READY)
#build assimp
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring assimp for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${ASSIMP_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building assimp for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${ASSIMP_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use assimp
add_library(MY_ASSIMP STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_ASSIMP PROPERTY IMPORTED_LOCATION_DEBUG ${ASSIMP_HOME}/lib/Debug/assimp-vc140-mt.lib)
	set_property(TARGET MY_ASSIMP PROPERTY IMPORTED_LOCATION_RELEASE ${ASSIMP_HOME}/lib/Release/assimp-vc140-mt.lib)
endif ()

#ApeAssimpAssetLoaderPlugin
set (HEADERS
	ApeAssimpAssetLoaderPlugin.h
	)

set (SOURCES 
	ApeAssimpAssetLoaderPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${ASSIMP_HOME}/include
	${RAPIDJSON_INCLUDE_DIR}
	)

# Add the dinamic library target to the project
add_library (ApeAssimpAssetLoaderPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
set_property (TARGET ApeAssimpAssetLoaderPlugin PROPERTY COMPILE_DEFINITIONS APE_BIN_DIR="${APE_BIN_DIR}" APE_SOURCE_DIR="${APE_SOURCE_DIR}")
target_link_libraries (ApeAssimpAssetLoaderPlugin ApePluginManager ApeSceneManager ApeEventManager ApeLogManager ApeSystemConfig ApeMainWindow MY_ASSIMP)

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApeAssimpAssetLoaderPlugin PROPERTY FOLDER "Plugins")

if (WIN32)
	# todo_ApeAssimpAssetLoaderPlugin are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the assimp bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/assimp-vc140-mt.dll)
		file(COPY ${ASSIMP_HOME}/bin/Debug/assimp-vc140-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/assimp-vc140-mt.dll)
		file(COPY ${ASSIMP_HOME}/bin/Release/assimp-vc140-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()



