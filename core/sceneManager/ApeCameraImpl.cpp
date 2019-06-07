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

#include "apeCameraImpl.h"
#include <iostream>

ape::CameraImpl::CameraImpl(std::string name) : ape::ICamera(name)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mFocalLength = 0.0f;
	mFrustumOffset = ape::Vector2();
	mFOVy = 0.0f;
	mNearClipDistance = 0.0f;
	mFarClipDistance = 0.0f;
	mAspectRatio = 0.0f;
	mIsAutoAspectRatio = false;
	mProjection = ape::Matrix4();
	mParentNode = ape::NodeWeakPtr();
	mProjectionType = ape::Camera::ProjectionType::INVALID;
	mOrthoWindowSize = ape::Vector2();
	mWindow = std::string();
	mVisibilityMask = 0;
}

ape::CameraImpl::~CameraImpl()
{
	
}

float ape::CameraImpl::getFocalLength()
{
	return mFocalLength;
}

void ape::CameraImpl::setFocalLength(float focalLength)
{
	mFocalLength = focalLength;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_FOCALLENGTH));
}

ape::Vector2 ape::CameraImpl::getFrustumOffset()
{
	return mFrustumOffset;
}

void ape::CameraImpl::setFrustumOffset(ape::Vector2 frustumOffset)
{
	mFrustumOffset = frustumOffset;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_FRUSTUMOFFSET));
}

ape::Radian ape::CameraImpl::getFOVy()
{
	return mFOVy;
}

void ape::CameraImpl::setFOVy(ape::Radian fovY)
{
	mFOVy = fovY;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_FOVY));
}

float ape::CameraImpl::getNearClipDistance()
{
	return mNearClipDistance;
}

void ape::CameraImpl::setNearClipDistance(float nearClipDistance)
{
	mNearClipDistance = nearClipDistance;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_NEARCLIP));
}

float ape::CameraImpl::getFarClipDistance()
{
	return mFarClipDistance;
}

void ape::CameraImpl::setFarClipDistance(float farClipDistance)
{
	mFarClipDistance = farClipDistance;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_FARCLIP));
}

float ape::CameraImpl::getAspectRatio()
{
	return mFocalLength;
}

void ape::CameraImpl::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_ASPECTRATIO));
}

void ape::CameraImpl::setAutoAspectRatio(bool enable)
{
	mIsAutoAspectRatio = enable;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_AUTOASPECTRATIO));
}

bool ape::CameraImpl::isAutoAspectRatio()
{
	return mIsAutoAspectRatio;
}

ape::Matrix4 ape::CameraImpl::getProjection()
{
	return mProjection;
}

void ape::CameraImpl::setProjection(ape::Matrix4 projection)
{
	mProjection = projection;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_PROJECTION));
}

void ape::CameraImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

ape::NodeWeakPtr ape::CameraImpl::getParentNode()
{
	return mParentNode;
}

void ape::CameraImpl::setProjectionType(ape::Camera::ProjectionType type)
{
	mProjectionType = type;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_PROJECTIONTYPE));
}

ape::Camera::ProjectionType ape::CameraImpl::getProjectionType()
{
	return mProjectionType;
}

void ape::CameraImpl::setOrthoWindowSize(float width, float height)
{
	mOrthoWindowSize.x = width;
	mOrthoWindowSize.y = height;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_ORTHOWINDOWSIZE));
}

ape::Vector2 ape::CameraImpl::getOrthoWindowSize()
{
	return mOrthoWindowSize;
}

void ape::CameraImpl::setWindow(std::string window)
{
	mWindow = window;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_WINDOW));
}

std::string ape::CameraImpl::getWindow()
{
	return mWindow;
}

void ape::CameraImpl::setVisibilityMask(unsigned int mask)
{
	mVisibilityMask = mask;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::CAMERA_VISIBILITY));
}

unsigned int ape::CameraImpl::getVisibilityMask()
{
	return mVisibilityMask;
}

