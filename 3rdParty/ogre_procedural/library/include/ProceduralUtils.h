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
#ifndef PROCEDURAL_UTILS_INCLUDED
#define PROCEDURAL_UTILS_INCLUDED
#include "OgreVector3.h"
#include "OgreAxisAlignedBox.h"
#include "OgreLogManager.h"
#include "ProceduralPlatform.h"
#include "OgreStringConverter.h"
#include "OgreCommon.h"

namespace Procedural
{
/// Holds a bunch of static utility functions
class _ProceduralExport Utils
{
	static int counter;
public:
	/// Outputs something to the ogre log, with a [PROCEDURAL] prefix
	static void log(const Ogre::String& st);

	/// Gets the min of the coordinates between 2 vectors
	static Ogre::Vector3 min(const Ogre::Vector3& v1, const Ogre::Vector3& v2)
	{
		return Ogre::Vector3(std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z));
	}

	/// Gets the max of the coordinates between 2 vectors
	static Ogre::Vector3 max(const Ogre::Vector3& v1, const Ogre::Vector3& v2)
	{
		return Ogre::Vector3(std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z));
	}

	/// Gets the min of the coordinates between 2 vectors
	static Ogre::Vector2 min(const Ogre::Vector2& v1, const Ogre::Vector2& v2)
	{
		return Ogre::Vector2(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
	}

	/// Gets the max of the coordinates between 2 vectors
	static Ogre::Vector2 max(const Ogre::Vector2& v1, const Ogre::Vector2& v2)
	{
		return Ogre::Vector2(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
	}

	/// Builds an AABB from a list of points
	static Ogre::AxisAlignedBox AABBfromPoints(std::vector<Ogre::Vector3> points)
	{
		Ogre::AxisAlignedBox aabb;
		if (points.size() == 0)
			return aabb;
		aabb.setMinimum(points[0]);
		aabb.setMaximum(points[0]);
		for (std::vector<Ogre::Vector3>::iterator it = points.begin(); it!=points.end(); ++it)
		{
			aabb.setMinimum(min(aabb.getMinimum(), *it));
			aabb.setMaximum(max(aabb.getMaximum(), *it));
		}

		return aabb;
	}

	/// Generate a name from a prefix and a counter
	static std::string getName(const std::string& prefix= "default");

	/// Shifts the components of the vector to the right
	static Ogre::Vector3 vectorPermute(const Ogre::Vector3& in)
	{
		return Ogre::Vector3(in.z, in.x, in.y);
	}

	/// Shifts the components of the vector to the left
	static Ogre::Vector3 vectorAntiPermute(const Ogre::Vector3& in)
	{
		return Ogre::Vector3(in.y, in.z, in.x);
	}

	// Rotates a Vector2 by a given oriented angle
	static inline Ogre::Vector2 rotateVector2(const Ogre::Vector2& in, Ogre::Radian angle)
	{
		return Ogre::Vector2(in.x* Ogre::Math::Cos(angle) - in.y * Ogre::Math::Sin(angle),
		                     in.x * Ogre::Math::Sin(angle) + in.y * Ogre::Math::Cos(angle));
	}

	/// Caps n between min and max
	static int cap(int n, int min, int max)
	{
		return std::max(std::min(n,max),min);
	}

	/**
	 * An extend version of the standard modulo, in that int values are "wrapped"
	 * in both directions, whereas with standard modulo, (-1)%2 == -1
	 * Always return an int between 0 and cap-1
	 */
	static int modulo(int n, int cap)
	{
		if (n>=0)
			return n%cap;
		return (cap-1)-((1+n)%cap);
	}

	/**
	 * Equivalent of Ogre::Vector3::angleBetween, applied to Ogre::Vector2
	 */
	static inline Ogre::Radian angleBetween(const Ogre::Vector2& v1, const Ogre::Vector2& v2)
	{
		Ogre::Real lenProduct = v1.length() * v2.length();
		// Divide by zero check
		if (lenProduct < 1e-6f)
			lenProduct = 1e-6f;

		Ogre::Real f = v1.dotProduct(v2) / lenProduct;

		f = Ogre::Math::Clamp(f, (Ogre::Real)-1.0, (Ogre::Real)1.0);
		return Ogre::Math::ACos(f);
	}

	/**
	 * Gives the oriented angle from v1 to v2 in the [0;2PI[ range
	 */
	static inline Ogre::Radian angleTo(const Ogre::Vector2& v1, const Ogre::Vector2& v2)
	{
		Ogre::Radian angle = angleBetween(v1, v2);

		if (v1.crossProduct(v2)<0)
			angle = (Ogre::Radian)Ogre::Math::TWO_PI - angle;

		return angle;
	}

	/**
	 * Gives the oriented angle from v1 to v2 in the ]-PI;PI] range
	 */
	static inline Ogre::Radian signedAngleTo(const Ogre::Vector2& v1, const Ogre::Vector2& v2)
	{
		Ogre::Radian angle = angleBetween(v1, v2);

		if (v1.crossProduct(v2)<0)
			angle = - angle;

		return angle;
	}

	/**
	 * Computes a quaternion between UNIT_Z and direction.
	 * It keeps the "up" vector to UNIT_Y
	 */
	static Ogre::Quaternion _computeQuaternion(const Ogre::Vector3& direction, const Ogre::Vector3& upVector = Ogre::Vector3::UNIT_Y);

	/**
	 * Maps a vector2 to vector3, with Y=0
	 */
	static inline Ogre::Vector3 vec2ToVec3Y(const Ogre::Vector2& pos)
	{
		return Ogre::Vector3(pos.x, 0, pos.y);
	}

	/**
	 * binomial coefficients (a over b)
	 */
	static inline unsigned int binom(unsigned int a, unsigned int b)
	{
		int tmpA, tmpB;
		if (( b == 0 ) || (a == b))
			return 1;
		else
		{
			tmpA = binom(a - 1, b);
			tmpB = binom(a - 1, b - 1);
			return tmpA + tmpB;
		}
	}

	/// Transforms an input vector expressed in the 0,0->1,1 rect towards another rect
	static inline Ogre::Vector2 reframe(const Ogre::RealRect& rect, const Ogre::Vector2& input)
	{
		return Ogre::Vector2(rect.left + input.x*rect.width(), rect.top + input.y*rect.height());
	}
};
}
#endif
