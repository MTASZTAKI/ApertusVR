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
#include "ProceduralConeGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void ConeGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mNumSegHeight+1)*(mNumSegBase+1)+mNumSegBase+2);
	buffer.estimateIndexCount(mNumSegHeight*mNumSegBase*6+3*mNumSegBase);

	Real deltaAngle = (Math::TWO_PI / mNumSegBase);
	Real deltaHeight = mHeight/(Real)mNumSegHeight;
	int offset = 0;

	Vector3 refNormal = Vector3(mRadius, mHeight, 0.f).normalisedCopy();
	Quaternion q;

	for (unsigned int i = 0; i <=mNumSegHeight; i++)
	{
		Real r0 = mRadius * (1 - i / (Real)mNumSegHeight);
		for (unsigned int j = 0; j<=mNumSegBase; j++)
		{
			Real x0 = r0* cosf(j*deltaAngle);
			Real z0 = r0 * sinf(j*deltaAngle);

			q.FromAngleAxis(Radian(-deltaAngle*j), Vector3::UNIT_Y);

			addPoint(buffer, Vector3(x0, i*deltaHeight, z0),
			         q*refNormal,
			         Vector2(j/(Real)mNumSegBase, i/(Real)mNumSegHeight));

			if (i != mNumSegHeight&& j != mNumSegBase)
			{
				buffer.index(offset + mNumSegBase + 2);
				buffer.index(offset);
				buffer.index(offset + mNumSegBase+1);
				buffer.index(offset + mNumSegBase + +2);
				buffer.index(offset + 1);
				buffer.index(offset);
			}

			offset ++;
		}
	}

	//low cap
	int centerIndex = offset;
	addPoint(buffer, Vector3::ZERO,
	         Vector3::NEGATIVE_UNIT_Y,
	         Vector2::UNIT_Y);
	offset++;
	for (unsigned int j=0; j<=mNumSegBase; j++)
	{
		Real x0 = mRadius * cosf(j*deltaAngle);
		Real z0 = mRadius * sinf(j*deltaAngle);

		addPoint(buffer, Vector3(x0, 0.0f, z0),
		         Vector3::NEGATIVE_UNIT_Y,
		         Vector2(j/(Real)mNumSegBase,0.0));

		if (j!=mNumSegBase)
		{
			buffer.index(centerIndex);
			buffer.index(offset);
			buffer.index(offset+1);
		}
		offset++;
	}
}
}
