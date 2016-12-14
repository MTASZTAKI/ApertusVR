Ogre Procedural
---------------

Ogre Procedural (http://code.google.com/p/ogre-procedural/) is a C++ library which aims to produce procedural geometry for Ogre3D (http://www.ogre3d.org).

It is licensed under the MIT license (see MIT.txt), which basically means you can do anything you want with it, including using it in a commercial, closed source program.
Although not compulsory, it is also considered to be nice if you provide a link to ogre-procedural in your product's credits.

You can find a user manual at http://www.ogreprocedural.org/manual and the full API reference at http://www.ogreprocedural.org/doc

If you have any question or remark, please PM me on ogre3d forum (username : mikachu) or send me an email (webmaster AT ogreprocedural.org).

How to build it?
--------------

Skip this step if you downloaded the precompiled SDK version.
Ogre Procedural is using the CMake build system. 
These are the steps to build it :
1. Make sure CMake and the Ogre3d SDK are present on your system.
2. Open CMake, set the source directory as the root directory of Ogre Procedural, choose anything you want for the build directory.
3. Click 'Add Entry'. Add the 'OGRE_HOME' variable (type=PATH), its value being the root folder of Ogre3d SDK.
4. Click 'configure', then 'generate'
5. Your project file is now ready to be used. Build the target 'INSTALL' in order to get a working SDK, which will be in the 'sdk' subfolder.

How to use it?
--------------

1. If you don't use CMake in your project

- Setup your IDE to use OgreProcedural's 'include' and 'lib' subdirectories.
- Copy the dynamic libraries OgreProcedural and OgreProcedural_d to a directory where they can be seen by your main executable.

2. If you use CMake in your project

- Copy FindOgreProcedural.cmake to a location where your CMake can use it.
- Setup OgreProcedural_HOME (either as system environnement variable or CMake variable) to point to OgreProcedural's SDK folder.
- When OgreProcedural is found, OgreProcedural_INCLUDE_DIRS and OgreProcedural_LIBRARIES are defined as CMake variables.