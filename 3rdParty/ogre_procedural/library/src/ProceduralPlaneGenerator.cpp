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
#include "ProceduralPlaneGenerator.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
void PlaneGenerator::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mNumSegX+1)*(mNumSegY+1));
	buffer.estimateIndexCount(mNumSegX*mNumSegY*6);
	int offset = 0;

	Vector3 vX = mNormal.perpendicular();
	Vector3 vY = mNormal.crossProduct(vX);
	Vector3 delta1 = mSizeX / (Real)mNumSegX * vX;
	Vector3 delta2 = mSizeY / (Real)mNumSegY * vY;
	// build one corner of the square
	Vector3 orig = -0.5f*mSizeX*vX - 0.5f*mSizeY*vY;

	for (unsigned short i1 = 0; i1<=mNumSegX; i1++)
		for (unsigned short i2 = 0; i2<=mNumSegY; i2++)
		{
			addPoint(buffer, orig+i1*delta1+i2*delta2,
			         mNormal,
			         Vector2(i1/(Real)mNumSegX, i2/(Real)mNumSegY));
		}

	bool reverse = false;
	if (delta1.crossProduct(delta2).dotProduct(mNormal)>0)
		reverse= true;
	for (unsigned short n1 = 0; n1<mNumSegX; n1++)
	{
		for (unsigned short n2 = 0; n2<mNumSegY; n2++)
		{
			if (reverse)
			{
				buffer.index(offset+0);
				buffer.index(offset+(mNumSegY+1));
				buffer.index(offset+1);
				buffer.index(offset+1);
				buffer.index(offset+(mNumSegY+1));
				buffer.index(offset+(mNumSegY+1)+1);
			}
			else
			{
				buffer.index(offset+0);
				buffer.index(offset+1);
				buffer.index(offset+(mNumSegY+1));
				buffer.index(offset+1);
				buffer.index(offset+(mNumSegY+1)+1);
				buffer.index(offset+(mNumSegY+1));
			}
			offset++;
		}
		offset++;
	}
}
}
