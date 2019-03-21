/*MIT License

Copyright (c) 2018 MTA SZTAKI

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
#include "datatypes/ApeGeometry.h"

namespace ape
{	
	struct GeometryIndexedFaceSetParameters
	{
		std::string groupName;
		ape::GeometryCoordinates coordinates;
		ape::GeometryIndices indices;
		ape::GeometryNormals normals;
		bool generateNormals;
		ape::GeometryColors colors;
		ape::GeometryTextureCoordinates textureCoordinates;
		ape::MaterialWeakPtr material;
		std::string materialName;
		
		GeometryIndexedFaceSetParameters()
		{
			this->groupName = std::string();
			this->coordinates = ape::GeometryCoordinates();
			this->indices = ape::GeometryIndices();
			this->normals = ape::GeometryNormals();
			this->generateNormals = true;
			this->colors = ape::GeometryColors();
			this->textureCoordinates = ape::GeometryTextureCoordinates();
			this->material = ape::MaterialWeakPtr();
			this->materialName = std::string();
		}

		GeometryIndexedFaceSetParameters(std::string groupName, ape::GeometryCoordinates coordinates, ape::GeometryIndices indices, ape::GeometryNormals normals, bool generateNormals, ape::GeometryColors colors, ape::GeometryTextureCoordinates textureCoordinates, ape::MaterialWeakPtr material)
		{
			this->groupName = groupName;
			this->coordinates = coordinates;
			this->indices = indices;
			this->normals = normals;
			this->generateNormals = generateNormals;
			this->colors = colors;
			this->textureCoordinates = textureCoordinates;
			this->material = material;
			if (auto materialPtr = this->material.lock())
				this->materialName = materialPtr->getName();
			else
				this->materialName = std::string();
		}

		ape::GeometryCoordinates getCoordinates()
		{
			return coordinates;
		}

		ape::GeometryIndices getIndices()
		{
			return indices;
		}

		ape::GeometryNormals getNormals()
		{
			return normals;
		}

		ape::GeometryColors getColors()
		{
			return colors;
		}

		ape::GeometryTextureCoordinates getTextureCoordinates()
		{
			return textureCoordinates;
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

			buff << "Normals(";
			for (auto const &item : normals) buff << item << ", ";
			buff << ")" << std::endl;

			buff << "Colors(";
			for (auto const &item : colors) buff << item << ", ";
			buff << ")" << std::endl;

			buff << "TextureCoordinates(";
			for (auto const &item : textureCoordinates) buff << item << ", ";
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
		virtual void setParameters(std::string groupName, ape::GeometryCoordinates coordinates, ape::GeometryIndices indices, ape::GeometryNormals normals, bool generateNormals, ape::GeometryColors colors, ape::GeometryTextureCoordinates textureCoordinates, ape::MaterialWeakPtr material) = 0;
		
		virtual ape::GeometryIndexedFaceSetParameters getParameters() = 0;

		virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual ape::MaterialWeakPtr getMaterial() = 0;
	};
}

#endif
