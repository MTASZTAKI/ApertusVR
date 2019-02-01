/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_OGREPOINTCLOUD_H
#define APE_OGREPOINTCLOUD_H

#include <iostream>
#include "Ogre.h"
#include "sceneelements/ApeINode.h"

namespace Ape {
	class OgrePointCloud
	{
	public:
		OgrePointCloud(const std::string& name, const std::string& resourcegroup, const int numpoints, float *parray, float *carray, float boundigSphereRadius,
			Ape::NodeWeakPtr userNode, Ape::NodeWeakPtr pointCloudNode, float pointScaleOffset, float unitScaleDistance);

		void updateVertexPositions(int size, float *points);

		void updateVertexColours(int size, float *colours);

		virtual ~OgrePointCloud();

	private:
		int mSize;

		Ogre::MeshPtr mMesh;

		Ogre::SubMesh* mSubMesh;

		Ogre::MaterialPtr mMaterial;

		Ape::NodeWeakPtr mUserNode;

		Ape::NodeWeakPtr mPointCloudNode;

		float mPointScaleOffset;

		float  mUnitScaleDistance;

		Ogre::HardwareVertexBufferSharedPtr mVbuf;

		Ogre::HardwareVertexBufferSharedPtr mCbuf;

		Ogre::RenderSystem* mRenderSystemForVertex;
	};
}
#endif
