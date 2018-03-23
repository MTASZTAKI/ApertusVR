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

#ifndef APE_EULER_H
#define APE_EULER_H

#include <cmath>
#include <iostream>
#include "ApeQuaternion.h"

namespace Ape
{
	struct Euler
	{
		Ape::Radian m_yaw;
		
		Ape::Radian m_pitch; 
		
		Ape::Radian m_roll;  
		
		Ape::Quaternion m_cachedQuaternion; 
		
		bool m_changed; 

		Euler(Ape::Radian y = Ape::Radian(0.0f), Ape::Radian p = Ape::Radian(0.0f), Ape::Radian r = Ape::Radian(0.0f)):m_yaw(y),m_pitch(p),m_roll(r),m_changed(true) 
		{
		}

		Euler(Ape::Quaternion q)
		{
			m_yaw = std::atan2(2.0 * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
			m_pitch = std::asin(-2.0 * (q.x * q.z - q.w * q.y));
			m_roll = std::atan2(2.0 * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
		}

		inline Ape::Radian getYaw() 
		{
			return m_yaw;
		}

		inline Ape::Radian getPitch() 
		{
			return m_pitch;
		}

		inline Ape::Radian getRoll() 
		{
			return m_roll;
		}

		inline Euler &setYaw(Ape::Radian y) 
		{
			m_yaw = y; 
			m_changed = true; 
			return *this;
		}

		inline Euler &setPitch(Ape::Radian p) 
		{
			m_pitch = p; 
			m_changed = true; 
			return *this;
		}

		inline Euler &setRoll(Ape::Radian r) 
		{
			m_roll = r; 
			m_changed = true; 
			return *this;
		}

		inline Ape::Quaternion toQuaternion() 
		{
			if(m_changed) 
			{
				double c1 = std::cos(m_yaw.radian / 2.0f);
				double s1 = std::sin(m_yaw.radian / 2.0f);
				double c2 = std::cos(m_pitch.radian / 2.0f);
				double s2 = std::sin(m_pitch.radian / 2.0f);
				double c3 = std::cos(m_roll.radian / 2.0f);
				double s3 = std::sin(m_roll.radian / 2.0f);
				double c1c2 = c1 * c2;
				double s1s2 = s1 * s2;
				double w = c1c2 * c3 - s1s2 * s3;
				double x = c1c2 * s3 + s1s2 * c3;
				double y = s1 * c2 * c3 + c1 * s2 * s3;
				double z = c1 * s2 * c3 - s1 * c2 * s3;
				m_cachedQuaternion = Ape::Quaternion((float) w, (float) y, (float) z, (float) x);
				m_changed = false;
			}
			return m_cachedQuaternion;
		}

		inline operator Ape::Quaternion()
		{
			return toQuaternion();
		}

		inline friend std::ostream &operator<<(std::ostream &o, const Euler &e)
		{
			o << "<Y:" << e.m_yaw.toDegree() << "�, P:" << e.m_pitch.toDegree() << "�, R:" << e.m_roll.toDegree() << "�>";
			return o;
		}       
	};
}

#endif
