/*
 -----------------------------------------------------------------------------
 This source file is part of ogre-procedural

 For the latest info, see http://code.google.com/p/ogre-procedural/

 Copyright (c) 2010-2013 Michael Broutin

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */

#include "ProceduralStableHeaders.h"
#include "OgreException.h"
#include "ProceduralMeshModifiers.h"
#include "ProceduralGeometryHelpers.h"

using namespace Ogre;

namespace Procedural
{
//--------------------------------------------------------------
void MeshLinearTransform::modify(TriangleBuffer::Section& inputSection) const
{
	std::vector<TriangleBuffer::Vertex>& vertices = inputSection.buffer->getVertices();
	for (size_t i = inputSection.mFirstVertex; i <= inputSection.mLastVertex; ++i)
		vertices[i].mPosition += mTranslation;
}
//--------------------------------------------------------------
void MeshUVTransform::modify(TriangleBuffer::Section& inputSection) const
{
	std::vector<TriangleBuffer::Vertex>& vertices = inputSection.buffer->getVertices();
	for (size_t i = inputSection.mFirstVertex; i <= inputSection.mLastVertex; ++i)
		vertices[i].mUV = mOrigin + mTile * vertices[i].mUV;
	if (mSwitchUV)
		for (size_t i = inputSection.mFirstVertex; i <= inputSection.mLastVertex; ++i)
			std::swap(vertices[i].mUV.x, vertices[i].mUV.y);
}
//--------------------------------------------------------------
void SpherifyModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", "Procedural::SpherifyModifier::modify()");

	for (std::vector<TriangleBuffer::Vertex>::iterator it = mInputTriangleBuffer->getVertices().begin(); it != mInputTriangleBuffer->getVertices().end(); ++it)
	{
		Real l = (it->mPosition - mCenter).length();
		if (l > 1e-6)
		{
			it->mNormal = (it->mPosition - mCenter) / l;
			it->mPosition = mCenter + mRadius * it->mNormal;
		}
	}
}
//--------------------------------------------------------------
void CalculateNormalsModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);

	if (mComputeMode == NCM_TRIANGLE)
	{
		if (mMustWeldUnweldFirst)
			UnweldVerticesModifier().setInputTriangleBuffer(mInputTriangleBuffer).modify();

		const std::vector<int>& indices = mInputTriangleBuffer->getIndices();
		std::vector<TriangleBuffer::Vertex>& vertices = mInputTriangleBuffer->getVertices();
		for (size_t i = 0; i<indices.size(); i+=3)
		{
			Vector3 v1 = vertices[indices[i]].mPosition;
			Vector3 v2 = vertices[indices[i+1]].mPosition;
			Vector3 v3 = vertices[indices[i+2]].mPosition;
			Vector3 n = (v2-v1).crossProduct(v3-v1).normalisedCopy();
			vertices[indices[i]].mNormal = n;
			vertices[indices[i+1]].mNormal = n;
			vertices[indices[i+2]].mNormal = n;
		}
	}
	else
	{
		if (mMustWeldUnweldFirst)
			WeldVerticesModifier().setInputTriangleBuffer(mInputTriangleBuffer).modify();
		const std::vector<int>& indices = mInputTriangleBuffer->getIndices();
		std::vector<TriangleBuffer::Vertex>& vertices = mInputTriangleBuffer->getVertices();
		std::vector<std::vector<Vector3> > tmpNormals;
		tmpNormals.resize(vertices.size());
		for (size_t i = 0; i<indices.size(); i+=3)
		{
			Vector3 v1 = vertices[indices[i]].mPosition;
			Vector3 v2 = vertices[indices[i+1]].mPosition;
			Vector3 v3 = vertices[indices[i+2]].mPosition;
			Vector3 n = (v2-v1).crossProduct(v3-v1);
			tmpNormals[indices[i]].push_back(n);
			tmpNormals[indices[i+1]].push_back(n);
			tmpNormals[indices[i+2]].push_back(n);
		}
		for (size_t i = 0; i<vertices.size(); i++)
		{
			Vector3 n(Vector3::ZERO);
			for (size_t j = 0; j<tmpNormals[i].size(); j++)
				n += tmpNormals[i][j];
			vertices[i].mNormal = n.normalisedCopy();
		}
	}
}
//--------------------------------------------------------------
void WeldVerticesModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);
	std::map<Vector3, size_t, Vector3Comparator> mapExistingVertices;
	std::vector<TriangleBuffer::Vertex>& vertices = mInputTriangleBuffer->getVertices();
	std::vector<int>& indices = mInputTriangleBuffer->getIndices();

	size_t newSize = vertices.size();
	for (std::vector<TriangleBuffer::Vertex>::iterator it = vertices.begin(); it!= vertices.end(); ++it)
	{
		size_t currentIndex = it - vertices.begin();
		if (currentIndex>=newSize)
			break;
		if (mapExistingVertices.find(it->mPosition) == mapExistingVertices.end())
			mapExistingVertices[it->mPosition] = currentIndex;
		else
		{
			size_t existingIndex = mapExistingVertices[it->mPosition];
			--newSize;
			if (currentIndex == newSize )
			{
				for (std::vector<int>::iterator it2 = indices.begin(); it2 != indices.end(); ++it2)
					if (*it2 == currentIndex)
						*it2 = existingIndex;
			}
			else
			{
				size_t lastIndex = newSize;
				*it = vertices[lastIndex];
				for (std::vector<int>::iterator it2 = indices.begin(); it2 != indices.end(); ++it2)
				{
					if (*it2 == currentIndex)
						*it2 = existingIndex;
					else if (*it2 == lastIndex)
						*it2 = currentIndex;
				}
			}
		}
	}
	vertices.resize(newSize);
}
//--------------------------------------------------------------
void UnweldVerticesModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);
	std::vector<TriangleBuffer::Vertex> newVertices;
	const std::vector<TriangleBuffer::Vertex>& originVertices = mInputTriangleBuffer->getVertices();
	const std::vector<int>& originIndices = mInputTriangleBuffer->getIndices();
	for (size_t i=0; i<originIndices.size(); i+=3)
	{
		newVertices.push_back(originVertices[originIndices[i]]);
		newVertices.push_back(originVertices[originIndices[i+1]]);
		newVertices.push_back(originVertices[originIndices[i+2]]);
	}
	mInputTriangleBuffer->getVertices().clear();
	mInputTriangleBuffer->getVertices().reserve(newVertices.size());
	for (std::vector<TriangleBuffer::Vertex>::const_iterator it = newVertices.begin(); it != newVertices.end(); ++it)
		mInputTriangleBuffer->getVertices().push_back(*it);
	mInputTriangleBuffer->getIndices().clear();
	mInputTriangleBuffer->getIndices().reserve(newVertices.size());
	for (size_t i=0; i<newVertices.size(); i++)
		mInputTriangleBuffer->getIndices().push_back(i);
}
//--------------------------------------------------------------
void PlaneUVModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);
	Vector3 xvec = mPlaneNormal.perpendicular();
	Vector3 yvec = mPlaneNormal.crossProduct(xvec);
	for (std::vector<TriangleBuffer::Vertex>::iterator it = mInputTriangleBuffer->getVertices().begin(); it != mInputTriangleBuffer->getVertices().end(); ++it)
	{
		Vector3 v = it->mPosition - mPlaneCenter;
		it->mUV.x = v.dotProduct(xvec);
		it->mUV.y = v.dotProduct(yvec);
	}
}
//--------------------------------------------------------------
void SphereUVModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);
	for (std::vector<TriangleBuffer::Vertex>::iterator it = mInputTriangleBuffer->getVertices().begin(); it != mInputTriangleBuffer->getVertices().end(); ++it)
	{
		Vector3 v = it->mPosition.normalisedCopy();
		Vector2 vxz(v.x, v.z);
		it->mUV.x = Vector2::UNIT_X.angleTo(vxz).valueRadians() / Math::TWO_PI;
		it->mUV.y = (Math::ATan(v.y / vxz.length()).valueRadians() + Math::HALF_PI) / Math::PI;
	}
}
//--------------------------------------------------------------
void HemisphereUVModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);
	for (std::vector<TriangleBuffer::Vertex>::iterator it = mInputTriangleBuffer->getVertices().begin(); it != mInputTriangleBuffer->getVertices().end(); ++it)
	{
		Vector3 input = it->mPosition.normalisedCopy();
		Vector3 v;
		Radian r;
		if (input.y > 0)
			Vector3::UNIT_Y.getRotationTo(input).ToAngleAxis(r, v);
		else
			Vector3::NEGATIVE_UNIT_Y.getRotationTo(input).ToAngleAxis(r, v);
		Vector2 v2(input.x, input.z);
		v2.normalise();
		Vector2 uv = Vector2(.5, .5) + .5f * (r / Math::HALF_PI).valueRadians() * v2;

		if (input.y > 0)
			it->mUV = Utils::reframe(mTextureRectangleTop, uv);
		else
			it->mUV = Utils::reframe(mTextureRectangleBottom, uv);
	}
}
//--------------------------------------------------------------
void CylinderUVModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);
	if (mHeight <=0)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Height must be strictly positive", __FUNCTION__);
	if (mRadius <= 0)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Radius must be strictly positive", __FUNCTION__);

	Real angleThreshold = Math::ATan(mHeight / mRadius).valueRadians();
	for (std::vector<TriangleBuffer::Vertex>::iterator it = mInputTriangleBuffer->getVertices().begin(); it != mInputTriangleBuffer->getVertices().end(); ++it)
	{
		Vector2 nxz(it->mNormal.x, it->mNormal.z);
		Real alpha = (Math::ATan(it->mNormal.y / nxz.length()).valueRadians() + Math::HALF_PI);
		if (Math::Abs(alpha) > angleThreshold)
		{
			Vector2 vxz(it->mPosition.x, it->mPosition.z);
			it->mUV = vxz / mRadius;
		}
		else
		{
			Vector2 vxz(it->mPosition.x, it->mPosition.z);
			it->mUV.x = Vector2::UNIT_X.angleTo(vxz).valueRadians()/Math::TWO_PI;
			it->mUV.y = it->mPosition.y/mHeight - 0.5f;
		}
	}
}
//--------------------------------------------------------------
void BoxUVModifier::modify()
{
	if (mInputTriangleBuffer == NULL)
		OGRE_EXCEPT(Exception::ERR_INVALID_STATE, "Input triangle buffer must be set", __FUNCTION__);

	Vector3 directions[6] = { Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,Vector3::NEGATIVE_UNIT_X, Vector3::NEGATIVE_UNIT_Y, Vector3::NEGATIVE_UNIT_Z  };

	for (std::vector<TriangleBuffer::Vertex>::iterator it = mInputTriangleBuffer->getVertices().begin(); it != mInputTriangleBuffer->getVertices().end(); ++it)
	{
		Vector3 v = it->mPosition - mBoxCenter;
		if (v.isZeroLength())
			continue;
		//v.normalise();
		v.x/=mBoxSize.x;
		v.y/=mBoxSize.y;
		v.z/=mBoxSize.z;
		Vector3 n = it->mNormal;
		Real maxAxis = 0;
		int principalAxis = 0;
		for (unsigned char i = 0; i < 6; i++)
		{
			if (directions[i].dotProduct(n) > maxAxis)
			{
				maxAxis = directions[i].dotProduct(n);
				principalAxis = i;
			}
		}

		Vector3 vX, vY;
		if (principalAxis%3 == 1)
			vY = Vector3::UNIT_X;
		else
			vY = Vector3::UNIT_Y;
		vX = directions[principalAxis].crossProduct(vY);

		Vector2 uv(0.5-vX.dotProduct(v), 0.5-vY.dotProduct(v));
		if (mMappingType == MT_FULL)
			it->mUV = uv;
		else if (mMappingType == MT_CROSS)
		{
		}
		else if (mMappingType == MT_PACKED)
			it->mUV = Vector2((uv.x + principalAxis%3)/3, (uv.y + principalAxis/3)/2);
	}
}
}
