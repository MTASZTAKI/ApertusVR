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
#include "ProceduralTorusGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void TorusGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mNumSegCircle+1)*(mNumSegSection+1));
	buffer.estimateIndexCount((mNumSegCircle)*(mNumSegSection+1)*6);

	Real deltaSection = (Math::TWO_PI / mNumSegSection);
	Real deltaCircle = (Math::TWO_PI / mNumSegCircle);
	int offset = 0;

	for (unsigned int i = 0; i <=mNumSegCircle; i++)
		for (unsigned int j = 0; j<=mNumSegSection; j++)
		{
			Vector3 c0(mRadius, 0.0, 0.0);
			Vector3 v0(mRadius+mSectionRadius * cosf(j*deltaSection),mSectionRadius * sinf(j*deltaSection),0.0);
			Quaternion q;
			q.FromAngleAxis(Radian(i*deltaCircle),Vector3::UNIT_Y);
			Vector3 v = q * v0;
			Vector3 c = q * c0;
			addPoint(buffer, v,
			         (v-c).normalisedCopy(),
			         Vector2(i/(Real)mNumSegCircle, j/(Real)mNumSegSection));

			if (i != mNumSegCircle)
			{
				buffer.index(offset + mNumSegSection + 1);
				buffer.index(offset);
				buffer.index(offset + mNumSegSection);
				buffer.index(offset + mNumSegSection + 1);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset ++;
		}
}
}
