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
#include "ProceduralTextureModifiers.h"
#include "ProceduralTextureGenerator.h"
#ifdef PROCEDURAL_USE_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#endif // PROCEDURAL_USE_FREETYPE
#include "ProceduralGeometryHelpers.h"

using namespace Ogre;

namespace Procedural
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Abnormals& Abnormals::setParameterImage(TextureBufferPtr image)
{
	mParam = image;
	return *this;
}

Abnormals& Abnormals::setRotation(Ogre::Real rotation)
{
	mW = Ogre::Radian(rotation * Ogre::Math::TWO_PI);
	return *this;
}

Abnormals& Abnormals::setRotation(Ogre::Radian rotation)
{
	mW = rotation;
	return *this;
}

Abnormals& Abnormals::setRotation(Ogre::Degree rotation)
{
	mW = (Ogre::Radian)rotation;
	return *this;
}

Abnormals& Abnormals::setAxis(Ogre::Vector3 axis)
{
	mAxis = axis;
	return *this;
}

Abnormals& Abnormals::setAxis(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mAxis = Ogre::Vector3(x, y, z);
	return *this;
}

Abnormals& Abnormals::setSensitivity(Ogre::uchar sensitivity)
{
	mSensitivity = sensitivity;
	return *this;
}

Abnormals& Abnormals::setCompensation(Abnormals::ABNORMALS_COMPENSATION compensation)
{
	mCompensation = compensation;
	return *this;
}

Abnormals& Abnormals::setMirror(Abnormals::ABNORMALS_MIRROR mirror)
{
	mMirror = mirror;
	return *this;
}

TextureBufferPtr Abnormals::process()
{
	Ogre::Quaternion qion;
	Ogre::Real sum;
	Ogre::Vector3 q;

	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();
	Ogre::Quaternion rotation(mW, mAxis);

	if (mParam != NULL && (mParam->getWidth() < w || mParam->getHeight() < h)) return mBuffer;

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::ColourValue pixel = mBuffer->getPixel(x, y);
			Ogre::Quaternion v(0.0f, ((pixel.r * 255.0f) - 127.5f) / 127.5f, ((pixel.b * 255.0f) - 127.5f) / 127.5f, ((pixel.g * 255.0f) - 127.5f) / 127.5f);

			if (mParam != NULL)
			{
				pixel = mParam->getPixel(x, y);
				switch (mCompensation)
				{
				case COMPENSATION_NORMAL:
					qion = Ogre::Quaternion(0.0f, (pixel.r * 255.0f) - 127.5f, (pixel.b * 255.0f) - 127.5f, (pixel.g * 255.0f) - 127.5f);
					v = v * (Ogre::Real)(1 - mSensitivity);
					v = v + qion * ((Ogre::Real)mSensitivity / 127.5f);
					break;

				case COMPENSATION_HEIGHT:
					sum = ((pixel.r + pixel.g + pixel.b) / 3.0f) * 255.0f;
					qion = Ogre::Quaternion(Ogre::Radian(Ogre::Math::TWO_PI * sum / 765.f * mSensitivity), Ogre::Vector3(0.0f, 1.0f, 0.0f));
					rotation = rotation * qion;
					break;

				case COMPENSATION_QUATERNION:
					q = Ogre::Vector3((pixel.r * 255.0f) - 127.5f, (pixel.b * 255.0f) - 127.5f, (pixel.g * 255.0f) - 127.5f);
					qion = Ogre::Quaternion(Ogre::Radian(2.0f / 255.f * Ogre::Math::PI * pixel.a * mSensitivity), q);
					rotation = rotation * qion;
					break;
				}
			}

			v = rotation * v * rotation.Inverse();
			Ogre::Real norm = v.normalise();

			if (mMirror == MIRROR_X_YZ || mMirror == MIRROR_X_Y_Z)
				mBuffer->setRed(x, y, (1.0f - v.x * 0.5f + 0.5f));
			else
				mBuffer->setRed(x, y, (v.x * 0.5f + 0.5f));
			if (mMirror == MIRROR_Y_XZ || mMirror == MIRROR_X_Y_Z)
				mBuffer->setGreen(x, y, (1.0f - v.z * 0.5f + 0.5f));
			else
				mBuffer->setGreen(x, y, (v.z * 0.5f + 0.5f));
			mBuffer->setBlue(x, y, (v.y * 0.5f + 0.5f));
		}
	}

	logMsg("Modify texture with abnormals filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Alpha& Alpha::setExtractColour(Ogre::ColourValue colour)
{
	mExtractColour = colour;
	return *this;
}

Alpha& Alpha::setExtractColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mExtractColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureBufferPtr Alpha::process()
{
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::Real r = (Ogre::Real)mBuffer->getPixelRedByte(x, y) * mExtractColour.r;
			Ogre::Real g = (Ogre::Real)mBuffer->getPixelGreenByte(x, y) * mExtractColour.g;
			Ogre::Real b = (Ogre::Real)mBuffer->getPixelBlueByte(x, y) * mExtractColour.b;
			Ogre::uchar a = (Ogre::uchar)(((unsigned long)mBuffer->getPixelAlphaByte(x, y) + (unsigned long)(r + g + b))>>1);
			mBuffer->setPixel(x, y, a, a, a, a);
		}
	}

	logMsg("Modify texture with alpha filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AlphaMask& AlphaMask::setColourAlphaMask(bool colourmask)
{
	mColourMask = colourmask;
	return *this;
}

AlphaMask& AlphaMask::setParameterImage(TextureBufferPtr image)
{
	mParam = image;
	return *this;
}

TextureBufferPtr AlphaMask::process()
{
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();

	if (mParam != NULL && (mParam->getWidth() < w || mParam->getHeight() < h)) return mBuffer;

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			if (mParam != NULL)
			{
				if (mColourMask)
				{
					Ogre::ColourValue pixelA = mBuffer->getPixel(x, y);
					Ogre::ColourValue pixelB = mParam->getPixel(x, y);
					Ogre::Vector3 c1(pixelA.r * 255.0f, pixelA.g * 255.0f, pixelA.b * 255.0f);
					Ogre::Vector3 c2(pixelB.r * 255.0f, pixelB.g * 255.0f, pixelB.b * 255.0f);

					Ogre::Real c1norm = c1.normalise();
					Ogre::Real c2norm = c2.normalise();

					Ogre::Real correctness = 0;

					if (c1norm > 0.0f && c2norm > 0.0f)
						correctness = c1.x * c2.x + c1.y * c2.y + c1.z * c2.z;

					mBuffer->setAlpha(x, y, (Ogre::uchar)(pixelA.a * correctness));
				}
				else
				{
					Ogre::ColourValue pixel = mParam->getPixel(x, y);
					Ogre::Real alpha = (pixel.r + pixel.g + pixel.b) / 3.f;
					mBuffer->setAlpha(x, y, mBuffer->getPixelAlphaReal(x, y) * alpha);
				}
			}
			else
			{
				Ogre::uchar a = mBuffer->getPixelAlphaByte(x, y);
				mBuffer->setPixel(x, y, a, a, a, 255);
			}
		}
	}

	logMsg("Modify texture with alphamask filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Blit& Blit::setInputBuffer(TextureBufferPtr inputBuffer)
{
	if (inputBuffer != NULL)
		if (inputBuffer->getHeight() >= mBuffer->getHeight() && inputBuffer->getWidth() >= mBuffer->getWidth())
		{
			mInputBuffer = inputBuffer;
			mInputRect.left = 0;
			mInputRect.top = 0;
			mInputRect.right = inputBuffer->getWidth();
			mInputRect.bottom = inputBuffer->getHeight();
		}
	return *this;
}

Blit& Blit::setInputRect(Ogre::RealRect rect, bool relative)
{
	if (mInputBuffer == NULL) return *this;
	if (relative)
	{
		mInputRect.left = (long)((Ogre::Real)mInputBuffer->getWidth() * std::min<Ogre::Real>(rect.left, 1.0f));
		mInputRect.top = (long)((Ogre::Real)mInputBuffer->getHeight() * std::min<Ogre::Real>(rect.top, 1.0f));
		mInputRect.right = (long)((Ogre::Real)mInputBuffer->getWidth() * std::min<Ogre::Real>(rect.right, 1.0f));
		mInputRect.bottom = (long)((Ogre::Real)mInputBuffer->getHeight() * std::min<Ogre::Real>(rect.bottom, 1.0f));
	}
	else
	{
		mInputRect.left = std::min<long>((long)rect.left, mInputBuffer->getWidth());
		mInputRect.top = std::min<long>((long)rect.top, mInputBuffer->getHeight());
		mInputRect.right = std::min<long>((long)rect.right, mInputBuffer->getWidth());
		mInputRect.bottom = std::min<long>((long)rect.bottom, mInputBuffer->getHeight());
	}
	return *this;
}

Blit& Blit::setInputRect(Ogre::Rect rect)
{
	if (mInputBuffer == NULL) return *this;
	mInputRect.left = std::min<long>(rect.left, mInputBuffer->getWidth());
	mInputRect.top = std::min<long>(rect.top, mInputBuffer->getHeight());
	mInputRect.right = std::min<long>(rect.right, mInputBuffer->getWidth());
	mInputRect.bottom = std::min<long>(rect.bottom, mInputBuffer->getHeight());
	return *this;
}

Blit& Blit::setInputRect(Ogre::Vector2 pos1, Ogre::Vector2 pos2, bool relative)
{
	if (mInputBuffer == NULL) return *this;
	if (relative)
	{
		mInputRect.left = (long)((Ogre::Real)mInputBuffer->getWidth() * std::min<Ogre::Real>(pos1.x, 1.0f));
		mInputRect.top = (long)((Ogre::Real)mInputBuffer->getHeight() * std::min<Ogre::Real>(pos1.y, 1.0f));
		mInputRect.right = (long)((Ogre::Real)mInputBuffer->getWidth() * std::min<Ogre::Real>(pos2.x, 1.0f));
		mInputRect.bottom = (long)((Ogre::Real)mInputBuffer->getHeight() * std::min<Ogre::Real>(pos2.y, 1.0f));
	}
	else
	{
		mInputRect.left = std::min<long>((long)pos1.x, mInputBuffer->getWidth());
		mInputRect.top = std::min<long>((long)pos1.y, mInputBuffer->getHeight());
		mInputRect.right = std::min<long>((long)pos2.x, mInputBuffer->getWidth());
		mInputRect.bottom = std::min<long>((long)pos2.y, mInputBuffer->getHeight());
	}
	return *this;
}

Blit& Blit::setInputRect(size_t x1, size_t y1, size_t x2, size_t y2)
{
	if (mInputBuffer == NULL) return *this;
	mInputRect.left = std::min<long>(x1, mInputBuffer->getWidth());
	mInputRect.top = std::min<long>(y1, mInputBuffer->getHeight());
	mInputRect.right = std::min<long>(x2, mInputBuffer->getWidth());
	mInputRect.bottom = std::min<long>(y2, mInputBuffer->getHeight());
	return *this;
}

Blit& Blit::setInputRect(Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2)
{
	if (mInputBuffer == NULL) return *this;
	mInputRect.left = (long)((Ogre::Real)mInputBuffer->getWidth() * std::min<Ogre::Real>(x1, 1.0f));
	mInputRect.top = (long)((Ogre::Real)mInputBuffer->getHeight() * std::min<Ogre::Real>(y1, 1.0f));
	mInputRect.right = (long)((Ogre::Real)mInputBuffer->getWidth() * std::min<Ogre::Real>(x2, 1.0f));
	mInputRect.bottom = (long)((Ogre::Real)mInputBuffer->getHeight() * std::min<Ogre::Real>(y2, 1.0f));
	return *this;
}

Blit& Blit::setOutputRect(Ogre::RealRect rect, bool relative)
{
	if (relative)
	{
		mOutputRect.left = (long)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(rect.left, 1.0f));
		mOutputRect.top = (long)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(rect.top, 1.0f));
		mOutputRect.right = (long)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(rect.right, 1.0f));
		mOutputRect.bottom = (long)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(rect.bottom, 1.0f));
	}
	else
	{
		mOutputRect.left = std::min<long>((long)rect.left, mBuffer->getWidth());
		mOutputRect.top = std::min<long>((long)rect.top, mBuffer->getHeight());
		mOutputRect.right = std::min<long>((long)rect.right, mBuffer->getWidth());
		mOutputRect.bottom = std::min<long>((long)rect.bottom, mBuffer->getHeight());
	}
	return *this;
}

Blit& Blit::setOutputRect(Ogre::Rect rect)
{
	mOutputRect.left = std::min<long>(rect.left, mBuffer->getWidth());
	mOutputRect.top = std::min<long>(rect.top, mBuffer->getHeight());
	mOutputRect.right = std::min<long>(rect.right, mBuffer->getWidth());
	mOutputRect.bottom = std::min<long>(rect.bottom, mBuffer->getHeight());
	return *this;
}

Blit& Blit::setOutputRect(Ogre::Vector2 pos1, Ogre::Vector2 pos2, bool relative)
{
	if (relative)
	{
		mOutputRect.left = (long)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(pos1.x, 1.0f));
		mOutputRect.top = (long)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(pos1.y, 1.0f));
		mOutputRect.right = (long)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(pos2.x, 1.0f));
		mOutputRect.bottom = (long)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(pos2.y, 1.0f));
	}
	else
	{
		mOutputRect.left = std::min<long>((long)pos1.x, mBuffer->getWidth());
		mOutputRect.top = std::min<long>((long)pos1.y, mBuffer->getHeight());
		mOutputRect.right = std::min<long>((long)pos2.x, mBuffer->getWidth());
		mOutputRect.bottom = std::min<long>((long)pos2.y, mBuffer->getHeight());
	}
	return *this;
}

Blit& Blit::setOutputRect(size_t x1, size_t y1, size_t x2, size_t y2)
{
	mOutputRect.left = std::min<long>(x1, mBuffer->getWidth());
	mOutputRect.top = std::min<long>(y1, mBuffer->getHeight());
	mOutputRect.right = std::min<long>(x2, mBuffer->getWidth());
	mOutputRect.bottom = std::min<long>(y2, mBuffer->getHeight());
	return *this;
}

Blit& Blit::setOutputRect(Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2)
{
	mOutputRect.left = (long)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(x1, 1.0f));
	mOutputRect.top = (long)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(y1, 1.0f));
	mOutputRect.right = (long)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(x2, 1.0f));
	mOutputRect.bottom = (long)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(y2, 1.0f));
	return *this;
}

TextureBufferPtr Blit::process()
{
	if (mInputBuffer == NULL) return mBuffer;
	for (long y = mOutputRect.top; y < mOutputRect.bottom; y++)
	{
		for (long x = mOutputRect.left; x < mOutputRect.right; x++)
		{
			size_t x0 = static_cast<size_t>(static_cast<Ogre::Real>(x - mOutputRect.left) / static_cast<Ogre::Real>(mOutputRect.width()) * static_cast<Ogre::Real>(mInputRect.width()) + static_cast<Ogre::Real>(mInputRect.left));
			size_t y0 = static_cast<size_t>(static_cast<Ogre::Real>(y - mOutputRect.top) / static_cast<Ogre::Real>(mOutputRect.height()) * static_cast<Ogre::Real>(mInputRect.height()) + static_cast<Ogre::Real>(mInputRect.top));
			mBuffer->setPixel(x, y, mInputBuffer->getPixel(x0, y0));
		}
	}
	logMsg("Modify texture with blit filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Blur& Blur::setSize(Ogre::uchar size)
{
	mSize = size;
	if (mSize < 3) mSize = 3;
	if ((mSize % 2) == 0) mSize++;
	return *this;
}

Blur& Blur::setSigma(Ogre::uchar sigma)
{
	mSigma = sigma;
	return *this;
}

Blur& Blur::setType(Blur::BLUR_TYPE type)
{
	mType = type;
	return *this;
}

TextureBufferPtr Blur::process()
{
	Ogre::Real blurKernel[25] = {	1, 2, 3, 2, 1, 2, 4, 5, 4, 2, 3, 5, 6, 5, 3, 2, 4, 5, 4, 2, 1, 2, 3, 2, 1 };
	Convolution filter(mBuffer);
	switch (mType)
	{
	default:
	case BLUR_BOX:
		filter.setKernel(5, blurKernel);
		break;

	case BLUR_MEAN:
		filter.setKernel(Ogre::Matrix3(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f)).calculateDivisor();
		break;

	case BLUR_GAUSSIAN:
		Ogre::Real fSigma = 0.5f + ((3.0f - 0.5f) / 255.0f) * (Ogre::Real)mSigma;
		int r = (int)mSize / 2;
		double min = Ogre::Math::Exp((Ogre::Real)(2 * r * r) / (-2.0f * fSigma * fSigma)) / (Ogre::Math::TWO_PI * fSigma * fSigma);
		int* kernel = new int[mSize * mSize];
		int divisor = 0;
		int y = -r;
		int x = -r;
		for (int i = 0; i < mSize; i++)
		{
			for (int j = 0; j < mSize; j++)
			{
				kernel[i * mSize + j] = (int)((Ogre::Math::Exp((Ogre::Real)(x * x + y * y) / (-2.0f * fSigma * fSigma)) / (Ogre::Math::TWO_PI * fSigma * fSigma)) / min);
				divisor += kernel[i * mSize + j];
				x++;
			}
			y++;
		}
		filter.setKernel(mSize, kernel).setDivisor((Ogre::Real)divisor);
		delete kernel;
		break;
	}
	filter.setIncludeAlphaChannel(true).process();

	logMsg("Modify texture with blur filter : " + StringConverter::toString(mType));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Channel& Channel::setSelection(CANNEL_SELECTION selection)
{
	mSelection = selection;
	return *this;
}

TextureBufferPtr Channel::process()
{
	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			if (mSelection == SELECT_GRAY)
			{
				Ogre::ColourValue pixel = mBuffer->getPixel(x, y);
				Ogre::Real gray = (pixel.r + pixel.g + pixel.b) / 3.0f;
				mBuffer->setPixel(x, y, gray, gray, gray, pixel.a);
			}
			else
			{
				if (mSelection != SELECT_RED) mBuffer->setRed(x, y, 0.0f);
				if (mSelection != SELECT_GREEN) mBuffer->setGreen(x, y, 0.0f);
				if (mSelection != SELECT_BLUE) mBuffer->setBlue(x, y, 0.0f);
				if (mSelection != SELECT_BLUE) mBuffer->setBlue(x, y, 0.0f);
			}
		}
	}

	logMsg("Modify texture with channel filter : " + StringConverter::toString(mSelection));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CircleTexture& CircleTexture::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

CircleTexture& CircleTexture::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

CircleTexture& CircleTexture::setRadius(size_t radius)
{
	mRadius = radius;
	return *this;
}

CircleTexture& CircleTexture::setRadius(Ogre::Real radius)
{
	mRadius = (size_t)((Ogre::Real)std::min<Ogre::uint>(mBuffer->getWidth(), mBuffer->getHeight()) * Ogre::Math::Abs(radius));
	return *this;
}

CircleTexture& CircleTexture::setCenterX(size_t x)
{
	mX = std::min<size_t>(x, mBuffer->getWidth() - 1);
	return *this;
}

CircleTexture& CircleTexture::setCenterX(Ogre::Real x)
{
	mX = std::min<size_t>((size_t)(x * (Ogre::Real)mBuffer->getWidth()), mBuffer->getWidth() - 1);
	return *this;
}

CircleTexture& CircleTexture::setCenterY(size_t y)
{
	mY = std::min<size_t>(y, mBuffer->getHeight() - 1);
	return *this;
}

CircleTexture& CircleTexture::setCenterY(Ogre::Real y)
{
	mY = std::min<size_t>((size_t)(y * (Ogre::Real)mBuffer->getHeight()), mBuffer->getHeight() - 1);
	return *this;
}

CircleTexture& CircleTexture::setCenter(Ogre::Vector2 pos, bool relative)
{
	setCenter(pos.x, pos.y, relative);
	return *this;
}

CircleTexture& CircleTexture::setCenter(size_t x, size_t y)
{
	setCenterX(x);
	setCenterY(y);
	return *this;
}

CircleTexture& CircleTexture::setCenter(Ogre::Real x, Ogre::Real y, bool relative)
{
	if (relative)
	{
		setCenterX(x);
		setCenterY(y);
	}
	else
	{
		setCenterX((size_t)x);
		setCenterY((size_t)y);
	}
	return *this;
}

TextureBufferPtr CircleTexture::process()
{
	long x = 0;
	long y = mRadius;
	long p = 3 - 2 * mRadius;
	while (x <= y)
	{
		for (long dy = -y; dy <= y; dy++)
		{
			_putpixel(+x, dy);
			_putpixel(-x, dy);
		}
		for (long dx = -x; dx <= x; dx++)
		{
			_putpixel(+y, dx);
			_putpixel(-y, dx);
		}
		if (p < 0)
			p += 4 * x++ + 6;
		else
			p += 4 * (x++ - y--) + 10;
	}
	logMsg("Modify texture with circle filter : x = " + StringConverter::toString(mX) + ", y = " + StringConverter::toString(mY) + ", Radius = " + StringConverter::toString(mRadius));
	return mBuffer;
}

void CircleTexture::_putpixel(long dx, long dy)
{
	if (mX + dx < 0 || mX + dx >= mBuffer->getWidth()) return;
	if (mY + dy < 0 || mY + dy >= mBuffer->getHeight()) return;
	mBuffer->setPixel(mX + dx, mY + dy, mColour);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Colours& Colours::setColourBase(Ogre::ColourValue colour)
{
	mColourBase = colour;
	return *this;
}

Colours& Colours::setColourBase(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourBase = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Colours& Colours::setColourPercent(Ogre::ColourValue colour)
{
	mColourPercent = colour;
	return *this;
}

Colours& Colours::setColourPercent(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourPercent = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Colours& Colours::setBrightness(Ogre::uchar brightness)
{
	mBrightness = brightness;
	return *this;
}

Colours& Colours::setContrast(Ogre::uchar contrast)
{
	mContrast = contrast;
	return *this;
}

Colours& Colours::setSaturation(Ogre::uchar saturation)
{
	mSaturation = saturation;
	return *this;
}

Colours& Colours::setAlpha(Ogre::uchar alpha)
{
	mAlpha = alpha;
	return *this;
}

TextureBufferPtr Colours::process()
{
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();
	long brightness = (((long)mBrightness) * 2) - 256;
	long contrast = (((long)mContrast));
	Ogre::Real fconstrast = (Ogre::Real)mContrast / 128.0f;
	fconstrast = fconstrast * fconstrast * fconstrast;
	contrast = (long)(fconstrast * 256.0f);
	Ogre::uchar minalpha = (mAlpha >= 127) ? (Ogre::uchar)((mAlpha - 127) * 2.f - (mAlpha - 127) / 128.f) : 0;
	Ogre::uchar maxalpha = (mAlpha <= 127) ? (Ogre::uchar)(mAlpha * 2.f + mAlpha / 127.f) : 255;
	Ogre::Real alphamult = (Ogre::Real)(maxalpha - minalpha) / 255.f;

	for (unsigned long y = 0; y < h; y++)
	{
		for (unsigned long x = 0; x < w; x++)
		{
			long r = (long)(mColourBase.r * 255.0f) + (((long)mBuffer->getPixelRedByte(x, y) * (long)(mColourPercent.r * 255.0f))>>8) + brightness;
			long g = (long)(mColourBase.g * 255.0f) + (((long)mBuffer->getPixelGreenByte(x, y) * (long)(mColourPercent.g * 255.0f))>>8) + brightness;
			long b = (long)(mColourBase.b * 255.0f) + (((long)mBuffer->getPixelBlueByte(x, y) * (long)(mColourPercent.b * 255.0f))>>8) + brightness;

			long c = (long)(((r - 127) * contrast)>>8) + 127;
			r = (c < 0x00) ? 0x00 : (c > 0xff) ? 0xff : c;

			c = (long) (((g - 127) * contrast)>>8) + 127;
			g = (c < 0x00) ? 0x00 : (c > 0xff) ? 0xff : c;

			c = (long) (((b - 127) * contrast)>>8) + 127;
			b = (c < 0x00) ? 0x00 : (c > 0xff) ? 0xff : c;

			if (mSaturation != 127)
			{
				long l = r + g + b;
				long u = (3 * r - l) * mSaturation / 127;
				long v = (3 * b - l) * mSaturation / 127;
				r = (u + l) / 3;
				g = (l - (u + v)) / 3;
				b = (v + l) / 3;
			}

			mBuffer->setRed(x, y, (Ogre::uchar)std::min<long>(std::max<long>(r, 0), 255));
			mBuffer->setGreen(x, y, (Ogre::uchar)std::min<long>(std::max<long>(g, 0), 255));
			mBuffer->setBlue(x, y, (Ogre::uchar)std::min<long>(std::max<long>(b, 0), 255));
			mBuffer->setAlpha(x, y, std::min<Ogre::uchar>(std::max<Ogre::uchar>((Ogre::uchar)((Ogre::Real)mBuffer->getPixelAlphaByte(x, y) * alphamult) + minalpha, 0), 255));
		}
	}

	logMsg("Modify texture with colours filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Combine& Combine::addImage(TextureBufferPtr image, COMBINE_METHOD method)
{
	if (image != NULL)
		if (image->getHeight() >= mBuffer->getHeight() && image->getWidth() >= mBuffer->getWidth())
		{
			LAYER l;
			l.action = method;
			l.image = image;
			mQueue.push(l);
		}
	return *this;
}

Combine& Combine::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Combine& Combine::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureBufferPtr Combine::process()
{
	int i = 0;
	while (mQueue.size() > 0)
	{
		LAYER l = mQueue.front();
		_process(l.image, l.action);
		mQueue.pop();
		i++;
		logMsg("Combine textures : " + StringConverter::toString(l.action));
	}

	return mBuffer;
}

void Combine::_process(TextureBufferPtr image, COMBINE_METHOD method)
{
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();
	unsigned long rcolPercent = (unsigned long)(mColour.r * 255.0f);
	unsigned long gcolPercent = (unsigned long)(mColour.g * 255.0f);
	unsigned long bcolPercent = (unsigned long)(mColour.b * 255.0f);

	switch (method)
	{
	case METHOD_ADD_CLAMP:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				unsigned long r = (unsigned long)(pxDst.r * 255.0f) + (((unsigned long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				unsigned long g = (unsigned long)(pxDst.g * 255.0f) + (((unsigned long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				unsigned long b = (unsigned long)(pxDst.b * 255.0f) + (((unsigned long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				mBuffer->setPixel(x, y, (Ogre::uchar)((r < 255) ? r : 255), (Ogre::uchar)((g < 255) ? g : 255), (Ogre::uchar)((b < 255) ? b : 255), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_ADD_WRAP:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				unsigned long r = (unsigned long)(pxDst.r * 255.0f) + (((unsigned long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				unsigned long g = (unsigned long)(pxDst.g * 255.0f) + (((unsigned long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				unsigned long b = (unsigned long)(pxDst.b * 255.0f) + (((unsigned long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				mBuffer->setPixel(x, y, (Ogre::uchar)(r % 255), (Ogre::uchar)(g % 255), (Ogre::uchar)(b % 255), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_SUB_CLAMP:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				long r = (long)(pxDst.r * 255.0f) - (((long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				long g = (long)(pxDst.g * 255.0f) - (((long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				long b = (long)(pxDst.b * 255.0f) - (((long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				mBuffer->setPixel(x, y, (Ogre::uchar)((r > 0) ? r : 0), (Ogre::uchar)((g > 0) ? g : 0), (Ogre::uchar)((b > 0) ? b : 0), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_SUB_WRAP:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				long r = (long)(pxDst.r * 255.0f) - (((long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				long g = (long)(pxDst.g * 255.0f) - (((long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				long b = (long)(pxDst.b * 255.0f) - (((long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				mBuffer->setPixel(x, y, (Ogre::uchar)(r % 255), (Ogre::uchar)(g % 255), (Ogre::uchar)(b % 255), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_MULTIPLY:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				unsigned long r = (unsigned long)(pxDst.r * 255.0f) * (((unsigned long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				unsigned long g = (unsigned long)(pxDst.g * 255.0f) * (((unsigned long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				unsigned long b = (unsigned long)(pxDst.b * 255.0f) * (((unsigned long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				mBuffer->setPixel(x, y, (Ogre::uchar)(r >> 8), (Ogre::uchar)(g >> 8), (Ogre::uchar)(b >> 8), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_MULTIPLY2:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				unsigned long r = (unsigned long)(pxDst.r * 255.0f) * (((unsigned long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				r >>= 7;
				unsigned long g = (unsigned long)(pxDst.g * 255.0f) * (((unsigned long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				g >>= 7;
				unsigned long b = (unsigned long)(pxDst.b * 255.0f) * (((unsigned long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				b >>= 7;
				mBuffer->setPixel(x, y, (Ogre::uchar)((r < 255) ? r : 255), (Ogre::uchar)((g < 255) ? g : 255), (Ogre::uchar)((b < 255) ? b : 255), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_BLEND:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				unsigned long r = (unsigned long)(pxDst.r * 255.0f) + (((unsigned long)(pxSrc.r * 255.0f) * rcolPercent)>>8);
				unsigned long g = (unsigned long)(pxDst.g * 255.0f) + (((unsigned long)(pxSrc.g * 255.0f) * gcolPercent)>>8);
				unsigned long b = (unsigned long)(pxDst.b * 255.0f) + (((unsigned long)(pxSrc.b * 255.0f) * bcolPercent)>>8);
				mBuffer->setPixel(x, y, (Ogre::uchar)(r >> 1), (Ogre::uchar)(g >> 1), (Ogre::uchar)(b >> 1), (Ogre::uchar)(pxDst.a * 255.0f));
			}
		}
		break;

	case METHOD_ALPHA:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				unsigned long a = (unsigned long)(pxDst.a * 255.0f) + (((unsigned long)(pxSrc.a * 255.0f) * bcolPercent)>>8);
				mBuffer->setAlpha(x, y, (Ogre::uchar)(a >> 1));
			}
		}
		break;

	default:
	case METHOD_LAYER:
		for (size_t y = 0; y < h; y++)
		{
			for (size_t x = 0; x < w; x++)
			{
				Ogre::ColourValue pxSrc = image->getPixel(x, y);
				Ogre::ColourValue pxDst = mBuffer->getPixel(x, y);
				mBuffer->setPixel(x, y,
								  (Ogre::uchar)(pxSrc.r * pxSrc.a * 255.0f + pxDst.r * 255.0f * (1.0f - pxSrc.a)),
								  (Ogre::uchar)(pxSrc.g * pxSrc.a * 255.0f + pxDst.g * 255.0f * (1.0f - pxSrc.a)),
								  (Ogre::uchar)(pxSrc.b * pxSrc.a * 255.0f + pxDst.b * 255.0f * (1.0f - pxSrc.a)),
								  (Ogre::uchar)((pxDst.a - pxDst.a * pxSrc.a) * 255.0f + pxSrc.a * 255.0f));
			}
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Convolution& Convolution::setKernel(Ogre::uchar size, Ogre::Real* data)
{
	if (size < 3 || size % 2 == 0) return *this;
	delete mKernelData;
	mKernelSize = size;
	mKernelData = new Ogre::Real[mKernelSize * mKernelSize];
	for (int y = 0; y < mKernelSize; y++)
	{
		for (int x = 0; x < mKernelSize; x++)
		{
			mKernelData[y * mKernelSize + x] = data[y * mKernelSize + x];
		}
	}
	calculateDivisor();
	return *this;
}

Convolution& Convolution::setKernel(Ogre::uchar size, int* data)
{
	if (size < 3 || size % 2 == 0) return *this;
	delete mKernelData;
	mKernelSize = size;
	mKernelData = new Ogre::Real[mKernelSize * mKernelSize];
	for (int y = 0; y < mKernelSize; y++)
	{
		for (int x = 0; x < mKernelSize; x++)
		{
			mKernelData[y * mKernelSize + x] = (Ogre::Real)data[y * mKernelSize + x];
		}
	}
	return *this;
}

Convolution& Convolution::setKernel(Ogre::Matrix3 data)
{
	delete mKernelData;
	mKernelSize = 3;
	mKernelData = new Ogre::Real[mKernelSize * mKernelSize];
	for (int y = 0; y < mKernelSize; y++)
	{
		for (int x = 0; x < mKernelSize; x++)
		{
			mKernelData[y * mKernelSize + x] = data[y][x];
		}
	}
	return *this;
}

Convolution& Convolution::setDivisor(Ogre::Real divisor)
{
	mDivisor = divisor;
	if (mDivisor == 0.0) mDivisor = 1.0;
	return *this;
}

Convolution& Convolution::calculateDivisor()
{
	mDivisor = 0.0;
	for (int y = 0; y < mKernelSize; y++)
	{
		for (int x = 0; x < mKernelSize; x++)
		{
			mDivisor += mKernelData[y * mKernelSize + x];
		}
	}
	if (mDivisor == 0.0) mDivisor = 1.0;

	return *this;
}

Convolution& Convolution::setThreshold(Ogre::uchar threshold)
{
	mThreshold = threshold;
	return *this;
}

Convolution& Convolution::setCalculateEdgeDivisor(bool calculateedgedivisor)
{
	mCalculateEdgeDivisor = calculateedgedivisor;
	return *this;
}

Convolution& Convolution::setIncludeAlphaChannel(bool usealpha)
{
	mIncludeAlphaChannel = usealpha;
	return *this;
}

TextureBufferPtr Convolution::process()
{
	int radius = ((int)mKernelSize) >> 1;
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	for (long y = 0; y < (long)mBuffer->getWidth(); y++)
	{
		for (long x = 0; x < (long)mBuffer->getHeight(); x++)
		{
			long r = 0;
			long g = 0;
			long b = 0;
			long a = 0;
			long div = 0;
			int processedKernelSize = 0;

			for (int i = 0; i < mKernelSize; i++)
			{
				int ir = i - radius;

				if ((y + ir) < 0)
					continue;
				if ((y + ir) >= (long)mBuffer->getHeight())
					break;

				for (int j = 0; j < (int)mKernelSize; j++)
				{
					int jr = j - radius;

					if ((x + jr) < 0)
						continue;
					if ((x + jr) < (long)mBuffer->getWidth())
					{
						Ogre::Real k = mKernelData[i * mKernelSize + j];
						Ogre::ColourValue pixel = mBuffer->getPixel(y + ir, x + jr);
						div += (long)k;
						k *= 255.0f;
						r += (long)(k * pixel.r);
						g += (long)(k * pixel.g);
						b += (long)(k * pixel.b);
						a += (long)(k * pixel.a);

						processedKernelSize++;
					}
				}
			}

			if (processedKernelSize == (mKernelSize * mKernelSize))
				div = (long)mDivisor;
			else
			{
				if (!mCalculateEdgeDivisor)
					div = (long)mDivisor;
			}

			if (div != 0)
			{
				r /= div;
				g /= div;
				b /= div;
				a /= div;
			}
			r += ((long)mThreshold - 128);
			g += ((long)mThreshold - 128);
			b += ((long)mThreshold - 128);
			if (mIncludeAlphaChannel)
				a += ((long)mThreshold - 128);
			else
				a = (long)mBuffer->getPixelAlphaByte(x, y);

			tmpBuffer->setPixel(y, x,
								(Ogre::uchar)((r > 255) ? 255 : ((r < 0) ? 0 : r)),
								(Ogre::uchar)((g > 255) ? 255 : ((g < 0) ? 0 : g)),
								(Ogre::uchar)((b > 255) ? 255 : ((b < 0) ? 0 : b)),
								(Ogre::uchar)((a > 255) ? 255 : ((a < 0) ? 0 : a)));
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	if (mLog)
	{
		Ogre::String strKernel = "Modify texture with convolution filter :\n";
		for (int i = 0; i < mKernelSize; i++)
		{
			strKernel.append("\t");
			for (int j = 0; j < mKernelSize; j++)
			{
				strKernel.append(StringConverter::toString(mKernelData[i * mKernelSize + j]));
				if (j < (mKernelSize - 1)) strKernel.append("\t");
			}
			strKernel.append("\n");
		}
		logMsg(strKernel);
	}
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Crack& Crack::setParameterImage(TextureBufferPtr image)
{
	mParam = image;
	return *this;
}

Crack& Crack::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Crack& Crack::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Crack& Crack::setCount(Ogre::uint count)
{
	mCount = count;
	return *this;
}

Crack& Crack::setVariation(Ogre::uchar variation)
{
	mVariation = variation;
	return *this;
}

Crack& Crack::setLength(Ogre::uchar length)
{
	mLength = length;
	return *this;
}

Crack& Crack::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

Crack& Crack::setLengthDecision(CRACK_LENGTH_DECISION lengthdecision)
{
	mLengthDecision = lengthdecision;
	return *this;
}

Crack& Crack::setQuality(CRACK_QUALITY quality)
{
	mQuality = quality;
	return *this;
}

TextureBufferPtr Crack::process()
{
	Ogre::ColourValue x1, y1, x2, y2;
	double cy2, cy1, cx2, cx1;
	size_t oxn, oyn;

	srand(mSeed);

	if (mParam == NULL) return mBuffer;

	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();

	if (mParam->getWidth() < w || mParam->getHeight() < h) return mBuffer;

	TextureBufferPtr tmpBuffer = mBuffer->clone();

	for (Ogre::uint n = 0; n < mCount; n++)
	{
		double x = ((double)rand() / RAND_MAX) * (double)w;
		double y = ((double)rand() / RAND_MAX) * (double)h;
		double a = Ogre::Math::TWO_PI * ((double)rand() / RAND_MAX);
		long count = (long)mLength;
		Ogre::ColourValue pixel = mParam->getPixel((size_t)x, (size_t)y);

		if (mParam->getWidth() && mLengthDecision == LENGTH_DECISION_NORMAL_BASED)
		{
			Ogre::Vector3 normal(pixel.r * 255.0f - 127.0f, pixel.g * 255.0f - 127.0f, 0.0f);
			Ogre::Real norm = normal.x * normal.x + normal.y * normal.y;
			norm = (norm > 0) ? Ogre::Math::Sqrt(norm) : 0;
			count = std::min<long>((long)(count * norm * norm / 8.0f), (long)mLength);
		}

		if (mLengthDecision == LENGTH_DECISION_RANDOM)
			count = (long)(count * ((double)rand() / RAND_MAX) * 2.0);

		while (--count >= 0)
		{
			a += (double)mVariation / 256.0 * (2.0 * ((double)rand() / RAND_MAX) - 1.0);

			x = x + Ogre::Math::Cos((Ogre::Real)a);
			y = y + Ogre::Math::Sin((Ogre::Real)a);
			if ((size_t)x >= w || (size_t)y >= h) break;

			if (mParam->getWidth())
			{
				Ogre::Vector3 normal(127.0f - pixel.r * 255.0f, pixel.g * 255.0f - 127.0f, 0.0f);
				if (normal.x == 0.0)
				{
					if (normal.y > 0.0)
						a = Ogre::Math::PI;
					else
						a = Ogre::Math::TWO_PI;
				}
				else if (normal.x < 0)
					a = Ogre::Math::ATan(normal.y / normal.x).valueRadians() + 1.5f * Ogre::Math::PI;
				else if (normal.y < 0)
					a = Ogre::Math::ATan(normal.y / normal.x).valueRadians() + 2.5f * Ogre::Math::PI;
				else
					a = Ogre::Math::ATan(normal.y / normal.x).valueRadians() + Ogre::Math::HALF_PI;
				Ogre::Real norm = normal.x * normal.x + normal.y * normal.y;
				norm = (norm > 0) ? Ogre::Math::Sqrt(norm) : 0;
				if (norm < (255.0f - pixel.a * 255.0f) / 4.0f)
					continue;
			}

			switch (mQuality)
			{
			case QUALITY_SUBPIXEL:
				cy2 = (x - floor(x)) * (y - floor(y));
				cy1 = (y - floor(y)) * (ceil(x) - x);
				cx2 = (x - floor(x)) * (ceil(y) - y);
				cx1 = 1 - (cx2 + cy1 + cy2);
				oxn = std::min<size_t>((size_t)x + 1, w);
				oyn = std::min<size_t>((size_t)x + 1, h);

				x1 = mBuffer->getPixel((size_t)x, (size_t)y);
				y1 = mBuffer->getPixel((size_t)x, oyn);
				x2 = mBuffer->getPixel(oxn, (size_t)y);
				y2 = mBuffer->getPixel(oxn, oyn);

				x1 *= (float)(1 - cx1);
				x2 *= (float)(1 - cx2);
				y1 *= (float)(1 - cy1);
				y2 *= (float)(1 - cy2);

				x1 += mColour * (float)cx1;
				y1 += mColour * (float)cy1;
				x2 += mColour * (float)cx2;
				y2 += mColour * (float)cy2;

				tmpBuffer->setPixel((size_t)x, (size_t)y, x1);
				tmpBuffer->setPixel((size_t)x, oyn, y1);
				tmpBuffer->setPixel(oxn, (size_t)y, x2);
				tmpBuffer->setPixel(oxn, oyn, y2);
				break;

			case QUALITY_ALPHA:
				tmpBuffer->setPixel((size_t)x, (size_t)y, mBuffer->getPixel((size_t)x, (size_t)y) + mColour);
				break;

			default:
				tmpBuffer->setPixel((size_t)x, (size_t)y, mColour);
				break;
			}
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with crack filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Cycloid& Cycloid::setType(Cycloid::CYCLOID_TYPE type)
{
	mType = type;
	Ogre::Real size = (Ogre::Real)std::min<Ogre::uint>(mBuffer->getHeight(), mBuffer->getWidth());
	switch (mType)
	{
	default:
	case HYPOCYCLOID:
		mParam_R = 3.0f / 6.0f * size;
		mParam_r = 1.0f / 6.0f * size;
		mParam_d = 0.0f;
		mParam_e = 0.0f;
		break;
	case HYPOTROCHOID:
		mParam_R = 5.0f / 14.0f * size;
		mParam_r = 3.0f / 14.0f * size;
		mParam_d = 5.0f / 14.0f * size;
		mParam_e = 0.0f;
		break;
	case EPICYCLOID:
		mParam_R = 3.0f / 10.0f * size;
		mParam_r = 1.0f / 10.0f * size;
		mParam_d = 0.0f;
		mParam_e = 0.0f;
		break;
	case EPITROCHOID:
		mParam_R = 3.0f / 10.0f * size;
		mParam_r = 1.0f / 10.0f * size;
		mParam_d = 1.0f / 20.0f * size;
		mParam_e = 0.0f;
		break;
	case ROSE_CURVE:
		mParam_R = 0.5f * size;
		mParam_r = 4.0f;
		mParam_d = 1.0f;
		mParam_e = 0.0f;
		break;
	case LISSAJOUS_CURVE:
		mParam_R = 0.5f * size;
		mParam_r = 5.0f;
		mParam_d = 4.0f;
		mParam_e = Ogre::Math::HALF_PI;
		break;
	}
	return *this;
}

Cycloid& Cycloid::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Cycloid& Cycloid::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Cycloid& Cycloid::setCenterX(Ogre::Real centerx)
{
	mCenterX = centerx;
	return *this;
}

Cycloid& Cycloid::setCenterY(Ogre::Real centery)
{
	mCenterY = centery;
	return *this;
}

Cycloid& Cycloid::setParameter(Cycloid::CYCLOID_PARAMETER paramType, Ogre::Real value)
{
	switch (paramType)
	{
	case PARAMETER_R:
		mParam_R = value;
		break;
	case PARAMETER_r:
		mParam_r = value;
		break;
	case PARAMETER_d:
		mParam_d = value;
		break;
	case PARAMETER_e:
		mParam_e = value;
		break;
	case PARAMETER_k:
		switch (mType)
		{
		default:
		case HYPOCYCLOID:
		case HYPOTROCHOID:
		case EPICYCLOID:
		case EPITROCHOID:
			mParam_R = value * mParam_r;
			break;
		case ROSE_CURVE:
		case LISSAJOUS_CURVE:
			mParam_r = value * mParam_d;
			break;
		}
		break;
	default:
		break;
	}
	return *this;
}

Cycloid& Cycloid::setPenSize(Ogre::uint size)
{
	mPenSize = size;
	return *this;
}

TextureBufferPtr Cycloid::process()
{
	if (mPenSize == 0) return mBuffer;
	long xpos = (long)((Ogre::Real)mBuffer->getWidth() * mCenterX);
	long ypos = (long)((Ogre::Real)mBuffer->getHeight() * mCenterY);
	Ogre::Real step = Ogre::Math::PI / (Ogre::Real)std::min<Ogre::uint>(mBuffer->getHeight(), mBuffer->getWidth());
	switch (mType)
	{
	default:
		break;
	case HYPOCYCLOID:
		_process_hypocycloid(xpos, ypos, step);
		break;
	case HYPOTROCHOID:
		_process_hypotrochoid(xpos, ypos, step);
		break;
	case EPICYCLOID:
		_process_epicycloid(xpos, ypos, step);
		break;
	case EPITROCHOID:
		_process_epitrochoid(xpos, ypos, step);
		break;
	case ROSE_CURVE:
		_process_rose_curve(xpos, ypos, step);
		break;
	case LISSAJOUS_CURVE:
		_process_lissajous_curve(xpos, ypos, step);
		break;
	}
	return mBuffer;
}

void Cycloid::_process_hypocycloid(long x, long y, Ogre::Real step)
{
	long px = 0;
	long py = 0;
	Ogre::Real phi = 0;

	long sx = x + (long)Ogre::Math::Floor(mParam_R + 0.5f);
	long sy = y;
	do
	{
		Ogre::Real dx = (mParam_R - mParam_r) * Ogre::Math::Cos(phi) + mParam_r * Ogre::Math::Cos(((mParam_R - mParam_r) / mParam_r) * phi);
		Ogre::Real dy = (mParam_R - mParam_r) * Ogre::Math::Sin(phi) - mParam_r * Ogre::Math::Sin(((mParam_R - mParam_r) / mParam_r) * phi);

		px = x + (long)Ogre::Math::Floor(dx + 0.5f);
		py = y - (long)Ogre::Math::Floor(dy + 0.5f);
		_process_paint(px, py, step);

		phi += step;
	}
	while (!(sx == px && sy == py && phi < 100.0f * Ogre::Math::PI) || phi < Ogre::Math::TWO_PI);
	logMsg("Modify texture with hypocycloid drawing");
}

void Cycloid::_process_hypotrochoid(long x, long y, Ogre::Real step)
{
	long px = 0;
	long py = 0;
	Ogre::Real phi = 0;

	long sx = x + (long)Ogre::Math::Floor((mParam_R - mParam_r) + mParam_d + 0.5f);
	long sy = y;
	do
	{
		Ogre::Real dx = (mParam_R - mParam_r) * Ogre::Math::Cos(phi) + mParam_d * Ogre::Math::Cos(((mParam_R - mParam_r) / mParam_r) * phi);
		Ogre::Real dy = (mParam_R - mParam_r) * Ogre::Math::Sin(phi) - mParam_d * Ogre::Math::Sin(((mParam_R - mParam_r) / mParam_r) * phi);

		px = x + (long)Ogre::Math::Floor(dx + 0.5f);
		py = y - (long)Ogre::Math::Floor(dy + 0.5f);
		_process_paint(px, py, step);

		phi += step;
	}
	while (!(sx == px && sy == py && phi < 100.0f * Ogre::Math::PI) || phi < Ogre::Math::TWO_PI);
	logMsg("Modify texture with hypotrochid drawing");
}

void Cycloid::_process_epicycloid(long x, long y, Ogre::Real step)
{
	long px = 0;
	long py = 0;
	Ogre::Real phi = 0;

	long sx = x + (long)Ogre::Math::Floor((mParam_R + mParam_r) - mParam_r + 0.5f);
	long sy = y;
	do
	{
		Ogre::Real dx = (mParam_R + mParam_r) * Ogre::Math::Cos(phi) - mParam_r * Ogre::Math::Cos(((mParam_R + mParam_r) / mParam_r) * phi);
		Ogre::Real dy = (mParam_R + mParam_r) * Ogre::Math::Sin(phi) - mParam_r * Ogre::Math::Sin(((mParam_R + mParam_r) / mParam_r) * phi);

		px = x + (long)Ogre::Math::Floor(dx + 0.5f);
		py = y - (long)Ogre::Math::Floor(dy + 0.5f);
		_process_paint(px, py, step);

		phi += step;
	}
	while (!(sx == px && sy == py && phi < 100.0f * Ogre::Math::PI) || phi < Ogre::Math::TWO_PI);
	logMsg("Modify texture with epicycloid drawing");
}

void Cycloid::_process_epitrochoid(long x, long y, Ogre::Real step)
{
	long px = 0;
	long py = 0;
	Ogre::Real phi = 0;

	long sx = x + (long)Ogre::Math::Floor((mParam_R + mParam_r) - mParam_d + 0.5f);
	long sy = y;
	do
	{
		Ogre::Real dx = (mParam_R + mParam_r) * Ogre::Math::Cos(phi) - mParam_d * Ogre::Math::Cos(((mParam_R + mParam_r) / mParam_r) * phi);
		Ogre::Real dy = (mParam_R + mParam_r) * Ogre::Math::Sin(phi) - mParam_d * Ogre::Math::Sin(((mParam_R + mParam_r) / mParam_r) * phi);

		px = x + (long)Ogre::Math::Floor(dx + 0.5f);
		py = y - (long)Ogre::Math::Floor(dy + 0.5f);
		_process_paint(px, py, step);

		phi += step;
	}
	while (!(sx == px && sy == py && phi < 100.0f * Ogre::Math::PI) || phi < Ogre::Math::TWO_PI);
	logMsg("Modify texture with epitrochoid drawing");
}

void Cycloid::_process_rose_curve(long x, long y, Ogre::Real step)
{
	long px = 0;
	long py = 0;
	Ogre::Real t = 0;
	Ogre::Real k = mParam_r / mParam_d;

	step = step / 10.0f;

	long sx = x;
	long sy = y;
	do
	{
		Ogre::Real dx = mParam_R * Ogre::Math::Cos(k * t) * Ogre::Math::Sin(t);
		Ogre::Real dy = mParam_R * Ogre::Math::Cos(k * t) * Ogre::Math::Cos(t);

		px = x + (long)Ogre::Math::Floor(dx + 0.5f);
		py = y - (long)Ogre::Math::Floor(dy + 0.5f);
		_process_paint(px, py, step);

		t += step;
	}
	while (t <= Ogre::Math::TWO_PI);
	logMsg("Modify texture with rose curve drawing");
}

void Cycloid::_process_lissajous_curve(long x, long y, Ogre::Real step)
{
	long px = 0;
	long py = 0;
	Ogre::Real t = 0;

	step = step / 10.0f;

	long sx = x;
	long sy = y;
	do
	{
		Ogre::Real dx = mParam_R * Ogre::Math::Sin(mParam_r * t + mParam_e);
		Ogre::Real dy = mParam_R * Ogre::Math::Cos(mParam_d * t + mParam_e);

		px = x + (long)Ogre::Math::Floor(dx + 0.5f);
		py = y - (long)Ogre::Math::Floor(dy + 0.5f);
		_process_paint(px, py, step);

		t += step;
	}
	while (t <= Ogre::Math::TWO_PI);
	logMsg("Modify texture with lissajous curve drawing");
}

void Cycloid::_process_paint(long x, long y, Ogre::Real step)
{
	if (mPenSize == 1)
	{
		if (x < 0 || y < 0 || x >= (long)mBuffer->getWidth() || y >= (long)mBuffer->getHeight()) return;
		mBuffer->setPixel(x, y, mColour);
	}
	else
	{
		for (Ogre::Real phi = 0; phi <= Ogre::Math::TWO_PI; phi += step)
		{
			Ogre::Real dx = Ogre::Math::Cos(phi);
			Ogre::Real dy = Ogre::Math::Sin(phi);
			for (Ogre::uint r = 0; r < mPenSize; r++)
			{
				long px = x + (long)Ogre::Math::Floor((Ogre::Real)r * dx + 0.5f);
				long py = y - (long)Ogre::Math::Floor((Ogre::Real)r * dy + 0.5f);
				if (px >= 0 && py >= 0 && px < (long)mBuffer->getWidth() && py < (long)mBuffer->getHeight())
					mBuffer->setPixel(px, py, mColour);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Dilate& Dilate::setIterations(Ogre::uchar iterations)
{
	mIterations = iterations;
	if (mIterations == 0) mIterations = 1;
	return *this;
}

TextureBufferPtr Dilate::process()
{
	long w = (long)mBuffer->getWidth();
	long h = (long)mBuffer->getHeight();
	TextureBufferPtr intBuffer = mBuffer->clone();
	TextureBufferPtr dstBuffer = mBuffer->clone();

	TextureBufferPtr pSrc = NULL;
	TextureBufferPtr pDst = NULL;

	for (long i = 0; i < (long)mIterations; i++)
	{
		if (i == 0)
			pSrc = mBuffer;
		else
		{
			pSrc = ((i % 2) != (mIterations % 2)) ? dstBuffer : intBuffer;
		}
		pDst = ((i % 2) == (mIterations % 2)) ? dstBuffer : intBuffer;

		for (long y = 0; y < h; y++)
		{
			for (long x = 0; x < w; x++)
			{
				long sum = -1;
				pDst->setPixel(x, y, pSrc->getPixel(x, y));

				for (long v = -1; v < 2; v++)
				{
					for (long u = -1; u < 2; u++)
					{
						Ogre::ColourValue pixel = pSrc->getPixel((x + w + u) % w, (y + h + v) % h);
						if ((pixel.r + pixel.g + pixel.b) * 255.0f > sum)
						{
							sum = (long)((pixel.r + pixel.g + pixel.b) * 255.0f);
							pDst->setPixel(x, y, pixel);
						}
					}
				}
			}
		}
	}

	mBuffer->setData(dstBuffer);
	delete intBuffer;
	delete dstBuffer;

	logMsg("Modify texture with dilate filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Distort& Distort::setParameterImage(TextureBufferPtr image)
{
	mParam = image;
	return *this;
}

Distort& Distort::setPower(Ogre::uchar power)
{
	mPower = power;
	return *this;
}

TextureBufferPtr Distort::process()
{
	if (mParam == NULL) return mBuffer;

	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	if (mParam->getWidth() < w || mParam->getHeight() < h) return mBuffer;

	Ogre::Real fPower = (Ogre::Real)mPower;

	for (size_t y = 0; y < h; ++y)
	{
		for (size_t x = 0; x < w; ++x)
		{
			Ogre::ColourValue pixel = mParam->getPixel(x, y);
			Ogre::Vector3 n(pixel.r * 255.0f - 127.0f, pixel.g * 255.0f - 127.0f, pixel.b * 255.0f - 127.0f);
			n.normalise();
			Ogre::Real u = fmod((Ogre::Real)(x + (n.x * fPower)), (Ogre::Real)w);
			Ogre::Real v = fmod((Ogre::Real)(y + (n.y * fPower)), (Ogre::Real)h);
			Ogre::Real uf = (u >= 0) ? (u - (long)u) : 1 + (u - (long)u);
			Ogre::Real vf = (v >= 0) ? (v - (long)v) : 1 + (v - (long)v);
			unsigned long ut = (u >= 0) ? (unsigned long)u : (unsigned long)u - 1;
			unsigned long vt = (v >= 0) ? (unsigned long)v : (unsigned long)v - 1;
			Ogre::ColourValue texel = mBuffer->getPixel(vt % h, ut % w) * (1.0f - uf) * (1.0f - vf);
			texel += mBuffer->getPixel(vt % h, (ut + 1) % w) * uf * (1.0f - vf);
			texel += mBuffer->getPixel((vt + 1) % h, ut % w) * (1.0f - uf) * vf;
			texel += mBuffer->getPixel((vt + 1) % h, (ut + 1) % w) * uf * vf;
			tmpBuffer->setPixel(x, y, texel);
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with distort filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EdgeDetection& EdgeDetection::setThresholdLow(Ogre::uchar threshold)
{
	mThresholdLow = threshold;
	return *this;
}

EdgeDetection& EdgeDetection::setThresholdHigh(Ogre::uchar threshold)
{
	mThresholdHigh = threshold;
	return *this;
}

EdgeDetection& EdgeDetection::setSigma(Ogre::uchar sigma)
{
	mSigma = sigma;
	return *this;
}

EdgeDetection& EdgeDetection::setType(DETECTION_TYPE type)
{
	mType = type;
	return *this;
}

TextureBufferPtr EdgeDetection::process()
{
	Ogre::ColourValue pixel;
	Ogre::Vector3* block;
	Ogre::Vector3 d, v, n;
	// Canny specific
	Ogre::Vector3* orientation;
	Ogre::Vector3* gradients;
	Ogre::Real div;

	TextureBufferPtr tmpBuffer = mBuffer->clone();
	Solid(tmpBuffer).setColour(Ogre::ColourValue::Black).process();

	long w = (long)mBuffer->getWidth();
	long h = (long)mBuffer->getHeight();

	switch (mType)
	{
	default:
	case DETECTION_SOBEL:
		//n = Ogre::Vector3::ZERO;
		for (long y = 0; y < h; y++)
		{
			for (long x = 0; x < w; x++)
			{
				pixel = mBuffer->getPixel((size_t)x, (size_t)y);
				block = getBlock(x, y);
				d = block[0] + 2.0f * block[1] + block[2] - block[6] - 2.0f * block[7] - block[8];
				d = Ogre::Vector3(Ogre::Math::Abs(d.x), Ogre::Math::Abs(d.y), Ogre::Math::Abs(d.z));
				v = block[2] + 2.0f * block[5] + block[8] - block[0] - 2.0f * block[3] - block[6];
				v = Ogre::Vector3(Ogre::Math::Abs(v.x), Ogre::Math::Abs(v.y), Ogre::Math::Abs(v.z));
				d = d + v;
				/*if(d.x > n.x) n.x = d.x;
				if(d.y > n.y) n.y = d.y;
				if(d.z > n.z) n.z = d.z;*/
				delete block;

				tmpBuffer->setPixel((size_t)x, (size_t)y, d.x, d.y, d.z, pixel.a);
			}
		}
		break;

	case DETECTION_DIFFERENCE:
		for (long y = 0; y < h; y++)
		{
			for (long x = 0; x < w; x++)
			{
				pixel = mBuffer->getPixel((size_t)x, (size_t)y);
				block = getBlock(x, y);
				n = Ogre::Vector3::ZERO;
				for (int j = 0; j < 3; j++)
				{
					d = block[j] - block[6 + (2 - j)];
					if (Ogre::Math::Abs(d.x) > n.x) n.x = Ogre::Math::Abs(d.x);
					if (Ogre::Math::Abs(d.y) > n.y) n.y = Ogre::Math::Abs(d.y);
					if (Ogre::Math::Abs(d.z) > n.z) n.z = Ogre::Math::Abs(d.z);
				}
				d = block[5] - block[3];
				if (Ogre::Math::Abs(d.x) > n.x) n.x = Ogre::Math::Abs(d.x);
				if (Ogre::Math::Abs(d.y) > n.y) n.y = Ogre::Math::Abs(d.y);
				if (Ogre::Math::Abs(d.z) > n.z) n.z = Ogre::Math::Abs(d.z);
				delete block;

				tmpBuffer->setPixel((size_t)x, (size_t)y, n.x, n.y, n.z, pixel.a);
			}
		}
		break;

	case DETECTION_HOMOGENITY:
		for (long y = 0; y < h; y++)
		{
			for (long x = 0; x < w; x++)
			{
				pixel = mBuffer->getPixel((size_t)x, (size_t)y);
				block = getBlock(x, y);
				v = block[4];
				n = Ogre::Vector3::ZERO;
				for (int j = 0; j < 3; j++)
				{
					for (int i = 0; i < 3; i++)
					{
						if (j == 1 && i == 1) continue;
						d = v - block[j * 3 + i];
						if (Ogre::Math::Abs(d.x) > n.x) n.x = Ogre::Math::Abs(d.x);
						if (Ogre::Math::Abs(d.y) > n.y) n.y = Ogre::Math::Abs(d.y);
						if (Ogre::Math::Abs(d.z) > n.z) n.z = Ogre::Math::Abs(d.z);
					}
				}
				delete block;
				tmpBuffer->setPixel((size_t)x, (size_t)y, n.x, n.y, n.z, pixel.a);
			}
		}
		break;

	case DETECTION_CANNY:
		// STEP 1 - blur image
		Blur(mBuffer).setSigma(mSigma).setType(Blur::BLUR_GAUSSIAN).process();

		// STEP 2 - calculate magnitude and edge orientation
		orientation = new Ogre::Vector3[w * h];
		gradients = new Ogre::Vector3[w * h];
		n = Ogre::Vector3(-std::numeric_limits<Ogre::Real>::infinity(), -std::numeric_limits<Ogre::Real>::infinity(), -std::numeric_limits<Ogre::Real>::infinity());
		for (long y = 0; y < h; y++)
		{
			for (long x = 0; x < w; x++)
			{
				pixel = mBuffer->getPixel((size_t)x, (size_t)y);
				block = getBlock(x, y);
				d = block[2] + block[8] - block[0] - block[6] + 2.0f * (block[5] - block[3]);
				v = block[0] + block[2] - block[6] - block[8] + 2.0f * (block[1] - block[7]);
				gradients[y * w + x] = Ogre::Vector3(Ogre::Math::Sqrt(d.x * d.x + v.x * v.x), Ogre::Math::Sqrt(d.y * d.y + v.y * v.y), Ogre::Math::Sqrt(d.z * d.z + v.z * v.z));
				if (gradients[y * w + x].x > n.x) n.x = gradients[y * w + x].x;
				if (gradients[y * w + x].y > n.y) n.y = gradients[y * w + x].y;
				if (gradients[y * w + x].z > n.z) n.z = gradients[y * w + x].z;
				delete block;
				orientation[y * w + x] = Ogre::Vector3::ZERO;
				if (d.x == 0.0f)
				{
					orientation[y * w + x].x = (v.x == 0.0f) ? 0.0f : 90.0f;
				}
				else
				{
					div = v.x / d.x;
					if (div < 0.0f)
						orientation[y * w + x].x = 180.0f - Ogre::Math::ATan(-div).valueDegrees();
					else
						orientation[y * w + x].x = Ogre::Math::ATan(div).valueDegrees();

					if (orientation[y * w + x].x < 22.5f)
						orientation[y * w + x].x = 0.0f;
					else if (orientation[y * w + x].x < 67.5f)
						orientation[y * w + x].x = 45.0f;
					else if (orientation[y * w + x].x < 112.5f)
						orientation[y * w + x].x = 90.0f;
					else if (orientation[y * w + x].x < 157.5f)
						orientation[y * w + x].x = 135.0f;
					else
						orientation[y * w + x].x = 0.0f;
				}
				if (d.y == 0.0f)
				{
					orientation[y * w + x].y = (v.y == 0.0f) ? 0.0f : 90.0f;
				}
				else
				{
					div = v.y / d.y;
					if (div < 0.0f)
						orientation[y * w + x].y = 180.0f - Ogre::Math::ATan(-div).valueDegrees();
					else
						orientation[y * w + x].y = Ogre::Math::ATan(div).valueDegrees();

					if (orientation[y * w + x].y < 22.5f)
						orientation[y * w + x].y = 0.0f;
					else if (orientation[y * w + x].y < 67.5f)
						orientation[y * w + x].y = 45.0f;
					else if (orientation[y * w + x].y < 112.5f)
						orientation[y * w + x].y = 90.0f;
					else if (orientation[y * w + x].y < 157.5f)
						orientation[y * w + x].y = 135.0f;
					else
						orientation[y * w + x].y = 0.0f;
				}
				if (d.z == 0.0f)
				{
					orientation[y * w + x].z = (v.z == 0.0f) ? 0.0f : 90.0f;
				}
				else
				{
					div = v.z / d.z;
					if (div < 0.0f)
						orientation[y * w + x].z = 180.0f - Ogre::Math::ATan(-div).valueDegrees();
					else
						orientation[y * w + x].z = Ogre::Math::ATan(div).valueDegrees();

					if (orientation[y * w + x].z < 22.5f)
						orientation[y * w + x].z = 0.0f;
					else if (orientation[y * w + x].z < 67.5f)
						orientation[y * w + x].z = 45.0f;
					else if (orientation[y * w + x].z < 112.5f)
						orientation[y * w + x].z = 90.0f;
					else if (orientation[y * w + x].z < 157.5f)
						orientation[y * w + x].z = 135.0f;
					else
						orientation[y * w + x].z = 0.0f;
				}
			}
		}

		// STEP 3 - suppres non maximums
		for (long y = 1; y < (h - 1); y++)
		{
			for (long x = 1; x < (w - 1); x++)
			{
				div = gradients[y * w + x].x / n.x;
				switch (((int)orientation[y * w + x].x))
				{
				default:
				case 0:
					if ((gradients[y * w + x].x < gradients[y * w + (x - 1)].x) || (gradients[y * w + x].x < gradients[y * w + (x + 1)].x))
						div = 0.0f;
					break;
				case 45:
					if ((gradients[y * w + x].x < gradients[(y + 1) * w + (x - 1)].x) || (gradients[y * w + x].x < gradients[(y - 1) * w + (x + 1)].x))
						div = 0.0f;
					break;
				case 90:
					if ((gradients[y * w + x].x < gradients[(y + 1) * w + x].x) || (gradients[y * w + x].x < gradients[(y - 1) * w + x].x))
						div = 0.0f;
					break;
				case 135:
					if ((gradients[y * w + x].x < gradients[(y + 1) * w + (x + 1)].x) || (gradients[y * w + x].x < gradients[(y - 1) * w + (x - 1)].x))
						div = 0.0f;
					break;
				}
				tmpBuffer->setRed((size_t)x, (size_t)y, div);
				div = gradients[y * w + x].y / n.y;
				switch (((int)orientation[y * w + x].y))
				{
				default:
				case 0:
					if ((gradients[y * w + x].y < gradients[y * w + (x - 1)].y) || (gradients[y * w + x].y < gradients[y * w + (x + 1)].y))
						div = 0.0f;
					break;
				case 45:
					if ((gradients[y * w + x].y < gradients[(y + 1) * w + (x - 1)].y) || (gradients[y * w + x].y < gradients[(y - 1) * w + (x + 1)].y))
						div = 0.0f;
					break;
				case 90:
					if ((gradients[y * w + x].y < gradients[(y + 1) * w + x].y) || (gradients[y * w + x].y < gradients[(y - 1) * w + x].y))
						div = 0.0f;
					break;
				case 135:
					if ((gradients[y * w + x].y < gradients[(y + 1) * w + (x + 1)].y) || (gradients[y * w + x].y < gradients[(y - 1) * w + (x - 1)].y))
						div = 0.0f;
					break;
				}
				tmpBuffer->setGreen((size_t)x, (size_t)y, div);
				div = gradients[y * w + x].z / n.z;
				switch (((int)orientation[y * w + x].z))
				{
				default:
				case 0:
					if ((gradients[y * w + x].z < gradients[y * w + (x - 1)].z) || (gradients[y * w + x].z < gradients[y * w + (x + 1)].z))
						div = 0.0f;
					break;
				case 45:
					if ((gradients[y * w + x].z < gradients[(y + 1) * w + (x - 1)].z) || (gradients[y * w + x].z < gradients[(y - 1) * w + (x + 1)].z))
						div = 0.0f;
					break;
				case 90:
					if ((gradients[y * w + x].z < gradients[(y + 1) * w + x].z) || (gradients[y * w + x].z < gradients[(y - 1) * w + x].z))
						div = 0.0f;
					break;
				case 135:
					if ((gradients[y * w + x].z < gradients[(y + 1) * w + (x + 1)].z) || (gradients[y * w + x].z < gradients[(y - 1) * w + (x - 1)].z))
						div = 0.0f;
					break;
				}
				tmpBuffer->setBlue((size_t)x, (size_t)y, div);
			}
		}

		// STEP 4 - hysteresis
		mBuffer->setData(tmpBuffer);
		div = (Ogre::Real)mThresholdHigh / 255.0f;
		for (long y = 1; y < (h - 1); y++)
		{
			for (long x = 1; x < (w - 1); x++)
			{
				pixel = mBuffer->getPixel((size_t)x, (size_t)y);
				if (pixel.r < div)
				{
					if (pixel.r < (Ogre::Real)mThresholdLow / 255.0f)
						tmpBuffer->setRed((size_t)x, (size_t)y, 0.0f);
					else
					{
						if ( (mBuffer->getPixelRedReal((size_t)(x - 1), (size_t)(y    )) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x + 1), (size_t)(y    )) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x - 1), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x    ), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x + 1), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x - 1), (size_t)(y + 1)) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x    ), (size_t)(y + 1)) < div) &&
								(mBuffer->getPixelRedReal((size_t)(x + 1), (size_t)(y + 1)) < div))
						{
							tmpBuffer->setRed((size_t)x, (size_t)y, 0.0f);
						}
					}
				}
				if (pixel.g < div)
				{
					if (pixel.g < (Ogre::Real)mThresholdLow / 255.0f)
						tmpBuffer->setGreen((size_t)x, (size_t)y, 0.0f);
					else
					{
						if ( (mBuffer->getPixelGreenReal((size_t)(x - 1), (size_t)(y    )) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x + 1), (size_t)(y    )) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x - 1), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x    ), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x + 1), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x - 1), (size_t)(y + 1)) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x    ), (size_t)(y + 1)) < div) &&
								(mBuffer->getPixelGreenReal((size_t)(x + 1), (size_t)(y + 1)) < div))
						{
							tmpBuffer->setGreen((size_t)x, (size_t)y, 0.0f);
						}
					}
				}
				if (pixel.b < div)
				{
					if (pixel.b < (Ogre::Real)mThresholdLow / 255.0f)
						tmpBuffer->setBlue((size_t)x, (size_t)y, 0.0f);
					else
					{
						if ( (mBuffer->getPixelBlueReal((size_t)(x - 1), (size_t)(y    )) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x + 1), (size_t)(y    )) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x - 1), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x    ), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x + 1), (size_t)(y - 1)) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x - 1), (size_t)(y + 1)) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x    ), (size_t)(y + 1)) < div) &&
								(mBuffer->getPixelBlueReal((size_t)(x + 1), (size_t)(y + 1)) < div))
						{
							tmpBuffer->setBlue((size_t)x, (size_t)y, 0.0f);
						}
					}
				}
			}
		}

		delete orientation;
		delete gradients;
		break;
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with edgedetection filter : " + StringConverter::toString(mType));
	return mBuffer;
}

Ogre::Vector3* EdgeDetection::getBlock(long x, long y)
{
	Ogre::ColourValue pixel = mBuffer->getPixel((size_t)x, (size_t)y);
	Ogre::Vector3* block = new Ogre::Vector3[9];
	for (int j = -1; j < 2; j++)
	{
		for (int i = -1; i < 2; i++)
		{
			block[(j + 1) * 3 + (i + 1)] = Ogre::Vector3(pixel.r, pixel.g, pixel.b);
			if (j == 0 && i == 0) continue;
			if ((x + i) < 0 || (x + i) >= (long)mBuffer->getWidth()) continue;
			if ((y + j) < 0 || (y + j) >= (long)mBuffer->getHeight()) continue;
			block[(j + 1) * 3 + (i + 1)] = Ogre::Vector3((Ogre::Real)mBuffer->getPixelRedReal((size_t)(x + i), (size_t)(y + j)),
										   (Ogre::Real)mBuffer->getPixelGreenReal((size_t)(x + i), (size_t)(y + j)),
										   (Ogre::Real)mBuffer->getPixelBlueReal((size_t)(x + i), (size_t)(y + j)));
		}
	}
	return block;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EllipseTexture& EllipseTexture::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

EllipseTexture& EllipseTexture::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

EllipseTexture& EllipseTexture::setRadiusX(size_t radiusx)
{
	mRadiusX = radiusx;
	return *this;
}

EllipseTexture& EllipseTexture::setRadiusX(Ogre::Real radiusx)
{
	mRadiusX = (size_t)((Ogre::Real)std::min<Ogre::uint>(mBuffer->getWidth(), mBuffer->getHeight()) * Ogre::Math::Abs(radiusx));
	return *this;
}

EllipseTexture& EllipseTexture::setRadiusY(size_t radiusy)
{
	mRadiusY = radiusy;
	return *this;
}

EllipseTexture& EllipseTexture::setRadiusY(Ogre::Real radiusy)
{
	mRadiusY = (size_t)((Ogre::Real)std::min<Ogre::uint>(mBuffer->getWidth(), mBuffer->getHeight()) * Ogre::Math::Abs(radiusy));
	return *this;
}

EllipseTexture& EllipseTexture::setRadius(size_t radiusx, size_t radiusy)
{
	mRadiusX = radiusx;
	mRadiusY = radiusy;
	return *this;
}

EllipseTexture& EllipseTexture::setRadius(Ogre::Real radiusx, Ogre::Real radiusy)
{
	mRadiusX = (size_t)((Ogre::Real)std::min<Ogre::uint>(mBuffer->getWidth(), mBuffer->getHeight()) * Ogre::Math::Abs(radiusx));
	mRadiusY = (size_t)((Ogre::Real)std::min<Ogre::uint>(mBuffer->getWidth(), mBuffer->getHeight()) * Ogre::Math::Abs(radiusy));
	return *this;
}

EllipseTexture& EllipseTexture::setCenterX(size_t x)
{
	mX = std::min<size_t>(x, mBuffer->getWidth() - 1);
	return *this;
}

EllipseTexture& EllipseTexture::setCenterX(Ogre::Real x)
{
	mX = std::min<size_t>((size_t)(x * (Ogre::Real)mBuffer->getWidth()), mBuffer->getWidth() - 1);
	return *this;
}

EllipseTexture& EllipseTexture::setCenterY(size_t y)
{
	mY = std::min<size_t>(y, mBuffer->getHeight() - 1);
	return *this;
}

EllipseTexture& EllipseTexture::setCenterY(Ogre::Real y)
{
	mY = std::min<size_t>((size_t)(y * (Ogre::Real)mBuffer->getHeight()), mBuffer->getHeight() - 1);
	return *this;
}

EllipseTexture& EllipseTexture::setCenter(Ogre::Vector2 pos, bool relative)
{
	setCenter(pos.x, pos.y, relative);
	return *this;
}

EllipseTexture& EllipseTexture::setCenter(size_t x, size_t y)
{
	setCenterX(x);
	setCenterY(y);
	return *this;
}

EllipseTexture& EllipseTexture::setCenter(Ogre::Real x, Ogre::Real y, bool relative)
{
	if (relative)
	{
		setCenterX(x);
		setCenterY(y);
	}
	else
	{
		setCenterX((size_t)x);
		setCenterY((size_t)y);
	}
	return *this;
}

TextureBufferPtr EllipseTexture::process()
{
	long dx = 0;
	long dy = mRadiusY;
	long rx2 = mRadiusX * mRadiusX;
	long ry2 = mRadiusY * mRadiusY;
	long err = ry2 - (2 * mRadiusY - 1)* rx2;
	long e2 = 0;

	do
	{
		for (long qy = -dy; qy <= dy; qy++)
		{
			_putpixel(+dx, qy);
			_putpixel(-dx, qy);
		}

		e2 = 2*err;
		if (e2 <  (2 * dx + 1) * ry2)
		{
			dx++;
			err += (2 * dx + 1) * ry2;
		}
		if (e2 > -(2 * dy - 1) * rx2)
		{
			dy--;
			err -= (2 * dy - 1) * rx2;
		}
	}
	while (dy >= 0);

	while (dx++ < (long)mRadiusX)
	{
		_putpixel(+dx, 0);
		_putpixel(-dx, 0);
	}

	logMsg("Modify texture with ellipse filter : x = " + StringConverter::toString(mX) + ", y = " + StringConverter::toString(mY) + ", Radius x = " + StringConverter::toString(mRadiusX) + ", Radius y = " + StringConverter::toString(mRadiusY));
	return mBuffer;
}

void EllipseTexture::_putpixel(long dx, long dy)
{
	if (mX + dx < 0 || mX + dx >= mBuffer->getWidth()) return;
	if (mY + dy < 0 || mY + dy >= mBuffer->getHeight()) return;
	mBuffer->setPixel(mX + dx, mY + dy, mColour);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Flip& Flip::setAxis(FLIP_AXIS axis)
{
	mAxis = axis;
	return *this;
}

TextureBufferPtr Flip::process()
{
	TextureBufferPtr tmpBuffer = mBuffer->clone();
	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			switch (mAxis)
			{
			case FLIP_HORIZONTAL:
				tmpBuffer->setPixel(x, mBuffer->getHeight() - 1 - y, mBuffer->getPixel(x, y));
				break;

			default:
			case FLIP_VERTICAL:
				tmpBuffer->setPixel(mBuffer->getWidth() - 1 - x, y, mBuffer->getPixel(x, y));
				break;

			case FLIP_POINT:
				tmpBuffer->setPixel(mBuffer->getWidth() - 1 - x, mBuffer->getHeight() - 1 - y, mBuffer->getPixel(x, y));
				break;
			}
		}
	}
	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with flip filter : " + StringConverter::toString(mAxis));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Glow& Glow::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Glow& Glow::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Glow& Glow::setCenterX(Ogre::Real centerx)
{
	mCenterX = centerx;
	return *this;
}

Glow& Glow::setCenterY(Ogre::Real centery)
{
	mCenterY = centery;
	return *this;
}

Glow& Glow::setRadiusX(Ogre::Real radiusx)
{
	mRadiusX = radiusx;
	return *this;
}

Glow& Glow::setRadiusY(Ogre::Real radiusy)
{
	mRadiusY = radiusy;
	return *this;
}

Glow& Glow::setAlpha(Ogre::Real alpha)
{
	mAlpha = alpha;
	return *this;
}

Glow& Glow::setGamma(Ogre::Real gamma)
{
	mGamma = gamma;
	return *this;
}

TextureBufferPtr Glow::process()
{
	long w = (long)mBuffer->getWidth();
	long h = (long)mBuffer->getHeight();
	long dwCenterX	= (long)(mCenterX * (Ogre::Real)w);
	long dwCenterY	= (long)(mCenterY * (Ogre::Real)h);
	long dwRadiusX	= (long)(mRadiusX * (Ogre::Real)w);
	long dwRadiusY	= (long)(mRadiusY * (Ogre::Real)h);
	Ogre::Real fRed = mColour.r * 255.0f;
	Ogre::Real fGreen = mColour.g * 255.0f;
	Ogre::Real fBlue = mColour.b * 255.0f;
	Ogre::Real f1_RadiusX = 1.0f / (Ogre::Real)dwRadiusX;
	Ogre::Real f1_RadiusY = 1.0f / (Ogre::Real)dwRadiusY;

	for (long y = 0; y < h; y++)
	{
		Ogre::Real dy = (Ogre::Real)(y - dwCenterY) * f1_RadiusY;

		for (long x = 0; x < w; x++)
		{
			Ogre::Real dx = (Ogre::Real)(x - dwCenterX) * f1_RadiusX;
			Ogre::Real d = sqrt(dx * dx + dy * dy);
			if (d > 1.0f) d = 1.0f;
			d = 1.0f - d;

			unsigned long r = (unsigned long)(((Ogre::Real)mBuffer->getPixelRedByte(x, y)) + ((mGamma * d * fRed) * mAlpha));
			unsigned long g = (unsigned long)(((Ogre::Real)mBuffer->getPixelGreenByte(x, y)) + ((mGamma * d * fGreen) * mAlpha));
			unsigned long b = (unsigned long)(((Ogre::Real)mBuffer->getPixelBlueByte(x, y)) + ((mGamma * d * fBlue) * mAlpha));
			Ogre::uchar a = mBuffer->getPixelAlphaByte(x, y);

			mBuffer->setPixel(x, y, (Ogre::uchar)((r < 255) ? r : 255), (Ogre::uchar)((g < 255) ? g : 255), (Ogre::uchar)((b < 255) ? b : 255), a);
		}
	}

	logMsg("Modify texture with glow filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextureBufferPtr Invert::process()
{
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::ColourValue pixel = mBuffer->getPixel(x, y);
			mBuffer->setPixel(x, y, 1.0f - pixel.r, 1.0f - pixel.g, 1.0f - pixel.b, pixel.a);
		}
	}

	logMsg("Modify texture with invert filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Jitter& Jitter::setRadius(Ogre::uchar radius)
{
	mRadius = radius;
	return *this;
}

Jitter& Jitter::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

TextureBufferPtr Jitter::process()
{
	TextureBufferPtr tmpBuffer = mBuffer->clone();
	srand(mSeed);
	int radius = (int)(1.0f + (9.0f / 255.0f) * ((Ogre::Real)mRadius - 1.0f));
	int max = radius * 2 + 1;
	for (long y = 0; y < (long)mBuffer->getHeight(); y++)
	{
		for (long x = 0; x < (long)mBuffer->getWidth(); x++)
		{
			long rx = x + (rand() % (radius * 2 + 1)) - radius;
			long ry = y + (rand() % (radius * 2 + 1)) - radius;

			if (rx >= 0 && rx < (long)mBuffer->getWidth() && ry >= 0 && ry < (long)mBuffer->getHeight())
				tmpBuffer->setPixel((size_t)rx, (size_t)ry, mBuffer->getPixel((size_t)x, (size_t)y));
		}
	}
	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with jitter filter : " + StringConverter::toString(mRadius));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Lerp& Lerp::setImageA(TextureBufferPtr image1)
{
	mBufferA = image1;
	return *this;
}

Lerp& Lerp::setImageB(TextureBufferPtr image2)
{
	mBufferB = image2;
	return *this;
}

TextureBufferPtr Lerp::process()
{
	for (size_t y = 0; y < mBuffer->getHeight(); y++)
	{
		for (size_t x = 0; x < mBuffer->getWidth(); x++)
		{
			Ogre::ColourValue pixelA = mBufferA->getPixel(x, y);
			Ogre::ColourValue pixelB = mBufferB->getPixel(x, y);
			Ogre::ColourValue pixelC = mBuffer->getPixel(x, y);

			mBuffer->setPixel(x, y,
							  pixelA.r * (1.0f - pixelC.r) + pixelB.r * pixelC.r,
							  pixelA.g * (1.0f - pixelC.g) + pixelB.g * pixelC.g,
							  pixelA.b * (1.0f - pixelC.b) + pixelB.b * pixelC.b,
							  pixelA.a * (1.0f - pixelC.a) + pixelB.a * pixelC.a);
		}
	}

	logMsg("Modify texture with lerp filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextureLightBaker& TextureLightBaker::setNormalMap(TextureBufferPtr normal)
{
	mNormal = normal;
	return *this;
}

TextureLightBaker& TextureLightBaker::setColourAmbient(Ogre::ColourValue colour)
{
	mColourAmbient = colour;
	return *this;
}

TextureLightBaker& TextureLightBaker::setColourAmbient(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourAmbient = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureLightBaker& TextureLightBaker::setColourDiffuse(Ogre::ColourValue colour)
{
	mColourDiffuse = colour;
	return *this;
}

TextureLightBaker& TextureLightBaker::setColourDiffuse(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourDiffuse = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureLightBaker& TextureLightBaker::setColourSpecular(Ogre::ColourValue colour)
{
	mColourSpecular = colour;
	return *this;
}

TextureLightBaker& TextureLightBaker::setColourSpecular(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColourSpecular = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureLightBaker& TextureLightBaker::setColours(Ogre::ColourValue ambient, Ogre::ColourValue diffuse, Ogre::ColourValue specular)
{
	mColourAmbient = ambient;
	mColourDiffuse = diffuse;
	mColourSpecular = specular;
	return *this;
}

TextureLightBaker& TextureLightBaker::setPosition(Ogre::Vector3 position)
{
	return setPosition(position.x, position.y, position.z);
}

TextureLightBaker& TextureLightBaker::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mPosition = Ogre::Vector3(std::max<Ogre::Real>(std::min<Ogre::Real>(x, 255.0f), 0.0f), std::max<Ogre::Real>(std::min<Ogre::Real>(y, 255.0f), 0.0f), std::max<Ogre::Real>(std::min<Ogre::Real>(z, 255.0f), 0.0f));
	return *this;
}

TextureLightBaker& TextureLightBaker::setSpecularPower(Ogre::uchar power)
{
	mSpecularPower = power;
	return *this;
}

TextureLightBaker& TextureLightBaker::setBumpPower(Ogre::uchar power)
{
	mBumpPower = power;
	return *this;
}

TextureBufferPtr TextureLightBaker::process()
{
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();
	Ogre::Vector3 light(mPosition.x - 127.0f, -(mPosition.y - 127.0f), -(127.0f - mPosition.z));
	light.normalise();
	Ogre::Real fSpecularPower = ((Ogre::Real)mSpecularPower) / 32.0f;
	Ogre::Real fBumpPower = ((Ogre::Real)mBumpPower) / 32.0f;

	if (mNormal != NULL && (mNormal->getWidth() < w || mNormal->getHeight() < h)) return mBuffer;

	TextureBufferPtr normalMap;
	if (mNormal != NULL)
		normalMap = mNormal->clone();
	else
	{
		normalMap = mBuffer->clone();
		Normals(normalMap).process();
	}

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::ColourValue pixel = normalMap->getPixel(x, y);
			Ogre::Vector3 n(pixel.r * 255.0f - 127.0f, pixel.g * 255.0f - 127.0f, pixel.b * 255.0f - 127.0f);
			n.normalise();

			Ogre::Real fdot = n.x * light.x + n.y * light.y + n.z * light.z;
			if (fdot < 0.0f) fdot = 0.0f;
			fdot *= fBumpPower;

			long r = (long)(mColourAmbient.r * 255.0f + (fdot * mColourDiffuse.r * 255.0f) + (fdot * fdot * mColourSpecular.r * fSpecularPower));
			long g = (long)(mColourAmbient.g * 255.0f + (fdot * mColourDiffuse.g * 255.0f) + (fdot * fdot * mColourSpecular.g * fSpecularPower));
			long b = (long)(mColourAmbient.b * 255.0f + (fdot * mColourDiffuse.b * 255.0f) + (fdot * fdot * mColourSpecular.b * fSpecularPower));

			pixel = mBuffer->getPixel(x, y);
			r = ((long)(pixel.r * 255.0f) + r) / 2;
			g = ((long)(pixel.g * 255.0f) + g) / 2;
			b = ((long)(pixel.b * 255.0f) + b) / 2;

			mBuffer->setPixel(x, y, (Ogre::uchar)((r < 255) ? r : 255), (Ogre::uchar)((g < 255) ? g : 255), (Ogre::uchar)((b < 255) ? b : 255), (Ogre::uchar)(pixel.a * 255.0f));
		}
	}

	delete normalMap;
	logMsg("Modify texture with light filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Lookup& Lookup::setParameterImage(TextureBufferPtr image)
{
	mParam = image;
	return *this;
}

TextureBufferPtr Lookup::process()
{
	if (mParam == NULL) return mBuffer;

	size_t tw = mBuffer->getWidth();
	size_t th = mBuffer->getHeight();
	size_t w = mParam->getWidth();
	size_t h = mParam->getHeight();
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	if (w < tw || h < th) return mBuffer;

	Ogre::Real scaleW = tw / 256.0f;
	Ogre::Real scaleH = th / 256.0f;

	for (size_t y = 0; y < h; ++y)
	{
		for (size_t x = 0; x < w; ++x)
		{
			Ogre::ColourValue pixel = mParam->getPixel(x, y);
			size_t u = (size_t)(pixel.r * (Ogre::Real)w);
			size_t v = (size_t)(pixel.g * (Ogre::Real)h);
			u = Math::Clamp<size_t>(u, 0, w-1);
			v = Math::Clamp<size_t>(v, 0, h-1);
			tmpBuffer->setPixel(x, y, mBuffer->getPixel(v, u));
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with lookup filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Normals& Normals::setAmplify(Ogre::uchar amplify)
{
	mAmplify = amplify;
	return *this;
}

TextureBufferPtr Normals::process()
{
	long w = (long)mBuffer->getWidth();
	long h = (long)mBuffer->getHeight();
	Ogre::Real fAmp = (Ogre::Real)mAmplify * 4.0f / 255.0f;
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	for (long y = 0; y < h; y++)
	{
		for (long x = 0; x < w; x++)
		{
			size_t xp = (x < 1) ? 0 : (x - 1) % w;
			size_t xn = (x + 1) % w;
			size_t yp = (y < 1) ? 0 : (y - 1) % h;
			size_t yn = (y + 1) % h;

			//Y Sobel filter
			Ogre::Real fPix = (Ogre::Real)mBuffer->getPixelRedByte(xp, yn);
			Ogre::Real dY  = fPix * -1.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(x, yn);
			dY += fPix * -2.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xn, yn);
			dY += fPix * -1.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xp, yp);
			dY += fPix * 1.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(x, yp);
			dY += fPix * 2.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xn, yp);
			dY += fPix * 1.0f;

			//X Sobel filter
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xp, yp);
			Ogre::Real dX  = fPix * -1.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xp, y);
			dX += fPix * -2.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xp, yn);
			dX += fPix * -1.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xn, yp);
			dX += fPix * 1.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xn, y);
			dX += fPix * 2.0f;
			fPix = (Ogre::Real)mBuffer->getPixelRedByte(xn, yn);
			dX += fPix * 1.0f;

			Ogre::Real normx = -dX * fAmp / 255.0f;
			Ogre::Real normy = -dY * fAmp / 255.0f;
			Ogre::Real norm = Ogre::Math::Sqrt(normx * normx + normy * normy + 1.0f);
			if (norm > (float)10e-6)
				norm = 1.0f / norm;
			else
				norm = 0.0f;
			normx = (normx * norm) * 127.0f + 128.0f;
			normy = (normy * norm) * 127.0f + 128.0f;
			Ogre::Real normz = norm * 127.0f + 128.0f;

			tmpBuffer->setPixel(x, y, (Ogre::uchar)normx, (Ogre::uchar)normy, (Ogre::uchar)normz, mBuffer->getPixelAlphaByte(x, y));
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with normals filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OilPaint& OilPaint::setRadius(Ogre::uchar radius)
{
	mRadius = radius;
	if (mRadius < 3) mRadius = 3;
	return *this;
}

OilPaint& OilPaint::setIntensity(Ogre::Real intensity)
{
	mIntensity = intensity;
	return *this;
}

TextureBufferPtr OilPaint::process()
{
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	int intensities[256];
	int red[256];
	int green[256];
	int blue[256];

	for (int y = mRadius; y < (int)(mBuffer->getHeight() - mRadius); y++)
	{
		for (int x = mRadius; x < (int)(mBuffer->getWidth() - mRadius); x++)
		{
			memset(intensities, 0, 256 * sizeof(int));
			memset(red, 0, sizeof(red));
			memset(green, 0, sizeof(green));
			memset(blue, 0, sizeof(blue));

			for (int j = -mRadius; j <= mRadius; j++)
			{
				for (int i = -mRadius; i <= mRadius; i++)
				{
					int r = mBuffer->getPixelRedByte((size_t)(x + i), (size_t)(y + j));
					int g = mBuffer->getPixelGreenByte((size_t)(x + i), (size_t)(y + j));
					int b = mBuffer->getPixelBlueByte((size_t)(x + i), (size_t)(y + j));

					int curr =  (int)((((Ogre::Real)(r + g + b) / 3.0f) * mIntensity) / 255.0f);
					if (curr > 255) curr = 255;
					intensities[curr]++;

					red[curr] += r;
					green[curr] += g;
					blue[curr] += b;
				}
			}

			int maxInt = 0;
			int maxIndex = 0;
			for (int i = 0; i < 256; i++)
			{
				if (intensities[i] > maxInt)
				{
					maxInt = intensities[i];
					maxIndex = i;
				}
			}

			tmpBuffer->setPixel((size_t)x, (size_t)y, (Ogre::uchar)(red[maxIndex] / maxInt), (Ogre::uchar)(green[maxIndex] / maxInt), (Ogre::uchar)(blue[maxIndex] / maxInt));
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with oilpaint filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Polygon& Polygon::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

Polygon& Polygon::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

Polygon& Polygon::setRadius(size_t radius)
{
	mRadius = radius;
	return *this;
}

Polygon& Polygon::setRadius(Ogre::Real radius)
{
	mRadius = (size_t)((Ogre::Real)std::min<Ogre::uint>(mBuffer->getWidth(), mBuffer->getHeight()) * Ogre::Math::Abs(radius));
	return *this;
}

Polygon& Polygon::setCenterX(size_t x)
{
	mX = std::min<size_t>(x, mBuffer->getWidth() - 1);
	return *this;
}

Polygon& Polygon::setCenterX(Ogre::Real x)
{
	mX = std::min<size_t>((size_t)(x * (Ogre::Real)mBuffer->getWidth()), mBuffer->getWidth() - 1);
	return *this;
}

Polygon& Polygon::setCenterY(size_t y)
{
	mY = std::min<size_t>(y, mBuffer->getHeight() - 1);
	return *this;
}

Polygon& Polygon::setCenterY(Ogre::Real y)
{
	mY = std::min<size_t>((size_t)(y * (Ogre::Real)mBuffer->getHeight()), mBuffer->getHeight() - 1);
	return *this;
}

Polygon& Polygon::setCenter(Ogre::Vector2 pos, bool relative)
{
	setCenter(pos.x, pos.y, relative);
	return *this;
}

Polygon& Polygon::setCenter(size_t x, size_t y)
{
	setCenterX(x);
	setCenterY(y);
	return *this;
}

Polygon& Polygon::setCenter(Ogre::Real x, Ogre::Real y, bool relative)
{
	if (relative)
	{
		setCenterX(x);
		setCenterY(y);
	}
	else
	{
		setCenterX((size_t)x);
		setCenterY((size_t)y);
	}
	return *this;
}

Polygon& Polygon::setSides(size_t n)
{
	mSides = n;
	return *this;
}

TextureBufferPtr Polygon::process()
{
	Ogre::Real deltaAngle = (Math::TWO_PI / (Ogre::Real)mSides);
	for(size_t ui = 0; ui < mSides; ui++)
	{
		Ogre::Vector2 p0(mRadius * Math::Cos((Ogre::Real)ui * deltaAngle), mRadius * Math::Sin((Ogre::Real)ui * deltaAngle));
		Ogre::Vector2 p1(mRadius * Math::Cos((Ogre::Real)(ui + 1 == mSides ? 0 : ui + 1) * deltaAngle), mRadius * Math::Sin((Ogre::Real)(ui + 1 == mSides ? 0 : ui + 1) * deltaAngle));

		long x0 = (long)p0.x, y0 = (long)p0.y;
		long x1 = (long)p1.x, y1 = (long)p1.y;
		long dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		long dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		long err = dx + dy, e2 = 0;
		for(;;)
		{
			_putpixel(x0, y0);
			if (x0 == x1 && y0 == y1) break;
			e2 = 2 * err;
			if (e2 > dy) { err += dy; x0 += sx; }
			if (e2 < dx) { err += dx; y0 += sy; }
		}
	}

	logMsg("Modify texture with polygon filter : x = " + StringConverter::toString(mX) + ", y = " + StringConverter::toString(mY) + ", Radius = " + StringConverter::toString(mRadius) + ", Sides = " + StringConverter::toString(mSides));
	return mBuffer;
}

void Polygon::_putpixel(long dx, long dy)
{
	if (mX + dx < 0 || mX + dx >= mBuffer->getWidth()) return;
	if (mY + dy < 0 || mY + dy >= mBuffer->getHeight()) return;
	mBuffer->setPixel(mX + dx, mY + dy, mColour);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RandomPixels& RandomPixels::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

RandomPixels& RandomPixels::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

RandomPixels& RandomPixels::setSeed(Ogre::uint seed)
{
	mSeed = seed;
	return *this;
}

RandomPixels& RandomPixels::setCount(Ogre::uint count)
{
	mCount = count;
	size_t area = mBuffer->getWidth() * mBuffer->getHeight();
	if (mCount > area) mCount = (size_t)(0.9f * (Ogre::Real)area);
	return *this;
}

TextureBufferPtr RandomPixels::process()
{
	IntVector2 pt;
	std::vector<IntVector2> list;

	srand(mSeed);
	size_t area = mBuffer->getWidth() * mBuffer->getHeight();
	if (mCount == area)
		RectangleTexture(mBuffer).setColour(mColour).process();
	else
	{
		while (list.size() != mCount)
		{
			pt.x = rand() % mBuffer->getWidth();
			pt.y = rand() % mBuffer->getHeight();

			bool bInList = false;
			for (std::vector<IntVector2>::iterator iter = list.begin(); iter != list.end(); iter++)
				if (iter->x == pt.x && iter->y == pt.y)
				{
					bInList = true;
					break;
				}

			if (!bInList)
			{
				list.push_back(pt);
				mBuffer->setPixel(pt.x, pt.y, mColour);
			}
		}
	}

	logMsg("Modify texture with randompixel filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RectangleTexture& RectangleTexture::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

RectangleTexture& RectangleTexture::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

RectangleTexture& RectangleTexture::setX1(size_t x1)
{
	mX1 = std::min<size_t>(x1, mBuffer->getWidth());
	return *this;
}

RectangleTexture& RectangleTexture::setX1(Ogre::Real x1)
{
	mX1 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(x1, 1.0f));
	return *this;
}

RectangleTexture& RectangleTexture::setY1(size_t y1)
{
	mY1 = std::min<size_t>(y1, mBuffer->getHeight());
	return *this;
}

RectangleTexture& RectangleTexture::setY1(Ogre::Real y1)
{
	mY1 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(y1, 1.0f));
	return *this;
}

RectangleTexture& RectangleTexture::setX2(size_t x2)
{
	mX2 = std::min<size_t>(x2, mBuffer->getWidth());
	return *this;
}

RectangleTexture& RectangleTexture::setX2(Ogre::Real x2)
{
	mX2 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(x2, 1.0f));
	return *this;
}

RectangleTexture& RectangleTexture::setY2(size_t y2)
{
	mY2 = std::min<size_t>(y2, mBuffer->getHeight());
	return *this;
}

RectangleTexture& RectangleTexture::setY2(Ogre::Real y2)
{
	mY2 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(y2, 1.0f));
	return *this;
}

RectangleTexture& RectangleTexture::setRectangle(Ogre::RealRect rect, bool relative)
{
	if (relative)
	{
		mX1 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(rect.left, 1.0f));
		mY1 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(rect.top, 1.0f));
		mX2 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(rect.right, 1.0f));
		mY2 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(rect.bottom, 1.0f));
	}
	else
	{
		mX1 = std::min<size_t>((size_t)rect.left, mBuffer->getWidth());
		mY1 = std::min<size_t>((size_t)rect.top, mBuffer->getHeight());
		mX2 = std::min<size_t>((size_t)rect.right, mBuffer->getWidth());
		mY2 = std::min<size_t>((size_t)rect.bottom, mBuffer->getHeight());
	}
	return *this;
}

RectangleTexture& RectangleTexture::setRectangle(Ogre::Rect rect)
{
	mX1 = std::min<size_t>(rect.left, mBuffer->getWidth());
	mY1 = std::min<size_t>(rect.top, mBuffer->getHeight());
	mX2 = std::min<size_t>(rect.right, mBuffer->getWidth());
	mY2 = std::min<size_t>(rect.bottom, mBuffer->getHeight());
	return *this;
}

RectangleTexture& RectangleTexture::setRectangle(Ogre::Vector2 pos1, Ogre::Vector2 pos2, bool relative)
{
	if (relative)
	{
		mX1 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(pos1.x, 1.0f));
		mY1 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(pos1.y, 1.0f));
		mX2 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(pos2.x, 1.0f));
		mY2 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(pos2.y, 1.0f));
	}
	else
	{
		mX1 = std::min<size_t>((size_t)pos1.x, mBuffer->getWidth());
		mY1 = std::min<size_t>((size_t)pos1.y, mBuffer->getHeight());
		mX2 = std::min<size_t>((size_t)pos2.x, mBuffer->getWidth());
		mY2 = std::min<size_t>((size_t)pos2.y, mBuffer->getHeight());
	}
	return *this;
}

RectangleTexture& RectangleTexture::setRectangle(size_t x1, size_t y1, size_t x2, size_t y2)
{
	mX1 = std::min<size_t>(x1, mBuffer->getWidth());
	mY1 = std::min<size_t>(y1, mBuffer->getHeight());
	mX2 = std::min<size_t>(x2, mBuffer->getWidth());
	mY2 = std::min<size_t>(y2, mBuffer->getHeight());
	return *this;
}

RectangleTexture& RectangleTexture::setRectangle(Ogre::Real x1, Ogre::Real y1, Ogre::Real x2, Ogre::Real y2)
{
	mX1 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(x1, 1.0f));
	mY1 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(y1, 1.0f));
	mX2 = (size_t)((Ogre::Real)mBuffer->getWidth() * std::min<Ogre::Real>(x2, 1.0f));
	mY2 = (size_t)((Ogre::Real)mBuffer->getHeight() * std::min<Ogre::Real>(y2, 1.0f));
	return *this;
}

TextureBufferPtr RectangleTexture::process()
{
	size_t xStart = std::min<size_t>(mX1, mX2);
	size_t yStart = std::min<size_t>(mY1, mY2);
	size_t xEnd = std::max<size_t>(mX1, mX2);
	size_t yEnd = std::max<size_t>(mY1, mY2);

	for (size_t y = yStart; y < yEnd; y++)
	{
		for (size_t x = xStart; x < xEnd; x++)
		{
			mBuffer->setPixel(x, y, mColour);
		}
	}

	logMsg("Modify texture with rectangle filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RotationZoom& RotationZoom::setCenterX(Ogre::Real centerx)
{
	mCenterX = centerx;
	return *this;
}

RotationZoom& RotationZoom::setCenterY(Ogre::Real centery)
{
	mCenterY = centery;
	return *this;
}

RotationZoom& RotationZoom::setZoomX(Ogre::Real zoomx)
{
	mZoomX = zoomx;
	return *this;
}

RotationZoom& RotationZoom::setZoomY(Ogre::Real zoomy)
{
	mZoomY = zoomy;
	return *this;
}

RotationZoom& RotationZoom::setRotation(Ogre::Real rotation)
{
	mRotation = Ogre::Real(rotation * Ogre::Math::TWO_PI);
	return *this;
}

RotationZoom& RotationZoom::setRotation(Ogre::Radian rotation)
{
	mRotation = rotation;
	return *this;
}

RotationZoom& RotationZoom::setRotation(Ogre::Degree rotation)
{
	mRotation = Ogre::Radian(rotation.valueRadians());
	return *this;
}

RotationZoom& RotationZoom::setWrap(bool wrap)
{
	mWrap = wrap;
	return *this;
}

TextureBufferPtr RotationZoom::process()
{
	unsigned long tw = mBuffer->getWidth();
	unsigned long th = mBuffer->getHeight();
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	Ogre::Real fZoomX = Ogre::Math::Pow(0.5f, mZoomX - 1);
	Ogre::Real fZoomY = Ogre::Math::Pow(0.5f, mZoomY - 1);
	Ogre::Real c = Ogre::Math::Cos(mRotation.valueRadians());
	Ogre::Real s = Ogre::Math::Sin(mRotation.valueRadians());
	Ogre::Real tw2 = (Ogre::Real)tw / 2.0f;
	Ogre::Real th2 = (Ogre::Real)th / 2.0f;
	Ogre::Real ys = s * -th2;
	Ogre::Real yc = c * -th2;

	for (unsigned long y = 0; y < mBuffer->getHeight(); y++)
	{
		Ogre::Real u = (((c * -tw2) - ys) * fZoomX) + (mCenterX * (Ogre::Real)tw);
		Ogre::Real v = (((s * -tw2) + yc) * fZoomY) + (mCenterY * (Ogre::Real)th);
		for (unsigned long x = 0; x < mBuffer->getWidth(); x++)
		{
			Ogre::Real uf = (u >= 0) ? (u - (long)u) : 1 + (u - (long)u);
			Ogre::Real vf = (v >= 0) ? (v - (long)v) : 1 + (v - (long)v);
			unsigned long ut = (u >= 0) ? (unsigned long)u : (unsigned long)u - 1;
			unsigned long vt = (v >= 0) ? (unsigned long)v : (unsigned long)v - 1;

			Ogre::ColourValue texel = mBuffer->getPixel(vt % th, ut % tw) * (1.0f - uf) * (1.0f - vf);
			texel += mBuffer->getPixel(vt % th, (ut + 1) % tw) * uf * (1.0f - vf);
			texel += mBuffer->getPixel((vt + 1) % th, ut % tw) * (1.0f - uf) * vf;
			texel += mBuffer->getPixel((vt + 1) % th, (ut + 1) % tw) * uf * vf;
			tmpBuffer->setPixel(x, y, texel);
			u += c * fZoomX;
			v += s * fZoomY;
		}
		ys += s;
		yc += c;
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with rotationzoom filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Segment& Segment::setColourSource(TextureBufferPtr coloursource)
{
	mColourSource = coloursource;
	return *this;
}

Segment& Segment::setThreshold(Ogre::uchar threshold)
{
	mThreshold = threshold;
	return *this;
}

TextureBufferPtr Segment::process()
{
	if (mColourSource == NULL) return mBuffer;

	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();

	if (mColourSource->getWidth() < w || mColourSource->getHeight() < h) return mBuffer;

	Ogre::uchar* pCoverage = new Ogre::uchar[w * h];
	memset(pCoverage, 0, w * h);
	IntVector2* pStack = new IntVector2[w * h * 4];
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	size_t stackPtr = 0;
	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::ColourValue pixelA = mBuffer->getPixel(x, y);
			Ogre::ColourValue pixelB = mColourSource->getPixel(x, y);

			if ((pixelA.r + pixelA.g + pixelA.b) * 255.0f > (Ogre::Real)mThreshold * 3.0f)
			{
				pStack[stackPtr].x = x;
				pStack[stackPtr].y = y;
				stackPtr++;
			}

			while (stackPtr > 0)
			{
				IntVector2 current = pStack[--stackPtr];
				if (pCoverage[current.x + current.y * w] != 0)
					continue;

				pixelA = mBuffer->getPixel(current.x, current.y);
				if ((pixelA.r + pixelA.g + pixelA.b) * 255.0f > (Ogre::Real)mThreshold * 3.0f)
				{
					pStack[stackPtr].x = current.x;
					pStack[stackPtr].y = (current.y + h - 1) % h;
					stackPtr++;
					pStack[stackPtr].x = current.x;
					pStack[stackPtr].y = (current.y + 1) % h;
					stackPtr++;
					pStack[stackPtr].x = (current.x + 1) % w;
					pStack[stackPtr].y = current.y ;
					stackPtr++;
					pStack[stackPtr].x = (current.x + w - 1) % w;
					pStack[stackPtr].y = current.y ;
					stackPtr++;

					pCoverage[current.x + current.y * w] = 1;
					tmpBuffer->setPixel(current.x, current.y, pixelB);
				}
			}

			if (pCoverage[x + y * w] == 0)
				tmpBuffer->setPixel(x, y, Ogre::ColourValue::Black);
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;
	delete pCoverage;
	delete pStack;

	logMsg("Modify texture with segment filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sharpen& Sharpen::setSize(Ogre::uchar size)
{
	mSize = size;
	if (mSize < 3) mSize = 3;
	if ((mSize % 2) == 0) mSize++;
	return *this;
}

Sharpen& Sharpen::setSigma(Ogre::uchar sigma)
{
	mSigma = sigma;
	return *this;
}

Sharpen& Sharpen::setType(Sharpen::SHARP_TYPE type)
{
	mType = type;
	return *this;
}

TextureBufferPtr Sharpen::process()
{
	Convolution filter(mBuffer);
	switch (mType)
	{
	default:
	case SHARP_BASIC:
		filter.setKernel(Ogre::Matrix3(0.0f, -1.0f, 0.0f, -1.0f, 5.0f, -1.0f, 0.0f, -1.0f, 0.0f)).calculateDivisor();
		break;

	case SHARP_GAUSSIAN:
		Ogre::Real fSigma = 0.5f + ((5.0f - 0.5f) / 255.0f) * (Ogre::Real)mSigma;
		int r = (int)mSize / 2;
		double min = Ogre::Math::Exp((Ogre::Real)(2 * r * r) / (-2.0f * fSigma * fSigma)) / (Ogre::Math::TWO_PI * fSigma * fSigma);
		int* kernel = new int[mSize * mSize];
		int sum = 0;
		int y = -r;
		int x = -r;
		for (int i = 0; i < mSize; i++)
		{
			for (int j = 0; j < mSize; j++)
			{
				kernel[i * mSize + j] = (int)((Ogre::Math::Exp((Ogre::Real)(x * x + y * y) / (-2.0f * fSigma * fSigma)) / (Ogre::Math::TWO_PI * fSigma * fSigma)) / min);
				sum += kernel[i * mSize + j];
				x++;
			}
			y++;
		}
		int c = (int)mSize >> 1;
		int divisor = 0;
		for (int i = 0; i < mSize; i++)
		{
			for (int j = 0; j < mSize; j++)
			{
				if ((i == c) && (j == c))
					kernel[i * mSize + j] = 2 * sum - kernel[i * mSize + j];
				else
					kernel[i * mSize + j] = -kernel[i * mSize + j];

				divisor += kernel[i * mSize + j];
			}
		}
		filter.setKernel(mSize, kernel).setDivisor((Ogre::Real)divisor);
		delete kernel;
		break;
	}
	filter.setIncludeAlphaChannel(true).process();

	logMsg("Modify texture with sharpen filter : " + StringConverter::toString(mType));
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PROCEDURAL_USE_FREETYPE

TextTexture& TextTexture::setText(Ogre::String text)
{
	mText = text;
	return *this;
}

TextTexture& TextTexture::setPositionX(size_t x)
{
	mX = std::min<size_t>(x, mBuffer->getWidth() - 1);
	return *this;
}

TextTexture& TextTexture::setPositionX(Ogre::Real x)
{
	mX = std::min<size_t>((size_t)(x * (Ogre::Real)mBuffer->getWidth()), mBuffer->getWidth() - 1);
	return *this;
}

TextTexture& TextTexture::setPositionY(size_t y)
{
	mY = std::min<size_t>(y, mBuffer->getHeight() - 1);
	return *this;
}

TextTexture& TextTexture::setPositionY(Ogre::Real y)
{
	mY = std::min<size_t>((size_t)(y * (Ogre::Real)mBuffer->getHeight()), mBuffer->getHeight() - 1);
	return *this;
}

TextTexture& TextTexture::setPosition(Ogre::Vector2 pos, bool relative)
{
	setPosition(pos.x, pos.y, relative);
	return *this;
}

TextTexture& TextTexture::setPosition(size_t x, size_t y)
{
	setPositionX(x);
	setPositionY(y);
	return *this;
}

TextTexture& TextTexture::setPosition(Ogre::Real x, Ogre::Real y, bool relative)
{
	if (relative)
	{
		setPositionX(x);
		setPositionY(y);
	}
	else
	{
		setPositionX((size_t)x);
		setPositionY((size_t)y);
	}
	return *this;
}

#if PROCEDURAL_PLATFORM == PROCEDURAL_PLATFORM_WIN32
TextTexture& TextTexture::setPosition(POINT pos)
{
	setPosition((size_t)pos.x, (size_t)pos.y);
	return *this;
}
#endif

TextTexture& TextTexture::setFont(Ogre::String fontName, Ogre::uchar fontSize)
{
	if (fontName.empty() || fontSize < 4) return *this;
	mFontName = fontName;
	mFontSize = fontSize;
	return *this;
}

TextTexture& TextTexture::setColour(Ogre::ColourValue colour)
{
	mColour = colour;
	return *this;
}

TextTexture& TextTexture::setColour(Ogre::uchar red, Ogre::uchar green, Ogre::uchar blue, Ogre::uchar alpha)
{
	mColour = Ogre::ColourValue((Ogre::Real)red / 255.0f, (Ogre::Real)green / 255.0f, (Ogre::Real)blue / 255.0f, (Ogre::Real)alpha / 255.0f);
	return *this;
}

TextTexture& TextTexture::setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	mColour = Ogre::ColourValue(red, green, blue, alpha);
	return *this;
}

TextureBufferPtr TextTexture::process()
{
	FT_Library ftlib;
	FT_Face face;
	FT_GlyphSlot slot;

	FT_Error error = FT_Init_FreeType(&ftlib);
	if (error == 0)
	{
		error = FT_New_Face(ftlib, getFontFileByName().c_str(), 0, &face);
		if (error == FT_Err_Unknown_File_Format)
			logMsg("FreeType ERROR: FT_Err_Unknown_File_Format");
		else if (error)
			logMsg("FreeType ERROR: FT_New_Face - " + Ogre::StringConverter::toString(error));
		else
		{
			FT_Set_Pixel_Sizes(face, 0, mFontSize);

			size_t px = (size_t)mX;
			size_t py = (size_t)mY;
			slot = face->glyph;

			for (size_t n = 0; n < mText.length(); n++)
			{
				error = FT_Load_Char(face, mText[n], FT_LOAD_RENDER);
				if (error) continue;

				for (long i = 0; i < (long)slot->bitmap.width; i++)
				{
					for (long j = 0; j < (long)slot->bitmap.rows; j++)
					{
						if (slot->bitmap.buffer[j * slot->bitmap.width + i] > 127)
							mBuffer->setPixel(px + i, py + j, mColour);
					}
				}

				px += slot->advance.x >> 6;
				py += slot->advance.y >> 6;
			}
			FT_Done_Face(face);
			logMsg("Modify texture with text processing : " + mText);
		}
		FT_Done_FreeType(ftlib);
	}
	else
		logMsg("FreeType ERROR: FT_Init_FreeType");
	return mBuffer;
}

Ogre::String TextTexture::getFontFileByName()
{
	Ogre::String ff;
	Ogre::String tmp;

#if PROCEDURAL_PLATFORM == PROCEDURAL_PLATFORM_WIN32
	char windows[MAX_PATH];
	GetWindowsDirectory(windows, MAX_PATH);

	bool result = getFontFile(mFontName, tmp, ff);
	if (!result) return mFontName;
	if (!(ff[0] == '\\' && ff[1] == '\\') && !(ff[1] == ':' && ff[2] == '\\'))
		return Ogre::String(windows) + "\\fonts\\" + ff;
	else
		return ff;
#else
	return mFontName;
#endif
}

#if PROCEDURAL_PLATFORM == PROCEDURAL_PLATFORM_WIN32
bool TextTexture::getFontFile(Ogre::String fontName, Ogre::String& displayName, Ogre::String& filePath)
{
	if (fontName.empty()) return false;

	if ((fontName[0] == '\\' && fontName[1] == '\\') || (fontName[1] == ':' && fontName[2] == '\\'))
	{
		displayName = fontName;
		filePath = fontName;
		return true;
	}

	char name[2 * MAX_PATH];
	char data[2 * MAX_PATH];
	filePath.empty();
	bool retVal = false;

	HKEY hkFont;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hkFont) == ERROR_SUCCESS)
	{
		char cname[MAX_PATH];
		DWORD icname = 0;
		DWORD isubkeys = 0;
		DWORD imaxsubkey = 0;
		DWORD imaxclass = 0;
		DWORD ivalues = 0;
		DWORD imaxvalues = 0;
		DWORD imaxnamevalues = 0;
		DWORD isecurity = 0;
		FILETIME dtlast;

		DWORD retCode = RegQueryInfoKey(hkFont, cname, &icname, NULL, &isubkeys, &imaxsubkey, &imaxclass, &ivalues, &imaxnamevalues, &imaxvalues, &isecurity, &dtlast);
		if (ivalues)
		{
			for (DWORD i = 0; i < ivalues; i++)
			{
				retCode = ERROR_SUCCESS;
				DWORD nsize = MAX_PATH - 1;
				DWORD dsize = MAX_PATH - 1;
				name[0] = 0;
				data[0] = 0;
				retCode = RegEnumValue(hkFont, i, name, &nsize, NULL, NULL, (LPBYTE)data, &dsize);
				if (retCode == ERROR_SUCCESS)
					if (strnicmp(name, fontName.c_str(), std::min<size_t>(strlen(name), fontName.length())) == 0)
					{
						displayName = name;
						filePath = data;
						retVal = true;
						break;
					}
			}
		}
	}
	RegCloseKey(hkFont);
	return retVal;
}
#endif

#endif // PROCEDURAL_USE_FREETYPE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Threshold& Threshold::setThreshold(Ogre::uchar threshold)
{
	mThreshold = threshold;
	return *this;
}

Threshold& Threshold::setRatio(Ogre::uchar ratio)
{
	mRatio = ratio;
	return *this;
}

Threshold& Threshold::setMode(Threshold::THRESHOLD_MODE mode)
{
	mMode = mode;
	return *this;
}

TextureBufferPtr Threshold::process()
{
	long t;
	size_t w = mBuffer->getWidth();
	size_t h = mBuffer->getHeight();
	Ogre::Real ratio = (mMode == MODE_EXPAND_DOWNWARDS || mMode == MODE_EXPAND_UPWARDS) ? 1 + mRatio * 0.1f : 1 + mRatio * 0.05f;

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::uchar r = mBuffer->getPixelRedByte(x, y);
			Ogre::uchar g = mBuffer->getPixelGreenByte(x, y);
			Ogre::uchar b = mBuffer->getPixelBlueByte(x, y);
			Ogre::uchar a = mBuffer->getPixelAlphaByte(x, y);

			if (mMode == MODE_EXPAND_DOWNWARDS)
			{
				if (r < mThreshold)
				{
					t = mThreshold - (long)((mThreshold - r) * ratio);
					r = (t < 0) ? 0 : (Ogre::uchar)t;
				}
				if (g < mThreshold)
				{
					t = mThreshold - (long)((mThreshold - g) * ratio);
					g = (t < 0) ? 0 : (Ogre::uchar)t;
				}
				if (b < mThreshold)
				{
					t = mThreshold - (long)((mThreshold - b) * ratio);
					b = (t < 0) ? 0 : (Ogre::uchar)t;
				}
			}
			else if (mMode == MODE_EXPAND_UPWARDS)
			{
				if (r > mThreshold)
				{
					t = (long)((r - mThreshold) * ratio) - mThreshold;
					r = (t > 255) ? 255 : (Ogre::uchar)t;
				}
				if (g > mThreshold)
				{
					t = (long)((g - mThreshold) * ratio) - mThreshold;
					g = (t > 255) ? 255 : (Ogre::uchar)t;
				}
				if (b > mThreshold)
				{
					t = (long)((b - mThreshold) * ratio) - mThreshold;
					b = (t > 255) ? 255 : (Ogre::uchar)t;
				}
			}
			else if (mMode == MODE_COMPRESS_BELOW)
			{
				if (r < mThreshold)
				{
					t = mThreshold - (long)((mThreshold - r) / ratio);
					r = (t < 0) ? 0 : (Ogre::uchar)t;
				}
				if (g < mThreshold)
				{
					t = mThreshold - (long)((mThreshold - g) / ratio);
					g = (t < 0) ? 0 : (Ogre::uchar)t;
				}
				if (b < mThreshold)
				{
					t = mThreshold - (long)((mThreshold - b) / ratio);
					b = (t < 0) ? 0 : (Ogre::uchar)t;
				}
			}
			else if (mMode == MODE_COMPRESS_ABOVE)
			{
				if (r > mThreshold)
				{
					t = (long)((r - mThreshold) / ratio) - mThreshold;
					r = (t > 255) ? 255 : (Ogre::uchar)t;
				}
				if (g > mThreshold)
				{
					t = (long)((g - mThreshold) / ratio) - mThreshold;
					g = (t > 255) ? 255 : (Ogre::uchar)t;
				}
				if (b > mThreshold)
				{
					t = (long)((b - mThreshold) / ratio) - mThreshold;
					b = (t > 255) ? 255 : (Ogre::uchar)t;
				}
			}
			mBuffer->setPixel(x, y, r, g, b, a);
		}
	}

	logMsg("Modify texture with threshold filter");
	return mBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vortex& Vortex::setCenterX(Ogre::Real centerx)
{
	mCenterX = centerx;
	return *this;
}

Vortex& Vortex::setCenterY(Ogre::Real centery)
{
	mCenterY = centery;
	return *this;
}

Vortex& Vortex::setRadiusX(Ogre::Real radiusx)
{
	mRadiusX = radiusx;
	return *this;
}

Vortex& Vortex::setRadiusY(Ogre::Real radiusy)
{
	mRadiusY = radiusy;
	return *this;
}

Vortex& Vortex::setTwist(Ogre::Real twist)
{
	mTwist = Ogre::Radian(twist * Ogre::Math::TWO_PI);
	return *this;
}

Vortex& Vortex::setTwist(Ogre::Radian twist)
{
	mTwist = twist;
	return *this;
}

Vortex& Vortex::setTwist(Ogre::Degree twist)
{
	mTwist = twist;
	return *this;
}

TextureBufferPtr Vortex::process()
{
	long w = (long)mBuffer->getWidth();
	long h = (long)mBuffer->getHeight();
	long dwCenterX	= (long)(mCenterX * (Ogre::Real)w);
	long dwCenterY	= (long)(mCenterY * (Ogre::Real)h);
	long dwRadiusX	= (long)(mRadiusX * (Ogre::Real)w);
	long dwRadiusY	= (long)(mRadiusY * (Ogre::Real)h);
	Ogre::Real f1_RadiusX = 1.0f / (Ogre::Real)dwRadiusX;
	Ogre::Real f1_RadiusY = 1.0f / (Ogre::Real)dwRadiusY;
	TextureBufferPtr tmpBuffer = mBuffer->clone();

	for (long y = 0; y < h; y++)
	{
		Ogre::Real dy = (Ogre::Real)(y - dwCenterY) * f1_RadiusY;

		for (long x = 0; x < w; x++)
		{
			Ogre::Real dx = (Ogre::Real)(x - dwCenterX) * f1_RadiusX;
			Ogre::Real d = sqrt(dx * dx + dy * dy);

			if (d > 1.0f)
				tmpBuffer->setPixel(x, y, mBuffer->getPixel(x, y));
			else
			{
				d = Ogre::Math::Cos(d * Ogre::Math::HALF_PI - Ogre::Math::HALF_PI);
				d = 1.0f - d;
				Ogre::Real nx = (Ogre::Real)(x - dwCenterX);
				Ogre::Real ny = (Ogre::Real)(y - dwCenterY);
				Ogre::Real rad = mTwist.valueRadians() * d;

				Ogre::Real bx = nx;
				nx = bx * Ogre::Math::Cos(rad) - ny * Ogre::Math::Sin(rad) + dwCenterX;
				ny = bx * Ogre::Math::Sin(rad) + ny * Ogre::Math::Cos(rad) + dwCenterY;

				if (nx >= w) nx = nx - w;
				if (ny >= h) ny = ny - h;
				if (nx < 0) nx = w + nx;
				if (ny < 0) ny = h + ny;

				int ix = (int)nx;
				int iy = (int)ny;

				Ogre::Real fracX = nx - ix;
				Ogre::Real fracY = ny - iy;

				Ogre::Real ul = (1.0f - fracX) * (1.0f - fracY);
				Ogre::Real ll = (1.0f - fracX) * fracY;
				Ogre::Real ur = fracX * (1.0f - fracY);
				Ogre::Real lr = fracX * fracY;

				int wrapx = (ix + 1) % w;
				int wrapy = (iy + 1) % h;
				Ogre::ColourValue pixelUL = mBuffer->getPixel(ix, iy);
				Ogre::ColourValue pixelUR = mBuffer->getPixel(wrapx, iy);
				Ogre::ColourValue pixelLL = mBuffer->getPixel(ix, wrapy);
				Ogre::ColourValue pixelLR = mBuffer->getPixel(wrapx, wrapy);

				tmpBuffer->setPixel(x, y,
									(Ogre::uchar)(ul * pixelUL.r * 255.0f + ll * pixelLL.r * 255.0f + ur * pixelUR.r * 255.0f + lr * pixelLR.r * 255.0f),
									(Ogre::uchar)(ul * pixelUL.g * 255.0f + ll * pixelLL.g * 255.0f + ur * pixelUR.g * 255.0f + lr * pixelLR.g * 255.0f),
									(Ogre::uchar)(ul * pixelUL.b * 255.0f + ll * pixelLL.b * 255.0f + ur * pixelUR.b * 255.0f + lr * pixelLR.b * 255.0f),
									(Ogre::uchar)(ul * pixelUL.a * 255.0f + ll * pixelLL.a * 255.0f + ur * pixelUR.a * 255.0f + lr * pixelLR.a * 255.0f));
			}
		}
	}

	mBuffer->setData(tmpBuffer);
	delete tmpBuffer;

	logMsg("Modify texture with vortex filter : " + StringConverter::toString(mTwist.valueDegrees()));
	return mBuffer;
}
}