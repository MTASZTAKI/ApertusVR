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

#ifndef APE_IINDEXEDFACESETGEOMETRY_H
#define APE_IINDEXEDFACESETGEOMETRY_H

#include <string>
#include <vector>
#include "ApeGeometry.h"

namespace Ape
{	

	struct GeometryIndexedFaceSetParameters
	{
		std::string groupName;
		Ape::GeometryCoordinates coordinates;
		Ape::GeometryIndices indices;
		Ape::MaterialWeakPtr material;
		
		GeometryIndexedFaceSetParameters()
		{
			this->groupName = std::string();
			this->coordinates = Ape::GeometryCoordinates();
			this->indices = Ape::GeometryIndices();
			this->material = Ape::MaterialWeakPtr();
		}

		GeometryIndexedFaceSetParameters(std::string groupName, Ape::GeometryCoordinates coordinates, Ape::GeometryIndices indices, Ape::MaterialWeakPtr material)
		{
			this->groupName = groupName;
			this->coordinates = coordinates;
			this->indices = indices;
			this->material = material;
		}

		Ape::GeometryCoordinates getCoordinates()
		{
			return coordinates;
		}

		Ape::GeometryIndices getIndices()
		{
			return indices;
		}

		std::string toString() const
		{
			std::ostringstream buff;

			buff << "Coordinates(";
			for (auto const &item : coordinates) buff << item << ", ";
			buff << ")" << std::endl;

			buff << "Indices(";
			for (auto const &item : indices) buff << item << ", ";
			buff << ")" << std::endl;

			return buff.str();
		}
	};
	
	class IIndexedFaceSetGeometry : public Geometry
	{
	protected:
		IIndexedFaceSetGeometry(std::string name) : Geometry(name, Entity::GEOMETRY_INDEXEDFACESET) {}

		virtual ~IIndexedFaceSetGeometry() {};
		
	public:
		virtual void setParameters(std::string groupName, Ape::GeometryCoordinates coordinates, Ape::GeometryIndices indices, Ape::MaterialWeakPtr material) = 0;
		
		virtual Ape::GeometryIndexedFaceSetParameters getParameters() = 0;

		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual void setMaterial(Ape::MaterialWeakPtr material) = 0;

		virtual Ape::MaterialWeakPtr getMaterial() = 0;

	};
}

#endif
