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

#ifndef APE_OISUSERINPUTPLUGIN_H
#define APE_OISUSERINPUTPLUGIN_H

#include <iostream>
#include <string>
#include <thread> 
#include "OIS.h"
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeOisUserInputPluginConfigs.h"
#include "ApeISystemConfig.h"
#include "ApeINode.h"
#include "ApeIScene.h"
#include "ApeIMainWindow.h"
#include "ApeIEventManager.h"
#include "ApeICamera.h"
#include "ApeITextGeometry.h"
#include "ApeInterpolator.h"

namespace Ape
{
	class OISUserInputPlugin : public Ape::IPlugin, public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		OISUserInputPlugin();

		~OISUserInputPlugin();

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

	private:
		struct UserNodePose
		{
			Ape::Vector3 position;
			Ape::Quaternion orientation;

			UserNodePose(
				Ape::Vector3 position,
				Ape::Quaternion orientation)
			{
				this->position = position;
				this->orientation = orientation;
			}
		};
		OIS::Keyboard* mpKeyboard; 

		OIS::Mouse* mpMouse;

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		std::map<OIS::KeyCode, bool> mKeyCodeMap;

		std::vector<UserNodePose> mUserNodePoses;

		int mUserNodePosesToggleIndex;

		int mTranslateSpeedFactor;

		int mRotateSpeedFactor;

		bool mIsKeyPressed;

		void moveUserNode();

		void saveUserNodePose(Ape::NodeSharedPtr userNode);

		void toggleUserNodePoses(Ape::NodeSharedPtr userNode);

		void eventCallBack(const Ape::Event& event);
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateOISUserInputPlugin()
	{
		return new OISUserInputPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOISUserInputPlugin(Ape::IPlugin *plugin)
	{
		delete (OISUserInputPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME("ApeOisUserInputPlugin");

	APE_PLUGIN_ALLOC()
	{
		std::cout << "ApeOisUserInputPlugin_CREATE" << std::endl;
		ApeRegisterPlugin("ApeOisUserInputPlugin", CreateOISUserInputPlugin, DestroyOISUserInputPlugin);
		return 0;
	}
}

#endif
