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
    bool settingsMenu = false;
    bool multiPlayer = false;
    bool namesLoaded = false;
    bool inRoomGui = false;
    float cameraSpeed = 1.0f;
    bool showStates = false;
};
struct updateInfo{
    std::string selectedItem = "";
    std::string rootOfSelected = "";
    ape::Vector3 position;
    ape::Quaternion orientation;
    std::vector<bool> playedAnimation;
    bool deleteSelected = false;
    bool pickUp = false;
    bool drop = false;
    std::string pickedItem = "";
    bool isAdmin = false;
    double now = 0;
    bool isPlayRunning = false;
    double StartTime = -1.0;
    double pauseTime = 0.0;
    int ChoosedBookmarkedAnimationID = 0;
    unsigned long long ClickedBookmarkTime = 0;
    int BookmarkID = -1;
    float TimeToSleepFactor = 1.0f;
    bool IsPauseClicked = false;
    bool IsStopClicked = false;
    bool IsPlayClicked = false;
    bool setPosition = false;
    bool setRotation = false;
    bool inRoom = false;
    bool inSettings = false;
    bool sendMessage = false;
    bool newChatMessage = false;
    std::vector<std::string> newMessage;
    std::string messageToSend ="";
    std::string changeKeyCode = "";
    bool changedKey = 0;
    std::map<std::string, std::string> keyLabel;
    std::vector<std::string> stateOfObjects;
    std::vector<std::string> nameOfState;
    std::vector<float> timeOfState;
};

namespace ape
{


	class VLFTImgui
	{
	public:

        VLFTImgui();

		~VLFTImgui();
        
        void init(updateInfo *updateinfo);
        
        void update();

	private:
        
        VLFTMainMenuInfo mpMainMenuInfo;
        
        ape::ISceneNetwork* mpSceneNetwork;
        
        ape::UserInputMacro* mpUserInputMacro;
        
        ape::ICoreConfig* mpCoreConfig;
        
        ape::IPluginManager* mpPluginManager;
        
        ape::ISceneManager* mpSceneManager;
        
        quicktype::Scene mScene;
        
        FILE* mApeVLFTSceneLoaderPluginConfigFile;
        
        updateInfo* mpUpdateInfo;
        
        double posX=0, posY=0, posZ = 0, rotAngle = 0, upX = 0, upY = 0, upZ = 0;
        
        std::vector<std::string> chatMessages;
        
        bool messageInBuffer = false;
        
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        
        void tooltipFloat(std::string tooltipText);
        
        void inRoomGUI();
        
        void adminRoomGUI();
        
        void studentRoomGUI();
        
        void leftPanelGUI();
        
        void rightPanelGUI();
        
        void statePanelGUI();
        
        void singlePlayerRoomGUI();
        
        bool curlData();
        
        void uploadGUI();
        
        void createStartButton(int width, int height);
        
        void createStopButton(int width, int height);
        
        bool createSettingsMenu(int width, int height);
        
        void createJoinButton(std::string userType,int width, int height);
        
        void listRoomNames(bool withState);
        
        void connectToRoom();
        
        void openFileBrowser();
        
        void getInfoAboutObject(float width, float height);
        
        
	};
}

#endif
