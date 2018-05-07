/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_VECTOR3_H
#define APE_VECTOR3_H

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

namespace Ape
{
	struct Vector3
	{
		float x, y, z;

		Vector3() :
			x(0.0f), y(0.0f), z(0.0f)
		{}

		Vector3(float _x, float _y, float _z) :
			x(_x), y(_y), z(_z)
		{}

		Vector3(float _d) :
			x(_d), y(_d), z(_d)
		{}

		float squaredLength() const
		{
			return x * x + y * y + z * z;
		}

		float length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		float distance(const Vector3& rkVector) const
		{
			return (*this - rkVector).length();
		}

		Vector3 crossProduct(const Vector3& rkVector) const
		{
			return Vector3(
			           y * rkVector.z - z * rkVector.y,
			           z * rkVector.x - x * rkVector.z,
			           x * rkVector.y - y * rkVector.x);
		}

		float dotProduct(const Vector3& vec) const
		{
			return x * vec.x + y * vec.y + z * vec.z;
		}

		Vector3& operator = (const Vector3& rkVector)
		{
			x = rkVector.x;
			y = rkVector.y;
			z = rkVector.z;

			return *this;
		}

		Vector3& operator = (const float fScaler)
		{
			x = fScaler;
			y = fScaler;
			z = fScaler;

			return *this;
		}

		bool operator == (const Vector3& rkVector) const
		{
			return equalTo(rkVector);
		}

		bool operator < (const Vector3& rkVector) const
		{
			return (x < rkVector.x && y < rkVector.y && z < rkVector.z);
		}

		bool operator > (const Vector3& rkVector) const
		{
			return (x > rkVector.x && y > rkVector.y && z > rkVector.z);
		}

		bool operator != (const Vector3& rkVector) const
		{
			return !equalTo(rkVector);
		}

		bool equalTo(const Vector3& rkVector) const
		{
			return (x == rkVector.x && y == rkVector.y && z == rkVector.z);
		}

		Vector3 operator * (const float fScalar) const
		{
			return Vector3(
			           x * fScalar,
			           y * fScalar,
			           z * fScalar);
		}

		Vector3 operator / (const float fScalar) const
		{
			return Vector3(
			           x / fScalar,
			           y / fScalar,
			           z / fScalar);
		}

		Vector3 operator / (const Vector3& rhs) const
		{
			return Vector3(
			           x / rhs.x,
			           y / rhs.y,
			           z / rhs.z);
		}

		Vector3 operator * (const Vector3& rhs) const
		{
			return Vector3(
			           x * rhs.x,
			           y * rhs.y,
			           z * rhs.z);
		}

		Vector3 operator + (const Vector3& rkVector) const
		{
			return Vector3(
			           x + rkVector.x,
			           y + rkVector.y,
			           z + rkVector.z);
		}

		Vector3 operator - (const Vector3& rkVector) const
		{
			return Vector3(
			           x - rkVector.x,
			           y - rkVector.y,
			           z - rkVector.z);
		}

		bool isNaN() const
		{
			return std::isnan(x) && std::isnan(y) && std::isnan(z);
		}

		std::string toString() const
		{
			std::ostringstream buff;
			buff << "Vector3(" << x << ", " << y << ", " << z << ")";
			return buff.str();
		}

		float normalise()
		{
			float fLength = sqrt(x * x + y * y + z * z);
			if (fLength > 1e-08)
			{
				float fInvLength = 1.0f / fLength;
				x *= fInvLength;
				y *= fInvLength;
				z *= fInvLength;
			}
			return fLength;
		}

		float getX()
		{
			return x;
		}

		float getY()
		{
			return y;
		}

		float getZ()
		{
			return z;
		}
	};
}

#endif
