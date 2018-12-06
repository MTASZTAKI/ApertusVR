/*MIT License

Copyright (c) 2018 MTA SZTAKI

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
#include <list>
#include <string>
#include <thread>
#include "system/ApeIMainWindow.h"
#include "plugin/ApeIPlugin.h"
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "managers/ApeISystemConfig.h"
#include "sceneelements/ApeIBrowser.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeIPointCloud.h"
#include "sceneelements/ApeIRayGeometry.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeIUnitTexture.h"
#include "sceneelements/ApeITorusGeometry.h"
#include "utils/ApeInterpolator.h"
#include "ApeOisUserInputPluginConfigs.h"
#include "OIS.h"

#define THIS_PLUGINNAME "ApeOisUserInputPlugin"

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

		bool isNodeSelected(std::string nodeName);

		void addNodeSelection(std::string nodeName);

		bool removeNodeSelection(std::string nodeName);

		void clearNodeSelection();

	private:
		struct MouseState
		{
			OIS::MouseState posStart;
			OIS::MouseState posEnd;
			OIS::MouseState posPrevious;
			OIS::MouseState posCurrent;
			std::map<OIS::MouseButtonID, bool> buttonDownMap;
			bool isDragModeLeft = false;
			bool isDragModeMiddle = false;
			bool isDragModeRight = false;
			bool isMouseMoved = false;
			int scrollVelocity = 0;
		};

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

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		Ape::NodeWeakPtr mNodeToMove;

		Ape::NodeWeakPtr mHeadNode;

		std::string mHeadNodeName;

		Ape::NodeWeakPtr mDummyNode;

		Ape::NodeWeakPtr mCursorNode;

		Ape::TextGeometryWeakPtr mCursorText;

		std::map<OIS::KeyCode, bool> mKeyCodeMap;

		MouseState mMouseState;

		std::vector<UserNodePose> mUserNodeTogglePoses;

		std::vector<UserNodePose> mUserNodeAnimatePoses;

		Ape::BrowserWeakPtr mOverlayBrowser;

		Ape::UnitTextureWeakPtr mOverlayMouseTexture;

		Ape::RayGeometryWeakPtr mRayGeometry;

		Ape::NodeWeakPtr mRayOverlayNode;

		std::map<std::string, Ape::NodeWeakPtr> mSelectedNodes;

		bool mEnableOverlayBrowserKeyEvents;

		bool mIsNewKeyEvent;

		int mUserNodePosesToggleIndex;

		float mTranslateSpeedFactorKeyboard;

		float mRotateSpeedFactorKeyboard;

		float mTranslateSpeedFactorMouse;

		float mRotateSpeedFactorMouse;

		float mGeneralSpeedFactor;

		bool mIsKeyPressed;

		bool mIsUserNodeAnimated;

		void moveUserNodeByKeyBoard();

		void moveUserNodeByMouse();

		void saveUserNodePose();

		void toggleUserNodePoses(Ape::NodeSharedPtr userNode);

		void eventCallBack(const Ape::Event& event);

		void animateUserNode(Ape::NodeSharedPtr userNode);

		void fillUserNodeAnimatePoses();
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateOISUserInputPlugin()
	{
		return new Ape::OISUserInputPlugin;
	}

	APE_PLUGIN_FUNC void DestroyOISUserInputPlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::OISUserInputPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateOISUserInputPlugin, DestroyOISUserInputPlugin);
		return 0;
	}
}

#endif
