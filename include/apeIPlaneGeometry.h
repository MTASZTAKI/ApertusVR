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

#ifndef APE_IPLANEGEOMETRY_H
#define APE_IPLANEGEOMETRY_H

#include <string>
#include <vector>
#include "apeGeometry.h"
#include "apeVector2.h"
#include "apeVector3.h"
#include "apeINode.h"

namespace ape
{
	struct GeometryPlaneParameters
	{
		ape::Vector2 numSeg;
		ape::Vector2 size;
		ape::Vector2 tile;

		GeometryPlaneParameters()
		{
			this->numSeg = ape::Vector2();
			this->size = ape::Vector2();
			this->tile = ape::Vector2();
		}

		GeometryPlaneParameters(
		    ape::Vector2 numSeg,
		    ape::Vector2 size,
		    ape::Vector2 tile)
		{
			this->numSeg = numSeg;
			this->size = size;
			this->tile = tile;
		}

		void write(std::ofstream& fileStreamOut, bool writeSize = true)
		{
			if (writeSize)
			{
				long sizeInBytes = 24;
				fileStreamOut.write(reinterpret_cast<char*>(&sizeInBytes), sizeof(long));
			}
			this->numSeg.write(fileStreamOut, false);
			this->size.write(fileStreamOut, false);
			this->tile.write(fileStreamOut, false);
		}

		void read(std::ifstream& fileStreamIn)
		{
			this->numSeg.read(fileStreamIn);
			this->size.read(fileStreamIn);
			this->tile.read(fileStreamIn);
		}
	};

	class IPlaneGeometry : public ape::Geometry
	{
	protected:
		IPlaneGeometry(std::string name, bool replicate, std::string ownerID) : ape::Geometry(name, ape::Entity::GEOMETRY_PLANE, replicate, ownerID) {}

		virtual ~IPlaneGeometry() {};

	public:
		virtual void setParameters(ape::Vector2 numSeg, ape::Vector2 size, ape::Vector2 tile) = 0;

		virtual ape::GeometryPlaneParameters getParameters() = 0;

		virtual void setParentNode(ape::NodeWeakPtr parentNode) = 0;

		virtual void setMaterial(ape::MaterialWeakPtr material) = 0;

		virtual ape::MaterialWeakPtr getMaterial() = 0;
	};
}

#endif
