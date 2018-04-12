/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/


#ifndef APE_TEXASEEPGLUGIN_H
#define APE_TEXASEEPGLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeIMainWindow.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeILight.h"
#include "ApeITextGeometry.h"
#include "ApeIFileGeometry.h"
#include "ApeIFileMaterial.h"
#include "ApeISystemConfig.h"
#include "ApeICamera.h"
#include "ApeEuler.h"
#include "OIS.h"

#define THIS_PLUGINNAME "ApeTexasEEGPlugin"

class ApeTexasEEGPlugin : public Ape::IPlugin, public OIS::KeyListener, public OIS::MouseListener
{
private:
	Ape::IEventManager* mpEventManager;

	Ape::IScene* mpScene;
	
	void eventCallBack(const Ape::Event& event);

	OIS::Mouse* mpMouse;

	OIS::Keyboard* mpKeyboard;

	Ape::IMainWindow* mpMainWindow;

	Ape::ISystemConfig* mpSystemConfig;

	Ape::NodeWeakPtr mUserNode;

	std::map<OIS::KeyCode, bool> mKeyCodeMap;

	void moveUserNodeByKeyBoard();

	void moveUserNodeByMouse();

	bool mIsMouseMoved;

	int mTranslateSpeedFactor;

	int mRotateSpeedFactor;

	bool mIsSwim;

	int mMouseMoveRelativeX;

	int mMouseMoveRelativeY;
	
public:
	ApeTexasEEGPlugin();

	~ApeTexasEEGPlugin();
	
	void Init() override;

	void Run() override;

	void Step() override;

	void Stop() override;

	void Suspend() override;

	void Restart() override;

	bool keyPressed(const OIS::KeyEvent& e) override;

	bool keyReleased(const OIS::KeyEvent& e) override;

	bool mouseMoved(const OIS::MouseEvent& e) override;

	bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id) override;

	bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id) override;
};

APE_PLUGIN_FUNC Ape::IPlugin* CreateApeTexasEEGPlugin()
{
	return new ApeTexasEEGPlugin;
}

APE_PLUGIN_FUNC void DestroyApeTexasEEGPlugin(Ape::IPlugin *plugin)
{
	delete (ApeTexasEEGPlugin*)plugin;
}

APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

APE_PLUGIN_ALLOC()
{
	LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
	ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeTexasEEGPlugin, DestroyApeTexasEEGPlugin);
	return 0;
}

#endif
