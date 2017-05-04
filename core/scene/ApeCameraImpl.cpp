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

#include "ApeCameraImpl.h"
#include <iostream>

Ape::CameraImpl::CameraImpl(std::string name) : Ape::ICamera(name)
{
	mpEventManagerImpl = ((Ape::EventManagerImpl*)Ape::IEventManager::getSingletonPtr());
	mpScene = Ape::IScene::getSingletonPtr();
	mFocalLength = 0.0f;
	mFrustumOffset = Ape::Vector2();
	mFOVy = 0.0f;
	mNearClipDistance = 0.0f;
	mFarClipDistance = 0.0f;
	mAspectRatio = 0.0f;
	mProjection = Ape::Matrix4();
	mPositionOffset = Ape::Vector3();
	mOrientationOffset = Ape::Quaternion();
	mParentNode = Ape::NodeWeakPtr();
	mProjectionType = Ape::Camera::ProjectionType::INVALID;
	mOrthoWindowSize = Ape::Vector2();
}

Ape::CameraImpl::~CameraImpl()
{
	
}

float Ape::CameraImpl::getFocalLength()
{
	return mFocalLength;
}

void Ape::CameraImpl::setFocalLength(float focalLength)
{
	mFocalLength = focalLength;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_FOCALLENGTH));
}

Ape::Vector2 Ape::CameraImpl::getFrustumOffset()
{
	return mFrustumOffset;
}

void Ape::CameraImpl::setFrustumOffset(Ape::Vector2 frustumOffset)
{
	mFrustumOffset = frustumOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_FRUSTUMOFFSET));
}

Ape::Radian Ape::CameraImpl::getFOVy()
{
	return mFOVy;
}

void Ape::CameraImpl::setFOVy(Ape::Radian fovY)
{
	mFOVy = fovY;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_FOVY));
}

float Ape::CameraImpl::getNearClipDistance()
{
	return mNearClipDistance;
}

void Ape::CameraImpl::setNearClipDistance(float nearClipDistance)
{
	mNearClipDistance = nearClipDistance;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_NEARCLIP));
}

float Ape::CameraImpl::getFarClipDistance()
{
	return mFarClipDistance;
}

void Ape::CameraImpl::setFarClipDistance(float farClipDistance)
{
	mFarClipDistance = farClipDistance;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_FARCLIP));
}

float Ape::CameraImpl::getAspectRatio()
{
	return mFocalLength;
}

void Ape::CameraImpl::setAspectRatio(float aspectRatio)
{
	mAspectRatio = aspectRatio;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_ASPECTRATIO));
}

Ape::Matrix4 Ape::CameraImpl::getProjection()
{
	return mProjection;
}

void Ape::CameraImpl::setProjection(Ape::Matrix4 projection)
{
	mProjection = projection;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_PROJECTION));
}

Ape::Vector3 Ape::CameraImpl::getPosition()
{
	return mPositionOffset;
}

void Ape::CameraImpl::setPosition(Ape::Vector3 positionOffset)
{
	mPositionOffset = positionOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_POSITION));
}

Ape::Quaternion Ape::CameraImpl::getOrientation()
{
	return mOrientationOffset;
}

void Ape::CameraImpl::setOrientation(Ape::Quaternion orientationOffset)
{
	mOrientationOffset = orientationOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_ORIENTATION));
}

void Ape::CameraImpl::setParentNode(Ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_PARENTNODE));
	}
	else
		mParentNode = Ape::NodeWeakPtr();
}

Ape::NodeWeakPtr Ape::CameraImpl::getParentNode()
{
	return mParentNode;
}

void Ape::CameraImpl::setProjectionType(Ape::Camera::ProjectionType type)
{
	mProjectionType = type;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_PROJECTIONTYPE));
}

Ape::Camera::ProjectionType Ape::CameraImpl::getProjectionType()
{
	return mProjectionType;
}

void Ape::CameraImpl::setOrthoWindowSize(float width, float height)
{
	mOrthoWindowSize.x = width;
	mOrthoWindowSize.y = height;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_ORTHOWINDOWSIZE));
}

Ape::Vector2 Ape::CameraImpl::getOrthoWindowSize()
{
	return mOrthoWindowSize;
}

