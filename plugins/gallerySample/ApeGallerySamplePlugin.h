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


#ifndef APE_GALLERYSAMPLEPLUGIN_H
#define APE_GALLERYSAMPLEPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "ApePluginAPI.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeIPlaneGeometry.h"
#include "ApeIFileGeometry.h"
#include "ApeIFileMaterial.h"
#include "ApeIManualMaterial.h"
#include "ApeIBrowser.h"
#include "ApeILogManager.h"

#define THIS_PLUGINNAME "ApeGallerySamplePlugin"

namespace Ape
{
	class ApeGallerySamplePlugin : public Ape::IPlugin
	{
	private:
		Ape::IScene* mpScene;

	public:
		ApeGallerySamplePlugin();

		~ApeGallerySamplePlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;
	};

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApeGallerySamplePlugin()
	{
		return new Ape::ApeGallerySamplePlugin;
	}

	APE_PLUGIN_FUNC void DestroyApeGallerySamplePlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApeGallerySamplePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeGallerySamplePlugin, DestroyApeGallerySamplePlugin);
		return 0;
	}
}

#endif
