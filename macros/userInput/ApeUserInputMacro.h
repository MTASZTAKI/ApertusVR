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
#include <list>
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeILight.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "sceneelements/ApeIConeGeometry.h"
#include "sceneelements/ApeIFileMaterial.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeIBrowser.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeIRayGeometry.h"
#include "sceneelements/ApeIPointCloud.h"
#include "sceneelements/ApeIUnitTexture.h"
#include "managers/ApeISystemConfig.h"
#include "utils/ApeInterpolator.h"

namespace Ape
{
    class APE_USERINPUTMACRO_DLL_EXPORT UserInputMacro
    {
	public: 
		struct ViewPose
		{
			Ape::Vector3 userPosition;

			Ape::Quaternion userOrientation;

			Ape::Vector3 headPosition;

			Ape::Quaternion headOrientation;

			ViewPose() {}

			ViewPose(Ape::Vector3 userPosition, Ape::Quaternion userOrientation, Ape::Vector3 headPosition, Ape::Quaternion headOrientation)
			{
				this->userPosition = userPosition;
				this->userOrientation = userOrientation;
				this->headPosition = headPosition;
				this->headOrientation = headOrientation;
			}
		};

		struct Pose
		{
			Ape::Vector3 position;

			Ape::Quaternion orientation;

			Ape::Vector3 scale;

			Pose() {}

			Pose(Ape::Vector3 position, Ape::Quaternion orientation, Ape::Vector3 scale)
			{
				this->position = position;
				this->orientation = orientation;
				this->scale = scale;
			}
		};

		struct OverlayBrowserCursor
		{
			Ape::Vector2 cursorTexturePosition;

			Ape::Vector2 cursorBrowserPosition;

			Ape::Vector2 cursorScrollPosition;

			bool cursorClick;

			Ape::Browser::MouseClick cursorClickType;

			OverlayBrowserCursor() {}

			OverlayBrowserCursor(Ape::Vector2 cursorTexturePosition, Ape::Vector2 cursorBrowserPosition, Ape::Vector2 cursorScrollPosition, bool cursorClick, Ape::Browser::MouseClick cursorClickType)
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

		UserInputMacro();

		~UserInputMacro();

		Ape::IEventManager* mpEventManager;

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::NodeWeakPtr mUserNode;

		std::string mUniqueUserNodeName;

		Ape::NodeWeakPtr mHeadNode;

		Ape::TextGeometryWeakPtr mOverlayText;

		std::map<std::string, Ape::CameraWeakPtr> mCameras;

		Ape::ManualMaterialWeakPtr mUserMaterial;

		std::map<std::string, Ape::NodeWeakPtr> mSelectedNodes;

		Ape::BrowserWeakPtr mOverlayBrowser;

		Ape::UnitTextureWeakPtr mOverlayMouseTexture;

		Ape::RayGeometryWeakPtr mRayGeometry;

		Ape::NodeWeakPtr mRayOverlayNode;

		Ape::TextGeometryWeakPtr mCursorText;

		std::string mKeyStringValue;

		bool mEnableOverlayBrowserKeyEvents;

		bool mIsNewKeyEvent;

	public:
		static UserInputMacro* getSingletonPtr()
		{
			if (mpInstance == nullptr)
				mpInstance = new UserInputMacro();
			return mpInstance;
		}

		void eventCallBack(const Ape::Event& event);

		void updateViewPose(ViewPose pose);

		void interpolateViewPose(ViewPose pose, unsigned int milliseconds);

		Ape::NodeWeakPtr getUserNode();

		Ape::NodeWeakPtr getHeadNode();

		Ape::CameraWeakPtr createCamera(std::string name);

		void createOverLayText(std::string caption);

		void updateOverLayText(std::string caption);

		void saveViewPose();

		bool isNodeSelected(std::string nodeName);

		void addNodeSelection(std::string nodeName);

		bool removeNodeSelection(std::string nodeName);

		void clearNodeSelection();

		void keyStringValue(std::string keyStringValue);

		void updateSelectedNodePose(Pose pose);

		void updateOverLayBrowserCursor(OverlayBrowserCursor overlayBrowserCursor);

		void rayQuery(Ape::Vector3 position);
    };
}

#endif
