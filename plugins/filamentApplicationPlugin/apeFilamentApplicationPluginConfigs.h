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

#ifndef APE_FILAMENTAPPLICATIONPLUGINCONFIGS_H
#define APE_FILAMENTAPPLICATIONPLUGINCONFIGS_H

#include <vector>
#include <string>
#include "apeVector3.h"
#include "apeQuaternion.h"
#include "apeDegree.h"
#include "apeRadian.h"

namespace ape
{
	struct FilamentWindowsCameraConfig
	{
		std::string name;

		ape::Vector3 positionOffset;
		
		ape::Quaternion orientationOffset;

		float nearClip;

		float farClip;

		Degree fovY;

		std::string parentNodeName;

		FilamentWindowsCameraConfig()
		{
			this->name = std::string();
			this->positionOffset = ape::Vector3();
			this->orientationOffset = ape::Quaternion();
			this->nearClip = 0.0f;
			this->farClip = 0.0f;
			this->fovY = 0.0f;
			this->parentNodeName = std::string();
		}

		FilamentWindowsCameraConfig(
			std::string name,
			ape::Vector3 positionOffset,
			ape::Quaternion orientationOffset,
			float nearClip,
			float farClip,
			Degree fovY,
			std::string parentNodeName = "")
		{
			this->name = name;
			this->positionOffset = positionOffset;
			this->orientationOffset = orientationOffset;
			this->nearClip = nearClip;
			this->farClip = farClip;
			this->fovY = fovY;
			this->parentNodeName = parentNodeName;
		}
	};

	struct FilamentWindowsLodLevelsConfig
	{
		bool autoGenerateAndSave;

		float bias;

		FilamentWindowsLodLevelsConfig()
		{
			this->autoGenerateAndSave = false;
			this->bias = 0.0f;
		}

		FilamentWindowsLodLevelsConfig(
			bool autoGenerateAndSave,
			float bias)
		{
			this->autoGenerateAndSave = autoGenerateAndSave;
			this->bias = bias;
		}
	};

	struct FilamentWindowsViewPortConfig
	{
		std::vector<FilamentWindowsCameraConfig> cameras;

		int zOrder;

		int left;

		int top;

		int width;

		int height;

		FilamentWindowsViewPortConfig()
		{
			this->cameras = std::vector<FilamentWindowsCameraConfig>();
			this->zOrder = 0;
			this->left = 0;
			this->top = 0;
			this->width = 0;
			this->height = 0;
		}

		FilamentWindowsViewPortConfig(
			std::vector<FilamentWindowsCameraConfig> cameras,
		int zOrder,
		int left,
		int top,
		int width,
		int height)
		{
			this->cameras = cameras;
			this->zOrder = zOrder;
			this->left = left;
			this->top = top;
			this->width = width;
			this->height = height;
		}
	};

	typedef std::vector < ape::FilamentWindowsViewPortConfig > FilamentWindowsViewPortConfigList;

	struct FilamentApplicationWindowConfig
	{
		bool enable;

		std::string name;

		int monitorIndex;

		bool hidden;

		int width;

		int height;

		bool vSync;

		int vSyncInterval;

		bool fullScreen;

		int fsaa;

		std::string fsaaHint;

		int colorDepth;

		ape::FilamentWindowsViewPortConfigList viewportList;

		std::string windowHandler;

		FilamentApplicationWindowConfig()
		{
			this->enable = true;
			this->name = "";
			this->monitorIndex = 0;
			this->hidden = false,
			this->width = 0;
			this->height = 0;
			this->vSync = false;
			this->vSyncInterval = 0;
			this->fullScreen = false;
			this->fsaa = 0;
			this->fsaaHint = "";
			this->colorDepth = 0;
			this->viewportList = ape::FilamentWindowsViewPortConfigList();
			this->windowHandler = "";
		}

		FilamentApplicationWindowConfig(bool enable,
		std::string name,
		int monitorIndex,
		bool hidden,
		int width,
		int height,
		bool vSync,
		int vSyncInterval,
		bool fullScreen,
		int fsaa,
		int fsaaHint,
		int colorDepth,
		ape::FilamentWindowsViewPortConfigList viewportList,
		std::string windowHandler = "")
		{
			this->enable = enable;
			this->name = name;
			this->monitorIndex = monitorIndex;
			this->hidden = hidden,
			this->width = width;
			this->height = height;
			this->vSync = vSync;
			this->vSyncInterval = vSyncInterval;
			this->fullScreen = fullScreen;
			this->fsaa = fsaa;
			this->fsaaHint = fsaaHint;
			this->colorDepth = colorDepth;
			this->viewportList = viewportList;
			this->windowHandler = windowHandler;
		}
	};

	typedef std::vector < ape::FilamentApplicationWindowConfig > FilamentApplicationWindowConfigList;

	struct FilamentApplicationPluginConfig
	{
		FilamentApplicationWindowConfigList FilamentApplicationConfigList;
		FilamentWindowsLodLevelsConfig filamentLodLevelsConfig;
		std::string shading;
		std::string renderSystem;

		FilamentApplicationPluginConfig()
		{
			this->FilamentApplicationConfigList = FilamentApplicationWindowConfigList();
			this->filamentLodLevelsConfig = FilamentWindowsLodLevelsConfig();
			this->shading = std::string();
			this->renderSystem = std::string();
		}

		FilamentApplicationPluginConfig(
			FilamentApplicationWindowConfigList FilamentApplicationConfigList,
			FilamentWindowsLodLevelsConfig filamentLodLevelsConfig,
			std::string shading,
			std::string renderSystem)
		{
			this->FilamentApplicationConfigList = FilamentApplicationConfigList;
			this->filamentLodLevelsConfig = filamentLodLevelsConfig;
			this->shading = shading;
			this->renderSystem = renderSystem;
		}
	};
}

#endif
