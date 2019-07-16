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


set (BULLET_HOME ${CMAKE_SOURCE_DIR}/3rdparty/bullet3)
set (BULLET_READY FALSE)

#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()



#find ois
if(EXISTS ${BULLET_HOME}/lib/Release/BulletDynamics.lib 
	AND EXISTS ${BULLET_HOME}/lib/Debug/BulletDynamics_Debug.lib
	AND EXISTS ${BULLET_HOME}/lib/Release/BulletCollision.lib 
	AND EXISTS ${BULLET_HOME}/lib/Debug/BulletCollision_Debug.lib
	AND EXISTS ${BULLET_HOME}/lib/Release/LinearMath.lib 
	AND EXISTS ${BULLET_HOME}/lib/Debug/LinearMath_Debug.lib)
	set(BULLET_READY TRUE)
	message (STATUS "bullet is installed" )
else ()
	set(BULLET_READY FALSE)
	message (STATUS "bullet is not installed" )
endif ()


if (NOT BULLET_READY)
#build bullet
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring bullet for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				.
				WORKING_DIRECTORY ${BULLET_HOME}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building bullet for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build . --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${BULLET_HOME}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()

#use bullet

add_library(BULLET_DYNAMICS STATIC IMPORTED)
if (WIN32)
	set_property(TARGET BULLET_DYNAMICS PROPERTY IMPORTED_LOCATION_DEBUG ${BULLET_HOME}/lib/Debug/BulletDynamics_Debug.lib )
	set_property(TARGET BULLET_DYNAMICS PROPERTY IMPORTED_LOCATION_RELEASE ${BULLET_HOME}/lib/Release/BulletDynamics.lib )
endif ()

add_library(BULLET_COLLISION STATIC IMPORTED)
if(WIN32)
	set_property(TARGET BULLET_COLLISION PROPERTY IMPORTED_LOCATION_DEBUG ${BULLET_HOME}/lib/Debug/BulletCollision_Debug.lib )
	set_property(TARGET BULLET_COLLISION PROPERTY IMPORTED_LOCATION_RELEASE ${BULLET_HOME}/lib/Release/BulletCollision.lib )
endif()

add_library(BULLET_LINMATH STATIC IMPORTED)
if(WIN32)
	set_property(TARGET BULLET_LINMATH PROPERTY IMPORTED_LOCATION_DEBUG ${BULLET_HOME}/lib/Debug/LinearMath_Debug.lib )
	set_property(TARGET BULLET_LINMATH PROPERTY IMPORTED_LOCATION_RELEASE ${BULLET_HOME}/lib/Release/LinearMath.lib )
endif()


#apeBulletPhysicsPlugin
set (HEADERS
	apeBulletPhysicsPlugin.h
	)

set (SOURCES 
	apeBulletPhysicsPlugin.cpp
	)

include_directories(
	${RAPIDJSON_INCLUDE_DIR}
	${BULLET_HOME}/src
	)

# Add the dinamic library target to the project
add_library (apeBulletPhysicsPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (apeBulletPhysicsPlugin
			apePluginManager
			apeSceneManager
			apeEventManager
			apeLogManager
			apeCoreConfig
			BULLET_DYNAMICS
			BULLET_COLLISION
			BULLET_LINMATH
			)

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apeBulletPhysicsPlugin PROPERTY FOLDER "Plugins")


if (WIN32)
	# TODO_apeOisUserInputPlugin are there any solution for doing this platform idependent way? maybe ogre install? sdk?
	# Copying the ois bins to the desired place
	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/BulletDynamics_Debug_d.lib)
		file(COPY ${BULLET_HOME}/lib/Debug/BulletDynamics_Debug.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/BulletDynamics_d.lib)
		file(COPY ${BULLET_HOME}/lib/Release/BulletDynamics.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()

	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/BulletCollision_Debug_d.lib)
		file(COPY ${BULLET_HOME}/lib/Debug/BulletCollision_Debug.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/BulletCollision_d.lib)
		file(COPY ${BULLET_HOME}/lib/Release/BulletCollision.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()

	# debug dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug/LinearMath_Debug_d.lib)
		file(COPY ${BULLET_HOME}/lib/Debug/LinearMath_Debug.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	endif()
	# release dlls
	if(NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release/LinearMath_d.lib)
		file(COPY ${BULLET_HOME}/lib/Release/LinearMath.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	endif()
endif ()

