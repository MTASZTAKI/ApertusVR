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

#ifndef APE_CAMERAIMPL_H
#define APE_CAMERAIMPL_H

#include "ApeICamera.h"
#include "ApeEventManagerImpl.h"
#include "ApeIScene.h"

namespace Ape
{
	class CameraImpl : public Ape::ICamera
	{
	public:
		CameraImpl(std::string name);

		~CameraImpl();
		
		float getFocalLength()  override;

		void setFocalLength(float focalLength)  override;

		Ape::Vector2 getFrustumOffset()  override;

		void setFrustumOffset(Ape::Vector2 frustumOffset)  override;

		Ape::Radian getFOVy()  override;

		void setFOVy(Ape::Radian fovY)  override;

		float getNearClipDistance()  override;

		void setNearClipDistance(float nearClipDistance)  override;

		float getFarClipDistance()  override;

		void setFarClipDistance(float farClipDistance)  override;

		float getAspectRatio() override;

		void setAspectRatio(float aspectRatio) override;
		
		Ape::Matrix4 getProjection() override;

		void setProjection(Ape::Matrix4 projection) override;

		Ape::Vector3 getPosition() override;

		void setPosition(Ape::Vector3 positionOffset) override;

		Ape::Quaternion getOrientation() override;

		void setOrientation(Ape::Quaternion orientationOffset) override;

		void setParentNode(Ape::NodeWeakPtr parentNode) override;

		Ape::NodeWeakPtr getParentNode() override;

		void setProjectionType(Ape::Camera::ProjectionType type) override;

		Ape::Camera::ProjectionType getProjectionType() override;

		void setOrthoWindowSize(float width, float height) override;

		Ape::Vector2 getOrthoWindowSize() override;

		void setWindow(std::string window) override;

		std::string getWindow() override;

	private:
		Ape::EventManagerImpl* mpEventManagerImpl;

		Ape::IScene* mpScene;

		float mFocalLength;

		Ape::Vector2 mFrustumOffset;

		Ape::Radian mFOVy;

		float mNearClipDistance;

		float mFarClipDistance;

		float mAspectRatio;
		
		Ape::Matrix4 mProjection;

		Ape::Quaternion mOrientationOffset;

		Ape::Vector3 mPositionOffset;

		NodeWeakPtr mParentNode;

		Ape::Camera::ProjectionType mProjectionType;

		Ape::Vector2 mOrthoWindowSize;

		std::string mWindow;
	};
}

#endif
