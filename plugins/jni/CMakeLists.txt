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

set(SOURCES
        apeJNIPlugin.cpp
        apeJNIEntity.cpp
        apeJNINode.cpp
        apeJNILight.cpp
        apeJNICamera.cpp
        apeJNIGeometry.cpp
        apeJNIConeGeometry.cpp
        apeJNIFileGeometry.cpp
        apeJNIPlaneGeometry.cpp
        apeJNIMaterial.cpp
        apeJNIFileMaterial.cpp
        apeJNIManualMaterial.cpp
        apeJNIFileTexture.cpp
        )

set(HEADERS
        apeJNIPlugin.h)

add_library(apeJNIPlugin SHARED ${SOURCES} ${HEADERS})

target_link_libraries(apeJNIPlugin
        apeCoreConfig
        apePluginManager
        apeSceneManager
        apeEventManager
        apeLogManager
#        apeSystem
        ${log-lib}
        android_fopen
#        ${android-lib}
        )

set_property (TARGET apeJNIPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property(TARGET apeJNIPlugin PROPERTY FOLDER "Plugins/jni")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})