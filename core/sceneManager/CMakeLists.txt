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

include("network/prepare_curl.cmake")
include("network/prepare_raknet.cmake")
include("network/prepare_zip.cmake")

set(HEADERS
	apeTextGeometryImpl.h
	apeLightImpl.h
	apeBrowserImpl.h
	apeNodeImpl.h
	apeCameraImpl.h
	apeSceneManagerImpl.h
	apeFileGeometryImpl.h
	apeFileTextureImpl.h
	apePlaneGeometryImpl.h
	apeBoxGeometryImpl.h
	apeIndexedFaceSetGeometryImpl.h
	apeIndexedLineSetGeometryImpl.h
	apeCylinderGeometryImpl.h
	apeCloneGeometryImpl.h
	apeConeGeometryImpl.h
	apeTorusGeometryImpl.h
	apeTubeGeometryImpl.h
	apeSphereGeometryImpl.h
	apeFileMaterialImpl.h
	apeManualMaterialImpl.h
	apeManualTextureImpl.h
	apeUnitTextureImpl.h
	apeRayGeometryImpl.h
	apeWaterImpl.h
	apeSkyImpl.h
	apePointCloudImpl.h
	apeRigidBodyImpl.h
	network/apeReplica.h
	network/apeSceneNetworkImpl.h
	network/apeReplicaManagerConnection.h
	network/apeReplicaManager.h
	network/apeLobbyManager.h
	network/apeHttpManager.h
	)

set(SOURCES
	apeTextGeometryImpl.cpp
	apeLightImpl.cpp
	apeBrowserImpl.cpp
	apeNodeImpl.cpp
	apeCameraImpl.cpp
	apeSceneManagerImpl.cpp
	apeFileTextureImpl.cpp
	apeFileGeometryImpl.cpp
	apePlaneGeometryImpl.cpp
	apeBoxGeometryImpl.cpp
	apeIndexedFaceSetGeometryImpl.cpp
	apeIndexedLineSetGeometryImpl.cpp
	apeCylinderGeometryImpl.cpp
	apeCloneGeometryImpl.cpp
	apeConeGeometryImpl.cpp
	apeTorusGeometryImpl.cpp
	apeTubeGeometryImpl.cpp
	apeSphereGeometryImpl.cpp
	apeFileMaterialImpl.cpp
	apeManualMaterialImpl.cpp
	apeManualTextureImpl.cpp
	apeUnitTextureImpl.cpp
	apeRayGeometryImpl.cpp
	apeWaterImpl.cpp
	apeSkyImpl.cpp
	apePointCloudImpl.cpp
	apeRigidBodyImpl.cpp
	network/apeReplica.cpp
	network/apeSceneNetworkImpl.cpp
	network/apeReplicaManagerConnection.cpp
	network/apeReplicaManager.cpp
	network/apeLobbyManager.cpp
	network/apeHttpManager.cpp
	)

include_directories(
	${PROJECT_SOURCE_DIR}/core/sceneManager
	${PROJECT_SOURCE_DIR}/core/sceneManager/network
	${PROJECT_SOURCE_DIR}/core/eventManager
	${PROJECT_SOURCE_DIR}/core/logManager
	${RAKNET_INCLUDE_DIR}
	${CURL_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/network/3rdParty/rapidjson/include
	${MY_ZIP_INCLUDE_DIR}
	)

if(ANDROID)
	include_directories(${OPENSSL_INCLUDE_DIRS})
endif()

# Add the dinamic library target to the project
add_library(apeSceneManager SHARED ${SOURCES} ${HEADERS})

# Link with dependency libraries and add preprocessor definitions with lists
set(APE_LINK_LIBS_SCENEMANAGER
	apeCoreConfig
	apeEventManager
	apeLogManager
	RAKNET
	MY_ZIP
	)

set(APE_PREPROC_DEF_SCENEMANAGER
	BUILDING_APE_SCENEMANAGER_DLL
	)

if (${CURL_READY})
	list(APPEND APE_LINK_LIBS_SCENEMANAGER MY_CURL)
	list(APPEND APE_PREPROC_DEF_SCENEMANAGER HTTPMANAGER_USE_CURL)
endif ()

if (WIN32)
	list(APPEND APE_LINK_LIBS_SCENEMANAGER ws2_32)
elseif(ANDROID)
	list(APPEND APE_LINK_LIBS_SCENEMANAGER libssl libcrypto)
endif ()

target_link_libraries(apeSceneManager ${APE_LINK_LIBS_SCENEMANAGER})
set_property(TARGET apeSceneManager PROPERTY COMPILE_DEFINITIONS ${APE_PREPROC_DEF_SCENEMANAGER} APE_SOURCE_DIR="${APE_SOURCE_DIR}")

# Place target xxx into the specified folder in IDEs supporting it
set_property(TARGET apeSceneManager PROPERTY FOLDER "Core")
