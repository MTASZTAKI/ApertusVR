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

# ois patch log begin
# CMakeLists.txt was copied to the ois root folder
# Win32KeyBoard.h ln 54 was edited { return KeyCode::KC_UNASSIGNED;/*TODO_apeOisUserInputPlugin Implement OS version*/; }
# ois patch log begin

if (NOT EXISTS ${CMAKE_BINARY_DIR}/ois/build)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ois/build)
endif()
set (OIS_BUILD_DIR ${CMAKE_BINARY_DIR}/ois/build)
set (OIS_HOME ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ois)
set (OIS_READY FALSE)


#error function
function (reportError what and when)
	if (${${when}})
		message (${${what}})
	endif ()
endfunction ()

#find ois
if(EXISTS ${OIS_BUILD_DIR}/Debug/OIS.lib AND EXISTS ${OIS_BUILD_DIR}/Release/OIS.lib)
	set(OIS_READY TRUE)
	message (STATUS "ois is installed" )
else ()
	set(OIS_READY FALSE)
	message (STATUS "ois is not installed" )
endif ()
if (NOT OIS_READY)
#build ois
	foreach (CONF ${CMAKE_CONFIGURATION_TYPES})
		message (STATUS "Configuring ois for ${CONF}, please wait.....")
		execute_process ( 
				COMMAND ${CMAKE_COMMAND} 
				-G ${CMAKE_GENERATOR} 
				${OIS_HOME}
				WORKING_DIRECTORY ${OIS_BUILD_DIR}
				RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output
				)		
		reportError (output if error)
		message (STATUS "Building ois for ${CONF}, please wait.....")
		execute_process ( 
			COMMAND ${CMAKE_COMMAND} --build ${OIS_BUILD_DIR} --config ${CONF} --target ALL_BUILD
			WORKING_DIRECTORY ${OIS_BUILD_DIR}
			RESULT_VARIABLE error OUTPUT_VARIABLE output ERROR_VARIABLE output)
		reportError (output if error)
	endforeach ()
endif ()
#use ois
add_library(MY_OIS STATIC IMPORTED)
if (WIN32)
	set_property(TARGET MY_OIS PROPERTY IMPORTED_LOCATION_DEBUG ${OIS_BUILD_DIR}/Debug/OIS.lib)
	set_property(TARGET MY_OIS PROPERTY IMPORTED_LOCATION_RELEASE ${OIS_BUILD_DIR}/Release/OIS.lib)
endif ()

#apeOisUserInputPlugin
set (HEADERS
	apeOisUserInputPlugin.h
	)

set (SOURCES 
	apeOisUserInputPlugin.cpp
	)

include_directories(	
	${OIS_HOME}/includes
	${RAPIDJSON_INCLUDE_DIR}
	)

# Add the dinamic library target to the project
add_library (apeOisUserInputPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (apeOisUserInputPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeCoreConfig apeUserInputMacro MY_OIS)
if (WIN32)
	# we have to link to some DirectX libraries 
	target_link_libraries (apeOisUserInputPlugin dinput8 dxguid)
endif ()	

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apeOisUserInputPlugin PROPERTY FOLDER "Plugins/UserInput")

if (WIN32)
	file(COPY ${OIS_BUILD_DIR}/Debug/OIS.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	file(COPY ${OIS_BUILD_DIR}/Release/OIS.lib DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
endif ()



