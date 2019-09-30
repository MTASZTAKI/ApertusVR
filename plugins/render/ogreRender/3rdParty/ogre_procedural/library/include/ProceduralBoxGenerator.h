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
#ifndef PROCEDURAL_BOX_GENERATOR_INCLUDED
#define PROCEDURAL_BOX_GENERATOR_INCLUDED

#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
/**
 * \ingroup objgengrp
 * Generates a box mesh centered on the origin.
 * Default size is 1.0 with 1 quad per face.
 * \image html primitive_box.png
 */
class _ProceduralExport BoxGenerator : public MeshGenerator<BoxGenerator>
{
	Ogre::Real mSizeX,mSizeY,mSizeZ;
	unsigned int mNumSegX,mNumSegY,mNumSegZ;
public:

	/// Contructor with arguments
	BoxGenerator(Ogre::Real sizeX=1.f, Ogre::Real sizeY=1.f, Ogre::Real sizeZ=1.f, unsigned int numSegX=1, unsigned int numSegY=1, unsigned int numSegZ=1) :
		mSizeX(sizeX), mSizeY(sizeY), mSizeZ(sizeZ), mNumSegX(numSegX), mNumSegY(numSegY), mNumSegZ(numSegZ) {}

	/**
	Sets size along X axis (default=1)
	\exception Ogre::InvalidParametersException X size must be larger than 0!
	*/
	BoxGenerator& setSizeX(Ogre::Real sizeX)
	{
		if (sizeX <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "X size must be larger than 0!", "Procedural::BoxGenerator::setSizeX(Ogre::Real)");
		mSizeX = sizeX;
		return *this;
	}

	/**
	Sets size along Y axis (default=1)
	\exception Ogre::InvalidParametersException Y size must be larger than 0!
	*/
	BoxGenerator& setSizeY(Ogre::Real sizeY)
	{
		if (sizeY <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Y size must be larger than 0!", "Procedural::BoxGenerator::setSizeY(Ogre::Real)");
		mSizeY = sizeY;
		return *this;
	}

	/**
	Sets size along Z axis (default=1)
	\exception Ogre::InvalidParametersException Z size must be larger than 0!
	*/
	BoxGenerator& setSizeZ(Ogre::Real sizeZ)
	{
		if (sizeZ <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Z size must be larger than 0!", "Procedural::BoxGenerator::setSizeZ(Ogre::Real)");
		mSizeZ = sizeZ;
		return *this;
	}

	/** Sets the size (default=1,1,1) */
	BoxGenerator& setSize(Ogre::Vector3 size)
	{
		setSizeX(size.x);
		setSizeY(size.y);
		setSizeZ(size.z);
		return *this;
	}

	/**
	Sets the number of segments along X axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegX is 1
	*/
	BoxGenerator& setNumSegX(unsigned int numSegX)
	{
		if (numSegX == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BoxGenerator::setNumSegX(unsigned int)");
		mNumSegX = numSegX;
		return *this;
	}

	/**
	Sets the number of segments along Y axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegY is 1
	*/
	BoxGenerator& setNumSegY(unsigned int numSegY)
	{
		if (numSegY == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BoxGenerator::setNumSegY(unsigned int)");
		mNumSegY = numSegY;
		return *this;
	}

	/**
	Sets the number of segments along Z axis (default=1)
	\exception Ogre::InvalidParametersException Minimum of numSegZ is 1
	*/
	BoxGenerator& setNumSegZ(unsigned int numSegZ)
	{
		if (numSegZ == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BoxGenerator::setNumSegZ(unsigned int)");
		mNumSegZ = numSegZ;
		return *this;
	}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

};


}
#endif
