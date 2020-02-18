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

#ifndef APE_VECTOR4_H
#define APE_VECTOR4_H

#include <vector>

namespace ape
{
	struct Vector4
	{
		float x, y, z, w;

		Vector4() :
			x(0.0f), y(0.0f), z(0.0f), w(0.0f)
		{}

		Vector4(float _x, float _y, float _z, float _w) :
			x(_x), y(_y), z(_z), w(_w)
		{}

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

		float getW()
		{
			return w;
		}

		float length() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		std::string toString() const
		{
			std::ostringstream buff;
			buff << x << ", " << y << ", " << z << std::endl;
			return buff.str();
		}

		std::string toJsonString() const
		{
			std::ostringstream buff;
			buff << "{ ";
			buff << "\"x\": " << x << ", ";
			buff << "\"y\": " << y << ", ";
			buff << "\"z\": " << z << ", ";
			buff << "\"w\": " << w;
			buff << " }";
			return buff.str();
		}

		std::vector<float> toVector() const
		{
			std::vector<float> vec{ x, y, z, w};
			return vec;
		}
	};
}

#endif
