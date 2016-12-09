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
#include "ProceduralTorusKnotGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void TorusKnotGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mNumSegCircle*mP+1)*(mNumSegSection+1));
	buffer.estimateIndexCount((mNumSegCircle*mP)*(mNumSegSection+1)*6);

	int offset = 0;

	for (unsigned int i = 0; i <= mNumSegCircle * mP; i++)
	{
		Real phi = Math::TWO_PI * i/(Real)mNumSegCircle;
		Real x0 = mRadius*(2 + cos(mQ*phi/(Real)mP)) * cos(phi) / 3.f;
		Real y0 = mRadius*sin(mQ*phi/(Real)mP) / 3.f;
		Real z0 = mRadius*(2 + cos(mQ*phi/(Real)mP)) * sin(phi) / 3.f;

		Real phi1 = Math::TWO_PI * (i+1)/(Real)mNumSegCircle;
		Real x1 = mRadius*(2 + cos(mQ*phi1/(Real)mP)) * cos(phi1) / 3.f;
		Real y1 = mRadius*sin(mQ*phi1/mP) / 3.f;
		Real z1 = mRadius*(2 + cos(mQ*phi1/(Real)mP)) * sin(phi1) / 3.f;

		Vector3 v0(x0,y0,z0);
		Vector3 v1(x1,y1,z1);
		Vector3 direction((v1-v0).normalisedCopy());

		Quaternion q = Utils::_computeQuaternion(direction);

		for (unsigned int j =0; j<=mNumSegSection; j++)
		{
			Real alpha = Math::TWO_PI *j/mNumSegSection;
			Vector3 vp = mSectionRadius*(q * Vector3(cos(alpha), sin(alpha),0));

			addPoint(buffer, v0+vp,
			         vp.normalisedCopy(),
			         Vector2(i/(Real)mNumSegCircle, j/(Real)mNumSegSection));

			if (i != mNumSegCircle * mP)
			{
				buffer.index(offset + mNumSegSection + 1);
				buffer.index(offset + mNumSegSection);
				buffer.index(offset);
				buffer.index(offset + mNumSegSection + 1);
				buffer.index(offset);
				buffer.index(offset + 1);
			}
			offset ++;
		}
	}
}
}
