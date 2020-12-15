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

#ifndef APE_SDLWINDOWPLUGINCONFIGS_H
#define APE_SDLWINDOWPLUGINCONFIGS_H

#include <vector>
#include <string>
#include "apeVector3.h"
#include "apeQuaternion.h"
#include "apeDegree.h"
#include "apeRadian.h"

namespace ape
{
	struct SdlWindowPluginWindowConfig
	{
		bool enable;

		std::string name;

		int monitorIndex;

		bool hidden;

		int width;

		int height;

		bool fullScreen;

		SdlWindowPluginWindowConfig()
		{
			this->enable = true;
			this->name = "";
			this->monitorIndex = 0;
			this->hidden = false,
			this->width = 0;
			this->height = 0;
			this->fullScreen = false;
		}

		SdlWindowPluginWindowConfig(bool enable,
		std::string name,
		int monitorIndex,
		bool hidden,
		int width,
		int height,
		bool vSync,
		int vSyncInterval,
		bool fullScreen)
		{
			this->enable = enable;
			this->name = name;
			this->monitorIndex = monitorIndex;
			this->hidden = hidden,
			this->width = width;
			this->height = height;
			this->fullScreen = fullScreen;
		}
	};

	typedef std::vector < ape::SdlWindowPluginWindowConfig > SdlWindowPluginWindowConfigList;

	struct SdlWindowPluginConfig
	{
		SdlWindowPluginWindowConfigList sdlWindowPluginWindowConfigList;

		SdlWindowPluginConfig()
		{
			this->sdlWindowPluginWindowConfigList = SdlWindowPluginWindowConfigList();
		}

		SdlWindowPluginConfig(
			SdlWindowPluginWindowConfigList sdlWindowPluginWindowConfigList)
		{
			this->sdlWindowPluginWindowConfigList = sdlWindowPluginWindowConfigList;
		}
	};
}

#endif
