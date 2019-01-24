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

#ifndef APE_HEADTRACKINGPLUGINCONFIGS_H
#define APE_HEADTRACKINGPLUGINCONFIGS_H

#include <vector>
#include <string>
#include "datatypes/ApeVector3.h"
#include "datatypes/ApeQuaternion.h"
#include "datatypes/ApeDegree.h"
#include "datatypes/ApeRadian.h"

namespace Ape
{
	struct HeadTrackerConfig
	{
		Ape::Vector3 translate;
		
		Ape::Quaternion rotation;
		
		Ape::Vector3 scale;

		Ape::Vector3 leftEyeOffset;

		Ape::Vector3 rightEyeOffset;

		HeadTrackerConfig()
		{
			this->translate = Ape::Vector3();
			this->rotation = Ape::Quaternion();
			this->scale = Ape::Vector3();
			this->leftEyeOffset = Ape::Vector3();
			this->rightEyeOffset = Ape::Vector3();
		}

		HeadTrackerConfig(
			Ape::Vector3 translate,
			Ape::Quaternion rotation,
			Ape::Vector3 scale,
			Ape::Vector3 leftEyeOffset,
			Ape::Vector3 rightEyeOffset)
		{
			this->translate = translate;
			this->rotation = rotation;
			this->scale = scale;
			this->leftEyeOffset = leftEyeOffset;
			this->rightEyeOffset = rightEyeOffset;
		}
	};

	struct HeadTrackerDisplayConfig
	{
		std::string name;

		Ape::CameraWeakPtr cameraLeft;

		Ape::CameraWeakPtr cameraRight;

		std::string cameraLeftName;

		std::string cameraRightName;

		Ape::Vector2 size;
		
		Ape::Vector3 position;
		
		Ape::Quaternion orientation;

		Ape::Vector3 bottomLeftCorner;

		Ape::Vector3 bottomRightCorner;

		Ape::Vector3 topLeftCorner;

		Ape::Vector3 width;

		Ape::Vector3 height;

		Ape::Vector3 normal;

		Ape::Matrix4 transform;

		HeadTrackerDisplayConfig()
		{
			this->size = Ape::Vector2();
			this->position = Ape::Vector3();
			this->orientation = Ape::Quaternion();
			this->bottomLeftCorner = Ape::Vector3();
			this->bottomRightCorner = Ape::Vector3();
			this->topLeftCorner = Ape::Vector3();
			this->width = Ape::Vector3();
			this->height = Ape::Vector3();
			this->normal = Ape::Vector3();
			this->transform = Ape::Matrix4();
		}

		HeadTrackerDisplayConfig(
			Ape::Vector2 size,
			Ape::Vector3 position,
			Ape::Quaternion orientation,
			Ape::Vector3 bottomLeftCorner,
			Ape::Vector3 bottomRightCorner,
			Ape::Vector3 topLeftCorner,
			Ape::Vector3 width,
			Ape::Vector3 height,
			Ape::Vector3 normal,
			Ape::Matrix4 transform)
		{
			this->size = size;
			this->position = position;
			this->orientation = orientation;
			this->bottomLeftCorner = bottomLeftCorner;
			this->bottomRightCorner = bottomRightCorner;
			this->topLeftCorner = topLeftCorner;
			this->width = width;
			this->height = height;
			this->normal = normal;
			this->transform = transform;
		}
	};

	typedef std::vector < Ape::HeadTrackerDisplayConfig > HeadTrackerDisplayConfigList;
}

#endif
