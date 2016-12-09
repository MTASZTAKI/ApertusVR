/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

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
#include "ProceduralSpringGenerator.h"
#include "ProceduralShape.h"
#include "ProceduralShapeGenerators.h"
#include "ProceduralPath.h"
#include "ProceduralPathGenerators.h"
#include "ProceduralExtruder.h"

using namespace Ogre;

namespace Procedural
{
//-----------------------------------------------------------------------
Path HelixPath::realizePath()
{
	Path helix;
	Real angleStep = Math::TWO_PI / static_cast<Real>(mNumSegPath);
	Real heightStep = mHeight / static_cast<Real>(mNumSegPath);

	for (size_t i=0; i<mNumRound*mNumSegPath; i++)
	{
		helix.addPoint(mRadius * Math::Cos(angleStep * i), heightStep * i, mRadius * Math::Sin(angleStep * i));
	}

	return helix;
}

//-----------------------------------------------------------------------
void SpringGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	Path p = HelixPath().setHeight(mHeight).setNumRound(mNumRound).setNumSegPath(mNumSegPath).setRadius(mRadiusHelix).realizePath();

	Shape s = CircleShape().setRadius(mRadiusCircle).setNumSeg(mNumSegCircle).realizeShape();

	Extruder().setExtrusionPath(&p).setShapeToExtrude(&s).addToTriangleBuffer(buffer);
}

}
