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
	apeCoreConfigImpl.h
	)
set (SOURCES
	apeCoreConfigImpl.cpp
	)
	
include_directories(
		${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)


add_library (apeCoreConfig SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
if(ANDROID)
	target_link_libraries (apeCoreConfig apeAAssetOpen)
endif()

# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET apeCoreConfig PROPERTY COMPILE_DEFINITIONS BUILDING_APE_CORECONFIG_DLL APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET apeCoreConfig PROPERTY FOLDER "Core")
