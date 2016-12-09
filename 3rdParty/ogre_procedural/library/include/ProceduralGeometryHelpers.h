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
#ifndef PROCEDURAL_GEOMETRYHELPERS_INCLUDED
#define PROCEDURAL_GEOMETRYHELPERS_INCLUDED

#include "OgrePlane.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "ProceduralPlatform.h"

namespace Procedural
{
struct Line;
//-----------------------------------------------------------------------
/// Represents a 2D circle
class _ProceduralExport Circle
{
private:
	Ogre::Vector2 mCenter;
	Ogre::Real mRadius;

public:

	Circle() : mCenter(Ogre::Vector2::ZERO), mRadius(1) {}

	/// Contructor with arguments
	Circle(Ogre::Vector2 center, Ogre::Real radius) : mCenter(center), mRadius(radius)
	{}

	/// Contructor with arguments
	Circle(Ogre::Vector2 p1, Ogre::Vector2 p2, Ogre::Vector2 p3);

	/// Tells whether that point is inside the circle or not
	bool isPointInside(const Ogre::Vector2& p) const
	{
		return (p-mCenter).length() < mRadius;
	}
};
//-----------------------------------------------------------------------
/// Extends the Ogre::Plane class to be able to compute the intersection between 2 planes
class _ProceduralExport Plane : public Ogre::Plane
{
public:
	Plane() : Ogre::Plane() {}

	/// Contructor with arguments
	Plane(const Ogre::Vector3& normal, const Ogre::Vector3& pos) : Ogre::Plane(normal, pos) {}

	Plane(Ogre::Real a, Ogre::Real b, Ogre::Real c, Ogre::Real d) : Ogre::Plane(a,b,c,d) {}

	/**
	 * Checks whether 2 planes intersect and compute intersecting line if it is the case.
	 * @param other the other plane with which to check for intersection
	 * @param outputLine the intersecting line, if planes actually intersect
	 * @returns true if planes intersect, false otherwise
	 */
	bool intersect(const Plane& other, Line& outputLine) const;
};
//-----------------------------------------------------------------------
/// Represents a line in 3D
struct _ProceduralExport Line
{
	Ogre::Vector3 mPoint;
	Ogre::Vector3 mDirection;

	Line() {}

	/// Contructor with arguments
	/// @param point a point on the line
	/// @param direction a normalized vector representing the direction of that line
	Line(Ogre::Vector3 point, Ogre::Vector3 direction) : mPoint(point), mDirection(direction.normalisedCopy()) {}

	/// Builds the line between 2 points
	void setFrom2Points(const Ogre::Vector3& a, const Ogre::Vector3& b)
	{
		mPoint = a;
		mDirection = (b-a).normalisedCopy();
	}

	/// Finds the shortest vector between that line and a point
	Ogre::Vector3 shortestPathToPoint(const Ogre::Vector3& point) const;
};
//-----------------------------------------------------------------------
/// Represents a line in 2D
class _ProceduralExport Line2D
{
	Ogre::Vector2 mPoint;
	Ogre::Vector2 mDirection;

public:
	Line2D() {}

	/// Contructor with arguments
	/// @param point a point on the line
	/// @param direction a normalized vector representing the direction of that line
	Line2D(Ogre::Vector2 point, Ogre::Vector2 direction) : mPoint(point), mDirection(direction.normalisedCopy()) {}

	/// Builds the line between 2 points
	void setFrom2Points(const Ogre::Vector2& a, const Ogre::Vector2& b)
	{
		mPoint = a;
		mDirection = (b-a).normalisedCopy();
	}

	/**
	 * Computes the interesction between current segment and another segment
	 * @param other the other segment
	 * @param intersection the point of intersection if outputed there if it exists
	 * @return true if segments intersect, false otherwise
	 */
	bool findIntersect(const Line2D& other, Ogre::Vector2& intersection) const;
};
//-----------------------------------------------------------------------
/// Represents a 2D segment
struct _ProceduralExport Segment2D
{
	Ogre::Vector2 mA;
	Ogre::Vector2 mB;

	Segment2D() {}

	/// Contructor with arguments
	Segment2D(Ogre::Vector2 a, Ogre::Vector2 b) : mA(a), mB(b) {}

	/**
	 * Computes the interesction between current segment and another segment
	 * @param other the other segment
	 * @param intersection the point of intersection if outputed there if it exists
	 * @return true if segments intersect, false otherwise
	 */
	bool findIntersect(const Segment2D& other, Ogre::Vector2& intersection) const;

	/// Tells whether this segments intersects the other segment
	bool intersects(const Segment2D& other) const;
};
//-----------------------------------------------------------------------
// Compares 2 Vector2, with some tolerance
struct _ProceduralExport Vector2Comparator
{
	bool operator()(const Ogre::Vector2& one, const Ogre::Vector2& two) const
	{
		if ((one - two).squaredLength() < 1e-6)
			return false;
		if (Ogre::Math::Abs(one.x - two.x) > 1e-3)
			return one.x < two.x;
		return one.y < two.y;
	}
};
//-----------------------------------------------------------------------
// Compares 2 Vector3, with some tolerance
struct _ProceduralExport Vector3Comparator
{
	bool operator()(const Ogre::Vector3& one, const Ogre::Vector3& two) const
	{
		if ((one-two).squaredLength()<1e-6)
			return false;
		if (Ogre::Math::Abs(one.x - two.x)>1e-3)
			return one.x<two.x;
		if (Ogre::Math::Abs(one.y - two.y)>1e-3)
			return one.y<two.y;
		return one.z<two.z;
	}
};
//-----------------------------------------------------------------------
/// Represents a 3D segment
struct _ProceduralExport Segment3D
{
	Ogre::Vector3 mA;
	Ogre::Vector3 mB;
	Segment3D() {}

	/// Contructor with arguments
	Segment3D(Ogre::Vector3 a, Ogre::Vector3 b) : mA(a), mB(b) {}

	bool epsilonEquivalent(const Segment3D& other) const
	{
		return ((mA.squaredDistance(other.mA) < 1e-8 && mB.squaredDistance(other.mB) < 1e-8) ||
		        (mA.squaredDistance(other.mB) < 1e-8 && mB.squaredDistance(other.mA) < 1e-8));
	}

	Segment3D orderedCopy() const
	{
		if (Vector3Comparator()(mB, mA))
			return Segment3D(mB,mA);
		return *this;
	}

};
//-----------------------------------------------------------------------
/// Represents a 2D triangle
struct _ProceduralExport Triangle2D
{
	Ogre::Vector2 mPoints[3];

	Triangle2D(const Ogre::Vector2& a,const Ogre::Vector2& b,const Ogre::Vector2& c)
	{
		mPoints[0]=a;
		mPoints[1]=b;
		mPoints[2]=c;
	}
};
//-----------------------------------------------------------------------
/// Represents a 3D triangle
struct _ProceduralExport Triangle3D
{
	Ogre::Vector3 mPoints[3];

	Triangle3D(const Ogre::Vector3& a,const Ogre::Vector3& b,const Ogre::Vector3& c)
	{
		mPoints[0]=a;
		mPoints[1]=b;
		mPoints[2]=c;
	}

	bool findIntersect(const Triangle3D& other, Segment3D& intersection) const;
};
//-----------------------------------------------------------------------
struct _ProceduralExport IntVector2
{
	size_t x, y;
};
}
#endif
