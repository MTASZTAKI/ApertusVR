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
#include "ProceduralPrismGenerator.h"
#include "ProceduralPlaneGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void PrismGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	if (mCapped)
	{
		buffer.estimateVertexCount((mNumSegHeight + 1) * (mNumSides + 1) + 2 * (mNumSides + 1) + 2);
		buffer.estimateIndexCount(mNumSegHeight * (mNumSides + 1) * 6 + 6 * mNumSides);
	}
	else
	{
		buffer.estimateVertexCount((mNumSegHeight + 1) * (mNumSides + 1));
		buffer.estimateIndexCount(mNumSegHeight * (mNumSides + 1) * 6);
	}

	Real deltaAngle = (Math::TWO_PI / mNumSides);
	Real deltaHeight = mHeight / (Real)mNumSegHeight;
	int offset = 0;

	for (unsigned int i = 0; i <= mNumSegHeight; i++)
		for (unsigned int j = 0; j <= mNumSides; j++)
		{
			Real x0 = mRadius * Math::Cos((Real)j * deltaAngle);
			Real z0 = mRadius * Math::Sin((Real)j * deltaAngle);

			addPoint(buffer, Vector3(x0, (Real)i * deltaHeight, z0),
			         Vector3(x0, 0.0f, z0).normalisedCopy(),
			         Vector2((Real)j / (Real)mNumSides, (Real)i / (Real)mNumSegHeight));

			if (i != mNumSegHeight)
			{
				buffer.index(offset + mNumSides + 1);
				buffer.index(offset);
				buffer.index(offset + mNumSides);
				buffer.index(offset + mNumSides + 1);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset ++;
		}
	if (mCapped)
	{
		//low cap
		int centerIndex = offset;
		addPoint(buffer, Vector3::ZERO,
		         Vector3::NEGATIVE_UNIT_Y,
		         Vector2::ZERO);
		offset++;
		for (unsigned int j = 0; j <= mNumSides; j++)
		{
			Real x0 = Math::Cos((Real)j * deltaAngle);
			Real z0 = Math::Sin((Real)j * deltaAngle);

			addPoint(buffer, Vector3(mRadius * x0, 0.0f, mRadius * z0),
			         Vector3::NEGATIVE_UNIT_Y,
			         Vector2(x0, z0));
			if (j != mNumSides)
			{
				buffer.index(centerIndex);
				buffer.index(offset);
				buffer.index(offset + 1);
			}
			offset++;
		}
		// high cap
		centerIndex = offset;
		addPoint(buffer, Vector3(0.0f, mHeight, 0.0f),
		         Vector3::UNIT_Y,
		         Vector2::ZERO);
		offset++;
		for (unsigned int j = 0; j <= mNumSides; j++)
		{
			Real x0 = Math::Cos((Real)j * deltaAngle);
			Real z0 = Math::Sin((Real)j * deltaAngle);

			addPoint(buffer, Vector3(x0 * mRadius, mHeight, mRadius * z0),
			         Vector3::UNIT_Y,
			         Vector2(x0, z0));
			if (j != mNumSides)
			{
				buffer.index(centerIndex);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset++;
		}
	}
}
}
