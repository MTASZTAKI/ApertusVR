/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

Copyright (c) 2012 Michael Broutin

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
#ifndef PROCEDURAL_SVG_INCLUDED
#define PROCEDURAL_SVG_INCLUDED

#include "ProceduralPath.h"
#include "ProceduralShape.h"
#include "ProceduralPlatform.h"
#include "ProceduralShapeGenerators.h"
#include <vector>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// Forward declarations
namespace rapidxml
{
template<class Ch> class xml_node;
}
#endif

namespace Procedural
{
//-----------------------------------------------------------------------
/**
Load shapes from an SVG file

\b Example:
\code
	// -- Extrude SVG primitive shapes
	Procedural::Path p;
	p.addPoint(0, 0, 0);
	p.addPoint(0, 100, 0);
	int nr = 1;

	Procedural::SvgLoader svg("test.svg", "Essential", 16);
	for(Procedural::SvgLoader::iterator svgShape = svg.begin(); svgShape != svg.end(); svgShape++)
	{
		Procedural::Extruder().setShapeToExtrude(&svgShape->shape).setExtrusionPath(&p).setScale(1).realizeMesh(svgShape->id);
		putMesh2(svgShape->id, Ogre::Vector3(svgShape->position.x, 10, svgShape->position.y));
	}
\endcode
 */
class _ProceduralExport SvgLoader
{
private:
	unsigned int mNumSeg;

	/** Internal class to parse path element */
	class SvgLoaderPath
	{
	private:
		size_t index;
		std::vector<std::string> parts;
		std::vector<Ogre::Vector2> curve;
		Ogre::Vector2 point;
		unsigned int mNumSeg;

	public:
		Shape shape;
		Ogre::Real px;
		Ogre::Real py;

		SvgLoaderPath(std::vector<std::string> p, unsigned int ns);

		inline void finishCurve(char lc);

		inline Shape getSvgShape()
		{
			shape.translate(px, py);
			return shape;
		}

		inline bool isValid()
		{
			return shape.getPoints().size() > 2;
		}

	private:
		inline void parseMoveTo(bool rel, bool next);

		inline void parseLineTo(bool rel, bool next);

		inline void parseHLineTo(bool rel, bool next);

		inline void parseVLineTo(bool rel, bool next);

		void parseCurveCTo(bool rel, bool next);

		void parseCurveSTo(bool rel, bool next);

		void parseCurveQTo(bool rel, bool next);

		void parseCurveTTo(bool rel, bool next);

		inline Ogre::Real CalculateVectorAngle(Ogre::Real ux, Ogre::Real uy, Ogre::Real vx, Ogre::Real vy) const
		{
			double ta = atan2(uy, ux);
			double tb = atan2(vy, vx);

			if (tb >= ta)
				return (Ogre::Real)(tb - ta);

			return Ogre::Math::TWO_PI - (Ogre::Real)(ta - tb);
		}

		void parseArcTo(bool rel, bool next);

		inline bool parseReal(Ogre::Real* var, Ogre::Real defaultReal = 0.0f)
		{
			if (var == NULL) return false;
			if (index >= parts.size()) return false;
			try
			{
				*var = Ogre::StringConverter::parseReal(parts[index], defaultReal);
				index++;
			}
			catch (...)
			{
				return false;
			}
			return true;
		}
	};

public:

	/**
	Parses a SVG file
	@param out MultiShape object where to store shapes from svg file
	@param fileName Filename of svg file
	@param groupName Resource group where svg file is listed
	@param segmentsNumber Number of segments for curves
	*/
	void parseSvgFile(MultiShape& out, const Ogre::String& fileName, const Ogre::String& groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, int segmentsNumber = 8);

private:
	void parseChildNode(MultiShape& out, rapidxml::xml_node<char>* pChild);
	void parseRect(MultiShape& out, rapidxml::xml_node<char>* pRectNode);
	void parseCircle(MultiShape& out, rapidxml::xml_node<char>* pCircleNode);
	void parseEllipse(MultiShape& out, rapidxml::xml_node<char>* pEllipseNode);
	void parsePolygon(MultiShape& out, rapidxml::xml_node<char>* pPolygonNode);
	void parsePath(MultiShape& out, rapidxml::xml_node<char>* pPathNode);

	Ogre::Real getAttribReal(rapidxml::xml_node<char>* pNode, const Ogre::String& attrib, Ogre::Real defaultValue = 0.0f);
	Ogre::Vector2 getAttribTranslate(rapidxml::xml_node<char>* pNode);
	std::vector<std::string> split(const std::string& str, const std::string& delimiters, bool removeEmpty = true);
	std::string xtrim(const char* val, const char* achar = " .-0123456789", char rchar = ' ');
};

}
#endif
