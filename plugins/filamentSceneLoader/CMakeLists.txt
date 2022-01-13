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

# Add all header and cpp files in the directory to the project
if(APPLE)
	find_package(nlohmann_json REQUIRED)
endif()
set (HEADERS
	apeFilamentSceneLoaderPlugin.h
	apeFilamentSceneLoaderPluginConfig.h
	)

set (SOURCES 
	apeFilamentSceneLoaderPlugin.cpp
	)

include_directories(
	${RAPIDJSON_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/tinygltf
	)

add_library (apeFilamentSceneLoaderPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
if(WIN32)
target_link_libraries (apeFilamentSceneLoaderPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeUserInputMacro apeSceneMakerMacro )
elseif(APPLE)
target_link_libraries (apeFilamentSceneLoaderPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeUserInputMacro apeSceneMakerMacro nlohmann_json::nlohmann_json)
endif()


set_property (TARGET apeFilamentSceneLoaderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeFilamentSceneLoaderPlugin PROPERTY FOLDER "Plugins/FilamentApplication")

set_property (TARGET apeFilamentSceneLoaderPlugin PROPERTY CXX_STANDARD 17)

set_property (TARGET apeFilamentSceneLoaderPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")


