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

#ifndef APE_VLFTIMGUI_H
#define APE_VLFTIMGUI_H

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include "apeIPlugin.h"
#include "apePluginAPI.h"
#include "apeIPluginManager.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeICoreConfig.h"
#include "apeIBoxGeometry.h"
#include "apeICamera.h"
#include "apeICloneGeometry.h"
#include "apeIConeGeometry.h"
#include "apeICylinderGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIFileMaterial.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeILight.h"
#include "apeIManualMaterial.h"
#include "apeIManualTexture.h"
#include "apeIPlaneGeometry.h"
#include "apeIPointCloud.h"
#include "apeIRayGeometry.h"
#include "apeISky.h"
#include "apeISphereGeometry.h"
#include "apeITextGeometry.h"
#include "apeITorusGeometry.h"
#include "apeITubeGeometry.h"
#include "apeIFileTexture.h"
#include "apeIUnitTexture.h"
#include "apeIWater.h"
#include "apeUserInputMacro.h"
#include "apeVLFTSceneLoaderPluginConfig.h"
#define APE_DOUBLEQUEUE_UNIQUE
#include "apeDoubleQueue.h"
#include <imgui.h>
#include "nfd.h"


//#include "generated/resources/gltf_viewer.h"

//Needs scene
//swapshain
//lightmanager
//sunlight

struct VLFTMainMenuInfo{
    int current_selected = -1;
    std::vector<bool> running_rooms;
    int sideBarWidth = 300;
    char uploadRoomName[128];
    std::vector<std::string> roomNames;
    bool admin = false;
    bool student = false;
    bool singlePlayer = false;
    bool inMainMenu = true;
    bool adminMenu = false;
    bool multiPlayer = false;
    bool namesLoaded = false;
    bool inRoomGui = false;
    float cameraSpeed = 1.0f;
};
struct updateInfo{
    std::string selectedItem = "";
    std::string rootOfSelected = "";
    std::vector<float> position[3];
    std::vector<float> orientation[3];
    bool deleteSelected = false;
    bool pickUp = false;
    std::string pickedItem = "";
    bool isAdmin = false;
    
};

namespace ape
{


	class VLFTImgui
	{
	public:
        VLFTImgui();

		~VLFTImgui();
        
        void init(updateInfo &updateinfo);
        
        void update();

	private:
        
        VLFTMainMenuInfo mpMainMenuInfo;
        
        ape::ISceneNetwork* mpSceneNetwork;
        ape::UserInputMacro* mpUserInputMacro;
        ape::ICoreConfig* mpCoreConfig;
        ape::IPluginManager* mpPluginManager;
        ape::ISceneManager* mpSceneManager;
        
        
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        
        void tooltipFloat(std::string tooltipText);
        updateInfo* mpUpdateInfo;
        void inRoomGUI();
        void adminRoomGUI();
        void studentRoomGUI();
        void leftPanelGUI();
        void rightPanelGUI();
        void singlePlayerRoomGUI();
        bool curlData();
        void uploadGUI();
        void createStartButton(int width, int height);
        void createStopButton(int width, int height);
        void createJoinButton(std::string userType,int width, int height);
        void listRoomNames(bool withState);
        void connectToRoom();
        void openFileBrowser();
        void getInfoAboutObject();
	};
}

#endif
