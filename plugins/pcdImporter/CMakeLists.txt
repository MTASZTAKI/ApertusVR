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

set (PCL_HOME ${CMAKE_SOURCE_DIR}/3rdParty/pcl)
set (PCL_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
#find PCL
if(EXISTS ${PCL_HOME}/lib/Debug/PCL-vc140-mt.lib AND EXISTS ${PCL_HOME}/lib/Release/PCL-vc140-mt.lib)
	set(PCL_READY TRUE)
	message (STATUS "PCL is installed" )
else ()
	set(PCL_READY FALSE)
	message (STATUS "PCL is not installed" )
endif ()
if (NOT PCL_READY)
#build PCL
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring PCL for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${PCL_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building PCL for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${PCL_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use PCL
add_library(MY_PCL STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_PCL PROPERTY IMPORTED_LOCATION_DEBUG ${PCL_HOME}/lib/Debug/PCL-vc140-mt.lib)
	set_property(TARGET MY_PCL PROPERTY IMPORTED_LOCATION_RELEASE ${PCL_HOME}/lib/Release/PCL-vc140-mt.lib)
endif ()

#ApePcdImporterPlugin
set (HEADERS
	ApePcdImporterPlugin.h
	)

set (SOURCES 
	ApePcdImporterPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${PCL_HOME}/include
	${RAPIDJSON_INCLUDE_DIR}
	)

# Add the dinamic library target to the project
add_library (ApePcdImporterPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
set_property (TARGET ApePcdImporterPlugin PROPERTY COMPILE_DEFINITIONS APE_BIN_DIR="${APE_BIN_DIR}" APE_SOURCE_DIR="${APE_SOURCE_DIR}")
target_link_libraries (ApePcdImporterPlugin ApePluginManager ApeSceneManager ApeEventManager ApeLogManager ApeSystemConfig ApeMainWindow MY_PCL)

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApePcdImporterPlugin PROPERTY FOLDER "Plugins")

if (WIN32)
	# todo are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the PCL bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/PCL-vc140-mt.dll)
		file(COPY ${PCL_HOME}/bin/Debug/PCL-vc140-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/PCL-vc140-mt.dll)
		file(COPY ${PCL_HOME}/bin/Release/PCL-vc140-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()



