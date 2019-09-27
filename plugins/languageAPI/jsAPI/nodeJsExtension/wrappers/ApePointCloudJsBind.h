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

#ifndef APE_POINTCLOUDJSBIND_H
#define APE_POINTCLOUDJSBIND_H

#include "nbind/nbind.h"
#include "nbind/api.h"
#include "ape.h"
#include "sceneelements/apeIPointCloud.h"
#include "apeJsBindNodeImpl.h"

#ifdef NBIND_CLASS

class PointCloudJsPtr
{
private:
	ape::PointCloudWeakPtr mPtr;

public:
	PointCloudJsPtr(ape::PointCloudWeakPtr ptr)
	{
		mPtr = ptr;
	}

	PointCloudJsPtr(ape::EntityWeakPtr ptr)
	{
		mPtr = std::static_pointer_cast<ape::IPointCloud>(ptr.lock());
	}

	// Pointers

	const ape::EntityWeakPtr getEntityWeakPtr()
	{
		return std::static_pointer_cast<ape::Entity>(mPtr.lock());
	}

	const ape::EntitySharedPtr getEntitySharedPtr()
	{
		return this->getEntityWeakPtr().lock();
	}

	ape::PointCloudSharedPtr getPointCloudSharedPtr()
	{
		return std::static_pointer_cast<ape::IPointCloud>(mPtr.lock());
	}

	ape::PointCloudWeakPtr getPointCloudWeakPtr()
	{
		return mPtr;
	}

	// ParentNode

	ape::NodeWeakPtr getParentNodeWeakPtr()
	{
		return mPtr.lock()->getParentNode();
	}

	void setParentNodeWeakPtr(ape::NodeWeakPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode);
	}

	NodeJsPtr getParentNodeJsPtr()
	{
		return NodeJsPtr(getParentNodeWeakPtr());
	}

	void setParentNodeJsPtr(NodeJsPtr parentNode)
	{
		mPtr.lock()->setParentNode(parentNode.getNodeWeakPtr());
	}

	// Entity

	const std::string getName()
	{
		return mPtr.lock()->getName();
	}

	const ape::Entity::Type getType()
	{
		return mPtr.lock()->getType();
	}

	// IPointCloud

	void setParameters(ape::PointCloudPoints points, ape::PointCloudColors colors, float boundigSphereRadius, float pointSize, bool pointScale,
		float pointScaleOffset, float unitScaleDistance, float scaleFactor)
	{
		mPtr.lock()->setParameters(points, colors, boundigSphereRadius, pointSize, pointScale, pointScaleOffset, unitScaleDistance, scaleFactor);
	}

	ape::PointCloudSetParameters getParameters()
	{
		return mPtr.lock()->getParameters();
	}

	void updatePoints(ape::PointCloudPoints points)
	{
		mPtr.lock()->updatePoints(points);
	}

	void updateColors(ape::PointCloudColors colors)
	{
		mPtr.lock()->updateColors(colors);
	}

	ape::PointCloudPoints getCurrentPoints()
	{
		return mPtr.lock()->getCurrentPoints();
	}

	ape::PointCloudColors getCurrentColors()
	{
		return mPtr.lock()->getCurrentColors();
	}
};

using namespace ape;

NBIND_CLASS(PointCloudJsPtr)
{
	construct<ape::PointCloudWeakPtr>();
	construct<ape::EntityWeakPtr>();

	// Pointers

	method(getEntityWeakPtr);
	method(getEntitySharedPtr);
	method(getPointCloudWeakPtr);
	method(getPointCloudSharedPtr);

	// ParentNode

	method(getParentNodeWeakPtr);
	method(setParentNodeWeakPtr);

	method(getParentNodeJsPtr);
	method(setParentNodeJsPtr);

	// Entity

	method(getName);
	method(getType);

	// IPointCloud
	method(setParameters);
	method(getParameters);
	method(updatePoints);
	method(updateColors);
	method(getCurrentPoints);
	method(getCurrentColors);
}

#endif

#endif
