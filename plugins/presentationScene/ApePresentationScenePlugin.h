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

#ifndef APE_PRESENATITONSCENEPLUGIN_H
#define APE_PRESENATITONSCENEPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
#include <fstream>
#include "Ape.h"
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeISceneManager.h"
#include "ApeINode.h"
#include "ApeILight.h"
#include "ApeICamera.h"
#include "ApeISystemConfig.h"
#include "ApeIFileMaterial.h"
#include "ApeIFileGeometry.h"
#include "ApeIPlaneGeometry.h"
#include "ApeITubeGeometry.h"
#include "ApeIConeGeometry.h"
#include "ApeISphereGeometry.h"
#include "ApeITextGeometry.h"
#include "ApeIIndexedFaceSetGeometry.h"
#include "ApeIIndexedLineSetGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeIPbsPass.h"
#include "ApeIMainWindow.h"
#include "ApeIBrowser.h"
#include "ApeIUnitTexture.h"
#include "ApeInterpolator.h"
#include "ApeEuler.h"
#include "ApeIManualPass.h"
#include "ApeIRayGeometry.h"
#include "OIS.h"

#define THIS_PLUGINNAME "ApePresentationScenePlugin"

namespace Ape
{
	class ApePresentationScenePlugin : public Ape::IPlugin, public OIS::KeyListener, public OIS::MouseListener
	{
	private:
		enum Context
		{
			UNKOWN,
			SPACE,
			BROWSER,
			INVALID
		};
		struct StoryElement
		{
			Ape::Vector3 cameraPosition;
			Ape::Quaternion cameraOrientation;
			std::string browserName;
			std::string browserURL;
			Ape::Vector3 browserPosition;
			Ape::Quaternion browserOrientation;
			int browserWidth;
			int browserHeight;
			int browserZoom;
			int browserResolutionVertical;
			int browserResolutionHorizontal;

			StoryElement()
			{
				this->cameraPosition = Ape::Vector3();
				this->cameraOrientation = Ape::Quaternion();
				this->browserName = std::string();
				this->browserURL = std::string();
				this->browserPosition = Ape::Vector3();
				this->browserOrientation = Ape::Quaternion();
				this->browserWidth = int();
				this->browserHeight = int();
				this->browserZoom = int();
				this->browserResolutionVertical = int();
				this->browserResolutionHorizontal = int();
			}

			StoryElement(
				Ape::Vector3 cameraPosition,
				Ape::Quaternion cameraOrientation,
				std::string browserName = std::string(),
				std::string browserURL = std::string(),
				Ape::Vector3 browserPosition = Ape::Vector3(),
				Ape::Quaternion browserOrientation = Ape::Quaternion(),
				int browserWidth = int(),
				int browserHeight = int(),
				int browserZoom = 0,
				int browserResolutionVertical = 1920,
				int browserResolutionHorizontal = 1080
			)
			{
				this->cameraPosition = cameraPosition;
				this->cameraOrientation = cameraOrientation;
				this->browserName = browserName;
				this->browserURL = browserURL;
				this->browserPosition = browserPosition;
				this->browserOrientation = browserOrientation;
				this->browserWidth = browserWidth;
				this->browserHeight = browserHeight;
				this->browserZoom = browserZoom;
				this->browserResolutionVertical = browserResolutionVertical;
				this->browserResolutionHorizontal = browserResolutionHorizontal;
			}
		};

		Ape::IEventManager* mpEventManager;

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::NodeWeakPtr mUserNode;

		Ape::CameraWeakPtr mCamera;

		void eventCallBack(const Ape::Event& event);

		Ape::Vector3 mOldXMLFormatTranslateVector;

		Ape::Vector3 mOldXMLFormatTranslateVectorCamera;

		Ape::Quaternion mOldXMLFormatRotationQuaternion;

		int mCurrentStoryElementIndex;

		std::vector<StoryElement> mStoryElements;

		float mTranslateSpeedFactor;

		float mRotateSpeedFactor;

		float mHumanHeight;

		float mHumanXoffset;

		float mHumanZoffset;

		std::map<OIS::KeyCode, bool> mKeyCodeMap;

		OIS::Keyboard* mpKeyboard;

		OIS::Mouse* mpMouse;

		std::map<std::string, Ape::BrowserWeakPtr> mBrowsers;

		std::map<std::string, Ape::UnitTextureWeakPtr> mGeometriesMouseTextures;

		std::map<std::string, Ape::BrowserWeakPtr> mBrowserMouseTextures;

		Ape::UnitTextureWeakPtr mActiveMouseTexture;

		Ape::BrowserWeakPtr mActiveBrowser;

		Ape::BrowserWeakPtr mOverlayBrowser;

		Ape::UnitTextureWeakPtr mOverlayMouseTexture;

		Ape::ManualMaterialWeakPtr mOverlayMouseMaterial;

		Ape::RayGeometryWeakPtr mRayGeometry;

		Ape::NodeWeakPtr mRayOverlayNode;

		clock_t mLastLeftClickTime;

		Ape::Vector3 mUserNodePositionBeforeFullScreen;

		Ape::Quaternion mUserNodeOrientationBeforeFullScreen;

		Context mContext;

		bool mIsFirstSpacePressed;

		void moveUserNode();

		void saveUserNodePose(Ape::NodeSharedPtr userNode);

		void animateToStoryElements(Ape::NodeSharedPtr userNode);

		void jumpToStoryElement(Ape::NodeSharedPtr userNode);

		void manageBrowser(StoryElement storyElement);

		void createMesh(std::string name, Ape::Vector3 position = Ape::Vector3(), Ape::Quaternion orientation = Ape::Quaternion());

	public:
		ApePresentationScenePlugin();

		~ApePresentationScenePlugin();

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

	APE_PLUGIN_FUNC Ape::IPlugin* CreateApePresentationScenePlugin()
	{
		return new Ape::ApePresentationScenePlugin;
	}

	APE_PLUGIN_FUNC void DestroyApePresentationScenePlugin(Ape::IPlugin *plugin)
	{
		delete (Ape::ApePresentationScenePlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		LOG(LOG_TYPE_DEBUG, THIS_PLUGINNAME << "_CREATE");
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateApePresentationScenePlugin, DestroyApePresentationScenePlugin);
		return 0;
	}
}

#endif
