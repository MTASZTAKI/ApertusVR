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
#include "ProceduralTextureBuffer.h"
#include "Ogre.h"
#include "ProceduralUtils.h"

namespace Procedural
{

using namespace Ogre;

#if OGRE_ENDIAN == OGRE_ENDIAN_LITTLE
#define PROCEDURAL_RED 3
#define PROCEDURAL_GREEN 2
#define PROCEDURAL_BLUE 1
#define PROCEDURAL_ALPHA 0
#else
#define PROCEDURAL_RED 0
#define PROCEDURAL_GREEN 1
#define PROCEDURAL_BLUE 2
#define PROCEDURAL_ALPHA 3
#endif

TextureBuffer::TextureBuffer(TextureBufferPtr tocopy)
{
	if (tocopy == NULL)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pointer to source image must not be NULL!", "Procedural::TextureBuffer::TextureBuffer(Procedural::TextureBufferPtr)");
	mWidth = tocopy->getWidth();
	mHeight = tocopy->getHeight();

	mPixels = new Ogre::uchar[mWidth * mHeight * 4];
	memcpy(mPixels, tocopy->mPixels, mWidth * mHeight * 4 * sizeof(Ogre::uchar));
}

TextureBuffer::TextureBuffer(Ogre::uint width_height)
	: mWidth(width_height), mHeight(width_height)
{
	if (width_height < 8)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Minimum edge size is 8!", "Procedural::TextureBuffer::TextureBuffer(Procedural::TextureBufferPtr)");
	mPixels = new Ogre::uchar[mWidth * mHeight * 4];
	memset(mPixels, 0, mWidth * mHeight * 4 * sizeof(Ogre::uchar));
	for (size_t y = 0; y < mHeight; y++)
	{
		for (size_t x = 0; x < mWidth; x++)
		{
			setAlpha(x, y, (Ogre::uchar)255);
		}
	}

#if OGRE_DEBUG_MODE
	Utils::log("Create texture buffer : " + StringConverter::toString(mWidth) + "x" + StringConverter::toString(mHeight));
#endif
}

TextureBuffer::~TextureBuffer()
{
	delete mPixels;
}



void TextureBuffer::setPixel(size_t x, size_t y, Ogre::uchar red, Ogre::uchar green, Ogre::uchar blue, Ogre::uchar alpha)
{
	setRed(x, y, red);
	setGreen(x, y, green);
	setBlue(x, y, blue);
	setAlpha(x, y, alpha);
}

void TextureBuffer::setPixel(size_t x, size_t y, Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	setRed(x, y, red);
	setGreen(x, y, green);
	setBlue(x, y, blue);
	setAlpha(x, y, alpha);
}

void TextureBuffer::setRed(size_t x, size_t y, Ogre::uchar red)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setRed(size_t, size_t, Ogre::uchar)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_RED] = red;
}

void TextureBuffer::setGreen(size_t x, size_t y, Ogre::uchar green)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setGreen(size_t, size_t, Ogre::uchar)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_GREEN] = green;
}

void TextureBuffer::setBlue(size_t x, size_t y, Ogre::uchar blue)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setBlue(size_t, size_t, Ogre::uchar)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_BLUE] = blue;
}

void TextureBuffer::setAlpha(size_t x, size_t y, Ogre::uchar alpha)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setAlpha(size_t, size_t, Ogre::uchar)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_ALPHA] = alpha;
}

void TextureBuffer::setRed(size_t x, size_t y, Ogre::Real red)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setRed(size_t, size_t, Ogre::Real)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_RED] = (Ogre::uchar)(std::min<Ogre::Real>(std::max<Ogre::Real>(red * 255.0f, 0.0f), 255.0f));
}

void TextureBuffer::setGreen(size_t x, size_t y, Ogre::Real green)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setGreen(size_t, size_t, Ogre::Real)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_GREEN] = (Ogre::uchar)(std::min<Ogre::Real>(std::max<Ogre::Real>(green * 255.0f, 0.0f), 255.0f));
}

void TextureBuffer::setBlue(size_t x, size_t y, Ogre::Real blue)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setBlue(size_t, size_t, Ogre::Real)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_BLUE] = (Ogre::uchar)(std::min<Ogre::Real>(std::max<Ogre::Real>(blue * 255.0f, 0.0f), 255.0f));
}

void TextureBuffer::setAlpha(size_t x, size_t y, Ogre::Real alpha)
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::setAlpha(size_t, size_t, Ogre::Real)");

	mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_ALPHA] = (Ogre::uchar)(std::min<Ogre::Real>(std::max<Ogre::Real>(alpha * 255.0f, 0.0f), 255.0f));
}

void TextureBuffer::setData(size_t width, size_t height, Ogre::uchar* data)
{
	if (data == NULL) return;
	if (width != mWidth || height != mHeight) return;
	memcpy(mPixels, data, mWidth * mHeight * 4 * sizeof(Ogre::uchar));
}

void TextureBuffer::setData(TextureBufferPtr buffer)
{
	if (buffer == NULL) return;
	if (buffer->getWidth() != mWidth || buffer->getHeight() != mHeight) return;
	memcpy(mPixels, buffer->mPixels, mWidth * mHeight * 4 * sizeof(Ogre::uchar));
}

Ogre::ColourValue TextureBuffer::getPixel(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixel(size_t, size_t)");

	return Ogre::ColourValue(getPixelRedReal(x, y), getPixelGreenReal(x, y), getPixelBlueReal(x, y), getPixelAlphaReal(x, y));
}

Ogre::uchar TextureBuffer::getPixelRedByte(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelRedByte(size_t, size_t)");

	return mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_RED];
}

Ogre::uchar TextureBuffer::getPixelGreenByte(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelGreenByte(size_t, size_t)");

	return mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_GREEN];
}

Ogre::uchar TextureBuffer::getPixelBlueByte(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelBlueByte(size_t, size_t)");

	return mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_BLUE];
}

Ogre::uchar TextureBuffer::getPixelAlphaByte(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelAlphaByte(size_t, size_t)");

	return mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_ALPHA];
}

Ogre::Real TextureBuffer::getPixelRedReal(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelRedReal(size_t, size_t)");

	return ((Ogre::Real)mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_RED]) / 255.0f;
}

Ogre::Real TextureBuffer::getPixelGreenReal(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelGreenReal(size_t, size_t)");

	return ((Ogre::Real)mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_GREEN]) / 255.0f;
}

Ogre::Real TextureBuffer::getPixelBlueReal(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelBlueReal(size_t, size_t)");

	return ((Ogre::Real)mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_BLUE]) / 255.0f;
}

Ogre::Real TextureBuffer::getPixelAlphaReal(size_t x, size_t y) const
{
	if (x >= mWidth || y >= mHeight)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Pixel location is out of bounds!", "Procedural::TextureBuffer::getPixelAlphaReal(size_t, size_t)");

	return ((Ogre::Real)mPixels[y * mWidth * 4 + x * 4 + PROCEDURAL_ALPHA]) / 255.0f;
}

TextureBufferPtr TextureBuffer::clone() const
{
	TextureBufferPtr clon = new TextureBuffer(mWidth);
	memcpy(clon->mPixels, mPixels, mWidth * mHeight * 4 * sizeof(Ogre::uchar));
	return clon;
}

Ogre::Image* TextureBuffer::getImage() const
{
	Ogre::Image* image = new Ogre::Image();
	image->loadDynamicImage(mPixels, mWidth, mHeight, 1, PF_R8G8B8A8);
	return image;
}

void TextureBuffer::saveImage(Ogre::String filename) const
{
	Ogre::Image* image = getImage();
	image->save(filename);
	delete image;
}

Ogre::TexturePtr TextureBuffer::createTexture(Ogre::String name, Ogre::String group) const
{
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingletonPtr()->createManual(
	                               name,
	                               group,
	                               TEX_TYPE_2D,
	                               mWidth,
	                               mHeight,
	                               0,
	                               PF_R8G8B8A8,
	                               TU_DEFAULT);

	Ogre::Image* image = getImage();
	texture->loadImage(*image);
	delete image;

	return texture;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TextureProcessing::TextureProcessing(TextureBufferPtr pBuffer, Ogre::String name)
{
	if (pBuffer == NULL)
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Texture buffer is not set!", "Procedural::TextureProcessing::TextureProcessing(TextureBufferPtr, Ogre::String)");
	mBuffer = pBuffer;
	mName = name;
#if OGRE_DEBUG_MODE
	mLog = true;
#else
	mLog = false;
#endif
}

Ogre::String TextureProcessing::getName()
{
	return mName;
}

void TextureProcessing::setLog(bool enable)
{
	mLog = enable;
}

void TextureProcessing::logMsg(Ogre::String msg)
{
	if (mLog)
		Procedural::Utils::log(msg);
}
}