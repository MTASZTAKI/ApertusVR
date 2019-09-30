/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

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
#include "ProceduralStableHeaders.h"
#include "ProceduralNoise.h"

namespace Procedural
{

PerlinNoise::PerlinNoise(Ogre::uint octaves, Ogre::Real persistence, Ogre::Real frequency, Ogre::Real amplitude)
	: mFrequency(frequency), mAmplitude(amplitude), mPersistance(persistence), mOctaves(octaves)
{
	if (mOctaves < 1) mOctaves = 1;
	if (mOctaves > 32) mOctaves = 32;
}

void PerlinNoise::setFrequency(Ogre::Real frequency)
{
	mFrequency = frequency;
}

void PerlinNoise::setAmplitude(Ogre::Real amplitude)
{
	mAmplitude = amplitude;
}

void PerlinNoise::setPersistence(Ogre::Real persistence)
{
	mPersistance = persistence;
}

void PerlinNoise::setOctaves(Ogre::uint octaves)
{
	mOctaves = octaves;
	if (mOctaves < 1) mOctaves = 1;
	if (mOctaves > 32) mOctaves = 32;
}

Ogre::Real PerlinNoise::function1D(size_t x)
{
	double freq = mFrequency;
	double amp = mAmplitude;
	double sum = 0.0;

	for (size_t i = 0; i < mOctaves; i++)
	{
		sum += smoothedNoise((double)x * freq) * amp;

		amp *= mPersistance;
		freq *= 2;
	}

	return (Ogre::Real)sum;
}

Ogre::Real PerlinNoise::function2D(size_t x, size_t y)
{
	double freq = mFrequency;
	double amp = mAmplitude;
	double sum = 0.0;

	for (size_t i = 0; i < mOctaves; i++)
	{
		sum += smoothedNoise((double)x * freq, (double)y * freq) * amp;

		amp *= mPersistance;
		freq *= 2;
	}

	return (Ogre::Real)sum;
}

double PerlinNoise::noise(double x)
{
	int n = ((int)x << 13) ^ (int)x;
	return 1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

double PerlinNoise::noise(double x, double y)
{
	int n = (int)(x + y * 57.0);
	n = (n << 13) ^ n;
	return 1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

double PerlinNoise::smoothedNoise(double x)
{
	int XInt = (int)x;
	double XFrac = x - (double)XInt;

	return interpolate(noise(XInt), noise(XInt + 1), XFrac);
}

double PerlinNoise::smoothedNoise(double x, double y)
{
	int XInt = (int)x;
	int YInt = (int)y;
	double XFrac = x - XInt;
	double YFrac = y - YInt;

	double n00 = noise(XInt    , YInt    );
	double n10 = noise(XInt + 1, YInt    );
	double n01 = noise(XInt    , YInt + 1);
	double n11 = noise(XInt + 1, YInt + 1);

	double i1 = interpolate(n00, n10, XFrac);
	double i2 = interpolate(n01, n11, XFrac);

	return interpolate(i1, i2, YFrac);
}

double PerlinNoise::interpolate(double x1, double x2, double a)
{
	double f = (1 - Ogre::Math::Cos((Ogre::Real)a * Ogre::Math::PI)) * 0.5;
	return x1 * (1 - f) + x2 * f;
}
}
