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
#ifndef PROCEDURAL_ICOSPHERE_GENERATOR_INCLUDED
#define PROCEDURAL_ICOSPHERE_GENERATOR_INCLUDED

#include "ProceduralMeshGenerator.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
/**
 * \ingroup objgengrp
 * Builds an icosphere mesh, ie a sphere built with equally sized triangles
 * \image html primitive_icosphere.png
 */
class _ProceduralExport IcoSphereGenerator : public MeshGenerator<IcoSphereGenerator>
{
	Ogre::Real mRadius;
	unsigned int mNumIterations;

public:
	/// Contructor with arguments
	IcoSphereGenerator(Ogre::Real radius = 1.f, unsigned int numIterations = 2) :
		mRadius(radius),
		mNumIterations(numIterations)
	{}

	/**
	 * Builds the mesh into the given TriangleBuffer
	 * @param buffer The TriangleBuffer on where to append the mesh.
	 */
	void addToTriangleBuffer(TriangleBuffer& buffer) const;

	/**
	Sets the radius of the sphere (default=1)
	\exception Ogre::InvalidParametersException Radius must be larger than 0!
	*/
	inline IcoSphereGenerator& setRadius(Ogre::Real radius)
	{
		if (radius <= 0.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Radius must be larger than 0!", "Procedural::IcoSphereGenerator::setRadius(Ogre::Real)");
		mRadius = radius;
		return *this;
	}

	/** Sets the number of iterations needed to build the sphere mesh.
		First iteration corresponds to a 20 face sphere.
		Each iteration has 3 more faces than the previous.
		(default=2)
	\exception Ogre::InvalidParametersException Minimum of numIterations is 1
	*/
	inline IcoSphereGenerator& setNumIterations(unsigned int numIterations)
	{
		if (numIterations == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 iterations", "Procedural::IcoSphereGenerator::setNumRings(unsigned int)");
		mNumIterations = numIterations;
		return *this;
	}

};
}
#endif
