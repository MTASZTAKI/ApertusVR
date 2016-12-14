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
#ifndef PROCEDURAL_MULTISHAPE_GENERATORS_INCLUDED
#define PROCEDURAL_MULTISHAPE_GENERATORS_INCLUDED

#include "ProceduralMultiShape.h"

namespace Procedural
{
/**
 * \ingroup shapegrp
 * @{
 */
#ifdef PROCEDURAL_USE_FREETYPE
//-----------------------------------------------------------------------
/**
 * Produces a multishape from a given text
 * \image html shape_text.png
 */
class _ProceduralExport TextShape
{
private:
	Ogre::String mText;
	Ogre::String mFontName;
	Ogre::uchar mFontSize;

public:
	/**
	Default constructor.
	*/
	TextShape()
		: mText("OgreProcedural"), mFontName("Arial"), mFontSize(12)
	{
	}

	/**
	Set the text content.
	\param text New text for processing (default "OgreProcedural")
	\exception Ogre::InvalidParametersException Empty text
	*/
	inline TextShape& setText(Ogre::String text)
	{
		if (text.empty())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 characters in text", "Procedural::TextShape::setText(Ogre::String text)");

		mText = text;
		return *this;
	}

	/**
	Set the font for the text.
	\param fontName Filenpath of a font or name of font (only on windows desktops)
	\param fontSize Size of font [px] (default 12)
	\exception Ogre::InvalidParametersException Empty font name
	\exception Ogre::InvalidParametersException Font size is below 4
	\todo Add search for font names on non windows systems.
	*/
	inline TextShape& setFont(Ogre::String fontName, Ogre::uchar fontSize)
	{
		if (fontName.empty())
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "There must be more than 0 characters in font name", "Procedural::TextShape::setFont(Ogre::String fontName, Ogre::uchar fontSize)");
		if (fontSize < 4)
			OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "Minimum font size is 4", "Procedural::TextShape::setFont(Ogre::String fontName, Ogre::uchar fontSize)");

		mFontName = fontName;
		mFontSize = fontSize;
		return *this;
	}


	/**
	 * Build a MultiShape from chars (one Shape per character)
	 * \exception Ogre::InternalErrorException Freetype error
	 * \todo Need to split shapes of multi region chars. For example the letter \c O
	 * has two shapes, but they are connected to one shape.
	 */
	MultiShape realizeShapes();

private:
	Ogre::String getFontFileByName();

#if PROCEDURAL_PLATFORM == PROCEDURAL_PLATFORM_WIN32
	bool getFontFile(Ogre::String fontName, Ogre::String& displayName, Ogre::String& filePath);
#endif
};
#endif

/** @} */
}

#endif
