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


#ifndef PROCEDURAL_TEXTURE_BUFFER_INCLUDED
#define PROCEDURAL_TEXTURE_BUFFER_INCLUDED

#include "ProceduralPlatform.h"
#include "Ogre.h"


namespace Procedural
{

class TextureBuffer;
//! Type for a TextureBuffer pointer
typedef TextureBuffer* TextureBufferPtr;


/**
\brief class to store image data while processing
\details Create a TextureBuffer object and move it to all classes inherited from TextureProcessing
\todo check byte order for image generation (OGRE_ENDIAN, OGRE_ENDIAN_LITTLE, OGRE_ENDIAN_BIG), see <a href="http://www.ogre3d.org/forums/viewtopic.php?f=2&t=72832" target="_blank">Ogre forum</a> for details.
\todo Increase speed of reading and writeing pixel values.
*/
class _ProceduralExport TextureBuffer
{
private:
	Ogre::uchar* mPixels;
	Ogre::uint mWidth;
	Ogre::uint mHeight;

public:

	/**
	\brief Set colour of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param colour New colour of pixel
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	inline void setPixel(size_t x, size_t y, Ogre::ColourValue colour)
	{
		setPixel(x, y, colour.r, colour.g, colour.b, colour.a);
	}

	/**
	\brief Set colour of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param red New red value of pixel colour [0, 255]
	\param green New green value of pixel colour [0, 255]
	\param blue New blue value of pixel colour [0, 255]
	\param alpha New alpha value of pixel colour [0, 255]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setPixel(size_t x, size_t y, Ogre::uchar red, Ogre::uchar green, Ogre::uchar blue, Ogre::uchar alpha = 255);

	/**
	\brief Set colour of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param red New red value of pixel colour
	\param green New green value of pixel colour [0.0, 1.0]
	\param blue New blue value of pixel colour [0.0, 1.0]
	\param alpha New alpha value of pixel colour [0.0, 1.0]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	\exception Ogre::InvalidParametersException Colour value must be between 0 and 1!
	*/
	void setPixel(size_t x, size_t y, Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	\brief Set red colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param red New red value of pixel colour [0, 255]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setRed(size_t x, size_t y, Ogre::uchar red);

	/**
	\brief Set green colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param green New green value of pixel colour [0, 255]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setGreen(size_t x, size_t y, Ogre::uchar green);

	/**
	\brief Set blue colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param blue New blue value of pixel colour [0, 255]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setBlue(size_t x, size_t y, Ogre::uchar blue);

	/**
	\brief Set alpha colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param alpha New alpha value of pixel colour [0, 255]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setAlpha(size_t x, size_t y, Ogre::uchar alpha);

	/**
	\brief Set red colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param red New red value of pixel colour [0.0, 1.0]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	\exception Ogre::InvalidParametersException Colour value must be between 0 and 1!
	*/
	void setRed(size_t x, size_t y, Ogre::Real red);

	/**
	\brief Set green colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param green New green value of pixel colour [0.0, 1.0]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setGreen(size_t x, size_t y, Ogre::Real green);

	/**
	\brief Set blue colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param blue New blue value of pixel colour [0.0, 1.0]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setBlue(size_t x, size_t y, Ogre::Real blue);

	/**
	\brief Set alpha colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\param alpha New alpha value of pixel colour [0.0, 1.0]
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	void setAlpha(size_t x, size_t y, Ogre::Real alpha);

	/**
	\brief Copy image data (RGBA) from a buffer to this object
	\param width Width of the image in buffer
	\param height Height of the image in buffer
	\param data Image buffer as source for copy
	*/
	void setData(size_t width, size_t height, Ogre::uchar* data);

	/**
	\brief Copy image data (RGBA) from an other TextureBuffer object
	\param buffer Image buffer as source for copy
	*/
	void setData(TextureBufferPtr buffer);

	/**
	\brief Get colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return colour value as an Ogre::ColourValue object.
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::ColourValue getPixel(size_t x, size_t y) const;

	/**
	\brief Get red colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return red colour value as a byte [0, 255].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::uchar getPixelRedByte(size_t x, size_t y) const;

	/**
	\brief Get green colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return green colour value as a byte [0, 255].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::uchar getPixelGreenByte(size_t x, size_t y) const;

	/**
	\brief Get blue colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return blue colour value as a byte [0, 255].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::uchar getPixelBlueByte(size_t x, size_t y) const;

	/**
	\brief Get alpha colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return alpha colour value as a byte [0, 255].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::uchar getPixelAlphaByte(size_t x, size_t y) const;

	/**
	\brief Get red colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return red colour value as a Ogre::Real [0.0, 1.0].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::Real getPixelRedReal(size_t x, size_t y) const;

	/**
	\brief Get green colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return green colour value as a Ogre::Real [0.0, 1.0].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::Real getPixelGreenReal(size_t x, size_t y) const;

	/**
	\brief Get blue colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return blue colour value as a Ogre::Real [0.0, 1.0].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::Real getPixelBlueReal(size_t x, size_t y) const;

	/**
	\brief Get alpha colour value of a specified pixel
	\param x X position of pixel to paint on (0 <= x < width)
	\param y Y position of pixel to paint on (0 <= y < height)
	\return Return alpha colour value as a Ogre::Real [0.0, 1.0].
	\exception Ogre::InvalidParametersException Pixel location is out of bounds!
	*/
	Ogre::Real getPixelAlphaReal(size_t x, size_t y) const;

	/**
	\brief Create a copy of the current texture image buffer
	\note You have to delete cloned object by yourself!
	*/
	TextureBufferPtr clone() const;

	/**
	\brief Standard constructor which copy a given image
	\param tocopy Image which to copy
	\exception Ogre::InvalidParametersException Pointer to source image must not be NULL!
	*/
	TextureBuffer(TextureBufferPtr tocopy);

	/**
	\brief Standard constructor which creates a quadratic image buffer with the given size
	\param width_height Edge length in px
	\exception Ogre::InvalidParametersException Minimum edge size is 8!
	*/
	TextureBuffer(Ogre::uint width_height);

	/// Destructor which release memory
	~TextureBuffer();

	/// Get the width of the stored image in px
	inline Ogre::uint getWidth() const
	{
		return mWidth;
	}

	/// Get the height of the stored image in px
	inline Ogre::uint getHeight() const
	{
		return mHeight;
	}

	/// Create a new image from buffer.
	Ogre::Image* getImage() const;

	/**
	\brief Save the image from the buffer to a file.
	\param filename Name (and path) of the image file where to save the buffer.
	*/
	void saveImage(Ogre::String filename) const;

	/**
	\brief Creates an OGRE texture and add it to current TextureManager instance.
	\param name Name of the texture
	\param group Name of the resource group where to list the texture
	*/
	Ogre::TexturePtr createTexture(Ogre::String name, Ogre::String group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME) const;
};

/**
\brief base class for material generation classes.
*/
class _ProceduralExport TextureProcessing
{
protected:
	TextureBufferPtr mBuffer;
	Ogre::String mName;
	bool mLog;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	\param name Filter name
	\exception Ogre::InvalidParametersException Texture buffer is not set!
	*/
	TextureProcessing(TextureBufferPtr pBuffer, Ogre::String name);

	/** Run processing algorithmus */
	virtual TextureBufferPtr process() =0;

	/** Get name of the filter as string. */
	Ogre::String getName();

	/**
	Enable/Disable logging.
	\param enable true enables logging of filter actions
	*/
	void setLog(bool enable = true);

	virtual ~TextureProcessing() {}

protected:
	void logMsg(Ogre::String msg);
};
}
#endif
