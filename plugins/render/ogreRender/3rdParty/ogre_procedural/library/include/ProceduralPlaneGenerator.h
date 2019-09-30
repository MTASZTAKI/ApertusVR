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
#ifndef PROCEDURAL_PLANE_GENERATOR_INCLUDED
#define PROCEDURAL_PLANE_GENERATOR_INCLUDED

#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
/**
 * \ingroup objgengrp
 * Builds a plane mesh
 * \image html primitive_plane.png
 * \note Note that X and Y values in that generator are not global X and Y,
 * but are computed to be : X = normal x global X and Y = normal x X
 */
class _ProceduralExport PlaneGenerator : public MeshGenerator<PlaneGenerator>
{
	unsigned int mNumSegX;
	unsigned int mNumSegY;
	Ogre::Vector3 mNormal;
	Ogre::Real mSizeX;
	Ogre::Real mSizeY;
public:

	PlaneGenerator(): mNumSegX(1), mNumSegY(1),
		mNormal(Ogre::Vector3::UNIT_Y),
		mSizeX(1), mSizeY(1)
	{}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/**
	Sets the number of segements along local X axis
	\exception Ogre::InvalidParametersException Minimum of numSegX is 1
	*/
	inline PlaneGenerator& setNumSegX(unsigned int numSegX)
	{
		if (numSegX == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::PlaneGenerator::setNumSegX(unsigned int)");
		mNumSegX = numSegX;
		return *this;
	}

	/**
	Sets the number of segments along local Y axis
	\exception Ogre::InvalidParametersException Minimum of numSegY is 1
	*/
	inline PlaneGenerator& setNumSegY(unsigned int numSegY)
	{
		if (numSegY == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::PlaneGenerator::setNumSegY(unsigned int)");
		mNumSegY = numSegY;
		return *this;
	}

	/**
	Sets the normal of the plane
	\exception Ogre::InvalidParametersException Normal must not be null
	*/
	inline PlaneGenerator& setNormal(Ogre::Vector3 normal)
	{
		if (mNormal.isZeroLength())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Normal must not be null", "Procedural::PlaneGenerator::setNormal(unsigned int)");
		mNormal = normal;
		return *this;
	}

	/**
	Sets the size of the plane along local X axis
	\exception Ogre::InvalidParametersException X size must be larger than 0!
	*/
	inline PlaneGenerator& setSizeX(Ogre::Real sizeX)
	{
		if (sizeX <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "X size must be larger than 0!", "Procedural::BoxGenerator::setSizeX(Ogre::Real)");
		mSizeX = sizeX;
		return *this;
	}

	/**
	Sets the size of the plane along local Y axis
	\exception Ogre::InvalidParametersException Y size must be larger than 0!
	*/
	inline PlaneGenerator& setSizeY(Ogre::Real sizeY)
	{
		if (sizeY <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Y size must be larger than 0!", "Procedural::BoxGenerator::setSizeY(Ogre::Real)");
		mSizeY = sizeY;
		return *this;
	}

	/** Sets the size (default=1,1) */
	inline PlaneGenerator& setSize(Ogre::Vector2 size)
	{
		setSizeX(size.x);
		setSizeY(size.y);
		return *this;
	}
};
}
#endif
