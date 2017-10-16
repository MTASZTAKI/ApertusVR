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

#dolphin patch log
#${CMAKE_SOURCE_DIR}/3rdParty/dolphin.CmakeLists.txt was changed

set (DOLPHIN_HOME ${CMAKE_SOURCE_DIR}/3rdParty/dolphin)
set (DOLPHIN_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()
#find dolphin
if(EXISTS ${DOLPHIN_HOME}/lib/Debug/dolphin-vc140-mt.lib AND EXISTS ${DOLPHIN_HOME}/lib/Release/dolphin-vc140-mt.lib)
	set(DOLPHIN_READY TRUE)
	message (STATUS "dolphin is installed" )
else ()
	set(DOLPHIN_READY FALSE)
	message (STATUS "dolphin is not installed" )
endif ()
if (NOT DOLPHIN_READY)
#build dolphin
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring dolphin for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${DOLPHIN_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building dolphin for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target dolphin
			WORKING_DIRECTORY ${DOLPHIN_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use dolphin
add_library(MY_DOLPHIN STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_DOLPHIN PROPERTY IMPORTED_LOCATION_DEBUG ${DOLPHIN_HOME}/lib/Debug/dolphin-vc140-mt.lib)
	set_property(TARGET MY_DOLPHIN PROPERTY IMPORTED_LOCATION_RELEASE ${DOLPHIN_HOME}/lib/Release/dolphin-vc140-mt.lib)
endif ()

#ApeDolphinEnvironmentSimulatorPlugin
set (HEADERS
	ApeDolphinEnvironmentSimulatorPlugin.h
	)

set (SOURCES 
	ApeDolphinEnvironmentSimulatorPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${DOLPHIN_HOME}/include
	${RAPIDJSON_INCLUDE_DIR}
	)

# Add the dinamic library target to the project
add_library (ApeDolphinEnvironmentSimulatorPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
set_property (TARGET ApeDolphinEnvironmentSimulatorPlugin PROPERTY COMPILE_DEFINITIONS APE_BIN_DIR="${APE_BIN_DIR}" APE_SOURCE_DIR="${APE_SOURCE_DIR}")
target_link_libraries (ApeDolphinEnvironmentSimulatorPlugin ApePluginManager ApeScene ApeEventManager ApeSystemConfig ApeMainWindow MY_DOLPHIN)	

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApeDolphinEnvironmentSimulatorPlugin PROPERTY FOLDER "Plugins")

if (WIN32)
	# todo are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the dolphin bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/dolphin-vc140-mt.dll)
		file(COPY ${DOLPHIN_HOME}/bin/Debug/dolphin-vc140-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/dolphin-vc140-mt.dll)
		file(COPY ${DOLPHIN_HOME}/bin/Release/dolphin-vc140-mt.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()



