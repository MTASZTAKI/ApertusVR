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
#ifndef PROCEDURAL_PATH_INCLUDED
#define PROCEDURAL_PATH_INCLUDED

#include "OgreVector3.h"
#include "OgreSimpleSpline.h"
#include "ProceduralUtils.h"
#include "ProceduralPlatform.h"
#include "ProceduralTrack.h"
#include "ProceduralGeometryHelpers.h"
#include "ProceduralShape.h"

namespace Procedural
{
/**
\defgroup pathgrp Path
Elements for path generation.
@{
@}
*/

/**
 * \ingroup pathgrp
 * Succession of points in 3D space.
 * Can be closed or not.
 */
class _ProceduralExport Path
{
	std::vector<Ogre::Vector3> mPoints;
	bool mClosed;
public:
	/// Default constructor
	Path() : mClosed(false)	{}

	/** Adds a point to the path, as a Vector3 */
	Path& addPoint(const Ogre::Vector3& pt)
	{
		mPoints.push_back(pt);
		return *this;
	}

	/** Adds a point to the path, using its 3 coordinates */
	Path& addPoint(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPoints.push_back(Ogre::Vector3(x,y,z));
		return *this;
	}

	/// Inserts a point to the path
	/// @param index the index before the inserted point
	/// @param x new point's x coordinate
	/// @param y new point's y coordinate
	/// @param z new point's z coordinate
	inline Path& insertPoint(size_t index, Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPoints.insert(mPoints.begin()+index, Ogre::Vector3(x, y, z));
		return *this;
	}

	/// Inserts a point to the path
	/// @param index the index before the inserted point
	/// @param pt new point's position
	inline Path& insertPoint(size_t index, const Ogre::Vector3& pt)
	{
		mPoints.insert(mPoints.begin()+index, pt);
		return *this;
	}

	/// Appends another path at the end of this one
	inline Path& appendPath(const Path& other)
	{
		mPoints.insert(mPoints.end(), other.mPoints.begin(), other.mPoints.end());
		return *this;
	}

	/// Appends another path at the end of this one, relative to the last point of this path
	inline Path& appendPathRel(const Path& other)
	{
		if (mPoints.empty())
			appendPath(other);
		else
		{
			Ogre::Vector3 refVector = *(mPoints.end()-1);
			std::vector<Ogre::Vector3> pointList(other.mPoints.begin(), other.mPoints.end());
			for (std::vector<Ogre::Vector3>::iterator it = pointList.begin(); it!=pointList.end(); ++it)
				*it += refVector;
			mPoints.insert(mPoints.end(), pointList.begin(), pointList.end());
		}
		return *this;
	}

	/** Clears the content of the Path */
	Path& reset()
	{
		mPoints.clear();
		return *this;
	}

	/**
	Define the path as being closed. Almost the same as adding a last point on the first point position
	\exception Ogre::InvalidStateException Cannot close an empty path
	*/
	Path& close()
	{
		if (mPoints.empty())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, "Cannot close an empty path", "Procedural::Path::close()");
		mClosed = true;
		return *this;
	}

	/** Tells if the path is closed or not */
	bool isClosed() const
	{
		return mClosed;
	}

	/** Gets the list of points as a vector of Vector3 */
	const std::vector<Ogre::Vector3>& getPoints() const
	{
		return mPoints;
	}

	/// Gets raw vector data of this path as a non-const reference
	inline std::vector<Ogre::Vector3>& getPointsReference()
	{
		return mPoints;
	}

	/** Safely gets a given point.
	 * Takes into account whether the path is closed or not.
	 * @param i the index of the point.
	 *          if it is <0 or >maxPoint, cycle through the list of points
	 */
	const Ogre::Vector3& getPoint(int i) const
	{
		if (mClosed)
			return mPoints[Utils::modulo(i,mPoints.size())];
		return mPoints[Utils::cap(i,0,mPoints.size()-1)];
	}

	/** Gets the number of segments in the path
	 * Takes into accound whether path is closed or not
	 */
	int getSegCount() const
	{
		return (mPoints.size()-1) + (mClosed?1:0);
	}

	/**
	 * Returns local direction after the current point
	 */
	Ogre::Vector3 getDirectionAfter(unsigned int i) const
	{
		// If the path isn't closed, we get a different calculation at the end, because
		// the tangent shall not be null
		if (!mClosed && i == mPoints.size() - 1 && i > 0)
			return (mPoints[i] - mPoints[i-1]).normalisedCopy();
		else
			return (getPoint(i+1) - getPoint(i)).normalisedCopy();
	}

	/**
	 * Returns local direction after the current point
	 */
	Ogre::Vector3 getDirectionBefore(unsigned int i) const
	{
		// If the path isn't closed, we get a different calculation at the end, because
		// the tangent shall not be null
		if (!mClosed && i == 1)
			return (mPoints[1] - mPoints[0]).normalisedCopy();
		else
			return (getPoint(i) - getPoint(i-1)).normalisedCopy();
	}

	/**
	 * Returns the local direction at the current point.
	 * @param i index of the point
	 */
	Ogre::Vector3 getAvgDirection(unsigned int i) const
	{
		return (getDirectionAfter(i) + getDirectionBefore(i)).normalisedCopy();
	}

	/// Returns the total lineic length of that shape
	Ogre::Real getTotalLength() const;


	Ogre::Real getLengthAtPoint(size_t index) const;

	/// Gets a position on the shape with index of the point and a percentage of position on the segment
	/// @param i index of the segment
	/// @param coord a number between 0 and 1 meaning the percentage of position on the segment
	/// @exception Ogre::InvalidParametersException i is out of bounds
	/// @exception Ogre::InvalidParametersException coord must be comprised between 0 and 1
	inline Ogre::Vector3 getPosition(unsigned int i, Ogre::Real coord) const
	{
		if (i >= mPoints.size())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Out of Bounds", "Procedural::Path::getPosition(unsigned int, Ogre::Real)");
		if (coord < 0.0f || coord > 1.0f)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Coord must be comprised between 0 and 1", "Procedural::Path::getPosition(unsigned int, Ogre::Real)");
		Ogre::Vector3 A = getPoint(i);
		Ogre::Vector3 B = getPoint(i+1);
		return A + coord*(B-A);
	}

	/// Gets a position on the shape from lineic coordinate
	/// @param coord lineic coordinate
	/// @exception Ogre::InvalidStateException The path must at least contain 2 points
	Ogre::Vector3 getPosition(Ogre::Real coord) const;

	/**
	 * Outputs a mesh representing the path.
	 * Mostly for debugging purposes
	 */
	Ogre::MeshPtr realizeMesh(const std::string& name = "") const;

	/// Creates a path with the keys of this path and extra keys coming from a track
	Path mergeKeysWithTrack(const Track& track) const;

	/**
	* Applies the given translation to all the points already defined.
	* Has strictly no effect on the points defined after that
	* @param translation the translation vector
	*/
	Path& translate(const Ogre::Vector3& translation)
	{
		for (std::vector<Ogre::Vector3>::iterator it = mPoints.begin(); it!=mPoints.end(); ++it)
			*it+=translation;
		return *this;
	}

	/**
	 * Applies the given translation to all the points already defined.
	 * Has strictly no effect on the points defined after that
	 * @param translationX X component of the translation vector
	 * @param translationY Y component of the translation vector
	 * @param translationZ Z component of the translation vector
	 */
	Path& translate(Ogre::Real translationX, Ogre::Real translationY, Ogre::Real translationZ)
	{
		return translate(Ogre::Vector3(translationX, translationY, translationZ));
	}

	/**
	 * Applies the given scale to all the points already defined.
	 * Has strictly no effect on the points defined after that
	 * @param amount amount of scale
	 */
	Path& scale(Ogre::Real amount)
	{
		return scale(amount, amount, amount);
	}

	/**
	 * Applies the given scale to all the points already defined.
	 * Has strictly no effect on the points defined after that
	 * @param scaleX amount of scale in the X direction
	 * @param scaleY amount of scale in the Y direction
	 * @param scaleZ amount of scale in the Z direction
	 */
	Path& scale(Ogre::Real scaleX, Ogre::Real scaleY, Ogre::Real scaleZ)
	{
		for (std::vector<Ogre::Vector3>::iterator it = mPoints.begin(); it!=mPoints.end(); ++it)
		{
			it->x *= scaleX;
			it->y *= scaleY;
			it->z *= scaleZ;
		}
		return *this;
	}

	/**
	 * Applies the given scale to all the points already defined.
	 * Has strictly no effect on the points defined after that
	 * @param amount of scale
	 */
	Path& scale(const Ogre::Vector3& amount)
	{
		return scale(amount.x, amount.y, amount.z);
	}

	/**
	 * Reflect all points in this path against a zero-origined plane with a given normal
	 * @param normal the normal
	 */
	Path& reflect(const Ogre::Vector3& normal)
	{
		for (std::vector<Ogre::Vector3>::iterator it = mPoints.begin(); it!=mPoints.end(); ++it)
		{
			*it = it->reflect(normal);
		}
		return *this;
	}

	/// Extracts a part of the shape as a new path
	/// @param first first index to be in the new path
	/// @param last last index to be in the new path
	inline Path extractSubPath(unsigned int first, unsigned int last)
	{
		Path p;
		for (unsigned int i=first; i<last; i++)
			p.addPoint(mPoints[i]);
		if (mClosed)
			p.close();
		return p;
	}

	/// Reverses direction of the path
	inline Path& reverse()
	{
		std::reverse(mPoints.begin(), mPoints.end());
		return *this;
	}

	void buildFromSegmentSoup(const std::vector<Segment3D>& segList, std::vector<Path>& out);

	/// Converts the path to a shape, with Y=0
	Shape convertToShape() const;

};

class _ProceduralExport MultiPath
{
public:
	struct PathCoordinate
	{
		unsigned int pathIndex;
		unsigned int pointIndex;
		PathCoordinate(unsigned int _pathIndex, unsigned int _pointIndex) : pathIndex(_pathIndex), pointIndex(_pointIndex) {}
		bool operator < (const PathCoordinate& other) const
		{
			if (pathIndex != other.pathIndex)
				return pathIndex < other.pathIndex;
			return pointIndex<other.pointIndex;
		}
	};
	typedef std::vector<PathCoordinate> PathIntersection;
private:
	std::vector<Path> mPaths;
	std::map<PathCoordinate, PathIntersection> mIntersectionsMap;
	std::vector<PathIntersection> mIntersections;
public:

	inline void clear()
	{
		mPaths.clear();
	}

	inline MultiPath& addPath(const Path& path)
	{
		mPaths.push_back(path);
		return *this;
	}

	inline MultiPath& addMultiPath(const MultiPath& multiPath)
	{
		for (std::vector<Path>::const_iterator it = multiPath.mPaths.begin(); it!=multiPath.mPaths.end(); ++it)
			mPaths.push_back(*it);
		return *this;
	}

	void setPath(unsigned int i, const Path& path) {
		mPaths[i] = path;
	}

	unsigned int getPathCount() const
	{
		return mPaths.size();
	}

	Path getPath(unsigned int i) const
	{
		return mPaths[i];
	}

	void _calcIntersections();

	inline const std::map<PathCoordinate, PathIntersection>& getIntersectionsMap() const
	{
		return mIntersectionsMap;
	}

	inline const std::vector<PathIntersection>& getIntersections() const
	{
		return mIntersections;
	}

	std::vector<std::pair<unsigned int, unsigned int> > getNoIntersectionParts(unsigned int pathIndex) const
	{
		Path path = mPaths[pathIndex];
		std::vector<std::pair<unsigned int, unsigned int> > result;
		std::vector<int> intersections;
		for (std::map<PathCoordinate, PathIntersection>::const_iterator it = mIntersectionsMap.begin(); it != mIntersectionsMap.end(); ++it)
			if (it->first.pathIndex == pathIndex)
				intersections.push_back(it->first.pointIndex);
		std::sort(intersections.begin(), intersections.end());
		int begin = 0;
		for (std::vector<int>::iterator it = intersections.begin(); it!= intersections.end(); ++it)
		{
			if (*it-1>begin)
				result.push_back(std::pair<unsigned int, unsigned int>(begin, *it-1));
			begin = *it+1;
		}
		if (path.getSegCount() > begin)
			result.push_back(std::pair<unsigned int, unsigned int>(begin, path.getSegCount()));
		return result;
	}
};

}

#endif

