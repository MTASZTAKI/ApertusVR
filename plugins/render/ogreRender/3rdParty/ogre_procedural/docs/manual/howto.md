How to build {#howto}
============

These are the detailed steps in order to build the SDK :

- Make sure CMake and the Ogre3d SDK are present on your system.
- Open CMake, set the source directory as the root directory of Ogre %Procedural, choose anything you want for the build directory.
- Click 'Add Entry'. Add the 'OGRE_HOME' variable (type=PATH), its value being the root folder of Ogre3d SDK.
- Click 'configure', then 'generate'
- Your project file is now ready to be used. Build the target 'INSTALL' in order to get a working SDK, which will be in the 'sdk' subfolder.

## Configuring your project

### If you don't use CMake in your project

- Setup your IDE to use OgreProcedural's 'include' and 'lib' subdirectories.
- Copy the dynamic libraries OgreProcedural and OgreProcedural_d to a directory where they can be seen by your main executable.

### If you use CMake in your project

- Copy FindOgreProcedural.cmake to a location where your CMake can use it.
- Setup OgreProcedural_HOME (either as system environnement variable or CMake variable) to point to OgreProcedural's SDK folder.
- When OgreProcedural is found, OgreProcedural_INCLUDE_DIRS and OgreProcedural_LIBRARIES are defined as CMake variables.

## Basic usage sample

~~~~~~~~~~~~~~
#include "Procedural.h"
// ...
Procedural::BoxGenerator().setSizeX(2.0).realizeMesh("myBox");
~~~~~~~~~~~~~~

That's it.

It's the minimal use case of OgreProcedural, and it produces a box mesh. 
You can then instantiate an entity from this mesh, and add it to your scene.

## MeshGenerators, TriangleBuffers and Ogre Meshes

In OgreProcedural, all the classes deriving from Procedural::MeshGenerator are made to produce Ogre Meshes.

There are two ways of creating a Mesh from a Procedural::MeshGenerator :
* Call _realizeMesh() if you want to directly produce Ogre::Mesh.
* Create a Procedural::TriangleBuffer and fill it using _MeshGenerator::addToTriangleBuffer()_. When it's ready to be used, call to _TriangleBuffer::transformToMesh()_ will get you an Ogre Mesh with every geometry accumulated from various Procedural::MeshGenerator as a single batch.

Procedural::MeshGenerator also have some utility functions to transform produced triangles. This is especially useful if you have several Procedural::MeshGenerator for one Procedural::TriangleBuffer.

For example :
~~~~~~~~~~~~~~
// Two boxes in one batch
Procedural::TriangleBuffer tb;
BoxGenerator b;
b.setPosition(2,0,0).addToTriangleBuffer(tb);
b.setPosition(-2,0,0).addToTriangleBuffer(tb);
tb.transformToMesh("twoBoxes");
~~~~~~~~~~~~~~