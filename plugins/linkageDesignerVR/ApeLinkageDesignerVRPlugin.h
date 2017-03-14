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


#ifndef APE_LINKAGEDESIGNERVRPLUGIN_H
#define APE_LINKAGEDESIGNERVRPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
#include <list>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeILight.h"
#include "ApeICamera.h"
#include "ApeISystemConfig.h"
#include "ApeIFileMaterial.h"
#include "ApeIPlaneGeometry.h"
#include "ApeITubeGeometry.h"
#include "ApeIConeGeometry.h"
#include "ApeISphereGeometry.h"
#include "ApeITextGeometry.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeIIndexedLineSetGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeIPbsPass.h"
#include "ApeInterpolator.h"
#include "ApeIFileGeometry.h"
#include "ApeIMainWindow.h"
#include "OIS.h"


#define THIS_PLUGINNAME "ApeLinkageDesignerVRPlugin"

class ApeLinkageDesignerVRPlugin : public Ape::IPlugin, public OIS::KeyListener, public OIS::MouseListener
{
private:
	struct ScenePose
	{
		Ape::Vector3 position;
		Ape::Quaternion orientation;

		ScenePose(
			Ape::Vector3 position,
			Ape::Quaternion orientation)
		{
			this->position = position;
			this->orientation = orientation;
		}
	};

	Ape::IEventManager* mpEventManager;

	Ape::IScene* mpScene;

	Ape::ISystemConfig* mpSystemConfig;

	std::vector<std::unique_ptr<Ape::Interpolator>> mInterpolators;
	
	void eventCallBack(const Ape::Event& event);

	std::map<OIS::KeyCode, bool> mKeyCodeMap;

	Ape::NodeWeakPtr mUserNode;

	Ape::IMainWindow* mpMainWindow;

	OIS::Keyboard* mpKeyboard;

	OIS::Mouse* mpMouse;

	int mSceneToggleIndex;

	std::vector<ScenePose> mScenePoses;

	int mSwitchNodeVisibilityToggleIndex;

	std::vector<std::string> mSwitchNodeVisibilityNames;

	std::vector<Ape::NodeWeakPtr> mSwitchNodes;

	float mTranslateSpeedFactor;

	float mRotateSpeedFactor;

	void moveUserNode();

	void toggleScenePoses(Ape::NodeSharedPtr userNode);

	void toggleSwitchNodesVisibility();
	
public:
	ApeLinkageDesignerVRPlugin();

	~ApeLinkageDesignerVRPlugin();
	
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

APE_PLUGIN_FUNC Ape::IPlugin* CreateApeLinkageDesignerVRPlugin()
{
	return new ApeLinkageDesignerVRPlugin;
}

APE_PLUGIN_FUNC void DestroyApeLinkageDesignerVRPlugin(Ape::IPlugin *plugin)
{
	delete (ApeLinkageDesignerVRPlugin*)plugin;
}

APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

APE_PLUGIN_ALLOC()
{
	std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
	ApeRegisterPlugin(THIS_PLUGINNAME, CreateApeLinkageDesignerVRPlugin, DestroyApeLinkageDesignerVRPlugin);
	return 0;
}

#endif
