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

#ifndef APE_MATRIX3_H
#define APE_MATRIX3_H

#include <assert.h>
#include <cmath>
#include <sstream>
#include <vector>
#include "datatypes/apeDegree.h"
#include "datatypes/apeRadian.h"
#include "datatypes/apeVector3.h"
#include "datatypes/apeVector4.h"

namespace ape
{
	struct Matrix3
	{
		float m[3][3];

		Matrix3()
		{
			m[0][0] = 0.0f;
			m[0][1] = 0.0f;
			m[0][2] = 0.0f;
			m[1][0] = 0.0f;
			m[1][1] = 0.0f;
			m[1][2] = 0.0f;
			m[2][0] = 0.0f;
			m[2][1] = 0.0f;
			m[2][2] = 0.0f;
		}

		Matrix3(
		    float m00, float m01, float m02,
		    float m10, float m11, float m12,
		    float m20, float m21, float m22)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
		}

		void swap(Matrix3& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
		}

		float* operator [](size_t iRow)
		{
			assert(iRow < 3);
			return m[iRow];
		}

		const float* operator [](size_t iRow) const
		{
			assert(iRow < 3);
			return m[iRow];
		}

		bool operator== (const Matrix3& rkMatrix) const
		{
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
				{
					if (m[iRow][iCol] != rkMatrix.m[iRow][iCol])
						return false;
				}
			}

			return true;
		}

		Matrix3 operator+ (const Matrix3& rkMatrix) const
		{
			Matrix3 kSum;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
				{
					kSum.m[iRow][iCol] = m[iRow][iCol] + rkMatrix.m[iRow][iCol];
				}
			}
			return kSum;
		}

		Matrix3 operator- (const Matrix3& rkMatrix) const
		{
			Matrix3 kDiff;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
				{
					kDiff.m[iRow][iCol] = m[iRow][iCol] - rkMatrix.m[iRow][iCol];
				}
			}
			return kDiff;
		}

		Matrix3 operator* (const Matrix3& rkMatrix) const
		{
			Matrix3 kProd;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
				{
					kProd.m[iRow][iCol] =
					    m[iRow][0] * rkMatrix.m[0][iCol] +
					    m[iRow][1] * rkMatrix.m[1][iCol] +
					    m[iRow][2] * rkMatrix.m[2][iCol];
				}
			}
			return kProd;
		}

		Matrix3 operator- () const
		{
			Matrix3 kNeg;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
					kNeg[iRow][iCol] = -m[iRow][iCol];
			}
			return kNeg;
		}

		Matrix3 operator* (float fScalar) const
		{
			Matrix3 kProd;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
					kProd[iRow][iCol] = fScalar * m[iRow][iCol];
			}
			return kProd;
		}

		Vector3 operator * (const Vector3& rkVector) const
		{
			return Vector3((m[0][0] * rkVector.x + m[0][1] * rkVector.y + m[0][2] * rkVector.z),
				(m[1][0] * rkVector.x + m[1][1] * rkVector.y + m[1][2] * rkVector.z), 
				(m[2][0] * rkVector.x + m[2][1] * rkVector.y + m[2][2] * rkVector.z));
		}

		Matrix3 Transpose() const
		{
			Matrix3 kTranspose;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
					kTranspose[iRow][iCol] = m[iCol][iRow];
			}
			return kTranspose;
		}

		bool Inverse(Matrix3& rkInverse, float fTolerance) const
		{
			rkInverse[0][0] = m[1][1] * m[2][2] -
			                  m[1][2] * m[2][1];
			rkInverse[0][1] = m[0][2] * m[2][1] -
			                  m[0][1] * m[2][2];
			rkInverse[0][2] = m[0][1] * m[1][2] -
			                  m[0][2] * m[1][1];
			rkInverse[1][0] = m[1][2] * m[2][0] -
			                  m[1][0] * m[2][2];
			rkInverse[1][1] = m[0][0] * m[2][2] -
			                  m[0][2] * m[2][0];
			rkInverse[1][2] = m[0][2] * m[1][0] -
			                  m[0][0] * m[1][2];
			rkInverse[2][0] = m[1][0] * m[2][1] -
			                  m[1][1] * m[2][0];
			rkInverse[2][1] = m[0][1] * m[2][0] -
			                  m[0][0] * m[2][1];
			rkInverse[2][2] = m[0][0] * m[1][1] -
			                  m[0][1] * m[1][0];

			float fDet =
			    m[0][0] * rkInverse[0][0] +
			    m[0][1] * rkInverse[1][0] +
			    m[0][2] * rkInverse[2][0];

			if (std::abs(fDet) <= fTolerance)
				return false;

			float fInvDet = 1.0f / fDet;
			for (size_t iRow = 0; iRow < 3; iRow++)
			{
				for (size_t iCol = 0; iCol < 3; iCol++)
					rkInverse[iRow][iCol] *= fInvDet;
			}

			return true;
		}

		Matrix3 Inverse(float fTolerance) const
		{
			Matrix3 kInverse = Matrix3();
			Inverse(kInverse, fTolerance);
			return kInverse;
		}

		float Determinant() const
		{
			float fCofactor00 = m[1][1] * m[2][2] -
			                    m[1][2] * m[2][1];
			float fCofactor10 = m[1][2] * m[2][0] -
			                    m[1][0] * m[2][2];
			float fCofactor20 = m[1][0] * m[2][1] -
			                    m[1][1] * m[2][0];

			float fDet =
			    m[0][0] * fCofactor00 +
			    m[0][1] * fCofactor10 +
			    m[0][2] * fCofactor20;

			return fDet;
		}

		void QDUDecomposition(ape::Matrix3& kQ, ape::Vector3& kD, ape::Vector3& kU) const
		{
			float fInvLength = 1.0f / std::sqrt(m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0]);

			kQ[0][0] = m[0][0] * fInvLength;
			kQ[1][0] = m[1][0] * fInvLength;
			kQ[2][0] = m[2][0] * fInvLength;

			float fDot = kQ[0][0] * m[0][1] + kQ[1][0] * m[1][1] + kQ[2][0] * m[2][1];
			kQ[0][1] = m[0][1] - fDot * kQ[0][0];
			kQ[1][1] = m[1][1] - fDot * kQ[1][0];
			kQ[2][1] = m[2][1] - fDot * kQ[2][0];
			fInvLength = 1.0f / std::sqrt(kQ[0][1] * kQ[0][1] + kQ[1][1] * kQ[1][1] + kQ[2][1] * kQ[2][1]);

			kQ[0][1] *= fInvLength;
			kQ[1][1] *= fInvLength;
			kQ[2][1] *= fInvLength;

			fDot = kQ[0][0] * m[0][2] + kQ[1][0] * m[1][2] + kQ[2][0] * m[2][2];
			kQ[0][2] = m[0][2] - fDot * kQ[0][0];
			kQ[1][2] = m[1][2] - fDot * kQ[1][0];
			kQ[2][2] = m[2][2] - fDot * kQ[2][0];
			fDot = kQ[0][1] * m[0][2] + kQ[1][1] * m[1][2] + kQ[2][1] * m[2][2];
			kQ[0][2] -= fDot * kQ[0][1];
			kQ[1][2] -= fDot * kQ[1][1];
			kQ[2][2] -= fDot * kQ[2][1];
			fInvLength = 1.0f / std::sqrt(kQ[0][2] * kQ[0][2] + kQ[1][2] * kQ[1][2] + kQ[2][2] * kQ[2][2]);

			kQ[0][2] *= fInvLength;
			kQ[1][2] *= fInvLength;
			kQ[2][2] *= fInvLength;

			float fDet = kQ[0][0] * kQ[1][1] * kQ[2][2] + kQ[0][1] * kQ[1][2] * kQ[2][0] +
			             kQ[0][2] * kQ[1][0] * kQ[2][1] - kQ[0][2] * kQ[1][1] * kQ[2][0] -
			             kQ[0][1] * kQ[1][0] * kQ[2][2] - kQ[0][0] * kQ[1][2] * kQ[2][1];

			if (fDet < 0.0)
			{
				for (size_t iRow = 0; iRow < 3; iRow++)
					for (size_t iCol = 0; iCol < 3; iCol++)
						kQ[iRow][iCol] = -kQ[iRow][iCol];
			}

			ape::Matrix3 kR;
			kR[0][0] = kQ[0][0] * m[0][0] + kQ[1][0] * m[1][0] + kQ[2][0] * m[2][0];
			kR[0][1] = kQ[0][0] * m[0][1] + kQ[1][0] * m[1][1] + kQ[2][0] * m[2][1];
			kR[1][1] = kQ[0][1] * m[0][1] + kQ[1][1] * m[1][1] + kQ[2][1] * m[2][1];
			kR[0][2] = kQ[0][0] * m[0][2] + kQ[1][0] * m[1][2] + kQ[2][0] * m[2][2];
			kR[1][2] = kQ[0][1] * m[0][2] + kQ[1][1] * m[1][2] + kQ[2][1] * m[2][2];
			kR[2][2] = kQ[0][2] * m[0][2] + kQ[1][2] * m[1][2] + kQ[2][2] * m[2][2];

			kD.x = kR[0][0];
			kD.y = kR[1][1];
			kD.z = kR[2][2];

			float fInvD0 = 1.0f / kD.x;
			kU.x = kR[0][1] * fInvD0;
			kU.y = kR[0][2] * fInvD0;
			kU.z = kR[1][2] / kD.y;
		}

		std::string toString() const
		{
			std::ostringstream buff;
			buff << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << std::endl
				 << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << std::endl
				 << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << std::endl;
			return buff.str();
		}

		std::string toJsonString() const
		{
			std::ostringstream buff;
			buff << "[ ";
			buff << "[ " << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << " ], ";
			buff << "[ " << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << " ], ";
			buff << "[ " << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << " ]";
			buff << " ]";
			return buff.str();
		}

		std::vector<float> toVector() const
		{
			std::vector<float> vec{ m[0][0], m[0][1], m[0][2],
									m[1][0], m[1][1], m[1][2],
									m[2][0], m[2][1], m[2][2] };
			return vec;
		}

	};
}

#endif
