#include "Plugin.h"

//----------------------------------------------------------------------------
bool dllStartPlugin(void* identifier, Ogre::String& name)
{
	name = "OgreProcedural Plugin";
	return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
	return true;
}

bool dllGetPluginName(Ogre::String& name)
{
	name = "OgreProcedural Plugin";
	return true;
}