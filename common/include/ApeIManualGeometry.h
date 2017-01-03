/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_IMANUALGEOMETRY_H
#define APE_IMANUALGEOMETRY_H

#include <string>
#include <vector>
#include "ApeGeometry.h"
#include "ApeVector3.h"

namespace Ape
{	
	typedef std::vector < int > IntVector;

	struct ManualGeometryParameter
	{
		Ape::Vector3Vector vertexList;
		Ape::Vector3Vector normalList;
		Ape::Vector3Vector colorList;
		Ape::IntVector indexList;
		Ape::Vector3Vector triangleList;
		Ape::Vector3Vector textureCoordList;

		ManualGeometryParameter()
		{
			vertexList = Ape::Vector3Vector();
			normalList = Ape::Vector3Vector();
			colorList = Ape::Vector3Vector();
			indexList = Ape::IntVector();
			triangleList = Ape::Vector3Vector();
			textureCoordList = Ape::Vector3Vector();
		}

		ManualGeometryParameter(
			Ape::Vector3Vector vertexList,
			Ape::Vector3Vector normalList,
			Ape::Vector3Vector colorList,
			Ape::IntVector indexList,
			Ape::Vector3Vector triangleList,
			Ape::Vector3Vector textureCoordList)
		{
			vertexList = vertexList;
			normalList = normalList;
			colorList = colorList;
			indexList = indexList;
			triangleList = triangleList;
			textureCoordList = textureCoordList;
		}
	};
	
	class IManualGeometry : public Geometry
	{
	protected:
		IManualGeometry(std::string name) : Geometry(name, Entity::GEOMETRY_MANUAL) {}

		virtual ~IManualGeometry() {};
		
	public:
		virtual ManualGeometryParameter getParameter() = 0;
		
		virtual void setParameter(ManualGeometryParameter parameter) = 0;

		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual void setMaterial(Ape::MaterialWeakPtr material) = 0;

		virtual Ape::MaterialWeakPtr getMaterial() = 0;
	};
}

#endif
