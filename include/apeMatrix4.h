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

#ifndef APE_MATRIX4_H
#define APE_MATRIX4_H

#include <assert.h>
#include <cmath>
#include <sstream>
#include <vector>
#include "apeDegree.h"
#include "apeMatrix3.h"
#include "apeQuaternion.h"
#include "apeRadian.h"
#include "apeVector3.h"
#include "apeVector4.h"

namespace ape
{
	struct Matrix4
	{
		float m[4][4];

		Matrix4()
		{
			m[0][0] = 0.0f;
			m[0][1] = 0.0f;
			m[0][2] = 0.0f;
			m[0][3] = 0.0f;
			m[1][0] = 0.0f;
			m[1][1] = 0.0f;
			m[1][2] = 0.0f;
			m[1][3] = 0.0f;
			m[2][0] = 0.0f;
			m[2][1] = 0.0f;
			m[2][2] = 0.0f;
			m[2][3] = 0.0f;
			m[3][0] = 0.0f;
			m[3][1] = 0.0f;
			m[3][2] = 0.0f;
			m[3][3] = 0.0f;
		}

		Matrix4(
		    float m00, float m01, float m02, float m03,
		    float m10, float m11, float m12, float m13,
		    float m20, float m21, float m22, float m23,
		    float m30, float m31, float m32, float m33)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[0][3] = m03;
			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[1][3] = m13;
			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
			m[2][3] = m23;
			m[3][0] = m30;
			m[3][1] = m31;
			m[3][2] = m32;
			m[3][3] = m33;
		}

		Matrix4(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
		{
			makeTransform(scale,rotation,translation);
		}

		void swap(Matrix4& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[0][3], other.m[0][3]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[1][3], other.m[1][3]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
			std::swap(m[2][3], other.m[2][3]);
			std::swap(m[3][0], other.m[3][0]);
			std::swap(m[3][1], other.m[3][1]);
			std::swap(m[3][2], other.m[3][2]);
			std::swap(m[3][3], other.m[3][3]);
		}

		float* operator [](size_t iRow)
		{
			assert(iRow < 4);
			return m[iRow];
		}

		const float* operator [](size_t iRow) const
		{
			assert(iRow < 4);
			return m[iRow];
		}

		Matrix4 concatenate(const Matrix4& m2) const
		{
			Matrix4 r;
			r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
			r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
			r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
			r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

			r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
			r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
			r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
			r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

			r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
			r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
			r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
			r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

			r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
			r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
			r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
			r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

			return r;
		}

		Matrix4 operator * (const Matrix4& m2) const
		{
			return concatenate(m2);
		}

		ape::Vector3 operator * (const ape::Vector3& v) const
		{
			ape::Vector3 r;

			float fInvW = 1.0f / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

			r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
			r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
			r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

			return r;
		}

		ape::Vector4 operator * (const ape::Vector4& v) const
		{
			return ape::Vector4(
			           m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
			           m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			           m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			           m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
			       );
		}

		Matrix4 operator + (const Matrix4& m2) const
		{
			Matrix4 r;

			r.m[0][0] = m[0][0] + m2.m[0][0];
			r.m[0][1] = m[0][1] + m2.m[0][1];
			r.m[0][2] = m[0][2] + m2.m[0][2];
			r.m[0][3] = m[0][3] + m2.m[0][3];

			r.m[1][0] = m[1][0] + m2.m[1][0];
			r.m[1][1] = m[1][1] + m2.m[1][1];
			r.m[1][2] = m[1][2] + m2.m[1][2];
			r.m[1][3] = m[1][3] + m2.m[1][3];

			r.m[2][0] = m[2][0] + m2.m[2][0];
			r.m[2][1] = m[2][1] + m2.m[2][1];
			r.m[2][2] = m[2][2] + m2.m[2][2];
			r.m[2][3] = m[2][3] + m2.m[2][3];

			r.m[3][0] = m[3][0] + m2.m[3][0];
			r.m[3][1] = m[3][1] + m2.m[3][1];
			r.m[3][2] = m[3][2] + m2.m[3][2];
			r.m[3][3] = m[3][3] + m2.m[3][3];

			return r;
		}

		Matrix4 operator - (const Matrix4& m2) const
		{
			Matrix4 r;
			r.m[0][0] = m[0][0] - m2.m[0][0];
			r.m[0][1] = m[0][1] - m2.m[0][1];
			r.m[0][2] = m[0][2] - m2.m[0][2];
			r.m[0][3] = m[0][3] - m2.m[0][3];

			r.m[1][0] = m[1][0] - m2.m[1][0];
			r.m[1][1] = m[1][1] - m2.m[1][1];
			r.m[1][2] = m[1][2] - m2.m[1][2];
			r.m[1][3] = m[1][3] - m2.m[1][3];

			r.m[2][0] = m[2][0] - m2.m[2][0];
			r.m[2][1] = m[2][1] - m2.m[2][1];
			r.m[2][2] = m[2][2] - m2.m[2][2];
			r.m[2][3] = m[2][3] - m2.m[2][3];

			r.m[3][0] = m[3][0] - m2.m[3][0];
			r.m[3][1] = m[3][1] - m2.m[3][1];
			r.m[3][2] = m[3][2] - m2.m[3][2];
			r.m[3][3] = m[3][3] - m2.m[3][3];

			return r;
		}

		bool operator == (const Matrix4& m2) const
		{
			if (
			    m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			    m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			    m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			    m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
				return false;
			return true;
		}

		bool operator != (const Matrix4& m2) const
		{
			if (
			    m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			    m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			    m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			    m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
				return true;
			return false;
		}

		Matrix4 transpose(void) const
		{
			return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
			               m[0][1], m[1][1], m[2][1], m[3][1],
			               m[0][2], m[1][2], m[2][2], m[3][2],
			               m[0][3], m[1][3], m[2][3], m[3][3]);
		}

		void makeTransform(const ape::Vector3& scale, const ape::Quaternion& rotation, const ape::Vector3& translate)
		{
			ape::Matrix3 rot3x3;
			rotation.ToRotationMatrix(rot3x3);

			m[0][0] = scale.x * rot3x3[0][0]; m[0][1] = scale.y * rot3x3[0][1]; m[0][2] = scale.z * rot3x3[0][2]; m[0][3] = translate.x;
			m[1][0] = scale.x * rot3x3[1][0]; m[1][1] = scale.y * rot3x3[1][1]; m[1][2] = scale.z * rot3x3[1][2]; m[1][3] = translate.y;
			m[2][0] = scale.x * rot3x3[2][0]; m[2][1] = scale.y * rot3x3[2][1]; m[2][2] = scale.z * rot3x3[2][2]; m[2][3] = translate.z;

			m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
		}

		void extract3x3Matrix(ape::Matrix3& m3x3) const
		{
			m3x3.m[0][0] = m[0][0];
			m3x3.m[0][1] = m[0][1];
			m3x3.m[0][2] = m[0][2];
			m3x3.m[1][0] = m[1][0];
			m3x3.m[1][1] = m[1][1];
			m3x3.m[1][2] = m[1][2];
			m3x3.m[2][0] = m[2][0];
			m3x3.m[2][1] = m[2][1];
			m3x3.m[2][2] = m[2][2];
		}

		void decomposition(ape::Vector3& scale, ape::Quaternion& rotation, ape::Vector3& translate)
		{
			ape::Matrix3 m3x3;
			extract3x3Matrix(m3x3);

			ape::Matrix3 matQ;
			ape::Vector3 vecU;
			m3x3.QDUDecomposition(matQ, scale, vecU);

			rotation = ape::Quaternion(matQ);
			translate = ape::Vector3(m[0][3], m[1][3], m[2][3]);
		}

		ape::Matrix4 inverse() const
		{
			float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
			float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
			float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
			float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

			float v0 = m20 * m31 - m21 * m30;
			float v1 = m20 * m32 - m22 * m30;
			float v2 = m20 * m33 - m23 * m30;
			float v3 = m21 * m32 - m22 * m31;
			float v4 = m21 * m33 - m23 * m31;
			float v5 = m22 * m33 - m23 * m32;

			float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
			float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
			float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
			float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

			float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

			float d00 = t00 * invDet;
			float d10 = t10 * invDet;
			float d20 = t20 * invDet;
			float d30 = t30 * invDet;

			float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
			float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
			float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
			float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

			v0 = m10 * m31 - m11 * m30;
			v1 = m10 * m32 - m12 * m30;
			v2 = m10 * m33 - m13 * m30;
			v3 = m11 * m32 - m12 * m31;
			v4 = m11 * m33 - m13 * m31;
			v5 = m12 * m33 - m13 * m32;

			float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
			float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
			float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
			float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

			v0 = m21 * m10 - m20 * m11;
			v1 = m22 * m10 - m20 * m12;
			v2 = m23 * m10 - m20 * m13;
			v3 = m22 * m11 - m21 * m12;
			v4 = m23 * m11 - m21 * m13;
			v5 = m23 * m12 - m22 * m13;

			float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
			float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
			float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
			float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

			return Matrix4(
				d00, d01, d02, d03,
				d10, d11, d12, d13,
				d20, d21, d22, d23,
				d30, d31, d32, d33);
		}

		std::string toString() const
		{
			std::ostringstream buff;
			buff << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << std::endl
			     << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << std::endl
			     << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << std::endl
			     << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << std::endl;
			return buff.str();
		}

		std::string toJsonString() const
		{
			std::ostringstream buff;
			buff << "[ ";
			buff << "[ " << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << " ], ";
			buff << "[ " << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << " ], ";
			buff << "[ " << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << " ], ";
			buff << "[ " << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << " ]";
			buff << " ]";
			return buff.str();
		}

		std::vector<float> toVector() const
		{
			std::vector<float> vec{ m[0][0], m[0][1], m[0][2], m[0][3],
									m[1][0], m[1][1], m[1][2], m[1][3],
									m[2][0], m[2][1], m[2][2], m[2][3],
									m[3][0], m[3][1], m[3][2], m[3][3] };
			return vec;
		}
	};

	static const Matrix4 MATRIX4IDENTITY
	(
	    1.0f, 0.0f, 0.0f, 0.0f,
	    0.0f, 1.0f, 0.0f, 0.0f,
	    0.0f, 0.0f, 1.0f, 0.0f,
	    0.0f, 0.0f, 0.0f, 1.0f
	);
}

#endif
