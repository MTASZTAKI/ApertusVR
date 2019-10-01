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

#ifndef APE_PLUGINAPI_H
#define APE_PLUGINAPI_H

#include "apeIPlugin.h"
#include "apePluginDefines.h"

#define APE_PLUGIN_API_VERSION  1

#define APE_PLUGIN_INTERNAL_FUNC     extern "C" APE_PLUGIN_INTERNAL

#define APE_PLUGIN_FUNC   extern "C" APE_PLUGIN_API

#define APE_PLUGIN_ALLOC() \
	const int apePluginVersion = APE_PLUGIN_API_VERSION; \
	APE_PLUGIN_FUNC int apePluginInit()

#define APE_PLUGIN_FREE() \
	APE_PLUGIN_FUNC int apePluginFree()

#define APE_PLUGIN_DISPLAY_NAME(name) \
	APE_PLUGIN_API const char * apePluginDisplayName = name

#define PLUGIN_MIN_VERSION(version) \
	APE_PLUGIN_API const char * apePluginMinVersion = version

#define PLUGIN_MAX_VERSION(version) \
	APE_PLUGIN_API const char * apePluginMaxVersion = version

typedef ape::IPlugin* (*apePluginAllocFunc)();

typedef void (*apePluginFreeFunc)(ape::IPlugin*);

APE_PLUGIN_INTERNAL_FUNC void apeRegisterPlugin(const char* type, apePluginAllocFunc init_cb, apePluginFreeFunc free_cb);

#endif
