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
#ifndef PROCEDURAL_PATH_GENERATORS_INCLUDED
#define PROCEDURAL_PATH_GENERATORS_INCLUDED

#include "ProceduralPath.h"
#include "ProceduralSplines.h"
#include "ProceduralPathGeneratorsBase.h"

namespace Procedural
{
/**
 * \addtogroup shapegrp
 * @{
 */
//-----------------------------------------------------------------------
/**
 * Builds a path from a Catmull-Rom Spline.
 * Catmull-Rom Spline is the exact equivalent of Ogre's simple spline, ie
 * a spline for which position is smoothly interpolated between control points
 */
class _ProceduralExport CatmullRomSpline3 : public BaseSpline3<CatmullRomSpline3>
{
	std::vector<Ogre::Vector3> mPoints;
public:
	/// Default constructor
	CatmullRomSpline3() {}

	/// Copy constructor from an Ogre simplespline
	CatmullRomSpline3(const Ogre::SimpleSpline& input)
	{
		mPoints.resize(input.getNumPoints());
		for (unsigned short i=0; i<input.getNumPoints(); i++)
			mPoints.push_back(input.getPoint(i));
	}

	/// Outputs current spline to an Ogre spline
	Ogre::SimpleSpline toSimpleSpline() const
	{
		Ogre::SimpleSpline spline;
		for (unsigned short i=0; i<mPoints.size(); i++)
			spline.addPoint(mPoints[i]);
		return spline;
	}

	/// Adds a control point
	inline CatmullRomSpline3& addPoint(const Ogre::Vector3& pt)
	{
		mPoints.push_back(pt);
		return *this;
	}

	/// Adds a control point
	inline CatmullRomSpline3& addPoint(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPoints.push_back(Ogre::Vector3(x,y,z));
		return *this;
	}

	/// Safely gets a control point
	inline const Ogre::Vector3& safeGetPoint(unsigned int i) const
	{
		if (mClosed)
			return mPoints[Utils::modulo(i,mPoints.size())];
		return mPoints[Utils::cap(i,0,mPoints.size()-1)];
	}

	/// Gets the number of control points
	inline const size_t getPointCount() const
	{
		return mPoints.size();
	}

	/**
	 * Build a path from Catmull-Rom control points
	 */
	Path realizePath();
};
//-----------------------------------------------------------------------
/**
 * Produces a path from Cubic Hermite control points
 */
class _ProceduralExport CubicHermiteSpline3 : public BaseSpline3<CubicHermiteSpline3>
{
public:
	typedef CubicHermiteSplineControlPoint<Ogre::Vector3> ControlPoint;
private:
	std::vector<ControlPoint> mPoints;

public:
	/// Adds a control point
	inline CubicHermiteSpline3& addPoint(const Ogre::Vector3& p, const Ogre::Vector3& before, const Ogre::Vector3& after)
	{
		mPoints.push_back(ControlPoint(p, before, after));
		return *this;
	}
	/// Adds a control point
	inline CubicHermiteSpline3& addPoint(const Ogre::Vector3& p, const Ogre::Vector3& tangent)
	{
		mPoints.push_back(ControlPoint(p, tangent, tangent));
		return *this;
	}
	/// Adds a control point
	inline CubicHermiteSpline3& addPoint(const Ogre::Vector3& p, CubicHermiteSplineAutoTangentMode autoTangentMode = AT_CATMULL)
	{
		ControlPoint cp;
		cp.position = p;
		cp.autoTangentBefore = autoTangentMode;
		cp.autoTangentAfter = autoTangentMode;
		mPoints.push_back(cp);
		return *this;
	}
	/// Adds a control point
	inline CubicHermiteSpline3& addPoint(Ogre::Real x, Ogre::Real y, Ogre::Real z, CubicHermiteSplineAutoTangentMode autoTangentMode = AT_CATMULL)
	{
		ControlPoint cp;
		cp.position = Ogre::Vector3(x,y,z);
		cp.autoTangentBefore = autoTangentMode;
		cp.autoTangentAfter = autoTangentMode;
		mPoints.push_back(cp);
		return *this;
	}
	/// Safely gets a control point
	inline const ControlPoint& safeGetPoint(unsigned int i) const
	{
		if (mClosed)
			return mPoints[Utils::modulo(i,mPoints.size())];
		return mPoints[Utils::cap(i,0,mPoints.size()-1)];
	}

	/// Gets the number of control points
	inline const size_t getPointCount() const
	{
		return mPoints.size();
	}

	/**
	 * Builds a path from control points
	 */
	Path realizePath();
};

//-----------------------------------------------------------------------
/// Builds a line Path between 2 points
class _ProceduralExport LinePath
{
	Ogre::Vector3 mPoint1, mPoint2;
	unsigned int mNumSeg;

public:
	/// Default constructor
	LinePath() : mPoint1(Ogre::Vector3::ZERO), mPoint2(Ogre::Vector3::UNIT_Y), mNumSeg(1) {}

	/// Sets first point
	inline LinePath& setPoint1(Ogre::Vector3 point1)
	{
		mPoint1 = point1;
		return *this;
	}

	/// Sets second point
	inline LinePath& setPoint2(Ogre::Vector3 point2)
	{
		mPoint2 = point2;
		return *this;
	}

	/// Sets the number of segments for this line
	/// \exception Ogre::InvalidParametersException Minimum of numSeg is 1
	inline LinePath& setNumSeg(unsigned int numSeg)
	{
		if (numSeg == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::LinePath::setNumSeg(unsigned int)");
		mNumSeg = numSeg;
		return *this;
	}

	/// Builds a linepath between 2 points
	inline LinePath& betweenPoints(Ogre::Vector3 point1, Ogre::Vector3 point2)
	{
		mPoint1 = point1;
		mPoint2 = point2;
		return *this;
	}

	/// Outputs a path
	Path realizePath()
	{
		Path p;
		for (unsigned int i = 0; i <= mNumSeg; ++i)
		{
			p.addPoint((1-i/(Ogre::Real)mNumSeg) * mPoint1 + i/(Ogre::Real)mNumSeg * mPoint2);
		}
		return p;
	}

};
//-----------------------------------------------------------------------
/**
 * Produces a path by rounding corners of a straight-lines path
 */
class _ProceduralExport RoundedCornerSpline3 : public BaseSpline3<RoundedCornerSpline3>
{
	Ogre::Real mRadius;

	std::vector<Ogre::Vector3> mPoints;

public:
	/// Default constructor
	RoundedCornerSpline3() : mRadius(.1f) {}

	/// Sets the radius of the corners (default = 0.1)
	inline RoundedCornerSpline3& setRadius(Ogre::Real radius)
	{
		mRadius = radius;
		return *this;
	}

	/// Adds a control point
	inline RoundedCornerSpline3& addPoint(const Ogre::Vector3& p)
	{
		mPoints.push_back(p);
		return *this;
	}

	/// Adds a control point
	inline RoundedCornerSpline3& addPoint(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPoints.push_back(Ogre::Vector3(x,y,z));
		return *this;
	}

	/// Safely gets a control point
	inline const Ogre::Vector3& safeGetPoint(unsigned int i) const
	{
		if (mClosed)
			return mPoints[Utils::modulo(i,mPoints.size())];
		return mPoints[Utils::cap(i,0,mPoints.size()-1)];
	}

	/// Gets the number of control points
	inline const size_t getPointCount() const
	{
		return mPoints.size();
	}

	/**
	 * Builds a shape from control points
	 * \exception Ogre::InvalidStateException The path contains no points
	 */
	Path realizePath();
};

//-----------------------------------------------------------------------
/**
 * Builds a path from a Bezier-Curve.
 */
class _ProceduralExport BezierCurve3 : public BaseSpline3<BezierCurve3>
{
	std::vector<Ogre::Vector3> mPoints;
	unsigned int mNumSeg;

public:
	/// Default constructor
	BezierCurve3() : mNumSeg(8) {}

	/// Sets number of segments per two control points
	/// \exception Ogre::InvalidParametersException Minimum of numSeg is 1
	inline BezierCurve3& setNumSeg(unsigned int numSeg)
	{
		if (numSeg == 0)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 segments", "Procedural::BezierCurve3::setNumSeg(unsigned int)");
		mNumSeg = numSeg;
		return *this;
	}

	/// Adds a control point
	inline BezierCurve3& addPoint(const Ogre::Vector3& pt)
	{
		mPoints.push_back(pt);
		return *this;
	}

	/// Adds a control point
	inline BezierCurve3& addPoint(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		mPoints.push_back(Ogre::Vector3(x,y,z));
		return *this;
	}

	/// Safely gets a control point
	inline const Ogre::Vector3& safeGetPoint(unsigned int i) const
	{
		if (mClosed)
			return mPoints[Utils::modulo(i,mPoints.size())];
		return mPoints[Utils::cap(i,0,mPoints.size()-1)];
	}

	/// Gets the number of control points
	inline const size_t getPointCount() const
	{
		return mPoints.size();
	}

	/**
	 * Build a path from bezier control points
	 * @exception Ogre::InvalidStateException The curve must at least contain 2 points
	 */
	Path realizePath();
};
/** @} */
}
#endif