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
	mPositionOffset = Ape::Vector3();
	mOrientationOffset = Ape::Quaternion();
	mParentNode = Ape::NodeWeakPtr();
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

Ape::Vector3 Ape::CameraImpl::getPositionOffset()
{
	return mPositionOffset;
}

void Ape::CameraImpl::setPositionOffset(Ape::Vector3 positionOffset)
{
	mPositionOffset = positionOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_POSITIONOFFSET));
}

Ape::Quaternion Ape::CameraImpl::getOrientationOffset()
{
	return mOrientationOffset;
}

void Ape::CameraImpl::setOrientationOffset(Ape::Quaternion orientationOffset)
{
	mOrientationOffset = orientationOffset;
	mpEventManagerImpl->fireEvent(Ape::Event(mName, Ape::Event::Type::CAMERA_ORIENTATIONOFFSET));
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

