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

#ifndef APE_GEOMETRY_H
#define APE_GEOMETRY_H

#include <array>
#include <memory>
#include <string>
#include <vector>
#include "ape.h"
#include "apeEntity.h"
#include "apeMaterial.h"
#include "apeVector3.h"

namespace ape
{
	typedef std::vector<float> GeometryCoordinates;

	typedef std::vector<int> GeometryIndices;	//-1 code for stop

	typedef std::vector<float> GeometryNormals;

	typedef std::vector<float> GeometryColors;

	typedef std::vector<float> GeometryTextureCoordinates;


	class Geometry : public Entity
	{
	protected:
		Geometry(std::string name, Entity::Type entityType, bool replicated, std::string ownerID) : Entity(name, entityType, replicated, ownerID)
			, mParentNode(ape::NodeWeakPtr()), mParentNodeName(std::string()) {};

		virtual ~Geometry() {};

		ape::NodeWeakPtr mParentNode;

		std::string mParentNodeName;

		bool mIntersectingEnabled;

		std::string mRunningAnimation;

		bool mIsAnimationRunning;

		std::vector<ape::EntityWeakPtr> mIntersections;


	public:
		ape::NodeWeakPtr getParentNode()
		{
			return mParentNode;
		};

		bool isIntersectingEnabled()
		{
			return mIntersectingEnabled;
		};

		std::vector<ape::EntityWeakPtr> getIntersections()
		{
			return mIntersections;
		};
	};
}

#endif
