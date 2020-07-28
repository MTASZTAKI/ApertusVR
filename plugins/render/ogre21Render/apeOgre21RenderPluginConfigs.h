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

#ifndef APE_OGRERENDER21PLUGINCONFIGS_H
#define APE_OGRERENDER21PLUGINCONFIGS_H

#include <vector>
#include <string>
#include "apeVector3.h"
#include "apeQuaternion.h"
#include "apeDegree.h"
#include "apeRadian.h"

namespace ape
{
	struct Ogre21CameraConfig
	{
		std::string name;

		ape::Vector3 positionOffset;
		
		ape::Quaternion orientationOffset;

		float nearClip;

		float farClip;

		Degree fovY;

		std::string parentNodeName;

		Ogre21CameraConfig()
		{
			this->name = std::string();
			this->positionOffset = ape::Vector3();
			this->orientationOffset = ape::Quaternion();
			this->nearClip = 0.0f;
			this->farClip = 0.0f;
			this->fovY = 0.0f;
			this->parentNodeName = std::string();
		}

		Ogre21CameraConfig(
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

	struct Ogre21LodLevelsConfig
	{
		bool autoGenerateAndSave;

		float bias;

		Ogre21LodLevelsConfig()
		{
			this->autoGenerateAndSave = false;
			this->bias = 0.0f;
		}

		Ogre21LodLevelsConfig(
			bool autoGenerateAndSave,
			float bias)
		{
			this->autoGenerateAndSave = autoGenerateAndSave;
			this->bias = bias;
		}
	};

	struct Ogre21ViewPortConfig
	{
		std::vector<Ogre21CameraConfig> cameras;

		int zOrder;

		int left;

		int top;

		int width;

		int height;

		Ogre21ViewPortConfig()
		{
			this->cameras = std::vector<Ogre21CameraConfig>();
			this->zOrder = 0;
			this->left = 0;
			this->top = 0;
			this->width = 0;
			this->height = 0;
		}

		Ogre21ViewPortConfig(
			std::vector<Ogre21CameraConfig> cameras,
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

	typedef std::vector < ape::Ogre21ViewPortConfig > Ogre21ViewPortConfigList;

	struct Ogre21RenderWindowConfig
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

		ape::Ogre21ViewPortConfigList viewportList;

		std::string windowHandler;

		Ogre21RenderWindowConfig()
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
			this->viewportList = ape::Ogre21ViewPortConfigList();
			this->windowHandler = "";
		}

		Ogre21RenderWindowConfig(bool enable,
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
		ape::Ogre21ViewPortConfigList viewportList,
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

	typedef std::vector < ape::Ogre21RenderWindowConfig > Ogre21RenderWindowConfigList;

	struct Ogre21RenderPluginConfig
	{
		Ogre21RenderWindowConfigList ogreRenderWindowConfigList;
		Ogre21LodLevelsConfig ogreLodLevelsConfig;
		std::string shading;
		std::string renderSystem;

		Ogre21RenderPluginConfig()
		{
			this->ogreRenderWindowConfigList = Ogre21RenderWindowConfigList();
			this->ogreLodLevelsConfig = Ogre21LodLevelsConfig();
			this->shading = std::string();
			this->renderSystem = std::string();
		}

		Ogre21RenderPluginConfig(
			Ogre21RenderWindowConfigList ogreRenderWindowConfigList,
			Ogre21LodLevelsConfig ogreLodLevelsConfig,
			std::string shading,
			std::string renderSystem)
		{
			this->ogreRenderWindowConfigList = ogreRenderWindowConfigList;
			this->ogreLodLevelsConfig = ogreLodLevelsConfig;
			this->shading = shading;
			this->renderSystem = renderSystem;
		}
	};
}

#endif
