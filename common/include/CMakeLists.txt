#MIT License
#
#Copyright (c) 2016 MTA SZTAKI
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
    Ape.h
	ApeColor.h
	ApeEntity.h
	ApeEvent.h
	ApeITextGeometry.h
	ApeICamera.h
	ApeILight.h
	ApeMaterial.h
	ApePass.h
	ApeIPbsPass.h
	ApeIManualPass.h
	ApeIManualMaterial.h
	ApeIFileMaterial.h
	ApeINode.h
	ApeIPluginManager.h
	ApeIPlugin.h
	ApePluginAPI.h
	ApePluginDefines.h
	ApeQuaternion.h
	ApeIScene.h
	ApeTexture.h
	ApeIManualTexture.h
	ApeVector2.h
	ApeVector3.h
	ApeVector4.h
	ApeGeometry.h
	ApeSubGeometry.h
	ApeIIndexedFaceSetGeometry.h
	ApeIIndexedLineSetGeometry.h
	ApeIFileGeometry.h
	ApeIPlaneGeometry.h
	ApeITorusGeometry.h
	ApeIConeGeometry.h
	ApeITubeGeometry.h
	ApeIBoxGeometry.h
	ApeISphereGeometry.h
	ApeICylinderGeometry.h
	ApeSingleton.h
	ApeIEventManager.h
	ApeISceneSession.h
	ApeDegree.h
	ApeRadian.h
	ApeEuler.h
	ApeMatrix4.h
	ApeSystem.h
	ApeISystemConfig.h
	ApeDoubleQueue.h
	ApeIMainWindow.h
	ApeInterpolator.h
	ApeIBrowser.h
	)

add_custom_target(
    Include ALL
    SOURCES ${HEADERS}
)

set_property (TARGET Include PROPERTY FOLDER "Common")
