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
#include "ProceduralTextureGenerator.h"
#include "ProceduralUtils.h"
#include "ProceduralNoise.h"

using namespace Ogre;

namespace Procedural
{

Cell& Cell::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Cell& Cell::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Cell& Cell::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

Cell& Cell::setRegularity(Ogre::uchar regularity)
{
	mRegularity = regularity;
	return *this;
}

Cell& Cell::setDensity(Ogre::uint density)
{
	mDensity = density;
	if (mDensity == 0) mDensity = 1;
	return *this;
}

Cell& Cell::setMode(Cell::CELL_MODE mode)
{
	mMode = mode;
	return *this;
}

Cell& Cell::setPattern(Cell::CELL_PATTERN pattern)
{
	mPattern = pattern;
	return *this;
}

TextureBufferPtr Cell::process()
{
	bool cfc;
	Ogre::Real coeff;

	srand(mSeed);
	const Ogre::Real regularity = mRegularity / 255.0f;
	Ogre::Vector3* cellPoints = (Ogre::Vector3*)malloc(sizeof(Ogre::Vector3) * mDensity * mDensity);

	for (size_t y = 0; y < mDensity; ++y)
	{
		for (size_t x = 0; x < mDensity; ++x)
		{
			Ogre::Real rand1 = (Ogre::Real)rand() / 65536.0f;
			Ogre::Real rand2 = (Ogre::Real)rand() / 65536.0f;
			cellPoints[x + y * mDensity].x = (x + 0.5f + (rand1 - 0.5f) * (1 - regularity)) / mDensity - 1.f / mBuffer->getWidth();
			cellPoints[x + y * mDensity].y = (y + 0.5f + (rand2 - 0.5f) * (1 - regularity)) / mDensity - 1.f / mBuffer->getHeight();
			cellPoints[x + y * mDensity].z = 0;
		}
	}

	for (size_t y = 0; y < mBuffer->getHeight(); ++y)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); ++x)
		{
			Ogre::Vector3 pixelPos;
			pixelPos.x = (Ogre::Real)x / (Ogre::Real)mBuffer->getWidth(),
			         pixelPos.y = (Ogre::Real)y / (Ogre::Real)mBuffer->getHeight();
			pixelPos.z = 0.0f;

			Ogre::Real minDist = 10;
			Ogre::Real nextMinDist = minDist;
			int xo = x * mDensity / mBuffer->getWidth();
			int yo = y * mDensity / mBuffer->getHeight();
			for (long v = -1; v < 2; ++v)
			{
				int vo = ((yo + mDensity + v) % mDensity) * mDensity;
				for (long u = -1; u < 2; ++u)
				{
					Ogre::Vector3 cellPos = cellPoints[((xo + mDensity + u) % mDensity) + vo];
					if (u == -1 && x * mDensity < mBuffer->getWidth()) cellPos.x -= 1;
					if (v == -1 && y * mDensity < mBuffer->getHeight()) cellPos.y -= 1;
					if (u == 1 && x * mDensity >= mBuffer->getWidth() * (mDensity - 1)) cellPos.x += 1;
					if (v == 1 && y * mDensity >= mBuffer->getHeight() * (mDensity - 1)) cellPos.y += 1;

					Real norm = pixelPos.distance(cellPos);
					if (norm < minDist)
					{
						nextMinDist = minDist;
						minDist = norm;
					}
					else if (norm < nextMinDist)
					{
						nextMinDist = norm;
					}
				}
			}

			switch (mPattern)
			{
			default:
			case PATTERN_BOTH:
				minDist = (nextMinDist - minDist) * mDensity;
				break;

			case PATTERN_CROSS:
				minDist = 2 * nextMinDist * mDensity - 1;
				break;

			case PATTERN_CONE:
				minDist = 1 - minDist * mDensity;
				break;
			}

			if (minDist < 0) minDist = 0;
			if (minDist > 1) minDist = 1;

			switch (mMode)
			{
			case MODE_CHESSBOARD:
				cfc = ((xo & 1) ^ (yo & 1)) != 0;
				coeff = (1 - 2 * cfc) / 2.5f;
				mBuffer->setRed(x, y, (Ogre::uchar)((cfc + coeff * minDist) * mColour.r * 255.0f));
				mBuffer->setGreen(x, y, (Ogre::uchar)((cfc + coeff * minDist) * mColour.g * 255.0f));
				mBuffer->setBlue(x, y, (Ogre::uchar)((cfc + coeff * minDist) * mColour.b * 255.0f));
				break;

			default:
			case MODE_GRID:
				mBuffer->setRed(x, y, (Ogre::uchar)(minDist * mColour.r * 255.0f));
				mBuffer->setGreen(x, y, (Ogre::uchar)(minDist * mColour.g * 255.0f));
				mBuffer->setBlue(x, y, (Ogre::uchar)(minDist * mColour.b * 255.0f));
				break;
			}
			mBuffer->setAlpha(x, y, mColour.a);
		}
	}

	logMsg("Create cell texture : " + StringConverter::toString(mDensity) + "x" + StringConverter::toString(mDensity));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Cloud& Cloud::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Cloud& Cloud::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Cloud& Cloud::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

TextureBufferPtr Cloud::process()
{
	srand(mSeed);
	int r = rand();
	PerlinNoise noise(8, 0.5, 1.0 / 32.0, 1.0);
	Ogre::Real filterLevel = 0.7f;
	Ogre::Real preserveLevel = 0.3f;

	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			Ogre::Real noiseVal = std::max(0.0f, std::min(1.0f, noise.function2D(x + r, y + r) * 0.5f + 0.5f));
			mBuffer->setRed(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.r * 255.0f + filterLevel * mColour.r * 255.0f * noiseVal, 255.0f));
			mBuffer->setGreen(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.g * 255.0f + filterLevel * mColour.g * 255.0f * noiseVal, 255.0f));
			mBuffer->setBlue(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.b * 255.0f + filterLevel * mColour.b * 255.0f * noiseVal, 255.0f));
			mBuffer->setAlpha(x, y, mColour.a);
		}
	}

	logMsg("Create cloud texture");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Gradient& Gradient::setColourA(Ogre::ColourValue colour)
{
	mColourA = colour;
	return *this;
}

Gradient& Gradient::setColourA(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourA = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Gradient& Gradient::setColourB(Ogre::ColourValue colour)
{
	mColourB = colour;
	return *this;
}

Gradient& Gradient::setColourB(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourB = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Gradient& Gradient::setColourC(Ogre::ColourValue colour)
{
	mColourC = colour;
	return *this;
}

Gradient& Gradient::setColourC(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourC = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Gradient& Gradient::setColourD(Ogre::ColourValue colour)
{
	mColourD = colour;
	return *this;
}

Gradient& Gradient::setColourD(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourD = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Gradient& Gradient::setColours(Ogre::ColourValue colourA, Ogre::ColourValue colourB, Ogre::ColourValue colourC, Ogre::ColourValue colourD)
{
	mColourA = colourA;
	mColourB = colourB;
	mColourC = colourC;
	mColourD = colourD;
	return *this;
}

TextureBufferPtr Gradient::process()
{
	float finv_WH = 1.0f / (float)(mBuffer->getWidth() * mBuffer->getHeight());
	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			float a = (float)((mBuffer->getWidth() - x) * (mBuffer->getHeight() - y)) * finv_WH;
			float b = (float)((                  x) * (mBuffer->getHeight() - y)) * finv_WH;
			float c = (float)((mBuffer->getWidth() - x) * (                   y)) * finv_WH;
			float d = (float)((                  x) * (                   y)) * finv_WH;

			mBuffer->setRed(x, y, (Ogre::uchar)(((mColourA.r * a) + (mColourB.r * b) + (mColourC.r * c) + (mColourD.r * d)) * 255.0f));
			mBuffer->setGreen(x, y, (Ogre::uchar)(((mColourA.g * a) + (mColourB.g * b) + (mColourC.g * c) + (mColourD.g * d)) * 255.0f));
			mBuffer->setBlue(x, y, (Ogre::uchar)(((mColourA.b * a) + (mColourB.b * b) + (mColourC.b * c) + (mColourD.b * d)) * 255.0f));
			mBuffer->setAlpha(x, y, (Ogre::uchar)(((mColourA.a * a) + (mColourB.a * b) + (mColourC.a * c) + (mColourD.a * d)) * 255.0f));
		}
	}

	logMsg("Create gradient texture");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Image& Image::setFile(Ogre::String filename, Ogre::String groupname)
{
	mFile = filename;
	mGroup = groupname;
	return *this;
}

TextureBufferPtr Image::process()
{
	Ogre::Image img;
	img.load(mFile, mGroup);
	if (img.getHeight() < mBuffer->getHeight() || img.getWidth() < mBuffer->getWidth()) return mBuffer;

	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			mBuffer->setPixel(x, y, img.getColourAt(x, y, 0));
		}
	}

	logMsg("Create texture from image");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Labyrinth& Labyrinth::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Labyrinth& Labyrinth::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Labyrinth& Labyrinth::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

TextureBufferPtr Labyrinth::process()
{
	srand(mSeed);
	int r = rand();
	PerlinNoise noise(1, 0.65f, 1.0f / 16.0f, 1.0f);
	Ogre::Real filterLevel = 0.7f;
	Ogre::Real preserveLevel = 0.3f;

	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			Ogre::Real noiseVal = std::min(1.0f, std::abs(noise.function2D(x + r, y + r)));
			mBuffer->setRed(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.r * 255.0f + filterLevel * mColour.r * 255.0f * noiseVal, 255.0f));
			mBuffer->setGreen(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.g * 255.0f + filterLevel * mColour.g * 255.0f * noiseVal, 255.0f));
			mBuffer->setBlue(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.b * 255.0f + filterLevel * mColour.b * 255.0f * noiseVal, 255.0f));
			mBuffer->setAlpha(x, y, mColour.a);
		}
	}

	logMsg("Create labyrinth texture");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Marble& Marble::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Marble& Marble::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Marble& Marble::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

TextureBuffer* Marble::process()
{
	srand(mSeed);
	int r = rand();
	PerlinNoise noise(2, 0.65f, 1.0f / 32.0f, 1.0f);
	Ogre::Real xFact = 1.0f / 96.0f;
	Ogre::Real yFact = 1.0f / 48.0f;
	Ogre::Real filterLevel = 0.7f;
	Ogre::Real preserveLevel = 0.3f;

	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			Ogre::Real noiseVal = std::min(1.0f, Ogre::Math::Abs(Ogre::Math::Sin(x * xFact + y * yFact + noise.function2D(x + r, y + r)) * Ogre::Math::PI));
			mBuffer->setRed(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.r * 255.0f + filterLevel * mColour.r * 255.0f * noiseVal, 255.0f));
			mBuffer->setGreen(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.g * 255.0f + filterLevel * mColour.g * 255.0f * noiseVal, 255.0f));
			mBuffer->setBlue(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.b * 255.0f + filterLevel * mColour.b * 255.0f * noiseVal, 255.0f));
			mBuffer->setAlpha(x, y, mColour.a);
		}
	}

	logMsg("Create marble texture");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Noise& Noise::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Noise& Noise::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Noise& Noise::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

Noise& Noise::setType(NOISE_TYPE type)
{
	mType = type;
	return *this;
}

TextureBufferPtr Noise::process()
{
	NoiseBasePtr noiseGen;
	switch (mType)
	{
	case NOISE_PERLIN:
		noiseGen = new PerlinNoise();
		break;

	default:
	case NOISE_WHITE:
		noiseGen = new WhiteNoise(mSeed);
		break;
	}

	Ogre::uchar* field = noiseGen->field2D(mBuffer->getWidth(), mBuffer->getHeight());
	for (size_t y = 0; y < mBuffer->getHeight(); ++y)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); ++x)
		{
			Ogre::Real noiseVal = (Ogre::Real)field[y * mBuffer->getWidth() + x];
			mBuffer->setRed(x, y, (Ogre::uchar)(noiseVal * mColour.r));
			mBuffer->setGreen(x, y, (Ogre::uchar)(noiseVal * mColour.g));
			mBuffer->setBlue(x, y, (Ogre::uchar)(noiseVal * mColour.b));
			mBuffer->setAlpha(x, y, (Ogre::uchar)(mColour.a * 255.0f));
		}
	}

	delete field;
	delete noiseGen;
	logMsg("Create noise texture : " + StringConverter::toString(mType));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Solid& Solid::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Solid& Solid::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureBufferPtr Solid::process()
{
	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			mBuffer->setPixel(x, y, mColour);
		}
	}

	logMsg("Create solid colour texture : " + StringConverter::toString((int)(mColour.r * 255.0f)) + ", " + StringConverter::toString((int)(mColour.g * 255.0f)) + ", " + StringConverter::toString((int)(mColour.b * 255.0f)));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Textile& Textile::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Textile& Textile::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Textile& Textile::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

TextureBufferPtr Textile::process()
{
	srand(mSeed);
	int r = rand();
	PerlinNoise noise(3, 0.65f, 1.0f / 8.0f, 1.0f);
	Ogre::Real filterLevel = 0.7f;
	Ogre::Real preserveLevel = 0.3f;

	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			Ogre::Real noiseVal = std::max(0.0f, std::min(1.0f, (Ogre::Math::Sin(x + noise.function2D(x + r, y + r )) + Ogre::Math::Sin(y + noise.function2D(x + r, y + r))) * 0.25f + 0.5f));
			mBuffer->setRed(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.r * 255.0f + filterLevel * mColour.r * 255.0f * noiseVal, 255.0f));
			mBuffer->setGreen(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.g * 255.0f + filterLevel * mColour.g * 255.0f * noiseVal, 255.0f));
			mBuffer->setBlue(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.b * 255.0f + filterLevel * mColour.b * 255.0f * noiseVal, 255.0f));
			mBuffer->setAlpha(x, y, mColour.a);
		}
	}

	logMsg("Create textile texture");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Wood& Wood::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Wood& Wood::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Wood& Wood::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

Wood& Wood::setRings(Ogre::uint rings)
{
	mRings = rings;
	if (mRings < 3) mRings = 3;
	return *this;
}

TextureBufferPtr Wood::process()
{
	srand(mSeed);
	int r = rand();
	Ogre::Real filterLevel = 0.7f;
	Ogre::Real preserveLevel = 0.3f;

	PerlinNoise noise(8, 0.5f, 1.0f / 32.0f, 0.05f);
	long w2 = mBuffer->getWidth() / 2;
	long h2 = mBuffer->getHeight() / 2;

	for (long y = 0; y < (long)mBuffer->getHeight(); y++)
	{
		for (long x = 0; x < (long)mBuffer->getWidth(); x++)
		{
			Ogre::Real xv = ((Ogre::Real)(x - w2)) / (Ogre::Real)mBuffer->getWidth();
			Ogre::Real yv = ((Ogre::Real)(y - h2)) / (Ogre::Real)mBuffer->getHeight();
			Ogre::Real noiseVal = std::min(1.0f, Ogre::Math::Abs(Ogre::Math::Sin((sqrt(xv * xv + yv * yv) + noise.function2D(x + r, y + r)) * Ogre::Math::PI * 2 * mRings)));
			mBuffer->setRed(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.r * 255.0f + filterLevel * mColour.r * 255.0f * noiseVal, 255.0f));
			mBuffer->setGreen(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.g * 255.0f + filterLevel * mColour.g * 255.0f * noiseVal, 255.0f));
			mBuffer->setBlue(x, y, (Ogre::uchar)std::min<Ogre::Real>(preserveLevel * mColour.b * 255.0f + filterLevel * mColour.b * 255.0f * noiseVal, 255.0f));
			mBuffer->setAlpha(x, y, mColour.a);
		}
	}

	logMsg("Create wood texture : " + StringConverter::toString(mRings));
	return mBuffer;
}
}
