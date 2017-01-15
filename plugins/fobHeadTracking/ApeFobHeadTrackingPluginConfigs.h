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

#ifndef APE_FOBHEADTRACKINGPLUGINCONFIGS_H
#define APE_FOBHEADTRACKINGPLUGINCONFIGS_H

#include <vector>
#include <string>
#include "ApeVector3.h"
#include "ApeQuaternion.h"
#include "ApeDegree.h"
#include "ApeRadian.h"

namespace Ape
{
	struct FobHeadTrackingTrackerConfig
	{
		Ape::Vector3 translate;
		
		Ape::Quaternion rotation;
		
		float scale;

		FobHeadTrackingTrackerConfig()
		{
			this->translate = Ape::Vector3();
			this->rotation = Ape::Quaternion();
			this->scale = 0.0f;
		}

		FobHeadTrackingTrackerConfig(
			Ape::Vector3 translate,
			Ape::Quaternion rotation,
			float scale)
		{
			this->translate = translate;
			this->rotation = rotation;
			this->scale = scale;
		}
	};

	struct FobHeadTrackingDisplayConfig
	{
		Ape::Vector2 size;
		
		Ape::Vector3 position;
		
		Ape::Quaternion orientation;
		
		float disparity;

		FobHeadTrackingDisplayConfig()
		{
			this->size = Ape::Vector2();
			this->position = Ape::Vector3();
			this->orientation = Ape::Quaternion();
			this->disparity = 0.0f;
		}

		FobHeadTrackingDisplayConfig(
			Ape::Vector2 size;
			Ape::Vector3 position,
			Ape::Quaternion orientation,
			float disparity)
		{
			this->size = size;
			this->position = position;
			this->orientation = orientation;
			this->disparity = disparity;
		}
	};

	typedef std::vector < Ape::FobHeadTrackingDisplayConfig > FobHeadTrackingDisplayConfigList;
}

#endif
