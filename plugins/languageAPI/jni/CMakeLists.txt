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

add_subdirectory(starter)

set (SOURCES
        apeJNIPlugin.cpp
        wrappers/apeJNIEntity.cpp
        wrappers/apeJNINode.cpp
        wrappers/apeJNILight.cpp
        wrappers/apeJNICamera.cpp
        wrappers/apeJNIGeometry.cpp
        wrappers/apeJNIConeGeometry.cpp
        wrappers/apeJNIFileGeometry.cpp
        wrappers/apeJNICloneGeometry.cpp
        wrappers/apeJNIPlaneGeometry.cpp
        wrappers/apeJNIMaterial.cpp
        wrappers/apeJNIFileMaterial.cpp
        wrappers/apeJNIManualMaterial.cpp
        wrappers/apeJNIFileTexture.cpp
        wrappers/apeJNISceneNetwork.cpp
        wrappers/apeJNISceneManager.cpp
        wrappers/apeJNICoreConfig.cpp
        wrappers/apeJNITextGeometry.cpp
        wrappers/apeJNISphereGeometry.cpp
        wrappers/apeJNICylinderGeometry.cpp
        wrappers/apeJNITorusGeometry.cpp
        wrappers/apeJNITubeGeometry.cpp
        wrappers/apeJNIBoxGeometry.cpp
        )

set (HEADERS
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
#        ${android-lib}
        )

set_property (TARGET apeJNIPlugin PROPERTY COMPILE_DEFINITIONS APE_SOURCE_DIR="${APE_SOURCE_DIR}")
set_property (TARGET apeJNIPlugin PROPERTY FOLDER "Plugins/languageAPI/jni")

include_directories (${CMAKE_CURRENT_SOURCE_DIR})