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
#ifndef PROCEDURAL_MATERIAL_GENERATOR_HELPER_INCLUDED
#define PROCEDURAL_MATERIAL_GENERATOR_HELPER_INCLUDED

#include "ProceduralPlatform.h"
#include <Ogre.h>

namespace Procedural
{

class _ProceduralExport NoiseBase
{
public:
	virtual Ogre::Real function1D(size_t x) = 0;
	virtual Ogre::Real function2D(size_t x, size_t y) = 0;

	virtual Ogre::uchar* field1D(size_t wx)
	{
		Ogre::uchar* retval = new Ogre::uchar[wx];
		double mi = 9999999999.9;
		double ma = -999999999.9;
		Ogre::Real* field = new Ogre::Real[wx];
		for (size_t x = 0; x < wx; ++x)
		{
			Ogre::Real val = function1D(x);
			if (val < mi) mi = val;
			if (val > ma) ma = val;
			field[x] = val;
		}
		for (size_t x = 0; x < wx; ++x)
		{
			retval[x] = (Ogre::uchar)((255.0/(ma - mi))*(field[x] - mi));
		}
		delete field;
		return retval;
	}

	virtual Ogre::uchar* field2D(size_t wx, size_t wy)
	{
		Ogre::uchar* retval = new Ogre::uchar[wx * wy];
		double mi = 9999999999.9;
		double ma = -999999999.9;
		Ogre::Real* field = new Ogre::Real[wy * wx];
		for (size_t y = 0; y < wy; ++y)
		{
			for (size_t x = 0; x < wx; ++x)
			{
				Ogre::Real val = function2D(x, y);
				if (val < mi) mi = val;
				if (val > ma) ma = val;
				field[y * wx + x] = val;
			}
		}
		for (size_t y = 0; y < wy; ++y)
		{
			for (size_t x = 0; x < wx; ++x)
			{
				retval[y * wx + x] = (Ogre::uchar)((255.0/(ma - mi))*(field[y * wx + x] - mi));
			}
		}
		delete field;
		return retval;
	}
};

typedef NoiseBase* NoiseBasePtr;

class _ProceduralExport WhiteNoise : public NoiseBase
{
public:
	WhiteNoise(Ogre::uint seed = 5120)
	{
		srand(seed);
	}

	virtual Ogre::Real function1D(size_t x)
	{
		return ((Ogre::Real)rand() / RAND_MAX);
	}
	virtual Ogre::Real function2D(size_t x, size_t y)
	{
		return function1D(x * y);
	}
};

class _ProceduralExport PerlinNoise : public NoiseBase
{
private:
	Ogre::Real mFrequency;
	Ogre::Real mAmplitude;
	Ogre::Real mPersistance;
	Ogre::uint mOctaves;

public:
	PerlinNoise(Ogre::uint octaves = 4, Ogre::Real persistence = 0.65f, Ogre::Real frequency = 1.0f, Ogre::Real amplitude = 1.0f);

	void setFrequency(Ogre::Real initFrequency);
	void setAmplitude(Ogre::Real initAmplitude);
	void setPersistence(Ogre::Real persistence);
	void setOctaves(Ogre::uint octaves);

	virtual Ogre::Real function1D(size_t x);
	virtual Ogre::Real function2D(size_t x, size_t y);

private:
	double noise(double x);
	double noise(double x, double y);
	double smoothedNoise(double x);
	double smoothedNoise(double x, double y);
	double interpolate(double x1, double x2, double a);
};
}
#endif
