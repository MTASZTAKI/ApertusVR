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

find_package(ZED 2 REQUIRED)
IF(NOT WIN32)
     SET(SPECIAL_OS_LIBS "pthread" "X11")
    add_definitions(-Wno-write-strings)
ENDIF()
find_package(CUDA ${ZED_CUDA_VERSION} EXACT REQUIRED)

#ApeZedPlugin
set (HEADERS
	ApeZedPlugin.h
	)

set (SOURCES 
	ApeZedPlugin.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/common/include  
	${RAPIDJSON_INCLUDE_DIR}
	${ZED_INCLUDE_DIRS}
	${CUDA_INCLUDE_DIRS}
	)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})

# Add the dinamic library target to the project
add_library (ApeZedPlugin SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries
target_link_libraries (ApeZedPlugin ApePluginManager ApeSceneManager ApeEventManager ApeLogManager ApeSystemConfig ApeMainWindow ${SPECIAL_OS_LIBS}
                        ${ZED_LIBRARIES}
                        ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY} ${CUDA_NPP_LIBRARIES_ZED})
	
# Place target xxx into the specified folder in IDEs supporting it
set_property (TARGET ApeZedPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET ApeZedPlugin PROPERTY FOLDER "Plugins")




