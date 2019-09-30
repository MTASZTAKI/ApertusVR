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
#include "Tests.h"
#include "Procedural.h"

//-------------------------------------------------------------------------------------
void Unit_Tests::createScene(void)
{
	Ogre::Overlay* o = Ogre::OverlayManager::getSingleton().create("myOverlay");
	Ogre::OverlayContainer* cont = (Ogre::OverlayContainer*)OverlayManager::getSingleton().createOverlayElement("Panel","myCont");
	o->add2D(cont);
	Ogre::OverlayElement* el = OverlayManager::getSingleton().createOverlayElement("TextArea","myText");
	cont->addChild(el);
	el->setCaption("Ogre program");
	el->setParameter("font_name","SdkTrays/Caption");
	o->show();

	// Register all unit tests
	//mUnitTests.push_back(new Test_Boolean(mSceneMgr));
	//mUnitTests.push_back(new Test_Extruder(mSceneMgr));
	mUnitTests.push_back(new Test_Extruder_MultiPath(mSceneMgr));
	mUnitTests.push_back(new Test_Primitives(mSceneMgr));
	mUnitTests.push_back(new Test_Triangulation(mSceneMgr));
	mUnitTests.push_back(new Test_ShapeBoolean(mSceneMgr));
	mUnitTests.push_back(new Test_Extruder(mSceneMgr));
	mUnitTests.push_back(new Test_Lathe(mSceneMgr));
	mUnitTests.push_back(new Test_Splines(mSceneMgr));
	mUnitTests.push_back(new Test_ShapeThick(mSceneMgr));
	mUnitTests.push_back(new Test_InvertNormals(mSceneMgr));

	// Init first test
	switchToTest(0);
}
//-------------------------------------------------------------------------------------
void Unit_Tests::destroyScene(void)
{
	for (std::vector<Unit_Test*>::iterator it = mUnitTests.begin(); it!=mUnitTests.end(); it++)
		delete *it;
}
//-------------------------------------------------------------------------------------
void Unit_Tests::createCamera(void)
{
	BaseApplication::createCamera();

	// Setup camera and light
	mCamera->setNearClipDistance(.5);
	mCamera->setPosition(0,10,-50);
	mCamera->lookAt(0,0,0);
}
//-------------------------------------------------------------------------------------
void Unit_Tests::createViewports(void)
{
	BaseApplication::createViewports();
	mCamera->getViewport()->setBackgroundColour(ColourValue(0.2f,0.4f,0.2f));
}
//-------------------------------------------------------------------------------------
bool Unit_Tests::frameStarted(const FrameEvent& evt)
{
	movingLight->setPosition(mCamera->getPosition());
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
		Unit_Tests app;

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
