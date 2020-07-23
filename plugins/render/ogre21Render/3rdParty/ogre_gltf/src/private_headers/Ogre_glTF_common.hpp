#pragma once
#include "Ogre_glTF_DLL.hpp"
#include <string>

///Easilly print a stringto the ogre log
void OgreLog(const std::string& message);

///Overload that takes a stringstream
void OgreLog(const std::stringstream& message);