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
	
	class IManualGeometry : public Geometry
	{
	protected:
		IManualGeometry(std::string name, std::string parentNodeName) : Geometry(name, parentNodeName, Entity::GEOMETRY_MANUAL) {}
		virtual ~IManualGeometry() {};
		
	public:
		virtual int getIndex() = 0;
		
		virtual Vector3Vector getVertexList() = 0;
		
		virtual Vector3Vector getNormalList() = 0;
		
		virtual Vector3Vector getColorList() = 0;
		
		virtual IntVector getIndexList() = 0;
	
		virtual Vector3Vector getTriangleList() = 0;
		
		virtual Vector3Vector getTextureCoordList() = 0;
		
		virtual void getVertexList(Vector3Vector vertices) = 0;
		
		virtual void getNormalList(Vector3Vector normals) = 0;
		
		virtual void getColorList(Vector3Vector colors) = 0;
		
		virtual void getIndexList(IntVector indices) = 0;
	
		virtual void getTriangleList(Vector3Vector triangles) = 0;
		
		virtual void getTextureCoordList(Vector3Vector textureCoords) = 0;
	};
}

#endif
