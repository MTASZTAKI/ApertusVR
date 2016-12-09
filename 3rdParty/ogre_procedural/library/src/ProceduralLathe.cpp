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
#include "ProceduralLathe.h"
#include "ProceduralTriangulator.h"

using namespace Ogre;

namespace Procedural
{
//-----------------------------------------------------------------------
void Lathe::_latheBodyImpl(TriangleBuffer& buffer, const Shape* shapeToExtrude) const
{
	if (shapeToExtrude == NULL)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Shape must not be null!", "Procedural::Lathe::_latheBodyImpl(Procedural::TriangleBuffer&, const Procedural::Shape*)");
	int numSegShape = shapeToExtrude->getSegCount();
	if (numSegShape < 2)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Shape must contain at least two points", "Procedural::Lathe::_latheBodyImpl(Procedural::TriangleBuffer&, const Procedural::Shape*)");
	int offset =0;

	//int numSeg = mClosed?mNumSeg+1:mNumSeg;
	int numSeg = mNumSeg+1;
	buffer.rebaseOffset();
	buffer.estimateIndexCount(numSeg*numSegShape*6);
	buffer.estimateVertexCount((numSegShape+1)*(numSeg+1));

	Radian angleEnd(mAngleEnd);
	if (mAngleBegin>mAngleEnd)
		angleEnd+=(Radian)Math::TWO_PI;

	for (int i=0; i<numSeg; i++)
	{
		Radian angle;
		if (mClosed)
			angle = i/(Real)mNumSeg*Math::TWO_PI;
		else
			angle = mAngleBegin + i/(Real)mNumSeg*(angleEnd-mAngleBegin);
		Quaternion q;
		q.FromAngleAxis(angle,Vector3::UNIT_Y);

		for (int j=0; j<=numSegShape; j++)
		{
			const Vector2& v0 = shapeToExtrude->getPoint(j);
			Vector3 vp(v0.x,v0.y,0);
			const Vector2& vp2direction = shapeToExtrude->getAvgDirection(j);
			Vector2 vp2normal = vp2direction.perpendicular();
			Vector3 normal(vp2normal.x, vp2normal.y, 0);
			normal.normalise();
			if (shapeToExtrude->getOutSide() == SIDE_RIGHT)
				normal = -normal;

			addPoint(buffer, q*vp,
			         q*normal,
			         Vector2(i/(Real)mNumSeg, j/(Real)numSegShape));

			if (j <numSegShape && i <numSeg-1)
			{
				if (shapeToExtrude->getOutSide() == SIDE_RIGHT)
				{
					buffer.triangle(offset + numSegShape + 2, offset, offset + numSegShape + 1);
					buffer.triangle(offset + numSegShape + 2, offset + 1, offset);
				}
				else
				{
					buffer.triangle(offset + numSegShape + 2, offset + numSegShape + 1, offset);
					buffer.triangle(offset + numSegShape + 2, offset, offset + 1);
				}
			}
			offset ++;
		}
	}
}
//-----------------------------------------------------------------------
void Lathe::_latheCapImpl(TriangleBuffer& buffer) const
{
	std::vector<int> indexBuffer;
	PointList pointList;

	buffer.rebaseOffset();

	Triangulator t;
	Shape shapeCopy;
	MultiShape multishapeCopy;

	if (mShapeToExtrude)
	{
		shapeCopy = *mShapeToExtrude;
		shapeCopy.close();
		t.setShapeToTriangulate(&shapeCopy);
	}
	else
	{
		multishapeCopy = *mMultiShapeToExtrude;
		multishapeCopy.close();
		t.setMultiShapeToTriangulate(mMultiShapeToExtrude);
	}
	t.triangulate(indexBuffer, pointList);
	buffer.estimateIndexCount(2*indexBuffer.size());
	buffer.estimateVertexCount(2*pointList.size());

	//begin cap
	buffer.rebaseOffset();
	Quaternion q;
	q.FromAngleAxis(mAngleBegin, Vector3::UNIT_Y);
	for (size_t j =0; j<pointList.size(); j++)
	{
		Vector2 vp2 = pointList[j];
		Vector3 vp(vp2.x, vp2.y, 0);
		Vector3 normal = Vector3::UNIT_Z;

		addPoint(buffer, q*vp,
		         q*normal,
		         vp2);
	}

	for (size_t i=0; i<indexBuffer.size()/3; i++)
	{
		buffer.index(indexBuffer[i*3]);
		buffer.index(indexBuffer[i*3+1]);
		buffer.index(indexBuffer[i*3+2]);
	}
	//end cap
	buffer.rebaseOffset();
	q.FromAngleAxis(mAngleEnd, Vector3::UNIT_Y);
	for (size_t j =0; j<pointList.size(); j++)
	{
		Vector2 vp2 = pointList[j];
		Vector3 vp(vp2.x, vp2.y, 0);
		Vector3 normal = -Vector3::UNIT_Z;

		addPoint(buffer, q*vp,
		         q*normal,
		         vp2);
	}

	for (size_t i=0; i<indexBuffer.size()/3; i++)
	{
		buffer.index(indexBuffer[i*3]);
		buffer.index(indexBuffer[i*3+2]);
		buffer.index(indexBuffer[i*3+1]);
	}
}
//-----------------------------------------------------------------------
void Lathe::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	if (mShapeToExtrude == NULL && mMultiShapeToExtrude == NULL)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Either shape or multishape must be defined!", "Procedural::Lathe::addToTriangleBuffer(Procedural::TriangleBuffer)");

	// Triangulate the begin and end caps
	if (!mClosed && mCapped)
		_latheCapImpl(buffer);

	// Extrudes the body
	if (mShapeToExtrude)
		_latheBodyImpl(buffer, mShapeToExtrude);
	else
		for (unsigned int i=0; i<mMultiShapeToExtrude->getShapeCount(); i++)
			_latheBodyImpl(buffer, &mMultiShapeToExtrude->getShape(i));


}
}
