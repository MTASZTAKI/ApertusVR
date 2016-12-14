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
#include "ProceduralShape.h"
#include "ProceduralShapeGenerators.h"
#include "ProceduralMultiShapeGenerators.h"
#ifdef PROCEDURAL_USE_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#endif // PROCEDURAL_USE_FREETYPE

using namespace Ogre;

namespace Procedural
{

//-----------------------------------------------------------------------
#ifdef PROCEDURAL_USE_FREETYPE

MultiShape TextShape::realizeShapes()
{
	MultiShape retVal;

	FT_Library ftlib;
	FT_Face face;
	FT_GlyphSlot slot;

	FT_Error error = FT_Init_FreeType(&ftlib);
	if (error == 0)
	{
		error = FT_New_Face(ftlib, getFontFileByName().c_str(), 0, &face);
		if (error == FT_Err_Unknown_File_Format)
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FreeType ERROR: FT_Err_Unknown_File_Format", "Procedural::TextShape::realizeShapes()");
		}
		else if (error)
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FreeType ERROR: FT_New_Face - " + Ogre::StringConverter::toString(error), "Procedural::TextShape::realizeShapes()");
		}
		else
		{
			FT_Set_Pixel_Sizes(face, 0, mFontSize);

			size_t px = 0;
			size_t py = 0;
			slot = face->glyph;

			for (size_t n = 0; n < mText.length(); n++)
			{
				error = FT_Load_Char(face, mText[n], FT_LOAD_NO_BITMAP);
				if (error) continue;

				Shape s;

				int nContours   = face->glyph->outline.n_contours;
				int startPos    = 0;
				char* tags     = face->glyph->outline.tags;
				FT_Vector* vec = face->glyph->outline.points;

				for (int k = 0; k < nContours; k++)
				{
					if (k > 0) startPos = face->glyph->outline.contours[k-1]+1;
					int endPos = face->glyph->outline.contours[k]+1;

					Ogre::Vector2 lastPoint = Ogre::Vector2::ZERO;
					for (int j = startPos; j < endPos; j++)
					{
						if (FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_ON)
						{
							lastPoint = Ogre::Vector2((Ogre::Real)vec[j].x, (Ogre::Real)vec[j].y);
							s.addPoint(lastPoint / 64.0f);
						}
						else
						{
							if (FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_CUBIC)
							{
								int prevPoint = j - 1;
								if (j == 0) prevPoint = endPos - 1;
								int nextIndex = j + 1;
								if (nextIndex >= endPos) nextIndex = startPos;
								Ogre::Vector2 nextPoint((Ogre::Real)vec[nextIndex].x, (Ogre::Real)vec[nextIndex].y);
								if ((FT_CURVE_TAG(tags[prevPoint]) != FT_CURVE_TAG_ON) && (FT_CURVE_TAG(tags[prevPoint]) == FT_CURVE_TAG_CUBIC))
								{
									BezierCurve2 bc;
									bc.addPoint(Ogre::Vector2((Ogre::Real)vec[prevPoint].x, (Ogre::Real)vec[prevPoint].y) / 64.0f);
									bc.addPoint(Ogre::Vector2((Ogre::Real)vec[j].x, (Ogre::Real)vec[j].y) / 64.0f);
									bc.addPoint(Ogre::Vector2((Ogre::Real)vec[nextIndex].x, (Ogre::Real)vec[nextIndex].y) / 64.0f);
									s.appendShape(bc.realizeShape());
								}
							}
							else
							{
								Ogre::Vector2 conicPoint((Ogre::Real)vec[j].x, (Ogre::Real)vec[j].y);
								if (j == startPos)
								{
									if ((FT_CURVE_TAG(tags[endPos-1]) != FT_CURVE_TAG_ON) && (FT_CURVE_TAG(tags[endPos-1]) != FT_CURVE_TAG_CUBIC))
									{
										Ogre::Vector2 lastConnic((Ogre::Real)vec[endPos - 1].x, (Ogre::Real)vec[endPos - 1].y);
										lastPoint = (conicPoint + lastConnic) / 2;
									}
								}

								int nextIndex = j + 1;
								if (nextIndex >= endPos) nextIndex = startPos;

								Ogre::Vector2 nextPoint((Ogre::Real)vec[nextIndex].x, (Ogre::Real)vec[nextIndex].y);

								bool nextIsConnic = (FT_CURVE_TAG(tags[nextIndex]) != FT_CURVE_TAG_ON) && (FT_CURVE_TAG(tags[nextIndex]) != FT_CURVE_TAG_CUBIC);
								if (nextIsConnic)
									nextPoint = (conicPoint + nextPoint) / 2;

								size_t pc = s.getPointCount();
								BezierCurve2 bc;
								if (pc == 0)
									bc.addPoint(Ogre::Vector2::ZERO);
								else
									bc.addPoint(s.getPoint(pc - 1));
								bc.addPoint(lastPoint / 64.0f);
								bc.addPoint(conicPoint / 64.0f);
								bc.addPoint(nextPoint / 64.0f);
								if (pc == 0)
									s.appendShape(bc.realizeShape());
								else
								{
									std::vector<Ogre::Vector2> subShape = bc.realizeShape().getPoints();
									for (std::vector<Ogre::Vector2>::iterator iter = subShape.begin(); iter != subShape.end(); iter++)
									{
										if (iter != subShape.begin()) s.addPoint(*iter);
									}
								}

								if (nextIsConnic) lastPoint = nextPoint;
							}
						}
					}
				}

				s.close();
				s.translate((Ogre::Real)px, (Ogre::Real)py);
				retVal.addShape(s);

				px += slot->advance.x >> 6;
				py += slot->advance.y >> 6;
			}
			FT_Done_Face(face);
		}
		FT_Done_FreeType(ftlib);
	}
	else
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "FreeType ERROR: FT_Init_FreeTyp", "Procedural::TextShape::realizeShapes()");
	}

	return retVal;
}

Ogre::String TextShape::getFontFileByName()
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
bool TextShape::getFontFile(Ogre::String fontName, Ogre::String& displayName, Ogre::String& filePath)
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

#endif

}