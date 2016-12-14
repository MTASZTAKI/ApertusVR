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
#include "ProceduralBoolean.h"
#include "ProceduralGeometryHelpers.h"
#include "ProceduralTriangulator.h"
#include "ProceduralPath.h"

using namespace Ogre;

namespace Procedural
{

struct Intersect
{
	Segment3D mSeg;
	int mTri1, mTri2;

	Intersect(Segment3D seg, int tri1, int tri2) : mSeg(seg), mTri1(tri1), mTri2(tri2)
	{
	}
};
//-----------------------------------------------------------------------

inline Vector2 projectOnAxis(const Vector3& input, const Vector3& origin, const Vector3& axis1, const Vector3& axis2)
{
	return Vector2((input - origin).dotProduct(axis1), (input - origin).dotProduct(axis2));
}
//-----------------------------------------------------------------------

inline Vector3 deprojectOnAxis(const Vector2& input, const Vector3& origin, const Vector3& axis1, const Vector3& axis2)
{
	return origin + input.x * axis1 + input.y* axis2;
}
//-----------------------------------------------------------------------

inline Segment2D projectOnAxis(const Segment3D& input, const Vector3& origin, const Vector3& axis1, const Vector3& axis2)
{
	return Segment2D(projectOnAxis(input.mA, origin, axis1, axis2), projectOnAxis(input.mB, origin, axis1, axis2));
}
//-----------------------------------------------------------------------

struct Seg3Comparator
{

	bool operator()(const Segment3D& one, const Segment3D& two) const
	{
		if (one.epsilonEquivalent(two)) return false;

		if (one.mA.squaredDistance(two.mA) > 1e-6)
			return Vector3Comparator()(one.mA, two.mA);
		return Vector3Comparator()(one.mB, two.mB);
	}
};
//-----------------------------------------------------------------------
typedef std::multimap<Segment3D, int, Seg3Comparator> TriLookup;

void _removeFromTriLookup(int k, TriLookup& lookup)
{
	for (std::multimap<Segment3D, int, Seg3Comparator>::iterator it2 = lookup.begin(); it2 != lookup.end();)
	{
		std::multimap<Segment3D, int, Seg3Comparator>::iterator removeIt = it2++;
		if (removeIt->second == k)
			lookup.erase(removeIt);
	}
}
//-----------------------------------------------------------------------

void _recursiveAddNeighbour(TriangleBuffer& result, const TriangleBuffer& source, int triNumber, TriLookup& lookup, const std::set<Segment3D, Seg3Comparator>& limits, bool inverted)
{
	if (triNumber == -1)
		return;
	Utils::log("tri " + StringConverter::toString(triNumber));
	const std::vector<int>& ind = source.getIndices();
	const std::vector<TriangleBuffer::Vertex>& vec = source.getVertices();
	result.rebaseOffset();
	if (inverted)
	{
		result.triangle(0, 2, 1);
		TriangleBuffer::Vertex v = vec[ind[triNumber * 3]];
		v.mNormal = -v.mNormal;
		result.vertex(v);
		v = vec[ind[triNumber * 3+1]];
		v.mNormal = -v.mNormal;
		result.vertex(v);
		v = vec[ind[triNumber * 3+2]];
		v.mNormal = -v.mNormal;
		result.vertex(v);
	}
	else
	{
		result.triangle(0, 1, 2);
		result.vertex(vec[ind[triNumber * 3]]);
		result.vertex(vec[ind[triNumber * 3 + 1]]);
		result.vertex(vec[ind[triNumber * 3 + 2]]);
	}

	//Utils::log("vertex " + StringConverter::toString(vec[ind[triNumber*3]].mPosition));
	//Utils::log("vertex " + StringConverter::toString(vec[ind[triNumber*3+1]].mPosition));
	//Utils::log("vertex " + StringConverter::toString(vec[ind[triNumber*3+2]].mPosition));

	std::multimap<Segment3D, int, Seg3Comparator>::iterator it;

	int nextTriangle1 = -1;
	int nextTriangle2 = -1;
	int nextTriangle3 = -1;
	it = lookup.find(Segment3D(vec[ind[triNumber * 3]].mPosition, vec[ind[triNumber * 3 + 1]].mPosition).orderedCopy());
	//if (it != lookup.end() && limits.find(it->first.orderedCopy()) != limits.end())
	//	Utils::log("Cross limit1");

	if (it != lookup.end() && limits.find(it->first.orderedCopy()) == limits.end())
	{
		nextTriangle1 = it->second;
		_removeFromTriLookup(nextTriangle1, lookup);
	}
	it = lookup.find(Segment3D(vec[ind[triNumber * 3 + 1]].mPosition, vec[ind[triNumber * 3 + 2]].mPosition).orderedCopy());
	//if (it != lookup.end() && limits.find(it->first.orderedCopy()) != limits.end())
	//Utils::log("Cross limit2");

	if (it != lookup.end() && limits.find(it->first.orderedCopy()) == limits.end())
	{
		nextTriangle2 = it->second;
		_removeFromTriLookup(nextTriangle2, lookup);
	}
	it = lookup.find(Segment3D(vec[ind[triNumber * 3]].mPosition, vec[ind[triNumber * 3 + 2]].mPosition).orderedCopy());
	//if (it != lookup.end() && limits.find(it->first.orderedCopy()) != limits.end())
	//	Utils::log("Cross limit3");
	if (it != lookup.end() && limits.find(it->first.orderedCopy()) == limits.end())
	{
		nextTriangle3 = it->second;
		_removeFromTriLookup(nextTriangle3, lookup);
	}
	//Utils::log("add " + StringConverter::toString(nextTriangle1) + " ," + StringConverter::toString(nextTriangle2) + " ,"+StringConverter::toString(nextTriangle3) );

	_recursiveAddNeighbour(result, source, nextTriangle1, lookup, limits, inverted);
	_recursiveAddNeighbour(result, source, nextTriangle2, lookup, limits, inverted);
	_recursiveAddNeighbour(result, source, nextTriangle3, lookup, limits, inverted);
}
//-----------------------------------------------------------------------

void _retriangulate(TriangleBuffer& newMesh, const TriangleBuffer& inputMesh, const std::vector<Intersect>& intersectionList, bool first)
{
	const std::vector<TriangleBuffer::Vertex>& vec = inputMesh.getVertices();
	const std::vector<int>& ind = inputMesh.getIndices();
	// Triangulate
	//  Group intersections by triangle indice
	std::map<int, std::vector<Segment3D> > meshIntersects;
	for (std::vector<Intersect>::const_iterator it = intersectionList.begin(); it != intersectionList.end(); ++it)
	{
		std::map<int, std::vector<Segment3D> >::iterator it2;
		if (first)
			it2 = meshIntersects.find(it->mTri1);
		else
			it2 = meshIntersects.find(it->mTri2);
		if (it2 != meshIntersects.end())
			it2->second.push_back(it->mSeg);
		else
		{
			std::vector<Segment3D> vec;
			vec.push_back(it->mSeg);
			if (first)
				meshIntersects[it->mTri1] = vec;
			else
				meshIntersects[it->mTri2] = vec;
		}
	}
	// Build a new TriangleBuffer holding non-intersected triangles and retriangulated-intersected triangles
	for (std::vector<TriangleBuffer::Vertex>::const_iterator it = vec.begin(); it != vec.end(); ++it)
		newMesh.vertex(*it);
	for (int i = 0; i < (int)ind.size() / 3; i++)
		if (meshIntersects.find(i) == meshIntersects.end())
			newMesh.triangle(ind[i * 3], ind[i * 3 + 1], ind[i * 3 + 2]);
	int numNonIntersected1 = newMesh.getIndices().size();
	for (std::map<int, std::vector<Segment3D> >::iterator it = meshIntersects.begin(); it != meshIntersects.end(); ++it)
	{
		std::vector<Segment3D>& segments = it->second;
		int triIndex = it->first;
		Vector3 v1 = vec[ind[triIndex * 3]].mPosition;
		Vector3 v2 = vec[ind[triIndex * 3+1]].mPosition;
		Vector3 v3 = vec[ind[triIndex * 3+2]].mPosition;
		Vector3 triNormal = ((v2-v1).crossProduct(v3-v1)).normalisedCopy();
		Vector3 xAxis = triNormal.perpendicular();
		Vector3 yAxis = triNormal.crossProduct(xAxis);
		Vector3 planeOrigin = vec[ind[triIndex * 3]].mPosition;

		// Project intersection segments onto triangle plane
		std::vector<Segment2D> segments2;

		for (std::vector<Segment3D>::iterator it2 = segments.begin(); it2 != segments.end(); it2++)
			segments2.push_back(projectOnAxis(*it2, planeOrigin, xAxis, yAxis));
		for (std::vector<Segment2D>::iterator it2 = segments2.begin(); it2 != segments2.end();)
			if ((it2->mA - it2->mB).squaredLength() < 1e-5)
				it2 = segments2.erase(it2);
			else
				it2++;

		// Triangulate
		Triangulator t;
		Triangle2D tri(projectOnAxis(vec[ind[triIndex * 3]].mPosition, planeOrigin, xAxis, yAxis),
		               projectOnAxis(vec[ind[triIndex * 3 + 1]].mPosition, planeOrigin, xAxis, yAxis),
		               projectOnAxis(vec[ind[triIndex * 3 + 2]].mPosition, planeOrigin, xAxis, yAxis));
		PointList outPointList;
		std::vector<int> outIndice;
		t.setManualSuperTriangle(&tri).setRemoveOutside(false).setSegmentListToTriangulate(&segments2).triangulate(outIndice, outPointList);

		// Deproject and add to triangleBuffer
		newMesh.rebaseOffset();
		for (std::vector<int>::iterator it = outIndice.begin(); it != outIndice.end(); ++it)
			newMesh.index(*it);
		Real x1 = tri.mPoints[0].x;
		Real y1 = tri.mPoints[0].y;
		Vector2 uv1 = vec[ind[triIndex * 3]].mUV;
		Real x2 = tri.mPoints[1].x;
		Real y2 = tri.mPoints[1].y;
		Vector2 uv2 = vec[ind[triIndex * 3 + 1]].mUV;
		Real x3 = tri.mPoints[2].x;
		Real y3 = tri.mPoints[2].y;
		Vector2 uv3 = vec[ind[triIndex * 3 + 2]].mUV;
		Real DET = x1 * y2 - x2 * y1 + x2 * y3 - x3 * y2 + x3 * y1 - x1*y3;
		Vector2 A = ((y2 - y3) * uv1 + (y3 - y1) * uv2 + (y1 - y2) * uv3) / DET;
		Vector2 B = ((x3 - x2) * uv1 + (x1 - x3) * uv2 + (x2 - x1) * uv3) / DET;
		Vector2 C = ((x2 * y3 - x3 * y2) * uv1 + (x3 * y1 - x1 * y3) * uv2 + (x1 * y2 - x2 * y1) * uv3) / DET;

		for (std::vector<Vector2>::iterator it = outPointList.begin(); it != outPointList.end(); ++it)
		{
			Vector2 uv = A * it->x + B * it->y + C;
			newMesh.position(deprojectOnAxis(*it, planeOrigin, xAxis, yAxis));
			newMesh.normal(triNormal);
			newMesh.textureCoord(uv);
		}
	}
}
//-----------------------------------------------------------------------
typedef std::multimap<Segment3D, int, Seg3Comparator> TriLookup;

void _buildTriLookup(TriLookup& lookup, const TriangleBuffer& newMesh)
{
	const std::vector<TriangleBuffer::Vertex>& nvec = newMesh.getVertices();
	const std::vector<int>& nind = newMesh.getIndices();
	for (int i = 0; i < (int)nind.size() / 3; i++)
	{
		lookup.insert(std::pair<Segment3D, int>(Segment3D(nvec[nind[i * 3]].mPosition, nvec[nind[i * 3 + 1]].mPosition).orderedCopy(), i));
		lookup.insert(std::pair<Segment3D, int>(Segment3D(nvec[nind[i * 3]].mPosition, nvec[nind[i * 3 + 2]].mPosition).orderedCopy(), i));
		lookup.insert(std::pair<Segment3D, int>(Segment3D(nvec[nind[i * 3 + 1]].mPosition, nvec[nind[i * 3 + 2]].mPosition).orderedCopy(), i));
	}
}
//-----------------------------------------------------------------------

void Boolean::addToTriangleBuffer(TriangleBuffer& buffer) const
{
	const std::vector<TriangleBuffer::Vertex>& vec1 = mMesh1->getVertices();
	const std::vector<int>& ind1 = mMesh1->getIndices();
	const std::vector<TriangleBuffer::Vertex>& vec2 = mMesh2->getVertices();
	const std::vector<int>& ind2 = mMesh2->getIndices();
	Segment3D intersectionResult;

	std::vector<Intersect> intersectionList;

	// Find all intersections between mMesh1 and mMesh2
	int idx1 = 0;
	for (std::vector<int>::const_iterator it = ind1.begin(); it != ind1.end(); idx1++)
	{
		Triangle3D t1(vec1[*it++].mPosition, vec1[*it++].mPosition, vec1[*it++].mPosition);

		int idx2 = 0;
		for (std::vector<int>::const_iterator it2 = ind2.begin(); it2 != ind2.end(); idx2++)
		{
			Triangle3D t2(vec2[*it2++].mPosition, vec2[*it2++].mPosition, vec2[*it2++].mPosition);

			if (t1.findIntersect(t2, intersectionResult))
			{
				Intersect intersect(intersectionResult, idx1, idx2);
				intersectionList.push_back(intersect);
			}
		}
	}
	// Remove all intersection segments too small to be relevant
	for (std::vector<Intersect>::iterator it = intersectionList.begin(); it != intersectionList.end();)
		if ((it->mSeg.mB - it->mSeg.mA).squaredLength() < 1e-8)
			it = intersectionList.erase(it);
		else
			++it;

	// Retriangulate
	TriangleBuffer newMesh1, newMesh2;
	_retriangulate(newMesh1, *mMesh1, intersectionList, true);
	_retriangulate(newMesh2, *mMesh2, intersectionList, false);

	//buffer.append(newMesh1);
	//buffer.append(newMesh2);
	//return;

	// Trace contours
	std::vector<Path> contours;
	std::vector<Segment3D> segmentSoup;
	for (std::vector<Intersect>::iterator it = intersectionList.begin(); it != intersectionList.end(); ++it)
		segmentSoup.push_back(it->mSeg);
	Path().buildFromSegmentSoup(segmentSoup, contours);

	// Build a lookup from segment to triangle
	TriLookup triLookup1, triLookup2;
	_buildTriLookup(triLookup1, newMesh1);
	_buildTriLookup(triLookup2, newMesh2);

	std::set<Segment3D, Seg3Comparator> limits;
	for (std::vector<Segment3D>::iterator it = segmentSoup.begin(); it != segmentSoup.end(); ++it)
		limits.insert(it->orderedCopy());
	// Build resulting mesh
	for (std::vector<Path>::iterator it = contours.begin(); it != contours.end(); ++it)
	{
		// Find 2 seed triangles for each contour
		Segment3D firstSeg(it->getPoint(0), it->getPoint(1));

		std::pair<TriLookup::iterator, TriLookup::iterator> it2mesh1 = triLookup1.equal_range(firstSeg.orderedCopy());
		std::pair<TriLookup::iterator, TriLookup::iterator> it2mesh2 = triLookup2.equal_range(firstSeg.orderedCopy());
		int mesh1seed1, mesh1seed2, mesh2seed1, mesh2seed2;

		if (it2mesh1.first != triLookup1.end() && it2mesh2.first != triLookup2.end())
		{
			// check which of seed1 and seed2 must be included (it can be 0, 1 or both)
			mesh1seed1 = it2mesh1.first->second;
			mesh1seed2 = (--it2mesh1.second)->second;
			mesh2seed1 = it2mesh2.first->second;
			mesh2seed2 = (--it2mesh2.second)->second;
			if (mesh1seed1 == mesh1seed2)
				mesh1seed2 = -1;
			if (mesh2seed1 == mesh2seed2)
				mesh2seed2 = -1;

			Vector3 vMesh1, nMesh1, vMesh2, nMesh2;
			for (int i=0; i<3; i++)
			{
				const Vector3& pos = newMesh1.getVertices()[newMesh1.getIndices()[mesh1seed1 * 3 + i]].mPosition;
				if (pos.squaredDistance(firstSeg.mA)>1e-6 && pos.squaredDistance(firstSeg.mB)>1e-6)
				{
					vMesh1 = pos;
					nMesh1 = newMesh1.getVertices()[newMesh1.getIndices()[mesh1seed1 * 3 + i]].mNormal;
					break;
				}
			}

			for (int i=0; i<3; i++)
			{
				const Vector3& pos = newMesh2.getVertices()[newMesh2.getIndices()[mesh2seed1 * 3 + i]].mPosition;
				if (pos.squaredDistance(firstSeg.mA)>1e-6 && pos.squaredDistance(firstSeg.mB)>1e-6)
				{
					vMesh2 = pos;
					nMesh2 = newMesh2.getVertices()[newMesh2.getIndices()[mesh2seed1 * 3 + i]].mNormal;
					break;
				}
			}

			bool M2S1InsideM1 = (nMesh1.dotProduct(vMesh2-firstSeg.mA) < 0);
			bool M1S1InsideM2 = (nMesh2.dotProduct(vMesh1-firstSeg.mA) < 0);

			_removeFromTriLookup(mesh1seed1, triLookup1);
			_removeFromTriLookup(mesh2seed1, triLookup2);
			_removeFromTriLookup(mesh1seed2, triLookup1);
			_removeFromTriLookup(mesh2seed2, triLookup2);

			// Recursively add all neighbours of these triangles
			// Stop when a contour is touched
			switch (mBooleanOperation)
			{
			case BT_UNION:
				if (M1S1InsideM2)
					_recursiveAddNeighbour(buffer, newMesh1, mesh1seed2, triLookup1, limits, false);
				else
					_recursiveAddNeighbour(buffer, newMesh1, mesh1seed1, triLookup1, limits, false);
				if (M2S1InsideM1)
					_recursiveAddNeighbour(buffer, newMesh2, mesh2seed2, triLookup2, limits, false);
				else
					_recursiveAddNeighbour(buffer, newMesh2, mesh2seed1, triLookup2, limits, false);
				break;
			case BT_INTERSECTION:
				if (M1S1InsideM2)
					_recursiveAddNeighbour(buffer, newMesh1, mesh1seed1, triLookup1, limits, false);
				else
					_recursiveAddNeighbour(buffer, newMesh1, mesh1seed2, triLookup1, limits, false);
				if (M2S1InsideM1)
					_recursiveAddNeighbour(buffer, newMesh2, mesh2seed1, triLookup2, limits, false);
				else
					_recursiveAddNeighbour(buffer, newMesh2, mesh2seed2, triLookup2, limits, false);
				break;
			case BT_DIFFERENCE:
				if (M1S1InsideM2)
					_recursiveAddNeighbour(buffer, newMesh1, mesh1seed2, triLookup1, limits, false);
				else
					_recursiveAddNeighbour(buffer, newMesh1, mesh1seed1, triLookup1, limits, false);
				if (M2S1InsideM1)
					_recursiveAddNeighbour(buffer, newMesh2, mesh2seed1, triLookup2, limits, true);
				else
					_recursiveAddNeighbour(buffer, newMesh2, mesh2seed2, triLookup2, limits, true);
				break;
			}
		}
	}
}
}
