#include "Ogre.h"
#include "Procedural.h"
#include "Ogitors.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifdef PLUGIN_EXPORT
#define PluginExport __declspec (dllexport)
#else
#define PluginExport __declspec (dllimport)
#endif
#else
#define PluginExport
#endif

extern "C" bool PluginExport dllStopPlugin();

extern "C" bool PluginExport dllGetPluginName(Ogre::String& name);

extern "C" bool PluginExport dllStartPlugin(void* identifier, Ogre::String& name);