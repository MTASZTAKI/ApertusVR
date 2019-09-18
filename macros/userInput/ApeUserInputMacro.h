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

#ifndef APE_USERINPUTMACRO_H
#define APE_USERINPUTMACRO_H

#ifdef _WIN32
#ifdef BUILDING_APE_USERINPUTMACRO_DLL
#define APE_USERINPUTMACRO_DLL_EXPORT __declspec(dllexport)
#else
#define APE_USERINPUTMACRO_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_USERINPUTMACRO_DLL_EXPORT 
#endif

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <memory>
#include <mutex>
#include <list>
#include "plugin/apePluginAPI.h"
#include "managers/apeIEventManager.h"
#include "managers/apeILogManager.h"
#include "managers/apeISceneManager.h"
#include "sceneelements/apeINode.h"
#include "sceneelements/apeILight.h"
#include "sceneelements/apeITextGeometry.h"
#include "sceneelements/apeIFileGeometry.h"
#include "sceneelements/apeIConeGeometry.h"
#include "sceneelements/apeIFileMaterial.h"
#include "sceneelements/apeIManualMaterial.h"
#include "sceneelements/apeIBrowser.h"
#include "sceneelements/apeICamera.h"
#include "sceneelements/apeIRayGeometry.h"
#include "sceneelements/apeIPointCloud.h"
#include "sceneelements/apeIUnitTexture.h"
#include "managers/apeICoreConfig.h"
#include "utils/apeInterpolator.h"

namespace ape
{
    class APE_USERINPUTMACRO_DLL_EXPORT UserInputMacro
    {
	public: 
		struct ViewPose
		{
			ape::Vector3 userPosition;

			ape::Quaternion userOrientation;

			ape::Vector3 headPosition;

			ape::Quaternion headOrientation;

			ViewPose() {}

			ViewPose(ape::Vector3 userPosition, ape::Quaternion userOrientation, ape::Vector3 headPosition, ape::Quaternion headOrientation)
			{
				this->userPosition = userPosition;
				this->userOrientation = userOrientation;
				this->headPosition = headPosition;
				this->headOrientation = headOrientation;
			}
		};

		struct Pose
		{
			ape::Vector3 position;

			ape::Quaternion orientation;

			ape::Vector3 scale;

			Pose() 
			{
				this->position = ape::Vector3(0, 0, 0);
				this->orientation = ape::Quaternion(1, 0, 0, 0);
				this->scale = ape::Vector3(1, 1, 1);
			}

			Pose(ape::Vector3 position, ape::Quaternion orientation, ape::Vector3 scale)
			{
				this->position = position;
				this->orientation = orientation;
				this->scale = scale;
			}
		};

		struct OverlayBrowserCursor
		{
			ape::Vector2 cursorTexturePosition;

			ape::Vector2 cursorBrowserPosition;

			ape::Vector2 cursorScrollPosition;

			bool cursorClick;

			ape::Browser::MouseClick cursorClickType;

			OverlayBrowserCursor() {}

			OverlayBrowserCursor(ape::Vector2 cursorTexturePosition, ape::Vector2 cursorBrowserPosition, ape::Vector2 cursorScrollPosition, bool cursorClick, ape::Browser::MouseClick cursorClickType)
			{
				this->cursorTexturePosition = cursorTexturePosition;
				this->cursorBrowserPosition = cursorBrowserPosition;
				this->cursorScrollPosition = cursorScrollPosition;
				this->cursorClick = cursorClick;
				this->cursorClickType = cursorClickType;
			}
		};

	private:
		static UserInputMacro* mpInstance;

		static std::mutex mpInstanceMutex;

		UserInputMacro();

		~UserInputMacro();

		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		ape::NodeWeakPtr mUserNode;

		std::string mUserNodeName;

		ape::NodeWeakPtr mHeadNode;

		ape::TextGeometryWeakPtr mOverlayText;

		ape::NodeWeakPtr mOverlayTextNode;

		std::map<std::string, ape::CameraWeakPtr> mCameras;

		ape::ManualMaterialWeakPtr mUserMaterial;

		ape::BrowserWeakPtr mOverlayBrowser;

		ape::ManualMaterialWeakPtr mOverlayBrowserMaterial;

		ape::UnitTextureWeakPtr mOverlayMouseTexture;

		ape::RayGeometryWeakPtr mRayGeometry;

		ape::NodeWeakPtr mRayOverlayNode;

		ape::TextGeometryWeakPtr mCursorText;

		bool mIsLockHeadNodePosition;

		bool mIsLockUserNodePosition;

		std::vector<std::function<void(const std::string&)>> mKeyPressedStringFunctions;

		std::vector<std::function<void(const std::string&)>> mKeyReleasedStringFunctions;

		std::vector<std::function<void(const std::string&)>> mMousePressedStringFunctions;

		std::vector<std::function<void(const std::string&)>> mMouseReleasedStringFunctions;

		std::vector<std::function<void(const ape::Vector2&, const ape::Vector2&)>> mMouseMovedFunctions;

		std::vector<std::function<void(const int&)>> mMouseScrolledFunctions;

		std::vector<std::function<void(const ape::Vector2&)>> mControllerTouchpadPressedValueFunctions;

		std::vector<std::function<void(const ape::Vector2&)>> mControllerTouchpadReleasedValueFunctions;

		std::vector<std::function<void(const std::string&)>> mControllerButtonReleasedStringFunctions;

		std::vector<std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)>> mControllerMovedFunctions;

		std::vector<std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)>> mHmdMovedFunctions;

	public:
		static UserInputMacro* getSingletonPtr()
		{
			mpInstanceMutex.lock();
			if (mpInstance == nullptr)
				mpInstance = new UserInputMacro();
			mpInstanceMutex.unlock();
			return mpInstance;
		}

		void eventCallBack(const ape::Event& event);

		void interpolateViewPose(ViewPose pose, unsigned int milliseconds);

		ape::NodeWeakPtr getUserNode();

		ape::NodeWeakPtr getHeadNode();

		ape::CameraWeakPtr createCamera(std::string name);

		void createOverLayText(std::string caption);

		void updateOverLayText(std::string caption);

		void updateOverLayTextPose(ape::UserInputMacro::Pose pose);

		void saveViewPose();

		void keyPressedStringValue(std::string keyPressedStringValue);

		void keyReleasedStringValue(std::string keyReleasedStringValue);

		void mousePressedStringValue(std::string mousePressedStringValue);

		void mouseReleasedStringValue(std::string mouseReleasedStringValue);

		void mouseMovedValue(ape::Vector2 mouseMovedValueRel, ape::Vector2 mouseMovedValueAbs);

		void mouseScrolledValue(int mouseSrcolledValue);

		void controllerTouchpadPressedValue(ape::Vector2 axis);

		void controllerTouchpadReleasedValue(ape::Vector2 axis);

		void controllerButtonPressedStringValue(std::string controllerButtonPressedStringValue);

		void controllerMovedValue(ape::Vector3 controllerMovedValuePos, ape::Quaternion controllerMovedValueOri, ape::Vector3 controllerMovedValueScl);

		void hmdMovedValue(ape::Vector3 hmdMovedValuePos, ape::Quaternion hmdMovedValueOri, ape::Vector3 hmdMovedValueScl);

		void registerCallbackForKeyPressedStringValue(std::function<void(const std::string&)> callback);

		void registerCallbackForKeyReleasedStringValue(std::function<void(const std::string&)> callback);

		void registerCallbackForMousePressedStringValue(std::function<void(const std::string&)> callback);

		void registerCallbackForMouseReleasedStringValue(std::function<void(const std::string&)> callback);

		void registerCallbackForMouseMovedValue(std::function<void(const ape::Vector2&, const ape::Vector2&)> callback);

		void registerCallbackForMouseScrolledValue(std::function<void(const int&)> callback);

		void registerCallbackForControllerTouchpadPressedValue(std::function<void(const ape::Vector2&)> callback);

		void registerCallbackForControllerTouchpadReleasedValue(std::function<void(const ape::Vector2&)> callback);

		void registerCallbackForControllerButtonPressedStringValue(std::function<void(const std::string&)> callback);

		void registerCallbackForControllerMovedValue(std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)> callback);

		void registerCallbackForHmdMovedValue(std::function<void(const ape::Vector3&, const ape::Quaternion&, const ape::Vector3&)> callback);

		void updateOverLayBrowserCursor(OverlayBrowserCursor overlayBrowserCursor);

		void showOverlayBrowser(bool show);

		bool isOverlayBrowserShowed();

		void setOverlayBrowserURL(std::string url);

		std::string getOverlayBrowserURL();

		void rayQuery(ape::Vector3 position);

		ape::RayGeometryWeakPtr getRayGeometry();
    };
}

#endif
