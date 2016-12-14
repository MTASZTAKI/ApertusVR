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
#include "ProceduralPath.h"
#include "ProceduralGeometryHelpers.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Procedural
{

Path Path::mergeKeysWithTrack(const Track& track) const
{
	if (!track.isInsertPoint() || track.getAddressingMode() == Track::AM_POINT)
		return *this;
	Real totalLength = getTotalLength();

	Real lineicPos = 0;
	Real pathLineicPos = 0;
	Path outputPath;
	outputPath.addPoint(getPoint(0));
	for (unsigned int i = 1; i < mPoints.size();)
	{
		Real nextLineicPos = pathLineicPos + (mPoints[i] - mPoints[i - 1]).length();

		std::map<Real, Real>::const_iterator it = track._getKeyValueAfter(lineicPos, lineicPos / totalLength, i - 1);

		Real nextTrackPos = it->first;
		if (track.getAddressingMode() == Track::AM_RELATIVE_LINEIC)
			nextTrackPos *= totalLength;

		// Adds the closest point to the curve, being either from the path or the track
		if (nextLineicPos <= nextTrackPos || lineicPos >= nextTrackPos)
		{
			outputPath.addPoint(mPoints[i]);
			i++;
			lineicPos = nextLineicPos;
			pathLineicPos = nextLineicPos;
		}
		else
		{
			outputPath.addPoint(getPosition(i - 1, (nextTrackPos - pathLineicPos) / (nextLineicPos - pathLineicPos)));
			lineicPos = nextTrackPos;
		}
	}
	return outputPath;
}

Ogre::MeshPtr Path::realizeMesh(const std::string& name) const
{
	Ogre::SceneManager* smgr = Ogre::Root::getSingleton().getSceneManagerIterator().begin()->second;
	Ogre::ManualObject* manual = smgr->createManualObject();
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	for (std::vector<Ogre::Vector3>::const_iterator itPos = mPoints.begin(); itPos != mPoints.end(); itPos++)
		manual->position(*itPos);
	if (mClosed)
		manual->position(*(mPoints.begin()));
	manual->end();

	Ogre::MeshPtr mesh;
	if (name == "")
		mesh = manual->convertToMesh(Utils::getName());
	else
		mesh = manual->convertToMesh(name);

	return mesh;
}

Ogre::Vector3 Path::getPosition(Ogre::Real coord) const
{
	if (mPoints.size() < 2)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "The path must at least contain 2 points", "Procedural::Path::getPosition(Ogre::Real)");
	unsigned int i = 0;
	while (true)
	{
		Ogre::Real nextLen = (getPoint(i + 1) - getPoint(i)).length();
		if (coord > nextLen)
			coord -= nextLen;
		else
			return getPosition(i, coord / nextLen);
		if (!mClosed && i >= mPoints.size() - 2)
			return mPoints.back();
		i++;
	}
}

Ogre::Real Path::getLengthAtPoint(size_t index) const
{
	Ogre::Real length = 0;
	for (unsigned int i = 0; i < index; ++i)
		length += (mPoints[i + 1] - mPoints[i]).length();
	//if (mClosed)
	//length += (mPoints.back()-*mPoints.begin()).length();
	return length;

}

Ogre::Real Path::getTotalLength() const
{
	Ogre::Real length = 0;
	for (unsigned int i = 0; i < mPoints.size() - 1; ++i)
		length += (mPoints[i + 1] - mPoints[i]).length();
	if (mClosed)
		length += (mPoints.back()-*mPoints.begin()).length();
	return length;
}

void Path::buildFromSegmentSoup(const std::vector<Segment3D>& segList, std::vector<Path>& out)
{
	typedef std::multimap<Vector3, Vector3, Vector3Comparator> Vec3MultiMap;
	Vec3MultiMap segs;
	for (std::vector<Segment3D>::const_iterator it = segList.begin(); it != segList.end(); ++it)
	{
		segs.insert(std::pair<Vector3, Vector3 > (it->mA, it->mB));
		segs.insert(std::pair<Vector3, Vector3 > (it->mB, it->mA));
	}
	while (!segs.empty())
	{
		Ogre::Vector3 headFirst = segs.begin()->first;
		Ogre::Vector3 headSecond = segs.begin()->second;
		Path p;
		p.addPoint(headFirst).addPoint(headSecond);
		Vec3MultiMap::iterator firstSeg = segs.begin();
		std::pair<Vec3MultiMap::iterator, Vec3MultiMap::iterator> correspondants2 = segs.equal_range(headSecond);
		for (Vec3MultiMap::iterator it = correspondants2.first; it != correspondants2.second;)
		{
			Vec3MultiMap::iterator removeIt = it++;
			if ((removeIt->second - firstSeg->first).squaredLength() < 1e-8)
				segs.erase(removeIt);
		}
		segs.erase(firstSeg);
		bool foundSomething = true;
		while (!segs.empty() && foundSomething)
		{
			foundSomething = false;
			Vec3MultiMap::iterator next = segs.find(headSecond);
			if (next != segs.end())
			{
				foundSomething = true;
				headSecond = next->second;
				p.addPoint(headSecond);
				std::pair<Vec3MultiMap::iterator, Vec3MultiMap::iterator> correspondants = segs.equal_range(headSecond);
				for (Vec3MultiMap::iterator it = correspondants.first; it != correspondants.second;)
				{
					Vec3MultiMap::iterator removeIt = it++;
					if ((removeIt->second - next->first).squaredLength() < 1e-8)
						segs.erase(removeIt);
				}
				segs.erase(next);
			}
			Vec3MultiMap::iterator previous = segs.find(headFirst);
			if (previous != segs.end())
			{
				foundSomething = true;
				p.insertPoint(0, previous->second);
				headFirst = previous->second;
				std::pair<Vec3MultiMap::iterator, Vec3MultiMap::iterator> correspondants = segs.equal_range(headFirst);
				for (Vec3MultiMap::iterator it = correspondants.first; it != correspondants.second;)
				{
					Vec3MultiMap::iterator removeIt = it++;
					if ((removeIt->second - previous->first).squaredLength() < 1e-8)
						segs.erase(removeIt);
				}
				segs.erase(previous);
			}
		}
		if (p.getPoint(0).squaredDistance(p.getPoint(p.getSegCount() + 1)) < 1e-6)
		{
			p.getPointsReference().pop_back();
			p.close();
		}
		out.push_back(p);
	}
}

//-----------------------------------------------------------------------
Shape Path::convertToShape() const
{
	Shape s;
	for (std::vector<Ogre::Vector3>::const_iterator it = mPoints.begin(); it!=mPoints.end(); ++it)
	{
		s.addPoint(it->x, it->y);
	}
	if (mClosed)
		s.close();

	return s;
}
//-----------------------------------------------------------------------
void MultiPath::_calcIntersections()
{
	mIntersectionsMap.clear();
	mIntersections.clear();
	std::map<Ogre::Vector3, PathIntersection, Vector3Comparator> pointSet;
	for (std::vector<Path>::iterator it = mPaths.begin(); it!= mPaths.end(); ++it)
	{
		for (std::vector<Ogre::Vector3>::const_iterator it2 = it->getPoints().begin(); it2 != it->getPoints().end(); ++it2)
		{
			PathCoordinate pc(it-mPaths.begin(), it2-it->getPoints().begin());
			if (pointSet.find(*it2)==pointSet.end())
			{
				PathIntersection pi;
				pi.push_back(pc);
				pointSet[*it2] = pi;
			}
			else
				pointSet[*it2].push_back(pc);
		}
	}
	for (std::map<Ogre::Vector3, PathIntersection, Vector3Comparator>::iterator it = pointSet.begin(); it != pointSet.end(); ++it)
		if (it->second.size()>1)
		{
			for (PathIntersection::iterator it2 = it->second.begin(); it2!=it->second.end(); ++it2)
				mIntersectionsMap[*it2] = it->second;
			mIntersections.push_back(it->second);
		}
}

}
