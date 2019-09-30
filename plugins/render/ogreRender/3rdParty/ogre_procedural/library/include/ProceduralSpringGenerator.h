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
#ifndef PROCEDURAL_SPRING_GENERATORS_INCLUDED
#define PROCEDURAL_PRING_GENERATORS_INCLUDED

#include "ProceduralPath.h"
#include "ProceduralPathGenerators.h"
#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
//-----------------------------------------------------------------------
/**
 * \ingroup pathgrp
 * Produces a helix path
 * \image html spline_helix.png
 */
class _ProceduralExport HelixPath
{
	Ogre::Real mHeight, mRadius;
	unsigned int mNumSegPath;
	Ogre::Real mNumRound;

public:
	/// Default constructor
	HelixPath() : mHeight(1.0f), mRadius(1.0f), mNumRound(5.0), mNumSegPath(8) {}

	/// Sets the height of the helix (default=1.0)
	/// \exception Ogre::InvalidParametersException Height must be larger than 0!
	inline HelixPath& setHeight(Ogre::Real height)
	{
		if (height <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Height must be larger than 0!", "Procedural::HelixPath::setHeight(Ogre::Real)");
		mHeight = height;
		return *this;
	}

	/// Sets the radius of the helix (default = 1.0)
	/// \exception Ogre::InvalidParametersException Radius must be larger than 0!
	inline HelixPath& setRadius(Ogre::Real radius)
	{
		if (radius <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::HelixPath::setRadius(Ogre::Real)");
		mRadius = radius;
		return *this;
	}

	/// Sets the number of rounds (default = 5.0)
	/// \exception Ogre::InvalidParametersException You have to rotate more then 0 times!
	inline HelixPath& setNumRound(Ogre::Real numRound)
	{
		if (numRound <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "You have to rotate more then 0 times!", "Procedural::HelixPath::setNumRound(Ogre::Real)");
		mNumRound = numRound;
		return *this;
	}

	/// Sets number of segments along the path per turn
	/// \exception Ogre::InvalidParametersException Minimum of numSeg is 1
	inline HelixPath& setNumSegPath(unsigned int numSeg)
	{
		if (numSeg == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::HelixPath::setNumSegPath(unsigned int)");
		mNumSegPath = numSeg;
		return *this;
	}

	/**
	 * Builds a shape from control points
	 */
	Path realizePath();
};

//-----------------------------------------------------------------------
/**
 * \ingroup objgengrp
 * Generates a spring mesh centered on the origin.
 * \image html primitive_spring.png
 */
class _ProceduralExport SpringGenerator : public MeshGenerator<SpringGenerator>
{
	Ogre::Real mHeight, mRadiusHelix, mRadiusCircle;
	int mNumSegPath, mNumSegCircle;
	Ogre::Real mNumRound;

public:
	/// Contructor with arguments
	SpringGenerator(Ogre::Real height=1.f, Ogre::Real radiusHelix=1.f, Ogre::Real radiusCircle=0.2f, Ogre::Real numRound=5.0, int numSegPath=10, int numSegCircle=8) :
		mHeight(height), mRadiusHelix(radiusHelix), mRadiusCircle(radiusCircle), mNumRound(numRound), mNumSegPath(numSegPath), mNumSegCircle(numSegCircle) {}

	/// Sets the height of the spring (default=1)
	/// \exception Ogre::InvalidParametersException Height must be larger than 0!
	inline SpringGenerator& setHeight(Ogre::Real height)
	{
		if (height <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Height must be larger than 0!", "Procedural::SpringGenerator::setHeight(Ogre::Real)");
		mHeight = height;
		return *this;
	}

	/// Sets helix radius (default=1)
	/// \exception Ogre::InvalidParametersException Radius must be larger than 0!
	inline SpringGenerator& setRadiusHelix(Ogre::Real radiusHelix)
	{
		if (radiusHelix <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::SpringGenerator::setRadiusHelix(Ogre::Real)");
		mRadiusHelix = radiusHelix;
		return *this;
	}

	/// Sets radius for extruding circle (default=0.1)
	/// \exception Ogre::InvalidParametersException Radius must be larger than 0!
	inline SpringGenerator& setRadiusCircle(Ogre::Real radiusCircle)
	{
		if (radiusCircle <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::SpringGenerator::setRadiusCircle(Ogre::Real)");
		mRadiusCircle = radiusCircle;
		return *this;
	}

	/// Sets the number of segments along the height of the spring (default=1)
	/// \exception Ogre::InvalidParametersException You have to rotate more then 0 times!
	inline SpringGenerator& setNumRound (Ogre::Real numRound)
	{
		if (numRound <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "You have to rotate more then 0 times!", "Procedural::SpringGenerator::setNumRound(Ogre::Real)");
		mNumRound = numRound;
		return *this;
	}

	/// Sets the number of segments along helix path (default=10)
	/// \exception Ogre::InvalidParametersException Minimum of numSegPath is 1
	inline SpringGenerator& setNumSegPath(int numSegPath)
	{
		if (numSegPath == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::SpringGenerator::setNumSegPath(unsigned int)");
		mNumSegPath = numSegPath;
		return *this;
	}

	/// Sets the number of segments for extruding circle (default=8)
	/// \exception Ogre::InvalidParametersException Minimum of numSegCircle is 1
	inline SpringGenerator& setNumSegCircle(int numSegCircle)
	{
		if (numSegCircle == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::SpringGenerator::setNumSegCircle(unsigned int)");
		mNumSegCircle = numSegCircle;
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
