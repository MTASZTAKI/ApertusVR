#pragma once

#ifdef _WIN32
#ifdef Ogre_glTF_DLL_EXPORT_CONFIG_ON
///Export symbol
#define Ogre_glTF_EXPORT __declspec(dllexport)
#else
///Import symbol
#define Ogre_glTF_EXPORT __declspec(dllimport)
#endif
#else
///Don't do anything with this, we're not on Windows :-)
#define Ogre_glTF_EXPORT //dummy
#endif
