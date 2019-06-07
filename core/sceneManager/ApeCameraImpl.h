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

#ifndef APE_CAMERAIMPL_H
#define APE_CAMERAIMPL_H

#include "managers/apeISceneManager.h"
#include "sceneelements/apeICamera.h"
#include "apeEventManagerImpl.h"

namespace ape
{
	class CameraImpl : public ape::ICamera
	{
	public:
		CameraImpl(std::string name);

		~CameraImpl();
		
		float getFocalLength()  override;

		void setFocalLength(float focalLength)  override;

		ape::Vector2 getFrustumOffset()  override;

		void setFrustumOffset(ape::Vector2 frustumOffset)  override;

		ape::Radian getFOVy()  override;

		void setFOVy(ape::Radian fovY)  override;

		float getNearClipDistance()  override;

		void setNearClipDistance(float nearClipDistance)  override;

		float getFarClipDistance()  override;

		void setFarClipDistance(float farClipDistance)  override;

		float getAspectRatio() override;

		void setAspectRatio(float aspectRatio) override;

		void setAutoAspectRatio(bool enable) override;

		bool isAutoAspectRatio() override;
		
		ape::Matrix4 getProjection() override;

		void setProjection(ape::Matrix4 projection) override;

		void setParentNode(ape::NodeWeakPtr parentNode) override;

		ape::NodeWeakPtr getParentNode() override;

		void setProjectionType(ape::Camera::ProjectionType type) override;

		ape::Camera::ProjectionType getProjectionType() override;

		void setOrthoWindowSize(float width, float height) override;

		ape::Vector2 getOrthoWindowSize() override;

		void setWindow(std::string window) override;

		std::string getWindow() override;

		void setVisibilityMask(unsigned int mask) override;

		unsigned int getVisibilityMask() override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		float mFocalLength;

		ape::Vector2 mFrustumOffset;

		ape::Radian mFOVy;

		float mNearClipDistance;

		float mFarClipDistance;

		float mAspectRatio;

		bool mIsAutoAspectRatio;
		
		ape::Matrix4 mProjection;

		NodeWeakPtr mParentNode;

		ape::Camera::ProjectionType mProjectionType;

		ape::Vector2 mOrthoWindowSize;

		std::string mWindow;

		unsigned int mVisibilityMask;
	};
}

#endif
