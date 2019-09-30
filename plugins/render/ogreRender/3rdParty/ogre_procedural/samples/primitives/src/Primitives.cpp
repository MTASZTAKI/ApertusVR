/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

Copyright (c) 2010-2013 Michael Broutin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "ProceduralStableHeaders.h"
#include "Primitives.h"
#include "Procedural.h"

//-------------------------------------------------------------------------------------
void Sample_Primitives::createScene(void)
{
	// Generates every type of primitive
	Procedural::PlaneGenerator().setNumSegX(20).setNumSegY(20).setSizeX(150).setSizeY(150).setUTile(5.0).setVTile(5.0).realizeMesh("planeMesh");
	putMesh2("planeMesh");
	Procedural::SphereGenerator().setRadius(2.f).setUTile(5.).setVTile(5.).realizeMesh("sphereMesh");
	putMesh("sphereMesh", Vector3(0,10,0));
	Procedural::CylinderGenerator().setHeight(3.f).setRadius(1.f).setUTile(3.).realizeMesh("cylinderMesh");
	putMesh("cylinderMesh", Vector3(10,10,0));
	Procedural::TorusGenerator().setRadius(3.f).setSectionRadius(1.f).setUTile(10.).setVTile(5.).realizeMesh("torusMesh");
	putMesh("torusMesh", Vector3(-10,10,0));
	Procedural::ConeGenerator().setRadius(2.f).setHeight(3.f).setNumSegBase(36).setNumSegHeight(2).setUTile(3.).realizeMesh("coneMesh");
	putMesh("coneMesh", Vector3(0,10,-10));
	Procedural::TubeGenerator().setHeight(3.f).setUTile(3.).realizeMesh("tubeMesh");
	putMesh("tubeMesh", Vector3(-10,10,-10));
	Procedural::BoxGenerator().setSizeX(2.0).setSizeY(4.f).setSizeZ(6.f).realizeMesh("boxMesh");
	putMesh("boxMesh", Vector3(10,10,-10));
	Procedural::CapsuleGenerator().setHeight(2.f).realizeMesh("capsuleMesh");
	putMesh("capsuleMesh", Vector3(0,10,10));
	Procedural::TorusKnotGenerator().setRadius(2.f).setSectionRadius(.5f).setUTile(3.f).setNumSegCircle(64).setNumSegSection(16).realizeMesh("torusKnotMesh");
	putMesh("torusKnotMesh", Vector3(-10,10,10));
	Procedural::IcoSphereGenerator().setRadius(2.).setNumIterations(3).setUTile(5.).setVTile(5.).realizeMesh("icoSphereMesh");
	putMesh("icoSphereMesh", Vector3(10,10,10));
	Procedural::RoundedBoxGenerator().setSizeX(1.f).setSizeY(5.f).setSizeZ(5.f).setChamferSize(1.f).realizeMesh("roundedBoxMesh");
	putMesh("roundedBoxMesh", Vector3(20,10,10));
	Procedural::SpringGenerator().setNumSegCircle(32).setNumSegPath(30).realizeMesh("springMesh");
	putMesh("springMesh", Vector3(20,10,0));
}
//-------------------------------------------------------------------------------------
void Sample_Primitives::createCamera(void)
{
	BaseApplication::createCamera();
}
//-------------------------------------------------------------------------------------
bool Sample_Primitives::frameStarted(const FrameEvent& evt)
{
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	movingLight->setPosition(mCamera->getPosition());
#else
	movingLight->getParentSceneNode()->setPosition(mCamera->getPosition());
#endif
	return true;
}
//-------------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
	int main(int argc, char* argv[])
#endif
	{
		// Create application object
		Sample_Primitives app;

		try
		{
			app.go();
		}
		catch ( Ogre::Exception& e )
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
			          e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif
