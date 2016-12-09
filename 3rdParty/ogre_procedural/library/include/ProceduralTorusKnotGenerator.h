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
#ifndef PROCEDURAL_TORUS_KNOT_GENERATOR_INCLUDED
#define PROCEDURAL_TORUS_KNOT_GENERATOR_INCLUDED

#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
/**
 * \ingroup objgengrp
 * Builds a torus knot mesh
 * \image html primitive_torusknot.png
 */
class _ProceduralExport TorusKnotGenerator : public MeshGenerator<TorusKnotGenerator>
{
	unsigned int mNumSegSection;
	unsigned int mNumSegCircle;
	Ogre::Real mRadius;
	Ogre::Real mSectionRadius;
	int mP;
	int mQ;
public:
	/// Constructor with arguments
	TorusKnotGenerator(Ogre::Real radius=1.f, Ogre::Real sectionRadius=.2f, int p=2, int q=3, unsigned int numSegSection=8, unsigned int numSegCircle=16) :
		mNumSegSection(numSegSection),
		mNumSegCircle(numSegCircle),
		mRadius(radius),
		mSectionRadius(sectionRadius),
		mP(p),
		mQ(q) {}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/**
	Sets the number of segments along the section (default=8)
	\exception Ogre::InvalidParametersException Minimum of numSegCircle is 1
	*/
	inline TorusKnotGenerator& setNumSegSection(unsigned int numSegSection)
	{
		if (numSegSection == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::TorusKnotGenerator::setNumSegSection(unsigned int)");
		mNumSegSection = numSegSection;
		return *this;
	}

	/**
	Sets the number of segments along the circle (default=16)
	\exception Ogre::InvalidParametersException Minimum of numSegCircle is 1
	*/
	inline TorusKnotGenerator& setNumSegCircle(unsigned int numSegCircle)
	{
		if (numSegCircle == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::TorusKnotGenerator::setNumSegCircle(unsigned int)");
		mNumSegCircle = numSegCircle;
		return *this;
	}

	/**
	Sets the main radius of the knot (default=1)
	\exception Ogre::InvalidParametersException Radius must be larger than 0!
	*/
	inline TorusKnotGenerator& setRadius(Ogre::Real radius)
	{
		if (radius <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::TorusKnotGenerator::setRadius(Ogre::Real)");
		mRadius = radius;
		return *this;
	}

	/**
	Sets the section radius (default=0.2)
	\exception Ogre::InvalidParametersException Radius must be larger than 0!
	*/
	inline TorusKnotGenerator& setSectionRadius(Ogre::Real sectionRadius)
	{
		if (sectionRadius <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::TorusKnotGenerator::setSectionRadius(Ogre::Real)");
		mSectionRadius = sectionRadius;
		return *this;
	}

	/**
	Sets the p parameter of the knot (default=2)
	\exception Ogre::InvalidParametersException Parameter p must be larger than 0!
	*/
	inline TorusKnotGenerator& setP(int p)
	{
		if (p <= 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Parameter p must be larger than 0!", "Procedural::TorusKnotGenerator::setP(int)");
		mP = p;
		return *this;
	}

	/**
	Sets the q parameter of the knot (default=3)
	\exception Ogre::InvalidParametersException Parameter q must be larger than 0!
	*/
	inline TorusKnotGenerator& setQ(int q)
	{
		if (q <= 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Parameter q must be larger than 0!", "Procedural::TorusKnotGenerator::setQ(int)");
		mQ = q;
		return *this;
	}

};
}
#endif
