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
#include "apeVector3.h"
#include "apeQuaternion.h"
#include "apeDegree.h"
#include "apeRadian.h"

namespace ape
{
	struct HeadTrackerConfig
	{
		ape::Vector3 translate;
		
		ape::Quaternion rotation;
		
		ape::Vector3 scale;

		ape::Vector3 leftEyeOffset;

		ape::Vector3 rightEyeOffset;

		HeadTrackerConfig()
		{
			this->translate = ape::Vector3();
			this->rotation = ape::Quaternion();
			this->scale = ape::Vector3();
			this->leftEyeOffset = ape::Vector3();
			this->rightEyeOffset = ape::Vector3();
		}

		HeadTrackerConfig(
			ape::Vector3 translate,
			ape::Quaternion rotation,
			ape::Vector3 scale,
			ape::Vector3 leftEyeOffset,
			ape::Vector3 rightEyeOffset)
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

		ape::CameraWeakPtr cameraLeft;

		ape::CameraWeakPtr cameraRight;

		std::string cameraLeftName;

		std::string cameraRightName;

		ape::Vector2 size;
		
		ape::Vector3 position;
		
		ape::Quaternion orientation;

		ape::Vector3 bottomLeftCorner;

		ape::Vector3 bottomRightCorner;

		ape::Vector3 topLeftCorner;

		ape::Vector3 width;

		ape::Vector3 height;

		ape::Vector3 normal;

		ape::Matrix4 transform;

		HeadTrackerDisplayConfig()
		{
			this->size = ape::Vector2();
			this->position = ape::Vector3();
			this->orientation = ape::Quaternion();
			this->bottomLeftCorner = ape::Vector3();
			this->bottomRightCorner = ape::Vector3();
			this->topLeftCorner = ape::Vector3();
			this->width = ape::Vector3();
			this->height = ape::Vector3();
			this->normal = ape::Vector3();
			this->transform = ape::Matrix4();
		}

		HeadTrackerDisplayConfig(
			ape::Vector2 size,
			ape::Vector3 position,
			ape::Quaternion orientation,
			ape::Vector3 bottomLeftCorner,
			ape::Vector3 bottomRightCorner,
			ape::Vector3 topLeftCorner,
			ape::Vector3 width,
			ape::Vector3 height,
			ape::Vector3 normal,
			ape::Matrix4 transform)
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

	typedef std::vector < ape::HeadTrackerDisplayConfig > HeadTrackerDisplayConfigList;
}

#endif
