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

if (NOT EXISTS ${CMAKE_BINARY_DIR}/assimp/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assimp/build)
endif()
set (ASSIMP_BUILD_DIR ${CMAKE_BINARY_DIR}/assimp/build)
set (ASSIMP_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/assimp)
set (ASSIMP_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
#find assimp
if(EXISTS ${ASSIMP_BUILD_DIR}/code/Debug/assimp-vc142-mtd.lib AND EXISTS ${ASSIMP_BUILD_DIR}/code/Release/assimp-vc142-mt.lib)
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
				${ASSIMP_HOME}
				WORKING_DIRECTORY ${ASSIMP_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building assimp for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${ASSIMP_BUILD_DIR} --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${ASSIMP_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use assimp
add_library(MY_ASSIMP STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_ASSIMP PROPERTY IMPORTED_LOCATION_DEBUG ${ASSIMP_BUILD_DIR}/code/Debug/assimp-vc142-mtd.lib)
	set_property(TARGET MY_ASSIMP PROPERTY IMPORTED_LOCATION_RELEASE ${ASSIMP_BUILD_DIR}/code/Release/assimp-vc142-mt.lib)
endif ()

#apeAssimpAssetLoaderPlugin
set (HEADERS
    3rdParty/stb_image.h
	apeAssimpAssetLoaderPlugin.h
	)

set (SOURCES 
	apeAssimpAssetLoaderPlugin.cpp
	)

include_directories(
	${ASSIMP_HOME}/include
	${ASSIMP_BUILD_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)

# Add the dinamic library target to the project
add_library (apeAssimpAssetLoaderPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
set_property (TARGET apeAssimpAssetLoaderPlugin PROPERTY COMPILE_DEFINITIONS APE_BIN_DIR="${APE_BIN_DIR}" APE_SOURCE_DIR="${APE_SOURCE_DIR}")
target_link_libraries (apeAssimpAssetLoaderPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeCoreConfig MY_ASSIMP)

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apeAssimpAssetLoaderPlugin PROPERTY FOLDER "Plugins/AssetImporter")

if (WIN32)
	file(COPY ${ASSIMP_BUILD_DIR}/code/Debug/assimp-vc142-mtd.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${ASSIMP_BUILD_DIR}/code/Release/assimp-vc142-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()



