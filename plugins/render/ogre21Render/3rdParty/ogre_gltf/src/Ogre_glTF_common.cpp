#include "Ogre_glTF_common.hpp"

#include <OgreLogManager.h>

void OgreLog(const std::string& message)
{
#ifdef _DEBUG
	Ogre::LogManager::getSingleton().logMessage(message);
#else
	//Do something with message?
#endif
}

void OgreLog(const std::stringstream& message)
{
#ifdef _DEBUG
	OgreLog(message.str()); //Also prevent codegen for reading the string insdie that stream
#else
#endif
}
