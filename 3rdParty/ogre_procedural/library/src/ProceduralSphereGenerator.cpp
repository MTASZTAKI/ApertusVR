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
#include "ProceduralSphereGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void SphereGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mNumRings+1)*(mNumSegments+1));
	buffer.estimateIndexCount(mNumRings*(mNumSegments+1)*6);

	Real fDeltaRingAngle = (Math::PI / mNumRings);
	Real fDeltaSegAngle = (Math::TWO_PI / mNumSegments);
	int offset = 0;

	// Generate the group of rings for the sphere
	for (unsigned int ring = 0; ring <= mNumRings; ring++ )
	{
		Real r0 = mRadius * sinf (ring * fDeltaRingAngle);
		Real y0 = mRadius * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for (unsigned int seg = 0; seg <= mNumSegments; seg++)
		{
			Real x0 = r0 * sinf(seg * fDeltaSegAngle);
			Real z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			addPoint(buffer, Vector3(x0, y0, z0),
			         Vector3(x0, y0, z0).normalisedCopy(),
			         Vector2((Real) seg / (Real) mNumSegments, (Real) ring / (Real) mNumRings));

			if (ring != mNumRings )
			{
				if (seg != mNumSegments)
				{
					// each vertex (except the last) has six indices pointing to it
					if (ring != mNumRings-1)
						buffer.triangle(offset + mNumSegments + 2, offset, offset + mNumSegments + 1);
					if (ring != 0)
						buffer.triangle(offset + mNumSegments + 2, offset + 1, offset);
				}
				offset ++;
			}
		}; // end for seg
	} // end for ring
}
}
