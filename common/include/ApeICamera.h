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

#ifndef APE_ICAMERA_H
#define APE_ICAMERA_H

#include <string>
#include "ApeEntity.h"
#include "ApeVector2.h"
#include "ApeVector3.h"
#include "ApeQuaternion.h"
#include "ApeRadian.h"

namespace Ape
{	
	class ICamera : public Entity
	{
	protected:
		ICamera(std::string name, std::string parentNodeName) : Entity(name, parentNodeName, Entity::CAMERA) {}
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

		virtual Ape::Vector3 getPositionOffset() = 0;

		virtual void setPositionOffset(Ape::Vector3 positionOffset) = 0;

		virtual Ape::Quaternion getOrientationOffset() = 0;

		virtual void setOrientationOffset(Ape::Quaternion orientation) = 0;

		virtual Ape::Vector3 getInitPositionOffset() = 0;

		virtual void setInitPositionOffset(Ape::Vector3 initPositionOffset) = 0;

		virtual Ape::Quaternion getInitOrientationOffset() = 0;

		virtual void setInitOrientationOffset(Ape::Quaternion initOrientationOffset) = 0;

	};

	typedef std::shared_ptr<Ape::ICamera> CameraSharedPtr;

	typedef std::weak_ptr<Ape::ICamera> CameraWeakPtr;
}

#endif
