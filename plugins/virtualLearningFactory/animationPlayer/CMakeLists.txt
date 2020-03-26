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
	apeVLFTAnimationPlayerPlugin.h
	apeVLFTAnimationPlayerPluginConfig.h
	)

set (SOURCES 
	apeVLFTAnimationPlayerPlugin.cpp
	)

include_directories(
	${RAPIDJSON_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty
	${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rapidjson/include
	)

add_library (apeVLFTAnimationPlayerPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (apeVLFTAnimationPlayerPlugin apePluginManager apeSceneManager apeEventManager apeLogManager apeUserInputMacro apeSceneMakerMacro gdiplus.lib)

set_property (TARGET apeVLFTAnimationPlayerPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

set_property (TARGET apeVLFTAnimationPlayerPlugin PROPERTY FOLDER "Plugins/VirtualLearningFactory")

set_property (TARGET apeVLFTAnimationPlayerPlugin PROPERTY CXX_STANDARD 17)

set_property (TARGET apeVLFTAnimationPlayerPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ffmpeg-4.2.2-win64-static/bin/ffmpeg.exe DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/ffmpeg-4.2.2-win64-static/bin/ffmpeg.exe DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)



