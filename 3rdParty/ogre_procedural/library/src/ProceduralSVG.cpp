/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

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
#include "ProceduralStableHeaders.h"
#include "ProceduralSVG.h"
#include "ProceduralShape.h"
#include "ProceduralShapeGenerators.h"
#include "ProceduralPath.h"
#include "ProceduralPathGenerators.h"
#include "rapidxml/rapidxml.hpp"

using namespace Ogre;

namespace Procedural
{
void SvgLoader::parseSvgFile(MultiShape& out, const String& fileName, const String& groupName, int segmentsNumber)
{
	mNumSeg = segmentsNumber;

	rapidxml::xml_document<> XMLDoc;    // character type defaults to char
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(fileName, groupName);
	char* svg = strdup(stream->getAsString().c_str());
	XMLDoc.parse<0>(svg);

	rapidxml::xml_node<>* pXmlRoot = XMLDoc.first_node("svg");
	if (pXmlRoot == NULL) return;
	rapidxml::xml_node<>* pXmlChildNode = pXmlRoot->first_node();
	while ( pXmlChildNode != NULL )
	{
		parseChildNode(out, pXmlChildNode);
		pXmlChildNode = pXmlChildNode->next_sibling();
	}
}

//-----------------------------------------------------------------------
void SvgLoader::parseChildNode(MultiShape& out, rapidxml::xml_node<>* pChild)
{
	std::string name = pChild->name();
	if (name.size() > 3)
	{
		if (stricmp(name.c_str(), "rect") == 0)
			parseRect(out, pChild);
		else if (stricmp(name.c_str(), "circle") == 0)
			parseCircle(out, pChild);
		else if (stricmp(name.c_str(), "ellipse") == 0)
			parseEllipse(out, pChild);
		else if (stricmp(name.c_str(), "polygon") == 0 || stricmp(name.c_str(), "polyline") == 0)
			parsePolygon(out, pChild);
		else if (stricmp(name.c_str(), "path") == 0)
			parsePath(out, pChild); // svg path is a shape
	}

	rapidxml::xml_node<>* pSubChildNode = pChild->first_node();
	while ( pSubChildNode != NULL )
	{
		parseChildNode(out, pSubChildNode);
		pSubChildNode = pSubChildNode->next_sibling();
	}
}

//-----------------------------------------------------------------------
void SvgLoader::parseRect(MultiShape& out, rapidxml::xml_node<>* pRectNode)
{
	Real width = getAttribReal(pRectNode, "width");
	Real height = getAttribReal(pRectNode, "height");
	if (width <= 0.0f || height <= 0.0f) return;
	Shape s = RectangleShape().setHeight(height).setWidth(width).realizeShape();
//	if(pRectNode->first_attribute("id"))
	//	ss.id = pRectNode->first_attribute("id")->value();
	Vector2 position;
	position.x = getAttribReal(pRectNode, "x");
	position.y = getAttribReal(pRectNode, "y");
	// Our rectangle are centered, but svg rectangles are defined by their corners
	position += .5f*Vector2(width,height);
	Vector2 trans = getAttribTranslate(pRectNode);
	position += trans;
	s.translate(position);

	out.addShape(s);
}

//-----------------------------------------------------------------------
void SvgLoader::parseCircle(MultiShape& out, rapidxml::xml_node<>* pCircleNode)
{
	Real r = getAttribReal(pCircleNode, "r");
	if (r <= 0.0f) return;
	Shape s = CircleShape().setNumSeg(mNumSeg).setRadius(r).realizeShape();
//	if(pCircleNode->first_attribute("id"))
	//	ss.id = pCircleNode->first_attribute("id")->value();
	Vector2 position;
	position.x = getAttribReal(pCircleNode, "cx");
	position.y = getAttribReal(pCircleNode, "cy");
	Vector2 trans = getAttribTranslate(pCircleNode);
	position += trans;
	s.translate(position);
	out.addShape(s);
}

//-----------------------------------------------------------------------
void SvgLoader::parseEllipse(MultiShape& out, rapidxml::xml_node<>* pEllipseNode)
{
	Real rx = getAttribReal(pEllipseNode, "rx");
	Real ry = getAttribReal(pEllipseNode, "ry");
	if (rx <= 0.0f || ry <= 0.0f) return;
	Shape s = EllipseShape().setNumSeg(mNumSeg).setRadiusX(rx).setRadiusY(ry).realizeShape();
//	if(pEllipseNode->first_attribute("id"))
	//	ss.id = pEllipseNode->first_attribute("id")->value();
	Vector2 position;
	position.x = getAttribReal(pEllipseNode, "cx");
	position.y = getAttribReal(pEllipseNode, "cy");
	Vector2 trans = getAttribTranslate(pEllipseNode);
	position += trans;
	s.translate(position);
	out.addShape(s);
}

//-----------------------------------------------------------------------
void SvgLoader::parsePolygon(MultiShape& out, rapidxml::xml_node<>* pPolygonNode)
{
	if (pPolygonNode->first_attribute("points"))
	{
		if (pPolygonNode->first_attribute("points")->value_size() < 3) return;
		std::string temp = xtrim(pPolygonNode->first_attribute("points")->value());
		std::vector<std::string> pts = split(temp, std::string(" "));
		if (pts.size() == 0) return;
		Shape s;
		for (size_t i = 0; i < pts.size() - 1; i+=2)
			s.addPoint(StringConverter::parseReal(pts[i + 0]), StringConverter::parseReal(pts[i + 1]));
		if (s.getPoints().size() == 0) return;
		s.close();
//		if(pPolygonNode->first_attribute("id"))
		//		ss.id = pPolygonNode->first_attribute("id")->value();
		s.translate(getAttribTranslate(pPolygonNode));
		out.addShape(s);
	}
}
//-----------------------------------------------------------------------
void SvgLoader::parsePath(MultiShape& out, rapidxml::xml_node<>* pPathNode)
{
	if (pPathNode->first_attribute("d"))
	{
		std::string temp = xtrim(pPathNode->first_attribute("d")->value()," .-0123456789mMlLhHvVcCsSqQtTaAzZ");
		std::vector<std::string> parts = split(temp, std::string(" "));
		for (size_t i = 0; i < parts.size(); i++)
			if (parts[i].size() > 1 && !(parts[i][0] == '-' || ('0' <= parts[i][0] && parts[i][0] <= '9')))
			{
				parts.insert(parts.begin() + i + 1, parts[i].c_str() + 1);
				parts[i].erase(1, parts[i].size());
			}

		SvgLoaderPath sp(parts, mNumSeg);
		if (!sp.isValid()) return;
		Shape ss = sp.getSvgShape();
		Vector2 line = ss.getPoint(1) - ss.getPoint(0);
		Real deg = line.angleBetween(ss.getPoint(2) - ss.getPoint(0)).valueDegrees();
		if ((0 <= deg && deg <= 180.0f) || (-180.0f <= deg && deg <= 0))
			ss.setOutSide(SIDE_LEFT);
		else
			ss.setOutSide(SIDE_RIGHT);

		//if(pPathNode->first_attribute("id"))
		//	ss.id = pPathNode->first_attribute("id")->value();
		ss.translate(getAttribTranslate(pPathNode));
		out.addShape(ss);
	}
}
//-----------------------------------------------------------------------
Real SvgLoader::getAttribReal(rapidxml::xml_node<>* pNode, const String& attrib, Real defaultValue)
{
	if (pNode->first_attribute(attrib.c_str()))
	{
		size_t len = pNode->first_attribute(attrib.c_str())->value_size();
		if (len == 0) return defaultValue;
		// remove units
		char* tmp = new char[len + 1];
		strcpy(tmp, pNode->first_attribute(attrib.c_str())->value());
		for (size_t i = 0; i <= len; i++)
			if (!(tmp[i] == '.' || ('0' <= tmp[i] && tmp[i] <= '9')))
			{
				tmp[i] = 0;
				break;
			}
		// convert
		Real retVal = StringConverter::parseReal(tmp);
		delete tmp;
		return retVal;
	}
	else
		return defaultValue;
}

//-----------------------------------------------------------------------
Vector2 SvgLoader::getAttribTranslate(rapidxml::xml_node<>* pNode)
{
	if (pNode->first_attribute("transform"))
	{
		std::string temp(pNode->first_attribute("transform")->value());
		int begin = temp.find("translate(");
		if (begin==std::string::npos)
			return Vector2::ZERO;
		begin+=10;
		int end = temp.find(")", begin);
		if (end == std::string::npos)
			return Vector2::ZERO;
		std::string temp2 = temp.substr(begin, end-begin);
		std::vector<std::string> parts = split(xtrim(temp2.c_str()), std::string(" "));
		if (parts.size() == 2)
			return Vector2(StringConverter::parseReal(parts[0]), StringConverter::parseReal(parts[1]));
		else
			return Vector2::ZERO;
	}
	else
		return Vector2::ZERO;
}

//-----------------------------------------------------------------------
std::vector<std::string> SvgLoader::split(const std::string& str, const std::string& delimiters, bool removeEmpty)
{
	std::vector<std::string> tokens;
	std::string::size_type delimPos = 0, tokenPos = 0, pos = 0;

	if (str.empty()) return tokens;
	while (true)
	{
		delimPos = str.find_first_of(delimiters, pos);
		tokenPos = str.find_first_not_of(delimiters, pos);

		if (std::string::npos != delimPos)
		{
			if (std::string::npos != tokenPos)
			{
				if (tokenPos < delimPos)
					tokens.push_back(str.substr(pos,delimPos-pos));
				else
				{
					if (!removeEmpty) tokens.push_back("");
				}
			}
			else
			{
				if (!removeEmpty) tokens.push_back("");
			}
			pos = delimPos+1;
		}
		else
		{
			if (std::string::npos != tokenPos)
				tokens.push_back(str.substr(pos));
			else
			{
				if (!removeEmpty) tokens.push_back("");
			}
			break;
		}
	}
	return tokens;
}

//-----------------------------------------------------------------------
std::string SvgLoader::xtrim(const char* val, const char* achar, char rchar)
{
	if (val == NULL) return std::string();
	size_t len = strlen(val);
	char* tmp = new char[len + 1];
	strcpy(tmp, val);
	tmp[len] = 0;
	for (size_t i = 0; i < len; i++)
	{
		bool valid = false;
		for (Ogre::uint j = 0; j < strlen(achar); j++)
		{
			valid = (tmp[i] == achar[j]);
			if (valid) break;
		}
		if (!valid) tmp[i] = rchar;
	}
	std::string temp(tmp);
	delete tmp;
	return temp;
}

//-----------------------------------------------------------------------
SvgLoader::SvgLoaderPath::SvgLoaderPath(std::vector<std::string> p, unsigned int ns)
	: parts(p), mNumSeg(ns), px(0.0f), py(0.0f), index(0)
{
	char lastCmd = 0;

	while (index < p.size())
	{
		try
		{
			char newCmd = parts[index][0];
			bool next = true;
			if (lastCmd != newCmd && newCmd != '.' && newCmd != '-' && (newCmd < '0' || newCmd > '9') && curve.size() > 3
			        && ((lastCmd =='c' || lastCmd == 'C') && (newCmd =='s' || newCmd == 'S') || (lastCmd =='q' || lastCmd == 'Q') && (newCmd =='t' || newCmd == 'T')))
			{
				// finish curve
				finishCurve(lastCmd);
			}
			switch (newCmd)
			{
			case 'l':
				parseLineTo(true, next);
				break;
			case 'L':
				parseLineTo(false, next);
				break;
			case 'm':
				parseMoveTo(true, next);
				newCmd = 'l';
				break;
			case 'M':
				parseMoveTo(false, next);
				newCmd = 'L';
				break;
			case 'h':
				parseHLineTo(true, next);
				break;
			case 'H':
				parseHLineTo(false, next);
				break;
			case 'v':
				parseVLineTo(true, next);
				break;
			case 'V':
				parseVLineTo(false, next);
				break;
			case 'c':
				curve.push_back(point);
				parseCurveCTo(true, next);
				break;
			case 'C':
				curve.push_back(point);
				parseCurveCTo(false, next);
				break;
			case 's':
				parseCurveSTo(true, next);
				break;
			case 'S':
				parseCurveSTo(false, next);
				break;
			case 'q':
				curve.push_back(point);
				parseCurveQTo(true, next);
				break;
			case 'Q':
				curve.push_back(point);
				parseCurveQTo(false, next);
				break;
			case 't':
				parseCurveTTo(true, next);
				break;
			case 'T':
				parseCurveTTo(false, next);
				break;
			case 'a':
				parseArcTo(true, next);
				break;
			case 'A':
				parseArcTo(false, next);
				break;
			case 'z':
			case 'Z':
				shape.close();
				index++;
				break;
			default:
				newCmd = lastCmd;
				next = false;
				switch (lastCmd)
				{
				case 'l':
					parseLineTo(true, next);
					break;
				case 'L':
					parseLineTo(false, next);
					break;
				case 'm':
					parseMoveTo(true, next);
					break;
				case 'M':
					parseMoveTo(false, next);
					break;
				case 'h':
					parseHLineTo(true, next);
					break;
				case 'H':
					parseHLineTo(false, next);
					break;
				case 'v':
					parseVLineTo(true, next);
					break;
				case 'V':
					parseVLineTo(false, next);
					break;
				case 'c':
					parseCurveCTo(true, next);
					break;
				case 'C':
					parseCurveCTo(false, next);
					break;
				case 's':
					parseCurveSTo(true, next);
					break;
				case 'S':
					parseCurveSTo(false, next);
					break;
				case 'q':
					parseCurveQTo(true, next);
					break;
				case 'Q':
					parseCurveQTo(false, next);
					break;
				case 't':
					parseCurveTTo(true, next);
					break;
				case 'T':
					parseCurveTTo(false, next);
					break;
				case 'a':
					parseArcTo(true, next);
					break;
				case 'A':
					parseArcTo(false, next);
					break;

				default:
					break;
				}
				break;
			}
			lastCmd = newCmd;
		}
		catch (...)
		{
		}
	}
	if (curve.size() > 0) finishCurve(lastCmd);
}

//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseArcTo(bool rel, bool next)
{
	if (next) index++;
	Real rx = 0.0f;
	if (!parseReal(&rx))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real ry = 0.0f;
	if (!parseReal(&ry))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real x_axis_rotation = 0.0f;
	if (!parseReal(&x_axis_rotation))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real large_arc_flag = 0.0f;
	if (!parseReal(&large_arc_flag))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real sweep_flag = 0.0f;
	if (!parseReal(&sweep_flag))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");

	Real RadiansPerDegree = Math::PI / 180.0f;
	Real epx = rel ? point.x + x : x;
	Real epy = rel ? point.y + y : y;
	bool largeArc = (large_arc_flag > 0);
	bool clockwise = (sweep_flag > 0);

	if (epx == point.x && epy == point.y)
		return;

	if (rx == 0.0f && ry == 0.0f)
	{
		point = Vector2(epx, epy);
		shape.addPoint(point);
		return;
	}

	Real sinPhi = sin(x_axis_rotation * RadiansPerDegree);
	Real cosPhi = cos(x_axis_rotation * RadiansPerDegree);

	Real x1dash = cosPhi * (point.x - epx) / 2.0f + sinPhi * (point.y - epy) / 2.0f;
	Real y1dash = -sinPhi * (point.x - epx) / 2.0f + cosPhi * (point.y - epy) / 2.0f;

	Real root;
	Real numerator = rx * rx * ry * ry - rx * rx * y1dash * y1dash - ry * ry * x1dash * x1dash;

	if (numerator < 0.0)
	{
		Real s = (Real)sqrt(1.0f - numerator / (rx * rx * ry * ry));

		rx *= s;
		ry *= s;
		root = 0.0;
	}
	else
	{
		root = ((largeArc && clockwise) || (!largeArc && !clockwise) ? -1.0f : 1.0f) * sqrt(numerator / (rx * rx * y1dash * y1dash + ry * ry * x1dash * x1dash));
	}

	Real cxdash = root * rx * y1dash / ry;
	Real cydash = -root * ry * x1dash / rx;

	Real cx = cosPhi * cxdash - sinPhi * cydash + (point.x + epx) / 2.0f;
	Real cy = sinPhi * cxdash + cosPhi * cydash + (point.y + epy) / 2.0f;

	Real theta1 = CalculateVectorAngle(1.0, 0.0, (x1dash - cxdash) / rx, (y1dash - cydash) / ry);
	Real dtheta = CalculateVectorAngle((x1dash - cxdash) / rx, (y1dash - cydash) / ry, (-x1dash - cxdash) / rx, (-y1dash - cydash) / ry);

	if (!clockwise && dtheta > 0)
		dtheta -= 2.0f * Math::PI;
	else if (clockwise && dtheta < 0)
		dtheta += 2.0f * Math::PI;

	int segments = (int)ceil((double)abs(dtheta / (Math::PI / 2.0f)));
	Real delta = dtheta / segments;
	Real t = 8.0f / 3.0f * sin(delta / 4.0f) * sin(delta / 4.0f) / sin(delta / 2.0f);

	Real startX = point.x;
	Real startY = point.y;

	BezierCurve2 bezier;
	bezier.addPoint(startX, startY);
	for (int i = 0; i < segments; ++i)
	{
		Real cosTheta1 = cos(theta1);
		Real sinTheta1 = sin(theta1);
		Real theta2 = theta1 + delta;
		Real cosTheta2 = cos(theta2);
		Real sinTheta2 = sin(theta2);

		Real endpointX = cosPhi * rx * cosTheta2 - sinPhi * ry * sinTheta2 + cx;
		Real endpointY = sinPhi * rx * cosTheta2 + cosPhi * ry * sinTheta2 + cy;

		Real dx1 = t * (-cosPhi * rx * sinTheta1 - sinPhi * ry * cosTheta1);
		Real dy1 = t * (-sinPhi * rx * sinTheta1 + cosPhi * ry * cosTheta1);

		Real dxe = t * (cosPhi * rx * sinTheta2 + sinPhi * ry * cosTheta2);
		Real dye = t * (sinPhi * rx * sinTheta2 - cosPhi * ry * cosTheta2);

		bezier.addPoint(startX + dx1, startY + dy1);
		bezier.addPoint(endpointX + dxe, endpointY + dye);

		theta1 = theta2;
		startX = endpointX;
		startY = endpointY;
	}
	point = Vector2(epx, epy);
	bezier.addPoint(point);
	bezier.setNumSeg(mNumSeg);
	std::vector<Vector2> pointList = bezier.realizeShape().getPoints();
	Vector2 lp = shape.getPoint(shape.getPoints().size() - 1);
	for (std::vector<Vector2>::iterator iter = pointList.begin(); iter != pointList.end(); iter++)
	{
		if (iter == pointList.begin())
		{
			if (*iter != lp) shape.addPoint(*iter);
		}
		else
			shape.addPoint(*iter);
	}
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::finishCurve(char lc)
{
	int n;
	if (lc == 'c' || lc == 'C' || lc == 's' || lc == 'S')
		n = 3;
	else if (lc == 'q' || lc == 'Q' || lc == 't' || lc == 'T')
		n = 2;
	else
		n = curve.size() - 1;

	for (size_t i = 0; i < curve.size(); i += n)
	{
		if (i + 3 >= curve.size()) break;
		BezierCurve2 bc2;
		bc2.setNumSeg(mNumSeg);
		bc2.addPoint(curve[i + 0]);
		bc2.addPoint(curve[i + 1]);
		bc2.addPoint(curve[i + 2]);
		bc2.addPoint(curve[i + 3]);
		Shape bc2shape = bc2.realizeShape();
		Vector2 lp = shape.getPoint(shape.getPoints().size() - 1);
		for (std::vector<Vector2>::iterator iter = bc2shape.getPoints().begin(); iter != bc2shape.getPoints().end(); iter++)
		{
			if (iter == bc2shape.getPoints().begin())
			{
				if (*iter != lp) shape.addPoint(*iter);
			}
			else
				shape.addPoint(*iter);
		}
	}
	curve.clear();
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseCurveSTo(bool rel, bool next)
{
	if (next) index++;
	Vector2 offset = Vector2::ZERO;
	if (rel) offset = point;

	Real x1 = point.x;
	Real y1 = point.y;
	if (curve.size() > 2)
	{
		Vector2 mirror = curve[curve.size() - 2];
		Vector2 diff = mirror - point;
		x1 = -1.0f * diff.x + point.x;
		y1 = -1.0f * diff.y + point.y;
	}
	Real x2 = 0.0f;
	if (!parseReal(&x2))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real y2 = 0.0f;
	if (!parseReal(&y2))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveSTo");
	curve.push_back(Vector2(x1, y1) + offset);
	curve.push_back(Vector2(x2, y2) + offset);
	point = Vector2(x, y) + offset;
	curve.push_back(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseCurveQTo(bool rel, bool next)
{
	if (next) index++;
	Vector2 offset = Vector2::ZERO;
	if (rel) offset = point;

	Real x1 = 0.0f;
	if (!parseReal(&x1))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveQTo");
	Real y1 = 0.0f;
	if (!parseReal(&y1))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveQTo");
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveQTo");
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveQTo");
	curve.push_back(Vector2(x1, y1) + offset);
	point = Vector2(x, y) + offset;
	curve.push_back(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseCurveTTo(bool rel, bool next)
{
	if (next) index++;
	Vector2 offset = Vector2::ZERO;
	if (rel) offset = point;

	Real x1 = point.x;
	Real y1 = point.y;
	if (curve.size() > 2)
	{
		Vector2 mirror = curve[curve.size() - 2];
		Vector2 diff = mirror - point;
		x1 = -1.0f * diff.x + point.x;
		y1 = -1.0f * diff.y + point.y;
	}
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveTTo");
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveTTo");
	curve.push_back(Vector2(x1, y1) + offset);
	point = Vector2(x, y) + offset;
	curve.push_back(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseCurveCTo(bool rel, bool next)
{
	if (next) index++;
	Vector2 offset = Vector2::ZERO;
	if (rel) offset = point;

	Real x1 = 0.0f;
	if (!parseReal(&x1))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveCTo");
	Real y1 = 0.0f;
	if (!parseReal(&y1))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveCTo");
	Real x2 = 0.0f;
	if (!parseReal(&x2))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveCTo");
	Real y2 = 0.0f;
	if (!parseReal(&y2))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveCTo");
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveCTo");
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseCurveCTo");
	curve.push_back(Vector2(x1, y1) + offset);
	curve.push_back(Vector2(x2, y2) + offset);
	point = Vector2(x, y) + offset;
	curve.push_back(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseMoveTo(bool rel, bool next)
{
	if (next) index++;
	Real x,y;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseMoveTo");
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseMoveTo");
	point = Vector2(x, y);
	shape.addPoint(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseLineTo(bool rel, bool next)
{
	if (next) index++;
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseLineTo");
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseLineTo");
	if (rel)
		point = Vector2(point.x + x, point.y + y);
	else
		point = Vector2(x, y);
	shape.addPoint(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseHLineTo(bool rel, bool next)
{
	if (next) index++;
	Real x = 0.0f;
	if (!parseReal(&x))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseHLineTo");
	if (rel)
		point.x += x;
	else
		point.x = x;
	shape.addPoint(point);
}
//-----------------------------------------------------------------------
void SvgLoader::SvgLoaderPath::parseVLineTo(bool rel, bool next)
{
	if (next) index++;
	Real y = 0.0f;
	if (!parseReal(&y))
		OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Expecting a Real number", "parseVLineTo");
	if (rel)
		point.y += y;
	else
		point.y = y;
	shape.addPoint(point);
}
}
