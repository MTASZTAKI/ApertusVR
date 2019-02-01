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

#ifndef APE_IPOINTCLOUD_H
#define APE_IPOINTCLOUD_H

#include <array>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "Ape.h"
#include "datatypes/ApeEntity.h"

namespace Ape
{
	typedef std::vector<float> PointCloudPoints;
	typedef std::vector<float> PointCloudColors;
	struct PointCloudSetParameters
	{
		Ape::PointCloudPoints points;
		Ape::PointCloudColors colors;
		float boundigSphereRadius;
		float pointScaleOffset;
		float unitScaleDistance;

		PointCloudSetParameters()
		{
			this->points = Ape::PointCloudPoints();
			this->points.resize(0);
			this->colors = Ape::PointCloudColors();
			this->colors.resize(0);
			this->boundigSphereRadius = 0.0f;
			this->pointScaleOffset = 0.0f;
			this->unitScaleDistance = 0.0f;
		}

		PointCloudSetParameters(Ape::PointCloudPoints points, Ape::PointCloudColors colors, float boundigSphereRadius, float pointScaleOffset, float unitScaleDistance)
		{
			this->points = points;
			this->colors = colors;
			this->boundigSphereRadius = boundigSphereRadius;
			this->pointScaleOffset = pointScaleOffset;
			this->unitScaleDistance = unitScaleDistance;
		}

		Ape::PointCloudPoints getPoints()
		{
			return points;
		}

		Ape::PointCloudColors getColors()
		{
			return colors;
		}

		float getBoundigSphereRadius()
		{
			return boundigSphereRadius;
		}

		Ape::Vector3 getPointScaleOffset()
		{
			return pointScaleOffset;
		}

		std::string toString() const
		{
			std::ostringstream buff;

			buff << "Points(";
			for (auto const& item : points) buff << item << ", ";
			buff << ")" << std::endl;

			buff << "Colors(";
			for (auto const& item : colors) buff << item << ", ";
			buff << ")" << std::endl;

			buff << "BoundigSphereRadius(" << boundigSphereRadius << ")" << std::endl;

			buff << "PointScaleOffset(" << pointScaleOffset << ")" << std::endl;

			return buff.str();
		}
	};

	class IPointCloud : public Entity
	{
	protected:
		IPointCloud(std::string name) : Entity(name, Entity::POINT_CLOUD) {}

		virtual ~IPointCloud() {};

	public:
		virtual void setParameters(Ape::PointCloudPoints points, Ape::PointCloudColors colors, float boundigSphereRadius, float pointScaleOffset, float unitScaleDistance) = 0;

		virtual Ape::PointCloudSetParameters getParameters() = 0;

		virtual void updatePoints(Ape::PointCloudPoints points) = 0;

		virtual void updateColors(Ape::PointCloudColors colors) = 0;

		virtual Ape::PointCloudPoints getCurrentPoints() = 0;

		virtual Ape::PointCloudColors getCurrentColors() = 0;

		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual Ape::NodeWeakPtr getParentNode() = 0;
	};
}

#endif
