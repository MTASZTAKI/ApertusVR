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

#include <iostream>
#include "apeRayGeometryImpl.h"

ape::RayGeometryImpl::RayGeometryImpl(std::string name) : ape::IRayGeometry(name)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mIntersectingEnabled = false;
	mIntersections = std::vector<ape::EntityWeakPtr>();
	mParentNode = ape::NodeWeakPtr();
}

ape::RayGeometryImpl::~RayGeometryImpl()
{
	
}

void ape::RayGeometryImpl::setIntersectingEnabled(bool enable)
{
	mIntersectingEnabled = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_RAY_INTERSECTIONENABLE));
}

void ape::RayGeometryImpl::setIntersections(std::vector<ape::EntityWeakPtr> intersections)
{
	mIntersections = intersections;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_RAY_INTERSECTION));
}

void ape::RayGeometryImpl::fireIntersectionQuery()
{
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY));
}

void ape::RayGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_RAY_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}
