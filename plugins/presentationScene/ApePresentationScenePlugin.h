///*MIT License
//
//Copyright (c) 2018 MTA SZTAKI
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.*/
//
//#ifndef APE_PRESENATITONSCENEPLUGIN_H
//#define APE_PRESENATITONSCENEPLUGIN_H
//
//#include <chrono>
//#include <fstream>
//#include <iostream>
//#include <memory>
//#include <mutex>
//#include <thread>
//#include <vector>
//#include "ape.h"
//#include "system/apeIMainWindow.h"
//#include "plugin/apePluginAPI.h"
//#include "managers/apeIEventManager.h"
//#include "managers/apeILogManager.h"
//#include "managers/apeISceneManager.h"
//#include "managers/apeICoreConfig.h"
//#include "datatypes/apeEuler.h"
//#include "sceneelements/apeIBrowser.h"
//#include "sceneelements/apeICamera.h"
//#include "sceneelements/apeIConeGeometry.h"
//#include "sceneelements/apeIFileGeometry.h"
//#include "sceneelements/apeIFileMaterial.h"
//#include "sceneelements/apeIIndexedFaceSetGeometry.h"
//#include "sceneelements/apeIIndexedLineSetGeometry.h"
//#include "sceneelements/apeILight.h"
//#include "sceneelements/apeIManualMaterial.h"
//#include "sceneelements/apeIManualPass.h"
//#include "sceneelements/apeINode.h"
//#include "sceneelements/apeIPbsPass.h"
//#include "sceneelements/apeIPlaneGeometry.h"
//#include "sceneelements/apeIRayGeometry.h"
//#include "sceneelements/apeISphereGeometry.h"
//#include "sceneelements/apeITextGeometry.h"
//#include "sceneelements/apeITubeGeometry.h"
//#include "sceneelements/apeIUnitTexture.h"
//#include "utils/apeInterpolator.h"
//#include "sceneelements/apeIRayGeometry.h"
//#include "OIS.h"
//
//#define THIS_PLUGINNAME "apePresentationScenePlugin"
//
//namespace ape
//{
//	class apePresentationScenePlugin : public ape::IPlugin, public OIS::KeyListener, public OIS::MouseListener
//	{
//	private:
//		enum Context
//		{
//			UNKOWN,
//			SPACE,
//			BROWSER,
//			INVALID
//		};
//		struct StoryElement
//		{
//			ape::Vector3 cameraPosition;
//			ape::Quaternion cameraOrientation;
//			std::string browserName;
//			std::string browserURL;
//			ape::Vector3 browserPosition;
//			ape::Quaternion browserOrientation;
//			int browserWidth;
//			int browserHeight;
//			int browserZoom;
//			int browserResolutionVertical;
//			int browserResolutionHorizontal;
//
//			StoryElement()
//			{
//				this->cameraPosition = ape::Vector3();
//				this->cameraOrientation = ape::Quaternion();
//				this->browserName = std::string();
//				this->browserURL = std::string();
//				this->browserPosition = ape::Vector3();
//				this->browserOrientation = ape::Quaternion();
//				this->browserWidth = int();
//				this->browserHeight = int();
//				this->browserZoom = int();
//				this->browserResolutionVertical = int();
//				this->browserResolutionHorizontal = int();
//			}
//
//			StoryElement(
//				ape::Vector3 cameraPosition,
//				ape::Quaternion cameraOrientation,
//				std::string browserName = std::string(),
//				std::string browserURL = std::string(),
//				ape::Vector3 browserPosition = ape::Vector3(),
//				ape::Quaternion browserOrientation = ape::Quaternion(),
//				int browserWidth = int(),
//				int browserHeight = int(),
//				int browserZoom = 0,
//				int browserResolutionVertical = 1920,
//				int browserResolutionHorizontal = 1080
//			)
//			{
//				this->cameraPosition = cameraPosition;
//				this->cameraOrientation = cameraOrientation;
//				this->browserName = browserName;
//				this->browserURL = browserURL;
//				this->browserPosition = browserPosition;
//				this->browserOrientation = browserOrientation;
//				this->browserWidth = browserWidth;
//				this->browserHeight = browserHeight;
//				this->browserZoom = browserZoom;
//				this->browserResolutionVertical = browserResolutionVertical;
//				this->browserResolutionHorizontal = browserResolutionHorizontal;
//			}
//		};
//
//		ape::IEventManager* mpEventManager;
//
//		ape::ISceneManager* mpSceneManager;
//
//		ape::ICoreConfig* mpCoreConfig;
//
//		ape::IMainWindow* mpMainWindow;
//
//		ape::NodeWeakPtr mUserNode;
//
//		ape::CameraWeakPtr mCamera;
//
//		void eventCallBack(const ape::Event& event);
//
//		ape::Vector3 mOldXMLFormatTranslateVector;
//
//		ape::Vector3 mOldXMLFormatTranslateVectorCamera;
//
//		ape::Quaternion mOldXMLFormatRotationQuaternion;
//
//		int mCurrentStoryElementIndex;
//
//		std::vector<StoryElement> mStoryElements;
//
//		float mTranslateSpeedFactor;
//
//		float mRotateSpeedFactor;
//
//		float mHumanHeight;
//
//		float mHumanXoffset;
//
//		float mHumanZoffset;
//
//		std::map<OIS::KeyCode, bool> mKeyCodeMap;
//
//		OIS::Keyboard* mpKeyboard;
//
//		OIS::Mouse* mpMouse;
//
//		std::map<std::string, ape::BrowserWeakPtr> mBrowsers;
//
//		std::map<std::string, ape::UnitTextureWeakPtr> mGeometriesMouseTextures;
//
//		std::map<std::string, ape::BrowserWeakPtr> mBrowserMouseTextures;
//
//		ape::UnitTextureWeakPtr mActiveMouseTexture;
//
//		ape::BrowserWeakPtr mActiveBrowser;
//
//		ape::BrowserWeakPtr mOverlayBrowser;
//
//		ape::UnitTextureWeakPtr mOverlayMouseTexture;
//
//		ape::ManualMaterialWeakPtr mOverlayMouseMaterial;
//
//		ape::RayGeometryWeakPtr mRayGeometry;
//
//		ape::NodeWeakPtr mRayOverlayNode;
//
//		clock_t mLastLeftClickTime;
//
//		ape::Vector3 mUserNodePositionBeforeFullScreen;
//
//		ape::Quaternion mUserNodeOrientationBeforeFullScreen;
//
//		Context mContext;
//
//		bool mIsFirstSpacePressed;
//
//		void moveUserNode();
//
//		void saveUserNodePose(ape::NodeSharedPtr userNode);
//
//		void animateToStoryElements(ape::NodeSharedPtr userNode);
//
//		void jumpToStoryElement(ape::NodeSharedPtr userNode);
//
//		void manageBrowser(StoryElement storyElement);
//
//		void createMesh(std::string name, ape::Vector3 position = ape::Vector3(), ape::Quaternion orientation = ape::Quaternion());
//
//	public:
//		apePresentationScenePlugin();
//
//		~apePresentationScenePlugin();
//
//		void Init() override;
//
//		void Run() override;
//
//		void Step() override;
//
//		void Stop() override;
//
//		void Suspend() override;
//
//		void Restart() override;
//
//		bool keyPressed(const OIS::KeyEvent& e) override;
//
//		bool keyReleased(const OIS::KeyEvent& e) override;
//
//		bool mouseMoved(const OIS::MouseEvent& e) override;
//
//		bool mousePressed(const OIS::MouseEvent& e, OIS::MouseButtonID id) override;
//
//		bool mouseReleased(const OIS::MouseEvent& e, OIS::MouseButtonID id) override;
//
//	};
//
//	APE_PLUGIN_FUNC ape::IPlugin* CreateapePresentationScenePlugin()
//	{
//		return new ape::apePresentationScenePlugin;
//	}
//
//	APE_PLUGIN_FUNC void DestroyapePresentationScenePlugin(ape::IPlugin *plugin)
//	{
//		delete (ape::apePresentationScenePlugin*)plugin;
//	}
//
//	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);
//
//	APE_PLUGIN_ALLOC()
//	{
//		APE_LOG_DEBUG(THIS_PLUGINNAME << "_CREATE");
//		apeRegisterPlugin(THIS_PLUGINNAME, CreateapePresentationScenePlugin, DestroyapePresentationScenePlugin);
//		return 0;
//	}
//}
//
//#endif
