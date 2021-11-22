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
#include "apeVLFTAnimationPlayerPluginConfig.h"
#define APE_DOUBLEQUEUE_UNIQUE
#include "apeDoubleQueue.h"
#include <imgui.h>


//#include "generated/resources/gltf_viewer.h"

//Needs scene
//swapshain
//lightmanager
//sunlight

struct VLFTMainMenuInfo{
    int current_selected = -1;
    int sideBarWidth = 300;
    int prevStateNum = 0;
    std::vector<std::string> roomNames;
    std::vector<bool> running_rooms;
    char uploadRoomName[128];
    bool admin = false;
    bool singlePlayer = false;
    bool loginMenu = true;
    bool mainMenu = false;
    bool adminMenu = false;
    bool settingsMenu = false;
    bool multiPlayer = false;
    bool namesLoaded = false;
    bool inRoomGui = false;
    bool uploadRoomGui = false;
    bool showStates = false;
    bool inSinglePlayerMode = false;
    float cameraSpeed = 1.0f;
};
struct updateInfo{
    std::map<std::string, std::string> keyLabel = std::map<std::string, std::string>();
    std::map<std::string, std::string> animationLinks = std::map<std::string, std::string>();
    std::map<std::string, float[5]> playerNamePositions = std::map<std::string, float[5]>();
    std::vector<std::string> stateOfObjects = std::vector<std::string>();
    std::vector<std::string> nameOfState = std::vector<std::string>();
    std::vector<std::string> newMessage = std::vector<std::string>();
    std::string userName = "";
    std::string password = "";
    std::string messageToSend ="";
    std::string changeKeyCode = "";
    std::string selectedItem = "";
    std::string rootOfSelected = "";
    std::string pickedItem = "";
    ape::Vector3 position;
    ape::Quaternion orientation;
    std::vector<bool> playedAnimation = std::vector<bool>();
    bool deleteSelected = false;
    bool pickUp = false;
    bool drop = false;
    bool isAdmin = false;
    bool IsPauseClicked = false;
    bool IsStopClicked = false;
    bool IsPlayClicked = false;
    bool setPosition = false;
    bool setRotation = false;
    bool rotate = false;
    bool inRoom = false;
    bool inSettings = false;
    bool sendMessage = false;
    bool newChatMessage = false;
    bool leftRoom = false;
    bool isPlayRunning = false;
    bool changedKey = false;
    bool checkLogin = false;
    bool logedIn = false;
    bool attachUsers = false;
    bool usersAttached = false;
    bool logMovements = false;
    bool mIsStudentsMovementLogging = false;
    bool setUpRoom = false;
    bool screenCast = false;
    bool takeScreenshot = false;
    bool screenCaptureOn = false;
    bool stoppingScreenCast = false;
    bool wrongPassword = false;
    bool switchOwner = false;
    bool leaveWait = false;
    bool callLeave = false;
    bool initRun = false;
    bool resourcesUpdated = false;
    bool inSinglePlayer = false;
    bool spaghettiVisible = false;
    bool isMapVisible = true;
    bool lightOn = true;
    bool changeLightDir = false;
    double StartTime = -1.0;
    double pauseTime = 0.0;
    double leaveTime = 30.0;
    double now = 0;
    int ChoosedBookmarkedAnimationID = 0;
    int BookmarkID = -1;
    unsigned long long ClickedBookmarkTime = 0;
    float TimeToSleepFactor = 1.0f;
    std::vector<float> timeOfState= std::vector<float>();
    std::vector<float> lightDirection = { 0.7, -1, -0.8 };
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
        
        ImFont* usernameFont;

        VLFTMainMenuInfo mpMainMenuInfo;
        
        ape::ISceneNetwork* mpSceneNetwork;
        
        ape::UserInputMacro* mpUserInputMacro;
        
        ape::ICoreConfig* mpCoreConfig;
        
        ape::IPluginManager* mpPluginManager;
        
        ape::ISceneManager* mpSceneManager;
        
        quicktype::Scene mScene;

        std::string mUploadScene;

        std::string mUploadAnimations;

        std::string mUploadApeCore;
        
        FILE* mApeVLFTSceneLoaderPluginConfigFile;
        
        updateInfo* mpUpdateInfo;
        
        double posX=0, posY=0, posZ = 0, rotAngle = 0, upX = 0, upY = 0, upZ = 0;
        
        std::vector<std::string> chatMessages;
        
        std::map<std::string, std::string> statesMap;
        
        std::string mPreChosenRoomName;
        
        bool messageInBuffer = false;
        
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        
        void tooltipFloat(std::string tooltipText);
        
        void inRoomGUI();
        
        void adminRoomGUI();
        
        void loginGUI();

        void studentRoomGUI();
        
        void leftPanelGUI();
        
        void infoPanelGUI();
        
        void statePanelGUI();
        
        void singlePlayerRoomGUI();
        
        void animationPanelGUI();
        
        void screenshotPanelGUI();
        
        void manipulatorPanelGUI();
        
        void studentPanelGUI();

        void drawUserNames();
        
        bool curlData();
        
        void uploadGUI(int width, int height);
        
        void createStartButton(int width, int height);
        
        void createStopButton(int width, int height);
        
        bool createSettingsMenu(int width, int height);
        
        void createJoinButton(std::string userType,int width, int height);
        
        void listRoomNames(bool withState);
        
        void connectToRoom();
        
        void openFileBrowser();
        
        void waitWindow();
        
        void updateResources();
        
        void getInfoAboutObject(float width, float height);
        
        bool downloadConfig(std::string url, std::string location);
        
	};
}

#endif
