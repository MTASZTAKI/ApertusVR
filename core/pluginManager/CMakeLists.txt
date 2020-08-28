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
set (HEADERS
	apeInternalPluginManager.h
	apePluginManagerImpl.h
	apePluginFactory.h
	apePluginInstance.h
	)

set (SOURCES 
	apePluginManagerImpl.cpp
	apePluginAPI.cpp
	apePluginFactory.cpp
	apeInternalPluginManager.cpp
	apePluginInstance.cpp
	)
	
include_directories(
	${PROJECT_SOURCE_DIR}/core/logManager
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)

# Add the dynamic library target to the project
add_library (apePluginManager SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
if(WIN32)
	target_link_libraries (apePluginManager apeCoreConfig apeLogManager)
elseif(ANDROID)
	target_link_libraries (apePluginManager apeCoreConfig apeLogManager dl)
else()
	target_link_libraries (apePluginManager apeCoreConfig apeLogManager dl pthread)
endif()

# Add preprocessor definitions
set_property (TARGET apePluginManager PROPERTY COMPILE_DEFINITIONS BUILDING_APE_PLUGINMANAGER_DLL BUILDING_APE_INTERNAL_PLUGINMANAGER)

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apePluginManager PROPERTY FOLDER "Core")

