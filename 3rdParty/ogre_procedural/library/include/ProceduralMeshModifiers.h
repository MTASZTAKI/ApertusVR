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
#ifndef PROCEDURAL_MESH_MODIFIERS_INCLUDED
#define PROCEDURAL_MESH_MODIFIERS_INCLUDED

#include "ProceduralPlatform.h"
#include "ProceduralTriangleBuffer.h"

namespace Procedural
{
/**
\brief Translates a mesh
*/
class _ProceduralExport MeshLinearTransform
{
	Ogre::Vector3 mTranslation;
	Ogre::Quaternion mRotation;
public:
	MeshLinearTransform() : mTranslation(Ogre::Vector3::ZERO), mRotation(Ogre::Quaternion::IDENTITY) {}

	void modify(TriangleBuffer::Section& inputTriangleBuffer) const;

	MeshLinearTransform& setTranslation(const Ogre::Vector3& translation)
	{
		mTranslation = translation;
		return *this;
	}

	MeshLinearTransform& setRotation(const Ogre::Quaternion& rotation)
	{
		mRotation = rotation;
		return *this;
	}
};
//--------------------------------------------------------------
/**
\brief Linear transforms on a mesh's UVs
*/
class _ProceduralExport MeshUVTransform
{
	Ogre::Vector2 mTile;
	Ogre::Vector2 mOrigin;
	bool mSwitchUV;
public:
	MeshUVTransform() : mTile(Ogre::Vector2::ZERO), mOrigin(Ogre::Vector2::ZERO), mSwitchUV(false) {}

	void modify(TriangleBuffer::Section& inputTriangleBuffer) const;

	MeshUVTransform& setTile(const Ogre::Vector2& tile)
	{
		mTile = tile;
		return *this;
	}

	MeshUVTransform& setOrigin(const Ogre::Vector2& origin)
	{
		mOrigin = origin;
		return *this;
	}

	MeshUVTransform& setSwitchUV(bool switchUV)
	{
		mSwitchUV = switchUV;
		return *this;
	}
};
//--------------------------------------------------------------
/**
 \brief Projects all TriangleBufferVertices on a sphere
 */
class _ProceduralExport SpherifyModifier
{
private:
	TriangleBuffer* mInputTriangleBuffer;
	Ogre::Vector3 mCenter;
	Ogre::Real mRadius;

public:

	SpherifyModifier() :
		mInputTriangleBuffer(0), mCenter(Ogre::Vector3::ZERO), mRadius(1)
	{
	}

	/// \exception Ogre::InvalidParametersException Input triangle buffer must not be null
	SpherifyModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		if (inputTriangleBuffer == NULL)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Input triangle buffer must not be null",
			            "Procedural::SpherifyModifier::setInputTriangleBuffer(Procedural::TriangleBuffer*)");
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	SpherifyModifier& setRadius(Ogre::Real radius)
	{
		if (mRadius <= 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be positive",
			            "Procedural::SpherifyModifier::setInputTriangleBuffer(Procedural::TriangleBuffer*)");
		mRadius = radius;
		return *this;
	}

	SpherifyModifier& setCenter(Ogre::Vector3 center)
	{
		mCenter = center;
		return *this;
	}

	/// \exception Ogre::InvalidStateException Input triangle buffer must be set
	void modify();
};

//--------------------------------------------------------------
/**
WIP
*/
class _ProceduralExport CalculateNormalsModifier
{
public:
	CalculateNormalsModifier() : mComputeMode(NCM_VERTEX), mInputTriangleBuffer(0), mMustWeldUnweldFirst(true) {}

	enum NormalComputeMode
	{
	    NCM_VERTEX, NCM_TRIANGLE
	};

	NormalComputeMode mComputeMode;
	TriangleBuffer* mInputTriangleBuffer;
	bool mMustWeldUnweldFirst;

	CalculateNormalsModifier& setComputeMode(NormalComputeMode computeMode)
	{
		mComputeMode = computeMode;
		return *this;
	}

	CalculateNormalsModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	/**
	 * Tells if the mesh must be first weld (NCM_VERTEX mode) or unweld (NCM_TRIANGLE) before computing normals.
	 * Has a performance impact if enabled.
	 * Default : true.
	 */
	CalculateNormalsModifier& setMustWeldUnweldFirst(bool mustWeldUnweldFirst)
	{
		mMustWeldUnweldFirst = mustWeldUnweldFirst;
		return *this;
	}

	void modify();
};
//--------------------------------------------------------------
/**
 * Welds together the vertices which are 'close enough' one to each other
 */
class _ProceduralExport WeldVerticesModifier
{
public:
	WeldVerticesModifier() : mInputTriangleBuffer(0), mTolerance(1e-3f) {}

	TriangleBuffer* mInputTriangleBuffer;
	Ogre::Real mTolerance;


	/// The triangle buffer to modify
	WeldVerticesModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	/// The tolerance in position to consider that 2 vertices are the same (default = 1e-3)
	WeldVerticesModifier& setTolerance(Ogre::Real tolerance)
	{
		mTolerance = tolerance;
		return *this;
	}

	void modify();
};
//--------------------------------------------------------------
/**
 * \brief Switches the triangle buffer from indexed triangles to (pseudo) triangle list
 * It can be used if you want discontinuities between all your triangles.
*/
class _ProceduralExport UnweldVerticesModifier
{
public:
	UnweldVerticesModifier() : mInputTriangleBuffer(0) {}

	TriangleBuffer* mInputTriangleBuffer;

	UnweldVerticesModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	void modify();
};
//--------------------------------------------------------------
/**
 * \brief Recomputes the mesh's UVs based on its projection on a plane
 */
class _ProceduralExport PlaneUVModifier
{
	Ogre::Vector3 mPlaneNormal;
	Ogre::Vector3 mPlaneCenter;
	Ogre::Vector2 mPlaneSize;
	TriangleBuffer* mInputTriangleBuffer;
public:

	PlaneUVModifier() :
		mPlaneNormal(Ogre::Vector3::UNIT_Y), mPlaneCenter(Ogre::Vector3::ZERO), mPlaneSize(Ogre::Vector2::UNIT_SCALE), mInputTriangleBuffer(0)
	{
	}

	PlaneUVModifier& setPlaneNormal(const Ogre::Vector3& planeNormal)
	{
		mPlaneNormal = planeNormal;
		return *this;
	}

	PlaneUVModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	PlaneUVModifier& setPlaneCenter(const Ogre::Vector3& planeCenter)
	{
		mPlaneCenter = planeCenter;
		return *this;
	}

	PlaneUVModifier& setPlaneSize(const Ogre::Vector2& planeSize)
	{
		mPlaneSize = planeSize;
		return *this;
	}

	/// \exception Ogre::InvalidStateException Input triangle buffer must be set
	void modify();
};
//--------------------------------------------------------------
class _ProceduralExport SphereUVModifier
{
	TriangleBuffer* mInputTriangleBuffer;

public:
	void modify();

	SphereUVModifier() :
		mInputTriangleBuffer(0)
	{
	}

	SphereUVModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}
};
//--------------------------------------------------------------
class _ProceduralExport HemisphereUVModifier
{
	TriangleBuffer* mInputTriangleBuffer;
	Ogre::RealRect mTextureRectangleTop;
	Ogre::RealRect mTextureRectangleBottom;
public:
	void modify();

	HemisphereUVModifier() :
		mInputTriangleBuffer(0), mTextureRectangleTop(0,0,1,1), mTextureRectangleBottom(0,0,1,1)
	{
	}

	HemisphereUVModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	HemisphereUVModifier& setTextureRectangleTop(const Ogre::RealRect& textureRectangleTop)
	{
		mTextureRectangleTop = textureRectangleTop;
		return *this;
	}

	HemisphereUVModifier& setTextureRectangleBottom(const Ogre::RealRect& textureRectangleBottom)
	{
		mTextureRectangleBottom = textureRectangleBottom;
		return *this;
	}

};
//--------------------------------------------------------------
class _ProceduralExport CylinderUVModifier
{
	TriangleBuffer* mInputTriangleBuffer;
	Ogre::Real mHeight;
	Ogre::Real mRadius;
public:
	void modify();

	CylinderUVModifier() :
		mInputTriangleBuffer(0), mRadius(1.0), mHeight(1.0)
	{
	}

	CylinderUVModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	CylinderUVModifier& setRadius(Ogre::Real radius)
	{
		mRadius = radius;
		return *this;
	}

	CylinderUVModifier& setHeight(Ogre::Real height)
	{
		mHeight = height;
		return *this;
	}

};
//--------------------------------------------------------------
class _ProceduralExport BoxUVModifier
{
public:
	enum MappingType
	{
	    MT_FULL, MT_CROSS, MT_PACKED,
	};
private:
	TriangleBuffer* mInputTriangleBuffer;
	MappingType mMappingType;
	Ogre::Vector3 mBoxSize;
	Ogre::Vector3 mBoxCenter;
public:

	void modify();

	BoxUVModifier() :
		mInputTriangleBuffer(0), mMappingType(MT_FULL), mBoxSize(Ogre::Vector3::UNIT_SCALE), mBoxCenter(Ogre::Vector3::ZERO)
	{
	}

	BoxUVModifier& setInputTriangleBuffer(TriangleBuffer* inputTriangleBuffer)
	{
		mInputTriangleBuffer = inputTriangleBuffer;
		return *this;
	}

	BoxUVModifier& setBoxSize(const Ogre::Vector3& boxSize)
	{
		mBoxSize = boxSize;
		return *this;
	}

	BoxUVModifier& setBoxCenter(const Ogre::Vector3& boxCenter)
	{
		mBoxCenter = boxCenter;
		return *this;
	}

	BoxUVModifier& setMappingType(MappingType mappingType)
	{
		mMappingType = mappingType;
		return *this;
	}
};
}

#endif
