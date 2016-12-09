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
#ifndef PROCEDURAL_TEXTURE_GENERATOR_INCLUDED
#define PROCEDURAL_TEXTURE_GENERATOR_INCLUDED

#include "ProceduralPlatform.h"
#include "ProceduralTextureBuffer.h"
#include <Ogre.h>

namespace Procedural
{

/** \addtogroup texturegrp Textures
Elements for procedural texture creation.
@{
*/


/**
\brief Create a texture consisting of cells aligned in a grid, or a chessboard.
\details Cells can be irregular. If considered bump map, they have pyramid form.

Examples:

\b Default (MODE_GRID + PATTERN_BOTH)
\code{.cpp}
Procedural::TextureBuffer bufferCellDefault(256);
Procedural::Cell(&bufferCellDefault).setDensity(4).process();
\endcode
\image html texture_cell_default.png

\b MODE_CHESSBOARD + PATTERN_CONE
\code{.cpp}
Procedural::TextureBuffer bufferCellChessCone(256);
Procedural::Cell(&bufferCellChessCone).setDensity(4).setMode(Procedural::Cell::MODE_CHESSBOARD).setPattern(Procedural::Cell::PATTERN_CONE).process();
\endcode
\image html texture_cell_chess.png

\b MODE_GRID + PATTERN_CROSS
\code{.cpp}
Procedural::TextureBuffer bufferCellGridCross(256);
Procedural::Cell(&bufferCellGridCross).setDensity(4).setMode(Procedural::Cell::MODE_GRID).setPattern(Procedural::Cell::PATTERN_CROSS).process();
\endcode
\image html texture_cell_grid.png
*/
class _ProceduralExport Cell : public TextureProcessing
{
public:
	//! Mode how to paint cells
	enum CELL_MODE
	{
	    MODE_GRID,		//!< Paint cells on a grid
	    MODE_CHESSBOARD	//!< Paint cells on a chessboard
	};

	//! Mode how to construct cells
	enum CELL_PATTERN
	{
	    PATTERN_BOTH,	//!< PATTERN_CROSS | PATTERN_CONE
	    PATTERN_CROSS,	//!< Construct cells from vertices
	    PATTERN_CONE	//!< Construct cells from cicles
	};

private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;
	Ogre::uint mRegularity;
	Ogre::uint mDensity;
	CELL_MODE mMode;
	CELL_PATTERN mPattern;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Cell(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Cell"), mColour(Ogre::ColourValue::White), mRegularity(128), mDensity(8), mMode(MODE_GRID), mPattern(PATTERN_BOTH), mSeed(5120)
	{
	}

	/**
	Set the colour of the cell top.
	\param colour New colour of the cell top (default Ogre::ColourValue::White)
	*/
	Cell& setColour(Ogre::ColourValue colour);

	/**
	Set the colour of the cell top.
	\param red Red value of the cell top colour [0.0, 1.0] \(default 1.0)
	\param green Green value of the cell top colour [0.0, 1.0] \(default 1.0)
	\param blue Blue value of the cell top colour [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of the cell top colour [0.0, 1.0] \(default 1.0)
	*/
	Cell& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Cell& setSeed(Ogre::uint seed);

	/**
	Set the regularity of texture.

	The maximum value of 255 creates identical cells. The minimum 0 creates random forms for each cells.
	\param regularity New value for chaotic cell forms (default 128)
	*/
	Cell& setRegularity(Ogre::uchar regularity);

	/**
	Set the density of cells in texture.

	At least you have to set number of rows and columns in the grid to 1 or above.
	\param density New number of columns and rows (default 8)
	*/
	Cell& setDensity(Ogre::uint density);

	/**
	Set the cell mode of texture.
	\param mode New mode for cell ground (default MODE_GRID)
	*/
	Cell& setMode(CELL_MODE mode);

	/**
	Set the cell pattern of texture.
	\param pattern New base of cell construction (default PATTERN_BOTH)
	*/
	Cell& setPattern(CELL_PATTERN pattern);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Creates a cloud structured image.
\details Creates a cloud structure from a specified perlin noise on a coloured background.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferCloud(256);
Procedural::Cloud(&bufferCloud).process();
\endcode
\image html texture_cloud.png
*/
class _ProceduralExport Cloud : public TextureProcessing
{
private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Cloud(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Cloud"), mColour(Ogre::ColourValue::White), mSeed(5120)
	{
	}

	/**
	Set the colour of the background.
	\param colour New colour for background (default Ogre::ColourValue::White)
	*/
	Cloud& setColour(Ogre::ColourValue colour);

	/**
	Sets the colour of the background
	\param red Red value of background colour [0.0, 1.0] \(default 1.0)
	\param green Green value of background colour [0.0, 1.0] \(default 1.0)
	\param blue Blue value of background colour [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of background colour [0.0, 1.0] \(default 1.0)
	*/
	Cloud& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Cloud& setSeed(Ogre::uint seed);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Fills full image with given colour gradients.
\details Each corner of the image has unique color.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferGradient(256);
Procedural::Gradient(&bufferGradient).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
\endcode
\image html texture_gradient.png
*/
class _ProceduralExport Gradient : public TextureProcessing
{
private:
	Ogre::ColourValue mColourA;
	Ogre::ColourValue mColourB;
	Ogre::ColourValue mColourC;
	Ogre::ColourValue mColourD;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Gradient(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Gradient"), mColourA(Ogre::ColourValue::Blue), mColourB(Ogre::ColourValue::Green), mColourC(Ogre::ColourValue::Red), mColourD(Ogre::ColourValue(0.0f, 1.0f, 1.0f))
	{
	}

	/**
	Set the colour in the top left corner of the image.
	\param colour New colour in the top left corner for processing (default Ogre::ColourValue::Blue)
	*/
	Gradient& setColourA(Ogre::ColourValue colour);

	/**
	Set the colour in the top left corner of the image.
	\param red Red value of new colour in the top left corner [0.0, 1.0] \(default 0.0)
	\param green Green value of new colour in the top left corner [0.0, 1.0] \(default 0.0)
	\param blue Blue value of new colour in the top left corner [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of new colour in the top left corner [0.0, 1.0] \(default 1.0)
	*/
	Gradient& setColourA(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the colour in the top right corner of the image.
	\param colour New colour in the top right corner for processing (default Ogre::ColourValue::Green)
	*/
	Gradient& setColourB(Ogre::ColourValue colour);

	/**
	Set the colour in the top right corner of the image.
	\param red Red value of new colour in the top right corner [0.0, 1.0] \(default 0.0)
	\param green Green value of new colour in the top right corner [0.0, 1.0] \(default 1.0)
	\param blue Blue value of new colour in the top right corner [0.0, 1.0] \(default 0.0)
	\param alpha %Alpha value of new colour in the top right corner [0.0, 1.0] \(default 1.0)
	*/
	Gradient& setColourB(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the colour in the bottom left corner of the image.
	\param colour New colour in the bottom left corner for processing (default Ogre::ColourValue::Red)
	*/
	Gradient& setColourC(Ogre::ColourValue colour);

	/**
	Set the colour in the bottom left corner of the image.
	\param red Red value of new colour in the bottom left corner [0.0, 1.0] \(default 1.0)
	\param green Green value of new colour in the bottom left corner [0.0, 1.0] \(default 0.0)
	\param blue Blue value of new colour in the bottom left corner [0.0, 1.0] \(default 0.0)
	\param alpha %Alpha value of new colour in the bottom left corner [0.0, 1.0] \(default 1.0)
	*/
	Gradient& setColourC(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the colour in the bottom right corner of the image.
	\param colour New colour in the bottom right corner for processing (default Ogre::ColourValue(0.0f, 1.0f, 1.0f))
	*/
	Gradient& setColourD(Ogre::ColourValue colour);

	/**
	Set the colour in the bottom right corner of the image.
	\param red Red value of new colour in the bottom right corner [0.0, 1.0] \(default 0.0)
	\param green Green value of new colour in the bottom right corner [0.0, 1.0] \(default 1.0)
	\param blue Blue value of new colour in the bottom right corner [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of new colour in the bottom right corner [0.0, 1.0] \(default 1.0)
	*/
	Gradient& setColourD(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Sets the colours of the image corners.
	\param colourA New colour in the top left corner (default Ogre::ColourValue::Blue)
	\param colourB New colour in the top right corner (default Ogre::ColourValue::Green)
	\param colourC New colour in the bottom left corner (default Ogre::ColourValue::Red)
	\param colourD New colour in the bottom right corner (default Ogre::ColourValue(0.0f, 1.0f, 1.0f))
	*/
	Gradient& setColours(Ogre::ColourValue colourA, Ogre::ColourValue colourB, Ogre::ColourValue colourC, Ogre::ColourValue colourD);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Load an image from a resource.
\details Try to load an image from a resource.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferImage(256);
Procedural::Image(&bufferImage).setFile("red_brick.jpg").process();
\endcode
\image html texture_image.png
*/
class _ProceduralExport Image : public TextureProcessing
{
private:
	Ogre::String mFile;
	Ogre::String mGroup;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Image(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Image")
	{
	}

	/**
	Set the colour of the background.
	\param filename Name of an image file to load.
	\param groupname Name of the resource group to search for the image
	*/
	Image& setFile(Ogre::String filename, Ogre::String groupname = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Creates a labyrinth structured image.
\details Creates a labyrinth structure from a specified perlin noise on a coloured background.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferLabyrinth(256);
Procedural::Labyrinth(&bufferLabyrinth).process();
\endcode
\image html texture_labyrinth.png
*/
class _ProceduralExport Labyrinth : public TextureProcessing
{
private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Labyrinth(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Labyrinth"), mColour(Ogre::ColourValue::White), mSeed(5120)
	{
	}

	/**
	Set the colour of the background.
	\param colour New colour for background (default Ogre::ColourValue::White)
	*/
	Labyrinth& setColour(Ogre::ColourValue colour);

	/**
	Sets the colour of the background
	\param red Red value of background colour [0.0, 1.0] \(default 1.0)
	\param green Green value of background colour [0.0, 1.0] \(default 1.0)
	\param blue Blue value of background colour [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of background colour [0.0, 1.0] \(default 1.0)
	*/
	Labyrinth& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Labyrinth& setSeed(Ogre::uint seed);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Creates a marble structured image.
\details Creates a marbel structure from a specified perlin noise on a coloured background.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferMarble(256);
Procedural::Marble(&bufferMarble).process();
\endcode
\image html texture_marble.png
*/
class _ProceduralExport Marble : public TextureProcessing
{
private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Marble(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Marble"), mColour(Ogre::ColourValue::White), mSeed(5120)
	{
	}

	/**
	Set the colour of the background.
	\param colour New colour for marble structure (default Ogre::ColourValue::White)
	*/
	Marble& setColour(Ogre::ColourValue colour);

	/**
	Set the colour of the background.
	\param red Red value of the marble structure colour [0.0, 1.0] \(default 1.0)
	\param green Green value of the marble structure colour [0.0, 1.0] \(default 1.0)
	\param blue Blue valu of the marble structure coloure [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of the marble structure colour [0.0, 1.0] \(default 1.0)
	*/
	Marble& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Marble& setSeed(Ogre::uint seed);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Fills full image with noise in a given colour.
\details High quality noise with various noise algorithms.

Examples:
\b White noise (default)
\code{.cpp}
Procedural::TextureBuffer bufferNoiseWhite(256);
Procedural::Noise(&bufferNoiseWhite).setType(Procedural::Noise::NOISE_WHITE).process();
\endcode
\image html texture_noise_white.png

\b Perlin noise
\code{.cpp}
Procedural::TextureBuffer bufferNoisePerlin(256);
Procedural::Noise(&bufferNoisePerlin).setType(Procedural::Noise::NOISE_PERLIN).process();
\endcode
\image html texture_noise_perlin.png
*/
class _ProceduralExport Noise : public TextureProcessing
{
public:
	//! Noise generator type
	enum NOISE_TYPE
	{
	    NOISE_WHITE,	//!< White noise
	    NOISE_PERLIN	//!< Perlin noise
	};

private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;
	NOISE_TYPE mType;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Noise(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Noise"), mColour(Ogre::ColourValue::White), mSeed(5120), mType(NOISE_WHITE)
	{
	}

	/**
	Set the colour of the noise.
	\param colour New colour of the noise (default Ogre::ColourValue::White)
	*/
	Noise& setColour(Ogre::ColourValue colour);

	/**
	Set the colour of the noise.
	\param red Red value of the noise colour [0.0, 1.0] \(default 1.0)
	\param green Green value of the noise colour [0.0, 1.0] \(default 1.0)
	\param blue Blue value of the noise colour [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of the noise colour [0.0, 1.0] \(default 1.0)
	*/
	Noise& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Noise& setSeed(Ogre::uint seed);

	/**
	Set the type of noise generation.
	\param type Type of noise generator (default NOISE_WHITE)
	*/
	Noise& setType(NOISE_TYPE type);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Fills full image with given colour.
\details Set all pixel to the same color.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferSolid(256);
Procedural::Solid(&bufferSolid).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
\endcode
\image html texture_solid.png
*/
class _ProceduralExport Solid : public TextureProcessing
{
private:
	Ogre::ColourValue mColour;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Solid(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Solid"), mColour(Ogre::ColourValue::Black)
	{
	}

	/**
	Sets the fill colour of the image.
	\param colour New colour for processing (default Ogre::ColourValue::Black)
	*/
	Solid& setColour(Ogre::ColourValue colour);

	/**
	Sets the fill colour of the image.
	\param red Red value [0.0, 1.0] \(default 0.0)
	\param green Green value [0.0, 1.0] \(default 0.0)
	\param blue Blue value [0.0, 1.0] \(default 0.0)
	\param alpha %Alpha value [0.0, 1.0] \(default 1.0)
	*/
	Solid& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Creates a textile structured image.
\details Creates a textile structure from a specified perlin noise on a coloured background.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferTextile(256);
Procedural::Textile(&bufferTextile).process();
\endcode
\image html texture_textile.png
*/
class _ProceduralExport Textile : public TextureProcessing
{
private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Textile(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Textile"), mColour(Ogre::ColourValue::White), mSeed(5120)
	{
	}

	/**
	Set the colour of the background.
	\param colour New colour for background (default Ogre::ColourValue::White)
	*/
	Textile& setColour(Ogre::ColourValue colour);

	/**
	Set the colour of the background.
	\param red Red value of background colour [0.0, 1.0] \(default 1.0)
	\param green Green value of background colour [0.0, 1.0] \(default 1.0)
	\param blue Blue value of background colour [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of background colour [0.0, 1.0] \(default 1.0)
	*/
	Textile& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Textile& setSeed(Ogre::uint seed);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/**
\brief Creates a wood slice image.
\details Creates a structure of annual rings from a specified perlin noise on a coloured background.

Example:
\code{.cpp}
Procedural::TextureBuffer bufferWood(256);
Procedural::Wood(&bufferWood).setRings(5).process();
\endcode
\image html texture_wood.png
*/
class _ProceduralExport Wood : public TextureProcessing
{
private:
	Ogre::ColourValue mColour;
	Ogre::uint mSeed;
	Ogre::uint mRings;

public:
	/**
	Default constructor.
	\param pBuffer Image buffer where to store the generated image.
	*/
	Wood(TextureBufferPtr pBuffer)
		: TextureProcessing(pBuffer, "Wood"), mColour(Ogre::ColourValue::White), mSeed(5120), mRings(8)
	{
	}

	/**
	Set the colour of the background.
	\param colour New colour for background (default Ogre::ColourValue::White)
	*/
	Wood& setColour(Ogre::ColourValue colour);

	/**
	Set the colour of the background.
	\param red Red value of background colour [0.0, 1.0] \(default 1.0)
	\param green Green value of background colour [0.0, 1.0] \(default 1.0)
	\param blue Blue value of background colour [0.0, 1.0] \(default 1.0)
	\param alpha %Alpha value of background colour [0.0, 1.0] \(default 1.0)
	*/
	Wood& setColour(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha = 1.0f);

	/**
	Set the seed for "random" number generator.
	\param seed Seed value where to set the random number generator (default 5120)
	*/
	Wood& setSeed(Ogre::uint seed);

	/**
	Sets the number of annual rings.
	\param rings Number of annual rings (minimum 3, default 8)
	*/
	Wood& setRings(Ogre::uint rings);

	/**
	Run image generation
	\return Pointer to image buffer which has been set in the constructor.
	*/
	TextureBufferPtr process();
};

/** @} */
}
#endif
