/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

Copyright (c) 2010-2012 Michael Broutin

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
#include "ScriptInterpreter.h"
#include "Procedural.h"

using namespace Ogre;
using namespace std;

extern "C"
{
	extern int luaopen_Procedural(lua_State* L); // declare the wrapped module
}
ScriptInterpreter* ScriptInterpreter::mInstance = 0;
//-------------------------------------------------------------------------------------
void ScriptInterpreter::createScene(void)
{
	if (mScriptSourceMode == SSM_SCRIPTDIR)
	{
		ResourceGroupManager::getSingleton().destroyResourceGroup("Scripts");
		ResourceGroupManager::getSingleton().createResourceGroup("Scripts");
		ResourceGroupManager::getSingleton().addResourceLocation(mScriptDir, "FileSystem", "Scripts");
	}
	mDefaultTriangleBuffer = PlaneGenerator().buildTriangleBuffer();
	Overlay* o = OverlayManager::getSingleton().create("myOverlay");
	OverlayContainer* cont = (OverlayContainer*)OverlayManager::getSingleton().createOverlayElement("Panel","myCont");
	o->add2D(cont);
	mTextMessage = OverlayManager::getSingleton().createOverlayElement("TextArea","myText");
	cont->addChild(mTextMessage);
	mTextMessage->setCaption("Ogre program");
	mTextMessage->setParameter("font_name","SdkTrays/Caption");
	o->show();

	checkScriptModified();
}
//-------------------------------------------------------------------------------------
void ScriptInterpreter::createLogManager(void)
{
	LogManager* logManager = OGRE_NEW LogManager();
	logManager->createLog("Ogre.log", true, false, false);
}
//-------------------------------------------------------------------------------------
void ScriptInterpreter::destroyScene(void)
{
	for (std::vector<Entity*>::iterator entity = mEntities.begin(); entity != mEntities.end(); entity++)
	{
		MeshManager::getSingletonPtr()->remove((*entity)->getMesh()->getName());
		mSceneMgr->destroyEntity(*entity);
	}
	for (std::vector<SceneNode*>::iterator sceneNode = mSceneNodes.begin(); sceneNode != mSceneNodes.end(); sceneNode++)
	{
		mSceneMgr->destroySceneNode(*sceneNode);
	}
	for (std::vector<MaterialPtr>::iterator it = mMaterials.begin(); it!=mMaterials.end(); it++)
	{
		TextureManager::getSingletonPtr()->remove((*it)->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName());
		MaterialManager::getSingletonPtr()->remove((*it)->getName());
	}
	mSceneNodes.clear();
	mEntities.clear();
	mMaterials.clear();
	mTextures.clear();
	delete mCurrentDotFile;
	mCurrentDotFile = 0;
}
//-------------------------------------------------------------------------------------
void ScriptInterpreter::writeEveythingToDisk()
{
	MeshSerializer serializer;
	int meshCounter = 0;
	for (std::vector<Entity*>::iterator it = mEntities.begin(); it != mEntities.end(); ++it)
	{
		meshCounter++;
		std::string fileName;
		if (mEntities.size()>1)
			fileName = mCurrentScriptName.substr(0, mCurrentScriptName.find_last_of(".")) + "_" + StringConverter::toString(meshCounter);
		else
			fileName = mCurrentScriptName.substr(0, mCurrentScriptName.find_last_of("."));
		serializer.exportMesh((*it)->getMesh().getPointer(), fileName + ".mesh", MESH_VERSION_LATEST);
	}
	/*MaterialSerializer matSer;
	int matCounter = 0;
	for (std::vector<MaterialPtr>::iterator it = mMaterials.begin(); it != mMaterials.end(); ++it)
	{
		matCounter++;
		std::string fileName;
		if (mEntities.size()>1)
			fileName = mCurrentScriptName.substr(0, mCurrentScriptName.find_last_of(".")) + "_" + StringConverter::toString(meshCounter);
		else
			fileName = mCurrentScriptName.substr(0, mCurrentScriptName.find_last_of("."));

		matSer.exportMaterial(*it, fileName + ".material");
	}*/
	for (std::vector<TexturePtr>::iterator it = mTextures.begin(); it!=mTextures.end(); ++it)
	{
		Ogre::Image im;
		(*it)->convertToImage(im);
		im.save(mCurrentScriptName.substr(0, mCurrentScriptName.find_last_of(".")) + ".png");
	}
	if (mCurrentDotFile)
		mCurrentDotFile->save(".");
}

//-------------------------------------------------------------------------------------
void ScriptInterpreter::reloadScript()
{
	reloadScriptNameFromIndex();
	String path = *ResourceGroupManager::getSingleton().findResourceLocation("Scripts", "*")->begin();

	lua_State* L;
	L=luaL_newstate();
	luaL_openlibs(L);
	luaopen_Procedural(L);	// load the wrappered module
	luaL_dostring(L, "tests = Procedural.ScriptInterpreter_getInstance()");
	destroyScene();

	Timer timer;
	std::string message;
	bool success;
	if (luaL_loadfile(L,(path + "/" + mCurrentScriptName).c_str())==0)
	{
		timer.reset();
		if (lua_pcall(L,0,0,0) ==0)
		{
			unsigned long timeSpent = timer.getMilliseconds();
			message = "loaded " + mCurrentScriptName + " in " + StringConverter::toString(timeSpent) + " milliseconds";
			success = true;
			if (mWriteToDisk)
				writeEveythingToDisk();
			if (mBatchMode)
			{
				if (mExecutionTimes.find(mCurrentScriptName) == mExecutionTimes.end())
					mExecutionTimes[mCurrentScriptName] = 0;
				mExecutionTimes[mCurrentScriptName] += timeSpent;
			}
		}
		else
		{
			message = "Failed to load " + mCurrentScriptName + " :" + lua_tostring(L,-1);
			success = false;
		}
	}
	else
	{
		message = "Failed to load " + mCurrentScriptName + " :" + lua_tostring(L,-1);
		success = false;
	}
	if (success)
		mTextMessage->setCaption("OK (" + message + ")");
	else
		mTextMessage->setCaption("KO (" + message + ")");
	if (success)
		mCamera->getViewport()->setBackgroundColour(ColourValue(0.2f,0.4f,0.2f));
	else
		mCamera->getViewport()->setBackgroundColour(ColourValue(0.4f,0.2f,0.2f));
	cout<<message<<endl;
	Utils::log(message);

	lua_close(L);
}

//-------------------------------------------------------------------------------------
void ScriptInterpreter::checkScriptModified()
{
	time_t newTime = ResourceGroupManager::getSingleton().resourceModifiedTime("Scripts", mCurrentScriptName);
	if (newTime > mCurrentScriptReloadTime || newTime == 0)
	{
		reloadScript();
		mCurrentScriptReloadTime = newTime;
		if (newTime == 0)
			mCurrentScriptReloadTime = ResourceGroupManager::getSingleton().resourceModifiedTime("Scripts", mCurrentScriptName);
	}
}
//-------------------------------------------------------------------------------------
void ScriptInterpreter::createCamera(void)
{
	BaseApplication::createCamera();

	// Setup camera and light
	mCamera->setNearClipDistance(.5);
	mCamera->setPosition(0,10,-50);
	mCamera->lookAt(0,0,0);
}
//-------------------------------------------------------------------------------------
void ScriptInterpreter::createViewports(void)
{
	BaseApplication::createViewports();
	mCamera->getViewport()->setBackgroundColour(ColourValue(0.2f,0.4f,0.2f));
}
//-------------------------------------------------------------------------------------
bool ScriptInterpreter::frameStarted(const FrameEvent& evt)
{
	movingLight->setPosition(mCamera->getPosition());
	if (mBatchMode)
	{
		size_t scriptCount = ResourceGroupManager::getSingleton().findResourceNames("Scripts", "*.lua")->size();
		if (mCurrentPerformanceIndex>=5 || !mPerformanceMode)
		{
			mCurrentScriptIndex++;
			mCurrentPerformanceIndex = 0;
		}
		if (mCurrentScriptIndex >= scriptCount)
		{
			writePerformanceFile();
			return false;
		}
		reloadScriptNameFromIndex();
		mCurrentScriptReloadTime=0;
		mCurrentPerformanceIndex++;
	}
	checkScriptModified();
	return true;
}
//-------------------------------------------------------------------------------------
bool ScriptInterpreter::keyReleased( const OIS::KeyEvent& arg )
{
	if (!mBatchMode)
	{
		if (arg.key == OIS::KC_M || arg.key == OIS::KC_ADD || arg.key == OIS::KC_PGDOWN)
		{
			size_t scriptCount = ResourceGroupManager::getSingleton().findResourceNames("Scripts", "*.lua")->size();
			mCurrentScriptIndex = Utils::modulo(mCurrentScriptIndex+1, scriptCount);
			reloadScriptNameFromIndex();
			mCurrentScriptReloadTime=0;
			return true;
		}
		if (arg.key == OIS::KC_N || arg.key == OIS::KC_SUBTRACT || arg.key == OIS::KC_PGUP)
		{
			size_t scriptCount = ResourceGroupManager::getSingleton().findResourceNames("Scripts", "*.lua")->size();
			mCurrentScriptIndex = Utils::modulo(mCurrentScriptIndex-1, scriptCount);
			reloadScriptNameFromIndex();
			mCurrentScriptReloadTime=0;
			return true;
		}
		if (arg.key == OIS::KC_F5)
			reloadScript();
		if (arg.key == OIS::KC_F2)
			writeEveythingToDisk();
	}
	return BaseApplication::keyReleased(arg);
}
//-------------------------------------------------------------------------------------
void ScriptInterpreter::writePerformanceFile()
{
	std::ofstream perfFile;
	perfFile.open("perf.csv");
	for (std::map<std::string, long>::iterator it = mExecutionTimes.begin(); it != mExecutionTimes.end(); it++)
		perfFile<<it->first<<";";
	perfFile<<std::endl;
	for (std::map<std::string, long>::iterator it = mExecutionTimes.begin(); it != mExecutionTimes.end(); it++)
		perfFile<<(it->second/5)<<";";
	perfFile<<std::endl;
	perfFile.close();
}
//-------------------------------------------------------------------------------------
bool ScriptInterpreter::processInput(int argc, char* argv[])
{
	mBatchMode = false;
	mScriptSourceMode = SSM_RESOURCES;
	mWriteToDisk = false;
	cout<<"OgreProcedural Lua Tester tool"<<endl;
	cout<<"-----------------------------"<<endl;
	cout<<endl;
	if (argc < 2)
	{
		cout<<"Running with default arguments. Type -help for additional options..."<<endl;
	}
	else
	{
		if (string(argv[1]) == "-help")
		{
			cout<<"Additional arguments :"<<endl;
			cout<<"-batch               runs all scripts in a sequence, and exit when done"<<endl;
			cout<<"-performance         runs all the scripts 5 times, outputing the time spent for each one (batch mode only)"<<endl;
			cout<<"-todisk              all output meshes and textures will be written in the current directory"<<endl;
			//cout<<"-script <scriptname> runs only that script"<<endl;
			cout<<"-scriptdir <path>    runs all the scripts contained in the path"<<endl;
			return false;
		}
		else
		{
			for (int i=1; i<argc; ++i)
			{
				string param(argv[i]);
				if (param == "-batch")
				{
					mBatchMode = true;
				}
				else if (param == "-todisk")
				{
					mWriteToDisk = true;
				}
				else if (param == "-script")
				{
					mScriptSourceMode = SSM_SCRIPTFILE;
					mScriptFileName = string(argv[++i]);
				}
				else if (param == "-scriptdir")
				{
					mScriptSourceMode = SSM_SCRIPTDIR;
					mScriptDir = string(argv[++i]);
				}
				else if (param == "-performance")
				{
					mPerformanceMode = true;
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
int main(int argc, char* argv[])
{
	ScriptInterpreter app;

	if (!app.processInput(argc, argv))
		return 0;
	try
	{
		app.go();
	}
	catch ( Exception& e )
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		cerr << "An exception has occured: " <<
		     e.getFullDescription().c_str() << endl;
#endif
	}

	return 0;
}
