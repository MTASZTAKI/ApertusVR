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


#ifndef APE_PRESENATITONSCENEPLUGIN_H
#define APE_PRESENATITONSCENEPLUGIN_H

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeIScene.h"
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
#include "ApeInterpolator.h"
#include "ApeEuler.h"
#include "OIS.h"


#define THIS_PLUGINNAME "ApePresentationScenePlugin"

class ApePresentationScenePlugin : public Ape::IPlugin, public OIS::KeyListener
{
private:
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

		StoryElement(
			Ape::Vector3 cameraPosition,
			Ape::Quaternion cameraOrientation,
			std::string browserName = std::string(),
			std::string browserURL = std::string(),
			Ape::Vector3 browserPosition = Ape::Vector3(),
			Ape::Quaternion browserOrientation = Ape::Quaternion(),
			int browserWidth = int(),
			int browserHeight = int()
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
		}
	};

	Ape::IEventManager* mpEventManager;

	Ape::IScene* mpScene;

	Ape::ISystemConfig* mpSystemConfig;

	Ape::IMainWindow* mpMainWindow;

	Ape::NodeWeakPtr mUserNode;
	
	void eventCallBack(const Ape::Event& event);

	Ape::Vector3 mOldXMLFormatTranslateVector;

	Ape::Vector3 mOldXMLFormatTranslateVectorCamera;
	
	Ape::Quaternion mOldXMLFormatRotationQuaternion;

	int mCurrentStoryElementIndex;

	std::vector<StoryElement> mStoryElements;

	float mTranslateSpeedFactor;

	float mRotateSpeedFactor;

	std::map<OIS::KeyCode, bool> mKeyCodeMap;

	OIS::Keyboard* mpKeyboard;

	void moveUserNode();

	void animateToStoryElements(Ape::NodeSharedPtr userNode);

	void jumpToStoryElement(Ape::NodeSharedPtr userNode);

	void createBrowser(std::string name, std::string url, Ape::Vector3 position, Ape::Quaternion orientation, int width, int height, int resolutionVertical = 1024, int resolutionHorizontal = 768);

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

};

APE_PLUGIN_FUNC Ape::IPlugin* CreateApePresentationScenePlugin()
{
	return new ApePresentationScenePlugin;
}

APE_PLUGIN_FUNC void DestroyApePresentationScenePlugin(Ape::IPlugin *plugin)
{
	delete (ApePresentationScenePlugin*)plugin;
}

APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

APE_PLUGIN_ALLOC()
{
	std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
	ApeRegisterPlugin(THIS_PLUGINNAME, CreateApePresentationScenePlugin, DestroyApePresentationScenePlugin);
	return 0;
}

#endif
