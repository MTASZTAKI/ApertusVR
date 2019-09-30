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
#include "ProceduralGeometryHelpers.h"
#include "ProceduralUtils.h"

using namespace Ogre;

namespace Procedural
{
//-----------------------------------------------------------------------
Circle::Circle(Vector2 p1, Vector2 p2, Vector2 p3)
{
	Vector2 c1 = .5*(p1+p2);
	Vector2 d1 = (p2-p1).perpendicular();
	float a1 = d1.y;
	float b1 = -d1.x;
	float g1 = d1.x*c1.y-d1.y*c1.x;

	Vector2 c3 = .5*(p2+p3);
	Vector2 d3 = (p3-p2).perpendicular();
	float a2 = d3.y;
	float b2 = -d3.x;
	float g2 = d3.x*c3.y-d3.y*c3.x;

	Vector2 intersect;
	float intersectx = (b2*g1-b1*g2)/(b1*a2-b2*a1);
	float intersecty = (a2*g1-a1*g2)/(a1*b2-a2*b1);

	intersect = Vector2(intersectx, intersecty);

	mCenter = intersect;
	mRadius = (intersect-p1).length();
}
//-----------------------------------------------------------------------
bool Segment2D::findIntersect(const Segment2D& other, Vector2& intersection) const
{
	const Vector2& p1 = mA;
	const Vector2& p2 = mB;
	const Vector2& p3 = other.mA;
	const Vector2& p4 = other.mB;


	Vector2 d1 = p2-p1;
	float a1 = d1.y;
	float b1 = -d1.x;
	float g1 = d1.x*p1.y-d1.y*p1.x;

	Vector2 d3 = p4-p3;
	float a2 = d3.y;
	float b2 = -d3.x;
	float g2 = d3.x*p3.y-d3.y*p3.x;

	// if both segments are parallel, early out
	if (d1.crossProduct(d3) == 0.)
		return false;

	Vector2 intersect;
	float intersectx = (b2*g1-b1*g2)/(b1*a2-b2*a1);
	float intersecty = (a2*g1-a1*g2)/(a1*b2-a2*b1);

	intersect = Vector2(intersectx, intersecty);

	if ((intersect-p1).dotProduct(intersect-p2)<0 && (intersect-p3).dotProduct(intersect-p4)<0)
	{
		intersection = intersect;
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------
bool Segment2D::intersects(const Segment2D& other) const
{
	// Early out if segments have nothing in common
	Vector2 min1 = Utils::min(mA, mB);
	Vector2 max1 = Utils::max(mA, mB);
	Vector2 min2 = Utils::min(other.mA, other.mB);
	Vector2 max2 = Utils::max(other.mA, other.mB);
	if (max1.x<min2.x || max1.y<min2.y || max2.x<min1.x || max2.y<min2.y)
		return false;
	Vector2 t;
	return findIntersect(other, t);
}
//-----------------------------------------------------------------------
bool Plane::intersect(const Plane& other, Line& outputLine) const
{
	//TODO : handle the case where the plane is perpendicular to T
	Vector3 point1(Vector3::ZERO);
	Vector3 direction = normal.crossProduct(other.normal);
	if (direction.squaredLength() < 1e-08)
		return false;

	Real cp = normal.x*other.normal.y-other.normal.x*normal.y;
	if (cp!=0)
	{
		Real denom = 1.f/cp;
		point1.x = (normal.y*other.d-other.normal.y*d)*denom;
		point1.y = (other.normal.x*d-normal.x*other.d)*denom;
		point1.z = 0;
	}
	else if ((cp= normal.y*other.normal.z-other.normal.y*normal.z)!=0)
	{
		//special case #1
		Real denom = 1.f/cp;
		point1.x = 0;
		point1.y = (normal.z*other.d-other.normal.z*d)*denom;
		point1.z = (other.normal.y*d-normal.y*other.d)*denom;
	}
	else if ((cp= normal.x*other.normal.z-other.normal.x*normal.z)!=0)
	{
		//special case #2
		Real denom = 1.f/cp;
		point1.x = (normal.z*other.d-other.normal.z*d)*denom;
		point1.y = 0;
		point1.z = (other.normal.x*d-normal.x*other.d)*denom;
	}

	outputLine = Line(point1, direction);

	return true;
}
//-----------------------------------------------------------------------
Vector3 Line::shortestPathToPoint(const Vector3& point) const
{
	Vector3 projection = (point-mPoint).dotProduct(mDirection) * mDirection;
	Vector3 vec = -projection+point-mPoint;
	return vec;
}
//-----------------------------------------------------------------------
bool Line2D::findIntersect(const Line2D& other, Vector2& intersection) const
{
	const Vector2& p1 = mPoint;
	//const Vector2& p2 = mPoint+mDirection;
	const Vector2& p3 = other.mPoint;
	//const Vector2& p4 = other.mPoint+other.mDirection;

	Vector2 d1 = mDirection;
	float a1 = d1.y;
	float b1 = -d1.x;
	float g1 = d1.x*p1.y-d1.y*p1.x;

	Vector2 d3 = other.mDirection;
	float a2 = d3.y;
	float b2 = -d3.x;
	float g2 = d3.x*p3.y-d3.y*p3.x;

	// if both segments are parallel, early out
	if (d1.crossProduct(d3) == 0.)
		return false;
	float intersectx = (b2*g1-b1*g2)/(b1*a2-b2*a1);
	float intersecty = (a2*g1-a1*g2)/(a1*b2-a2*b1);

	intersection = Vector2(intersectx, intersecty);
	return true;
}
//-----------------------------------------------------------------------

void isect(Real VV0,Real VV1,Real VV2,Real D0, Real D1,Real D2,Real& isect0,Real& isect1)
{
	isect0=VV0+(VV1-VV0)*D0/(D0-D1);
	isect1=VV0+(VV2-VV0)*D0/(D0-D2);
}


void computeIntervals(Real VV0,Real VV1,Real VV2,Real D0,Real D1,Real D2,Real D0D1,Real D0D2,Real& isect0,Real& isect1)
{
	if (D0D1>0.0f)
	{
		/* here we know that D0D2<=0.0 */
		/* that is D0, D1 are on the same side, D2 on the other or on the plane */
		isect(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);
	}
	else if (D0D2>0.0f)
	{
		/* here we know that d0d1<=0.0 */
		isect(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);
	}
	else if (D1*D2>0.0f || D0!=0.0f)
	{
		/* here we know that d0d1<=0.0 or that D0!=0.0 */
		isect(VV0,VV1,VV2,D0,D1,D2,isect0,isect1);
	}
	else if (D1!=0.0f)
	{
		isect(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);
	}
	else if (D2!=0.0f)
	{
		isect(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);
	}
	else
	{
		/* triangles are coplanar */
		//return coplanar_tri_tri(N1,V0,V1,V2,U0,U1,U2);
	}
}

bool Triangle3D::findIntersect(const Triangle3D& other, Segment3D& intersection) const
{
	// Compute plane equation of first triangle
	Vector3 e1 = mPoints[1]-mPoints[0];
	Vector3 e2 = mPoints[2]-mPoints[0];
	Vector3 n1 = e1.crossProduct(e2);
	Real d1 = - n1.dotProduct(mPoints[0]);

	// Put second triangle in first plane equation to compute distances to the plane
	Real du[3];
	for (short i=0; i<3; i++)
	{
		du[i] = n1.dotProduct(other.mPoints[i]) + d1;
		if (Math::Abs(du[i])<1e-6)
			du[i]=0.0;
	}

	Real du0du1=du[0]*du[1];
	Real du0du2=du[0]*du[2];

	if (du0du1>0.0f && du0du2>0.0f) /* same sign on all of them + not equal 0 ? */
		return false;                    /* no intersection occurs */

	// Compute plane equation of first triangle
	e1 = other.mPoints[1]-other.mPoints[0];
	e2 = other.mPoints[2]-other.mPoints[0];
	Vector3 n2 = e1.crossProduct(e2);
	Real d2 = - n2.dotProduct(other.mPoints[0]);

	// Put first triangle in second plane equation to compute distances to the plane
	Real dv[3];
	for (short i=0; i<3; i++)
	{
		dv[i] = n2.dotProduct(mPoints[i]) + d2;
		if (Math::Abs(dv[i])<1e-6)
			dv[i]=0.0;
	}

	Real dv0dv1=dv[0]*dv[1];
	Real dv0dv2=dv[0]*dv[2];

	if (dv0dv1>0.0f && dv0dv2>0.0f) /* same sign on all of them + not equal 0 ? */
		return false;                    /* no intersection occurs */

	//Compute the direction of intersection line
	Vector3 d = n1.crossProduct(n2);

	// We don't do coplanar triangles
	if (d.squaredLength()<1e-6)
		return false;

	// Project triangle points onto the intersection line

	/* compute and index to the largest component of D */
	Real max=Math::Abs(d[0]);
	int index=0;
	Real b=Math::Abs(d[1]);
	Real c=Math::Abs(d[2]);
	if (b>max) max=b,index=1;
	if (c>max) max=c,index=2;

	/* this is the simplified projection onto L*/
	Real vp0=mPoints[0][index];
	Real vp1=mPoints[1][index];
	Real vp2=mPoints[2][index];

	Real up0=other.mPoints[0][index];
	Real up1=other.mPoints[1][index];
	Real up2=other.mPoints[2][index];

	Real isect1[2];
	Real isect2[2];
	/* compute interval for triangle 1 */
	computeIntervals(vp0,vp1,vp2,dv[0],dv[1],dv[2],dv0dv1,dv0dv2,isect1[0],isect1[1]);

	/* compute interval for triangle 2 */
	computeIntervals(up0,up1,up2,du[0],du[1],du[2],du0du1,du0du2,isect2[0],isect2[1]);

	if (isect1[0]>isect1[1])
		std::swap(isect1[0],isect1[1]);
	if (isect2[0]>isect2[1])
		std::swap(isect2[0],isect2[1]);

	if (isect1[1]<isect2[0] || isect2[1]<isect1[0]) return false;

	// Deproject segment onto line
	Real r1 = std::max(isect1[0], isect2[0]);
	Real r2 = std::min(isect1[1], isect2[1]);

	Plane pl1(n1.x, n1.y, n1.z, d1);
	Plane pl2(n2.x, n2.y, n2.z, d2);
	Line interLine;
	pl1.intersect(pl2, interLine);
	Vector3 p=interLine.mPoint;
	d.normalise();
	Vector3 v1 = p+(r1-p[index]) /d[index] *d;
	Vector3 v2 = p+(r2-p[index]) /d[index] *d;
	intersection.mA = v1;
	intersection.mB = v2;


	return true;
}

}
