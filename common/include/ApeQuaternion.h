/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#ifndef APE_QUATERNION_H
#define APE_QUATERNION_H

#include <vector>
#include <cmath>
#include <sstream>
#include "ApeVector3.h"
#include "ApeDegree.h"
#include "ApeRadian.h"

namespace Ape
{
	struct Quaternion
	{
		float w, x, y, z;

		Quaternion() : 
			w(1.0f), x(0.0f), y(0.0f), z(0.0f)
		{}
		
		Quaternion(float _w, float _x, float _y, float _z) : 
			w(_w), x(_x), y(_y), z(_z)
		{}
		
		Quaternion(const Ape::Degree& _degRot, const Ape::Vector3& _axRot)
		{
			FromAngleAxis(_degRot, _axRot);
		}
		
		Quaternion(const Ape::Radian& _radRot, const Ape::Vector3& _axRot)
		{
			FromAngleAxis(_radRot, _axRot);
		}
		
		Quaternion operator+ (const Quaternion& rkQ) const
		{
			return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
		}
		
		Quaternion operator- (const Quaternion& rkQ) const
		{
			return Quaternion(w - rkQ.w, x - rkQ.x, y - rkQ.y, z - rkQ.z);
		}
		
		Quaternion operator- () const
		{
			return Quaternion(-w, -x, -y, -z);
		}

		Quaternion operator / (const float fScalar) const
		{
			return Quaternion(
				w / fScalar,
				x / fScalar,
				y / fScalar,
				z / fScalar);
		}
		
		Ape::Vector3 operator* (const Ape::Vector3& v) const
		{
			Ape::Vector3 uv, uuv;
			Ape::Vector3 qvec(x, y, z);
			uv = qvec.crossProduct(v);
			uuv = qvec.crossProduct(uv);
			uv = uv * (2.0f * w);
			uuv = uuv * 2.0f;
			return v + uv + uuv;
		}

		bool operator < (const Quaternion& rkQ) const
		{
			return (w < rkQ.w && x < rkQ.x && y < rkQ.y && z < rkQ.z);
		}

		bool operator > (const Quaternion& rkQ) const
		{
			return (w > rkQ.w  && x > rkQ.x && y > rkQ.y && z > rkQ.z);
		}
		
		Quaternion operator* (const Quaternion& rkQ) const
		{
			return Quaternion(
				w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
				w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
				w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
				w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x);
		}
		
		void FromAngleAxis(Ape::Radian _angleRadian, const Ape::Vector3& _axis)
		{
			float fHalfAngle ((float)(0.5* _angleRadian.radian));
			float fSin = std::sin(fHalfAngle);
			w = std::cos(fHalfAngle);
			x = fSin * _axis.x;
			y = fSin * _axis.y;
			z = fSin * _axis.z;
		}
		
		void FromAngleAxis(Ape::Degree _angleDegree, const Ape::Vector3& _axis)
		{
			FromAngleAxis(Ape::Radian(_angleDegree.toRadian()), _axis);
		}
		
		bool equals(const Ape::Quaternion& _q2, Ape::Radian _tolerance)
		{
			float fCos = w * _q2.w + x * _q2.x + y * _q2.y + z * _q2.z;
			float angle = std::acos(fCos); 
			return (abs(angle) <= _tolerance.radian) || abs(angle - ape_PI) <= _tolerance.radian;
		}
		
		float Norm () const
		{
			return w * w + x * x + y * y + z * z;
		}
		
		float normalise()
		{
			float len = Norm();
			float factor = 1.0f / std::sqrt(len);
			w = w * factor;
			x = x * factor;
			y = y * factor;
			z = z * factor;
			return len;
		}
		
		Quaternion Inverse () const
		{
			float fNorm = w * w + x * x + y * y + z * z;
			if ( fNorm > 0.0 )
			{
				float fInvNorm = 1.0f / fNorm;
				return Quaternion(w * fInvNorm, -x * fInvNorm, -y * fInvNorm, -z * fInvNorm);
			}
			else
			{
				return Quaternion(0, 0, 0, 0);
			}
		}
		
		std::string toString() const
		{
			std::ostringstream buff;
			buff << "Quaternion(" << w << ", " << x << ", " << y << ", " << z << ")";
			return buff.str();
		}
		
		float Dot(const Quaternion& rkQ) const
		{
			return w * rkQ.w + x * rkQ.x + y * rkQ.y + z * rkQ.z;
		}

		static Quaternion Slerp(float fT, const Quaternion& rkP,
			const Quaternion& rkQ, bool shortestPath)
		{
			float fCos = rkP.Dot(rkQ);
			Quaternion rkT;
			if (fCos < 0.0f && shortestPath)
			{
				fCos = -fCos;
				rkT = -rkQ;
			}
			else
			{
				rkT = rkQ;
			}

			if (abs(fCos) < 1 - 1e-03)
			{
				float fSin = std::sqrt(1 - std::pow(fCos, 2));
				Radian fAngle = std::atan2(fSin, fCos);
				float fInvSin = 1.0f / fSin;
				float fCoeff0 = std::sin((1.0f - fT) * fAngle.radian) * fInvSin;
				float fCoeff1 = std::sin(fT * fAngle.radian) * fInvSin;
				return Quaternion(fCoeff0 * rkP.w, fCoeff0 * rkP.x, fCoeff0 * rkP.y, fCoeff0 * rkP.z) + Quaternion(fCoeff1 * rkT.w, fCoeff1 * rkT.x, fCoeff1 * rkT.y, fCoeff1 * rkT.z);
			}
			else
			{
				float fScalar0 = (1.0f - fT);
				Quaternion t = Quaternion(fScalar0 * rkP.w, fScalar0 * rkP.x, fScalar0 * rkP.y, fScalar0 * rkP.z) + Quaternion(fT * rkT.w, fT * rkT.x, fT * rkT.y, fT * rkT.z);
				t.normalise();
				return t;
			}
		}

		float getW() { return w; }
		float getX() { return x; }
		float getY() { return y; }
		float getZ() { return z; }

	};
}

#endif
