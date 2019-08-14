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

set (HEADERS
	apeIBoxGeometry.h
	apeIBrowser.h
	apeICamera.h
	apeICloneGeometry.h
	apeIConeGeometry.h
	apeICylinderGeometry.h
	apeIFileGeometry.h
	apeIFileMaterial.h
	apeIFileTexture.h
	apeIIndexedFaceSetGeometry.h
	apeIIndexedLineSetGeometry.h
	apeILight.h
	apeIManualMaterial.h
	apeIManualPass.h
	apeIManualTexture.h
	apeINode.h
	apeIPbsPass.h
	apeIPlaneGeometry.h
	apeIPointCloud.h
	apeIRayGeometry.h
	apeIRigidBody.h
	apeISky.h
	apeISphereGeometry.h
	apeITextGeometry.h
	apeITorusGeometry.h
	apeITubeGeometry.h
	apeIUnitTexture.h
	apeIWater.h
)

add_custom_target(
    SceneElements ALL
    SOURCES ${HEADERS}
)

set_property (TARGET SceneElements PROPERTY FOLDER "Common")
