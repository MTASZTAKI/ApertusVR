#MIT License
#
#Copyright (c) 2016 - present MTA SZTAKI
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
		ape.h
		apeColor.h
		apeDegree.h
		apeDoubleQueue.h
		apeEntity.h
		apeEuler.h
		apeEvent.h
		apeFileSystem.h
		apeGeometry.h
		apeHeadTrackingConfigs.h
		apeIBoxGeometry.h
		apeIBrowser.h
		apeICamera.h
		apeICloneGeometry.h
		apeIConeGeometry.h
		apeICoreConfig.h
		apeICylinderGeometry.h
		apeIEventManager.h
		apeIFileGeometry.h
		apeIFileMaterial.h
		apeIFileTexture.h
		apeIIndexedFaceSetGeometry.h
		apeIIndexedLineSetGeometry.h
		apeILight.h
		apeILogManager.h
		apeIManualMaterial.h
		apeIManualTexture.h
		apeINode.h
		apeInterpolator.h
		apeIPlaneGeometry.h
		apeIPlugin.h
		apeIPluginManager.h
		apeIPointCloud.h
		apeIRayGeometry.h
		apeIRigidBody.h
		apeISceneManager.h
		apeISceneNetwork.h
		apeISky.h
		apeISphereGeometry.h
		apeITextGeometry.h
		apeITorusGeometry.h
		apeITubeGeometry.h
		apeIUnitTexture.h
		apeIWater.h
		apeMaterial.h
		apeMatrix3.h
		apeMatrix4.h
		apePlatform.h
		apePluginAPI.h
		apePluginDefines.h
		apeQuaternion.h
		apeRadian.h
		apeSingleton.h
		apeSubGeometry.h
		apeSystem.h
		apeTexture.h
		apeVector2.h
		apeVector3.h
		apeVector4.h
	)

add_custom_target(
    Include ALL
    SOURCES ${HEADERS}
)

set_property (TARGET Include PROPERTY FOLDER "Include")