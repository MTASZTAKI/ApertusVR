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

#ifndef APE_ICAMERA_H
#define APE_ICAMERA_H

#include <string>
#include "ApeEntity.h"
#include "ApeINode.h"
#include "ApeMatrix4.h"
#include "ApeQuaternion.h"
#include "ApeRadian.h"
#include "ApeVector2.h"
#include "ApeVector3.h"

namespace Ape
{
	namespace Camera
	{
		enum ProjectionType
		{
			ORTHOGRAPHIC,
			PERSPECTIVE,
			INVALID
		};
	}

	class ICamera : public Entity
	{
	protected:
		ICamera(std::string name) : Entity(name, Entity::CAMERA) {}

		virtual ~ICamera() {};

	public:

		virtual float getFocalLength() = 0;

		virtual void setFocalLength(float focalLength) = 0;

		virtual Ape::Vector2 getFrustumOffset() = 0;

		virtual void setFrustumOffset(Ape::Vector2 frustumOffset) = 0;

		virtual Ape::Radian getFOVy() = 0;

		virtual void setFOVy(Ape::Radian fovY) = 0;

		virtual float getNearClipDistance() = 0;

		virtual void setNearClipDistance(float nearClipDistance) = 0;

		virtual float getFarClipDistance() = 0;

		virtual void setFarClipDistance(float farClipDistance) = 0;

		virtual float getAspectRatio() = 0;

		virtual void setAspectRatio(float aspectRatio) = 0;

		virtual Ape::Matrix4 getProjection() = 0;

		virtual void setProjection(Ape::Matrix4 projection) = 0;

		virtual Ape::Vector3 getPosition() = 0;

		virtual void setPosition(Ape::Vector3 positionOffset) = 0;

		virtual Ape::Quaternion getOrientation() = 0;

		virtual void setOrientation(Ape::Quaternion orientation) = 0;

		virtual void setParentNode(Ape::NodeWeakPtr parentNode) = 0;

		virtual Ape::NodeWeakPtr getParentNode() = 0;

		virtual void setProjectionType(Ape::Camera::ProjectionType type) = 0;

		virtual Ape::Camera::ProjectionType getProjectionType() = 0;

		virtual void setOrthoWindowSize(float width, float height) = 0;

		virtual Ape::Vector2 getOrthoWindowSize() = 0;

		virtual void setWindow(std::string window) = 0;

		virtual std::string getWindow() = 0;
	};

	typedef std::shared_ptr<Ape::ICamera> CameraSharedPtr;

	typedef std::weak_ptr<Ape::ICamera> CameraWeakPtr;
}

#endif
