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
#include "Svg.h"
#include "Procedural.h"
#include "ProceduralUtils.h"

//-------------------------------------------------------------------------------------
void Sample_Svg::createScene(void)
{
	// -- Extrude SVG primitive shapes
	Procedural::Path p;
	p.addPoint(0, 0, 0);
	p.addPoint(0, 10, 0);

	Procedural::MultiShape out;
	Procedural::SvgLoader svg;
	svg.parseSvgFile(out, "test.svg", "Essential", 16);
	Procedural::Extruder().setMultiShapeToExtrude(&out).setExtrusionPath(&p).setScale(.05f).realizeMesh("svg");
	putMesh2("svg");
}
//-------------------------------------------------------------------------------------
void Sample_Svg::createCamera(void)
{
	BaseApplication::createCamera();
}
//-------------------------------------------------------------------------------------
bool Sample_Svg::frameStarted(const FrameEvent& evt)
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
		Sample_Svg app;

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
