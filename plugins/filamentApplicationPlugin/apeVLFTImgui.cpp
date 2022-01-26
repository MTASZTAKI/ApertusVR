#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "apeVLFTImgui.h"
#include "NativeWindowHelper.h"
#include "nfd.h"
#ifdef __APPLE__
#include <sys/stat.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

bool gDownloadRemoteConfigFileFinished = false;

ape::VLFTImgui::VLFTImgui( )
{
    mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
    mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
    mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
    mpPluginManager = ape::IPluginManager::getSingletonPtr();
    mpSceneManager = ape::ISceneManager::getSingletonPtr();
    statesMap =  std::map<std::string, std::string>();
}

ape::VLFTImgui::~VLFTImgui()
{
}

void ape::VLFTImgui::updateResources(){
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(width-40, height-40));
    //ImGui::Begin("ResouceUpdate", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Begin("ResouceUpdate", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowFontScale(1.5);
    ImGui::SetCursorPos(ImVec2(width/2-150, height/2-90));
    ImGui::Text("Resources are being updated");
    ImGui::SetWindowFontScale(1);
    ImGui::End();
}

void ape::VLFTImgui::init(updateInfo *updateinfo)
{
//    std::stringstream fileFullPath;
//    fileFullPath << APE_SOURCE_DIR << "/samples/virtualLearningFactory/apeVLFTSceneLoaderPlugin.json";
    mpUpdateInfo = updateinfo;
    if(!mpUpdateInfo->resourcesUpdated)
        updateResources();
    mUploadScene = "";
    mUploadAnimations = "";
    mPreChosenRoomName = "";
    auto networkConfig =mpCoreConfig->getNetworkConfig();
    if(networkConfig.userName.find("VLFTAdmin") != std::string::npos){
        if(networkConfig.userName.find("_VLFTTeacher")!= std::string::npos){
            mpUpdateInfo->isAdmin = true;
        }
        else{
            mpUpdateInfo->isAdmin = false;
        }
        
        mPreChosenRoomName = networkConfig.lobbyConfig.roomName;
        if(networkConfig.selected == networkConfig.INTERNET){
            mpMainMenuInfo.multiPlayer = true;
            mpMainMenuInfo.singlePlayer = false;
        }
        else{
            mpMainMenuInfo.multiPlayer = false;
            mpMainMenuInfo.singlePlayer = true;
            mpUpdateInfo->inSinglePlayer = true;
        }
        mpMainMenuInfo.loginMenu = false;
        mpUpdateInfo->resourcesUpdated = true;
    }
    mpUpdateInfo->keyLabel["w"] = "w";
    mpUpdateInfo->keyLabel["s"] = "s";
    mpUpdateInfo->keyLabel["a"] = "a";
    mpUpdateInfo->keyLabel["d"] = "d";
    mpUpdateInfo->keyLabel["e"] = "e";
    mpUpdateInfo->keyLabel["q"] = "q";
    chatMessages.push_back("Write messages here for others to see");
    if(!mpUpdateInfo->resourcesUpdated){
        auto systemCommand = [this]() {
            mpSceneNetwork->updateResources();
            mpUpdateInfo->resourcesUpdated = true;
            if(mPreChosenRoomName != "")
                connectToRoom();
        };
        std::thread systemThread(systemCommand);
        systemThread.detach();
    }
    if(mPreChosenRoomName != "")
        connectToRoom();
    mpUpdateInfo->initRun = true;
}


void ape::VLFTImgui::tooltipFloat(std::string tooltipText){
    if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltipText.c_str());
    }
}
size_t ape::VLFTImgui::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void ape::VLFTImgui::listRoomNames(bool withState){
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    for (int n = 0; n < mpMainMenuInfo.roomNames.size(); n++)
    {
        if (ImGui::Selectable(mpMainMenuInfo.roomNames[n].c_str(), mpMainMenuInfo.current_selected == n, ImGuiSelectableFlags_AllowDoubleClick)){
            mpMainMenuInfo.current_selected = n;
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)){
                curlData();
                if (mpMainMenuInfo.current_selected != -1 && mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected])
                {
                    loadingRoomGUI();
                    auto systemCommand = [this]() {
                        mpUpdateInfo->resourcesUpdated = false;
                        mpUpdateInfo->userName;
                        mpSceneNetwork->updateRoomResources(mpUpdateInfo->userName + "/" + mpCoreConfig->getNetworkConfig().lobbyConfig.roomName);
                        mpUpdateInfo->resourcesUpdated = true;
                        mpMainMenuInfo.connectToRoom = true;
                    };
                    std::thread systemThread(systemCommand);
                    systemThread.detach();
                    mpUpdateInfo->loadingRoom = true;
                }
            }
        }
        if(withState){
            if( mpMainMenuInfo.running_rooms.size() > n && mpMainMenuInfo.running_rooms[n]){
                ImGui::SameLine(width-150); ImGui::Text("Online");
            }
            else{
                ImGui::SameLine(width-150); ImGui::Text("Offline");
            }
        }
    }
}

void ape::VLFTImgui::createStopButton(int width, int height){
    if (ImGui::Button("Stop",ImVec2(width/8, height/15))){
        if(mpMainMenuInfo.current_selected > -1 && mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected]){
        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) {
            std::string roomToStop = mpMainMenuInfo.roomNames[mpMainMenuInfo.current_selected];
            std::string postCommand = "command=stopRoom/:"+roomToStop;
            curl_easy_setopt(curl, CURLOPT_URL,  "195.111.1.138:8888");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postCommand.c_str());
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curlData();
        }
           
        }
    }
}

size_t downloadRemoteConfigFile(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    gDownloadRemoteConfigFileFinished = true;
    return written;
}

bool ape::VLFTImgui::downloadConfig(std::string url, std::string location){
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        APE_LOG_DEBUG("try to download from: " << url << " to: " << location);
        FILE* downloadedConfigFile = fopen(location.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadRemoteConfigFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, downloadedConfigFile);
        CURLcode configFileRes = curl_easy_perform(curl);
        if (configFileRes != CURLE_OK)
        {
            APE_LOG_DEBUG("curl_easy_perform() failed: " << curl_easy_strerror(configFileRes));
        }
        else
        {
            APE_LOG_DEBUG("curl_easy_perform() succes: " << curl_easy_strerror(configFileRes));
        }
        while (!gDownloadRemoteConfigFileFinished)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        fclose(downloadedConfigFile);
        gDownloadRemoteConfigFileFinished = false;
        return true;
    }
    return false;
}


void ape::VLFTImgui::connectToRoom(){
    std::string roomName;
    if(mPreChosenRoomName != ""){
        roomName = mPreChosenRoomName;
        std::string urlSceneConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
        std::string locationSceneConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
        std::string urlAnimationConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTAnimationPlayerPlugin.json";
        std::string locationAnimationConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTAnimationPlayerPlugin.json";
        if(mpMainMenuInfo.multiPlayer){
           
            downloadConfig(urlSceneConfig, locationSceneConfig);
            downloadConfig(urlAnimationConfig, locationAnimationConfig);
            mApeVLFTSceneLoaderPluginConfigFile = std::fopen(locationSceneConfig.c_str(), "r");
            mScene = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
            std::fclose(mApeVLFTSceneLoaderPluginConfigFile);
            std::vector<std::string> urls;
            std::vector<std::string> locations;
            urls.push_back(urlAnimationConfig);
            locations.push_back(locationAnimationConfig);
            mpSceneNetwork->connectToRoom(roomName, urls, locations);
            mpMainMenuInfo.inRoomGui = true;
            mpUpdateInfo->inRoom = true;
            mpUpdateInfo->resourcesUpdated = true;
            mpUpdateInfo->isMapVisible = true;
        }
        else if(mpMainMenuInfo.singlePlayer){
            mApeVLFTSceneLoaderPluginConfigFile = std::fopen(locationSceneConfig.c_str(), "r");
            mScene = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
            std::fclose(mApeVLFTSceneLoaderPluginConfigFile);
            
//            std::vector<std::string> urls;
//            std::vector<std::string> locations;
//            urls.push_back(urlSceneConfig);
//            locations.push_back(locationSceneConfig);
            //mpSceneNetwork->downloadConfigs(urls, locations);
            
            mpPluginManager->loadPlugin("apeVLFTSceneLoaderPlugin");
            mpMainMenuInfo.inRoomGui = true;
            mpMainMenuInfo.inSinglePlayerMode = true;
            mpUpdateInfo->inSinglePlayer = true;
            mpUpdateInfo->inRoom = true;
            mpUpdateInfo->resourcesUpdated = true;
        }
        
    }
    else{
        roomName= mpMainMenuInfo.roomNames[mpMainMenuInfo.current_selected];
        std::string urlSceneConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
        std::string locationSceneConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
        std::string urlAnimationConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTAnimationPlayerPlugin.json";
        std::string locationAnimationConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTAnimationPlayerPlugin.json";
        if((mpMainMenuInfo.multiPlayer || (mpMainMenuInfo.adminMenu && mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected])) && !mpMainMenuInfo.inRoomGui){
            
            downloadConfig(urlSceneConfig, locationSceneConfig);
            downloadConfig(urlAnimationConfig, locationAnimationConfig);
            mApeVLFTSceneLoaderPluginConfigFile = std::fopen(locationSceneConfig.c_str(), "r");
            mScene = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
            std::fclose(mApeVLFTSceneLoaderPluginConfigFile);
            
            std::vector<std::string> urls;
            std::vector<std::string> locations;
            urls.push_back(urlAnimationConfig);
            locations.push_back(locationAnimationConfig);
            mpSceneNetwork->connectToRoom(roomName, urls, locations);
            //mpSceneNetwork->
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            mpMainMenuInfo.inRoomGui = true;
            mpUpdateInfo->inRoom = true;
            mpUpdateInfo->isMapVisible = true;
            mpMainMenuInfo.inSinglePlayerMode = false;
        }
        else if(mpMainMenuInfo.singlePlayer){
            
            downloadConfig(urlSceneConfig, locationSceneConfig);
            downloadConfig(urlAnimationConfig, locationAnimationConfig);
            mApeVLFTSceneLoaderPluginConfigFile = std::fopen(locationSceneConfig.c_str(), "r");
            mScene = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
            std::fclose(mApeVLFTSceneLoaderPluginConfigFile);
            
            std::vector<std::string> urls;
            std::vector<std::string> locations;
            urls.push_back(urlSceneConfig);
            locations.push_back(locationSceneConfig);
            mpSceneNetwork->downloadConfigs(urls, locations);
            auto &networkConfig = mpCoreConfig->getNetworkConfig();
            networkConfig.lobbyConfig.roomName = roomName;
            mpPluginManager->loadPlugin("apeFilamentSceneLoaderPlugin");
            
            mpMainMenuInfo.inRoomGui = true;
            mpMainMenuInfo.inSinglePlayerMode = true;
            mpUpdateInfo->inRoom = true;
            mpUpdateInfo->inSinglePlayer = true;
        }
    }
    mpUpdateInfo->leftRoom = false;
    mpUpdateInfo->setUpRoom = true;
}

void ape::VLFTImgui::createJoinButton(std::string userType,int width, int height){
    if(mpMainMenuInfo.singlePlayer){
        if (ImGui::Button("Start",ImVec2(width/8, height/15))){
            if (mpMainMenuInfo.current_selected != -1) {
                loadingRoomGUI();
                auto systemCommand = [this]() {
                    mpUpdateInfo->resourcesUpdated = false;
                    mpSceneNetwork->updateRoomResources(mpUpdateInfo->userName + "/" + mpCoreConfig->getNetworkConfig().lobbyConfig.roomName);
                    mpUpdateInfo->resourcesUpdated = true;
                    mpMainMenuInfo.connectToRoom = true;
                };
                std::thread systemThread(systemCommand);
                systemThread.detach();
                mpUpdateInfo->loadingRoom = true;
            }
        }
    }
    else{
        if (ImGui::Button("Join",ImVec2(width/8, height/15))){
            curlData();
            if (mpMainMenuInfo.current_selected != -1 && mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected]) {
                loadingRoomGUI();
                auto systemCommand = [this]() {
                    mpUpdateInfo->resourcesUpdated = false;
                    mpSceneNetwork->updateRoomResources(mpUpdateInfo->userName + "/" + mpCoreConfig->getNetworkConfig().lobbyConfig.roomName);
                    mpUpdateInfo->resourcesUpdated = true;
                    mpMainMenuInfo.connectToRoom = true;
                };
                std::thread systemThread(systemCommand);
                systemThread.detach();
                mpUpdateInfo->loadingRoom = true;
            }
              
        }
    }
    
}
void ape::VLFTImgui::createStartButton(int width, int height){
    if (ImGui::Button("Start",ImVec2(width/8, height/15))){
        if(true || !mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected]){
        CURL *curl;
        CURLcode res;
        std::string readBuffer;

        curl = curl_easy_init();
        if(curl) {
            std::string roomToStart = mpMainMenuInfo.roomNames[mpMainMenuInfo.current_selected];
            std::string postCommand = "command=startRoom/:"+roomToStart;
            curl_easy_setopt(curl, CURLOPT_URL,  "195.111.1.138:8888");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postCommand.c_str());
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            curlData();
        }
        //mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected] = true;
        }
    }
}


bool ape::VLFTImgui::curlData(){
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
      
    curl = curl_easy_init();
    if(curl) {
        mpMainMenuInfo.roomNames.clear();
        mpMainMenuInfo.running_rooms.clear();
        curl_easy_setopt(curl, CURLOPT_URL,  "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if(res != CURLE_COULDNT_CONNECT){
            std::size_t foldersPos = readBuffer.find("[DIR]");
            readBuffer = readBuffer.substr(foldersPos, std::string::npos );
            std::size_t folderStartPos = 0;
            while(folderStartPos != 2){
                folderStartPos = readBuffer.find("/\">")+3;
                if(folderStartPos == 2)
                    break;
                std::size_t folderEndPos = readBuffer.find("/</a>");
                std::string roomName = readBuffer.substr(folderStartPos, folderEndPos-folderStartPos);
                mpMainMenuInfo.roomNames.push_back(roomName);
                readBuffer = readBuffer.substr(folderEndPos+5, std::string::npos);
            }
            for(int i =0; i < mpMainMenuInfo.roomNames.size(); i++){
                  if(mpSceneNetwork->isRoomRunning(mpMainMenuInfo.roomNames[i])){
                     mpMainMenuInfo.running_rooms.push_back(true);
                  }
                  else{
                    if(mpMainMenuInfo.multiPlayer)
                        mpMainMenuInfo.roomNames.erase(mpMainMenuInfo.roomNames.begin()+i--);
                    else
                        mpMainMenuInfo.running_rooms.push_back(false);
                  }
            }
            return true;
        }
    }
    return false;
}

void ape::VLFTImgui::uploadGUI(int width, int height) {
    ImGui::SetCursorPos(ImVec2(width/2-125, height/10));
    ImGui::Text("Enter the name of the room for uploading");
    static char uploadRoomName[255];
    ImGui::InputText(u8"RoomName", uploadRoomName, IM_ARRAYSIZE(uploadRoomName));
    char* filePath;
    if (ImGui::Button("Chose a scene json file ", ImVec2(width / 7, height / 15))) {
        nfdresult_t result = NFD_OpenDialog("json", NULL, &filePath);
        std::ifstream file(filePath);
        std::ostringstream tmp;
        tmp << file.rdbuf();
        mUploadScene = tmp.str();
    }
    if (mUploadScene != "") {
        ImGui::SameLine(width / 7+25);
        if (ImGui::Button("Upload", ImVec2(width / 8, height / 15))) {
            CURL* curl;
            CURLcode res;
            std::string readBuffer;

            curl = curl_easy_init();
            if (curl) {
                std::string postCommand = "command=uploadSceneJson_" + std::string(uploadRoomName) + "/:" + mUploadScene;
                curl_easy_setopt(curl, CURLOPT_URL, "195.111.2.95:4444");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postCommand.c_str());
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
                curlData();
            }
            mUploadScene = "";
        }
    }
    if (ImGui::Button("Chose an animation json file ", ImVec2(width / 7, height / 15))) {
        nfdresult_t result = NFD_OpenDialog("json", NULL, &filePath);
        std::ifstream file(filePath);
        std::ostringstream tmp;
        tmp << file.rdbuf();
        mUploadAnimations = tmp.str();
    }
    if (mUploadAnimations != "") {
        ImGui::SameLine(width / 7 + 25);
        if (ImGui::Button("Upload", ImVec2(width / 8, height / 15))) {
            CURL* curl;
            CURLcode res;
            std::string readBuffer;

            curl = curl_easy_init();
            if (curl) {
                std::string postCommand = "command=uploadAnimationJson_" + std::string(uploadRoomName) + "/:" + mUploadAnimations;
                curl_easy_setopt(curl, CURLOPT_URL, "195.111.2.95:4444");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postCommand.c_str());
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
                curlData();
            }
            mUploadAnimations = "";
        }
    }
    if (ImGui::Button("Chose an apeCore json file ", ImVec2(width / 7, height / 15))) {
        nfdresult_t result = NFD_OpenDialog("json", NULL, &filePath);
        std::ifstream file(filePath);
        std::ostringstream tmp;
        tmp << file.rdbuf();
        mUploadApeCore = tmp.str();
    }
    if (mUploadApeCore != "") {
        ImGui::SameLine(width / 7 + 25);
        if (ImGui::Button("Upload", ImVec2(width / 8, height / 15))) {
            CURL* curl;
            CURLcode res;
            std::string readBuffer;

            curl = curl_easy_init();
            if (curl) {
                std::string postCommand = "command=uploadCoreJson_" + std::string(uploadRoomName) + "/:" + mUploadApeCore;
                curl_easy_setopt(curl, CURLOPT_URL, "195.111.2.95:4444");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postCommand.c_str());
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
                curlData();
            }
            mUploadApeCore = "";
        }
    }
    ImGui::SetCursorPos(ImVec2(width - (width / 8 + 75), height - (height / 15 + 75)));
    if (ImGui::Button("Back", ImVec2(width / 8, height / 15))) {
        mpMainMenuInfo.uploadRoomGui = false;
        mpMainMenuInfo.mainMenu = true;
    }
}

void ape::VLFTImgui::studentRoomGUI(){
       
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    if (mpUpdateInfo->loadingRoom) {
        ImGui::SetNextWindowSize(ImVec2(width / 3 - 40, height / 3 - 40));
        ImGui::Begin("Admin", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 1;
        //ImGui::SetNextWindowFocus();
        char* loadText = "Loading room";
        ImGui::SetCursorPos(ImVec2(width / 6 - 60, height / 6 - 40));
        ImGui::Text(loadText);
        ImGui::End();
        ImGui::GetStyle().Alpha = 0.8;
    }
    else {
        ImGui::SetNextWindowSize(ImVec2(width - 40, height - 40));
        ImGui::Begin("Student", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 0.8;
        if (mpMainMenuInfo.mainMenu) {
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25));
            if (ImGui::Button("SinglePlayer", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.singlePlayer = true;
                mpUpdateInfo->inSinglePlayer = true;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.multiPlayer = false;
            }
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25 + height / 12 + 12));
            if (ImGui::Button("MultiPlayer", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.multiPlayer = true;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.inSinglePlayerMode = false;
                mpUpdateInfo->inSinglePlayer = false;
            }
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25 + height / 6 + 24));
            if (ImGui::Button("Settings", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.adminMenu = false;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.settingsMenu = true;
                mpUpdateInfo->inSinglePlayer = false;
            }
        }
        else if (mpMainMenuInfo.singlePlayer) {
            if (!mpMainMenuInfo.namesLoaded) {
                curlData();
                mpMainMenuInfo.namesLoaded = true;
            }
            if (ImGui::Button("Refresh", ImVec2(width / 6, height / 14))) {
                curlData();
            }
            listRoomNames(false);
            ImGui::SetCursorPos(ImVec2(5, height - (height / 15 + 50)));
            createJoinButton("Student", width, height);
            ImGui::SameLine(width - (width / 8 + 48));
            if (ImGui::Button("Back", ImVec2(width / 8, height / 15))) {
                mpMainMenuInfo.singlePlayer = false;
                mpMainMenuInfo.mainMenu = true;
            }
        }
        else if (mpMainMenuInfo.multiPlayer) {
            if (!mpMainMenuInfo.namesLoaded) {
                curlData();
                mpMainMenuInfo.namesLoaded = true;
            }
            if (ImGui::Button("Refresh", ImVec2(width / 6, height / 14))) {
                curlData();
            }
            listRoomNames(true);
            ImGui::SetCursorPos(ImVec2(5, height - (height / 15 + 50)));
            createJoinButton("Teacher", width, height);
            ImGui::SameLine(width - (width / 8 + 48));
            if (ImGui::Button("Back", ImVec2(width / 8, height / 15))) {
                mpMainMenuInfo.multiPlayer = false;
                mpMainMenuInfo.mainMenu = true;
            }
        }
        else if (mpMainMenuInfo.settingsMenu) {
            mpMainMenuInfo.mainMenu = createSettingsMenu(width, height);

        }
        if (mpUpdateInfo->changedKey) {
            const float width = ImGui::GetIO().DisplaySize.x;
            const float height = ImGui::GetIO().DisplaySize.y;
            ImGui::SetNextWindowSize(ImVec2(200, 50));
            ImGui::SetNextWindowPos(ImVec2(width / 2 - 100, height / 2 - 25));
            ImGui::SetNextWindowFocus();
            ImGui::Begin("Press a key");
            ImGui::Text("Press a key to set the conrtol");
            ImGui::End();
        }
        mpMainMenuInfo.sideBarWidth = ImGui::GetWindowWidth();

        ImGui::End();
    }
}

bool ape::VLFTImgui::createSettingsMenu(int width, int height){
    mpUpdateInfo->inSettings = true;
    ImGui::SetCursorPos(ImVec2(width/3,height/4));
    ImGui::Text("Forward");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button((mpUpdateInfo->keyLabel["w"] + " ").c_str(),ImVec2(40, 20))){
        if (!mpUpdateInfo->changedKey) {
            mpUpdateInfo->changeKeyCode = "w";
            mpUpdateInfo->changedKey = true;
        }
    }
    ImGui::SetCursorPosX(width/3);
    ImGui::Text("Backward");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button((mpUpdateInfo->keyLabel["s"] + " ").c_str(),ImVec2(40, 20))){
        if (!mpUpdateInfo->changedKey) {
            mpUpdateInfo->changeKeyCode = "s";
            mpUpdateInfo->changedKey = true;
        }
    }
    ImGui::SetCursorPosX(width/3);
    ImGui::Text("Left");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button((mpUpdateInfo->keyLabel["a"] + " ").c_str(),ImVec2(40, 20))){
        if (!mpUpdateInfo->changedKey) {
            mpUpdateInfo->changeKeyCode = "a";
            mpUpdateInfo->changedKey = true;
        }
    }
    ImGui::SetCursorPosX(width/3);
    ImGui::Text("Right");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button((mpUpdateInfo->keyLabel["d"] + " ").c_str(),ImVec2(40, 20))){
        if (!mpUpdateInfo->changedKey) {
            mpUpdateInfo->changeKeyCode = "d";
            mpUpdateInfo->changedKey = true;
        }
    }
    ImGui::SetCursorPosX(width/3);
    ImGui::Text("Up");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button((mpUpdateInfo->keyLabel["e"] + " ").c_str(),ImVec2(40, 20))){
        if (!mpUpdateInfo->changedKey) {
            mpUpdateInfo->changeKeyCode = "e";
            mpUpdateInfo->changedKey = true;
        }
    }
    ImGui::SetCursorPosX(width/3);
    ImGui::Text("Down");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button((mpUpdateInfo->keyLabel["q"] + " ").c_str(),ImVec2(40, 20))){
        if (!mpUpdateInfo->changedKey) {
            mpUpdateInfo->changeKeyCode = "q";
            mpUpdateInfo->changedKey = true;
        }
    }
    ImGui::SetCursorPosX(width / 3);
    ImGui::Checkbox("Lights on", &mpUpdateInfo->lightOn);
    ImGui::SetCursorPosX(width / 3);
    if (ImGui::Button("Change light direction", ImVec2(150, 25))) {
        mpUpdateInfo->changeLightDir = true;
    }    
    ImGui::SetCursorPosX(width / 3);
    ImGui::PushItemWidth(75);
    ImGui::InputFloat("x", &mpUpdateInfo->lightDirection[0]);
    ImGui::SameLine();
    ImGui::InputFloat("y", &mpUpdateInfo->lightDirection[1]);
    ImGui::SameLine();
    ImGui::InputFloat("z", &mpUpdateInfo->lightDirection[2]);
    if (mpUpdateInfo->inRoom) {
        static char saveRoomName[255];
        ImGui::SetCursorPosX(width / 3);
        ImGui::InputText(u8"RoomName", saveRoomName, IM_ARRAYSIZE(saveRoomName));
        ImGui::SameLine(width / 3 * 2);
        if (ImGui::Button("Save room", ImVec2(width / 8, height / 15)))
        {
            std::ofstream outJson;
            outJson.open("savedRoom.json");
            outJson << "{\n\t\"context\":{\n\t\t\"untiOfMeasureScale\":1.0,\n\t\t\"Zup\":false,\n\t\t\"RepoPath\":\"\"\n\t},";


            auto nodes = mpSceneManager->getNodes();
            std::vector<std::string> sceneNodes;
            outJson << "\n\t\"scene\": [\n";
            bool first = true;
            for (auto node : nodes)
            {
                if (auto nodeSP = node.second.lock())
                {
                    if (nodeSP->isVisible()) {
                        if (!first) {
                            outJson << ",\n";
                        }
                        first = false;
                        outJson << "\t\t\"" << nodeSP->getName() << "\"";
                       
                    }
                }
            }
            outJson << "\n\t],\n\t\"assets\": [\n";
            first = true;
            for (auto node : nodes)
            {
                if (!first)
                    outJson << ",\n";
                first = false;
                outJson << "\t\t{\n";
                if (auto nodeSP = node.second.lock())
                {
                    quicktype::Asset nodeAsset;
                    bool assetExists = false;
                    for (auto asset : mScene.get_assets()) {
                        if (asset.get_id() == nodeSP->getName()) {
                            nodeAsset = asset;
                            assetExists = true;
                        }
                    }
                    outJson << "\t\t\t\"id\": \"" << nodeSP->getName() << "\",\n";
                    if (assetExists) {
                        outJson << "\t\t\t\"type\": \"" << nodeAsset.get_type() << "\",\n";
                        if(nodeAsset.get_descr())
                            outJson << "\t\t\t\"descr\": \"" << *nodeAsset.get_descr() << "\",\n";
                        if (nodeAsset.get_representations()) {
                            outJson << "\t\t\t\"representations\": [\n";
                            for (auto representation : *nodeAsset.get_representations())
                            {
                                outJson << "\t\t\t{";
                                std::string filePath = representation.get_file();
                                outJson << "\t\t\t\t\"file\": \"" << filePath << "\",\n";
                                std::string fileUnit = std::to_string(*representation.get_unit());
                                outJson << "\t\t\t\t\"unit\": \"" << fileUnit << "\",\n";
                                outJson << "\t\t\t}";
                            }
                        }
                        if (nodeAsset.get_model()) {
                            outJson << "\t\t\t\"model\": \"" << *nodeAsset.get_model() << "\",\n";
                        }
                    }
                    auto pos = nodeSP->getPosition();
                    outJson << "\t\t\t\"position\": [\n";
                    outJson << "\t\t\t\t" << std::to_string(pos.getX()) << ",\n";
                    outJson << "\t\t\t\t" << std::to_string(pos.getY()) << ",\n";
                    outJson << "\t\t\t\t" << std::to_string(pos.getZ()) << "\n";
                    outJson << "\t\t\t],\n";

                    auto rot = nodeSP->getOrientation();
                    outJson << "\t\t\t\"position\": [\n";
                    outJson << "\t\t\t\t" << std::to_string(rot.getW()) << ",\n";
                    outJson << "\t\t\t\t" << std::to_string(rot.getX()) << ",\n";
                    outJson << "\t\t\t\t" << std::to_string(rot.getY()) << ",\n";
                    outJson << "\t\t\t\t" << std::to_string(rot.getZ()) << "\n";
                    outJson << "\t\t\t],\n";
                    if (assetExists) {
                        if (nodeAsset.get_placement_rel_to()) {
                            outJson << "\t\t\t\"placementRelTo\": \"" << *nodeAsset.get_placement_rel_to() << "\",\n";
                        }
                    }
                    if (auto parent = nodeSP->getParentNode().lock()) {
                        outJson << "\t\t\t\"parentObject\": \"" << parent->getName() << "\",\n";
                    }
                    else {
                        if (assetExists) {
                            if (nodeAsset.get_parent_object()) {
                                outJson << "\t\t\t\"parentObject\": \"" << *nodeAsset.get_parent_object() << "\",\n";
                            }
                        }
                    }
                }


                outJson << "\t\t}";
            }
            outJson << "]";
            outJson << "}";
            outJson.close();
        }
    }
    ImGui::PopItemWidth();
    ImGui::SetCursorPos(ImVec2(0,height-(height/15+25)));
    if(mpUpdateInfo->leftRoom == false && mpUpdateInfo->inRoom){
        if(ImGui::Button("Leave room",ImVec2(width/8, height/15)))
        {
            mpUpdateInfo->leftRoom = true;
            mpMainMenuInfo.inRoomGui = false;
            mpUpdateInfo->inRoom = false;
            mpMainMenuInfo.adminMenu = true;
            mpMainMenuInfo.settingsMenu = false;
            mpUpdateInfo->inSettings = false;
            mpUpdateInfo->playerNamePositions.clear();
            mpUpdateInfo->stateOfObjects.clear();
            mpUpdateInfo->nameOfState.clear();
            mpUpdateInfo->newMessage.clear();
            mpUpdateInfo->timeOfState.clear();
            chatMessages.clear();
            chatMessages.push_back("Write messages here for others to see");
        }
    }
    ImGui::SetCursorPos(ImVec2(width-(width/8+25)-25,height-(height/15+25)-25));
    if (ImGui::Button("Back",ImVec2(width/8, height/15))){
        mpMainMenuInfo.settingsMenu = false;
        mpUpdateInfo->inSettings = false;
        return true;
    }
    return false;
}

void ape::VLFTImgui::adminRoomGUI(){
    ImGui::GetStyle().Alpha = 0.8;
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
  
    if (mpUpdateInfo->loadingRoom) {
        ImGui::SetNextWindowSize(ImVec2(width/3 - 40, height/3 - 40));
        ImGui::Begin("Admin", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 1;
        //ImGui::SetNextWindowFocus();
        char* loadText = "Loading room";
        ImGui::SetCursorPos(ImVec2(width / 6 - 60, height / 6-40));
        ImGui::Text(loadText);
        ImGui::End();
        ImGui::GetStyle().Alpha = 0.8;
    }
    else {
        ImGui::SetNextWindowSize(ImVec2(width - 40, height - 40));
        ImGui::Begin("Admin", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::GetStyle().Alpha = 0.95;
        if (mpMainMenuInfo.mainMenu) {
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25));
            if (ImGui::Button("SinglePlayer", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.singlePlayer = true;
                mpUpdateInfo->inSinglePlayer = true;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.multiPlayer = false;
            }
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25 + height / 12 + 12));
            if (ImGui::Button("MultiPlayer", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.multiPlayer = true;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.inSinglePlayerMode = false;
                mpUpdateInfo->inSinglePlayer = false;
            }
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25 + height / 6 + 24));
            if (ImGui::Button("Admin Room", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.adminMenu = true;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.inSinglePlayerMode = false;
                mpUpdateInfo->inSinglePlayer = false;
            }
            ImGui::SetCursorPos(ImVec2(width / 2 - width / 10, height * 0.25 + height / 4 + 36));
            if (ImGui::Button("Settings", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.adminMenu = false;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.settingsMenu = true;
            }
            ImGui::SetCursorPos(ImVec2(width - width / 5 - 75, 10));
            if (ImGui::Button("Upload room", ImVec2(width / 5, height / 12))) {
                mpMainMenuInfo.adminMenu = false;
                mpMainMenuInfo.mainMenu = false;
                mpMainMenuInfo.namesLoaded = false;
                mpMainMenuInfo.uploadRoomGui = true;
            }
        }
        else if (mpMainMenuInfo.singlePlayer) {
            if (!mpMainMenuInfo.namesLoaded) {
                curlData();
                mpMainMenuInfo.namesLoaded = true;
            }
            if (ImGui::Button("Refresh", ImVec2(width / 6, height / 14))) {
                curlData();
            }
            listRoomNames(false);
            ImGui::SetCursorPos(ImVec2(5, height - (height / 15 + 50)));
            createJoinButton("Student", width, height);
            ImGui::SameLine(width - (width / 8 + 48));
            if (ImGui::Button("Back", ImVec2(width / 8, height / 15))) {
                mpMainMenuInfo.singlePlayer = false;
                mpMainMenuInfo.mainMenu = true;
            }
        }
        else if (mpMainMenuInfo.multiPlayer) {
            if (!mpMainMenuInfo.namesLoaded) {
                curlData();
                mpMainMenuInfo.namesLoaded = true;
            }
            if (ImGui::Button("Refresh", ImVec2(width / 6, height / 14))) {
                curlData();
            }
            listRoomNames(true);
            ImGui::SetCursorPos(ImVec2(5, height - (height / 15 + 50)));
            createJoinButton("Teacher", width, height);
            ImGui::SameLine(width - (width / 8 + 48));
            if (ImGui::Button("Back", ImVec2(width / 8, height / 15))) {
                mpMainMenuInfo.mainMenu = true;
                mpMainMenuInfo.multiPlayer = false;
            }
        }
        else if (mpMainMenuInfo.adminMenu) {
            if (!mpMainMenuInfo.namesLoaded) {
                curlData();
                mpMainMenuInfo.namesLoaded = true;
            }
            if (ImGui::Button("Refresh", ImVec2(width / 6, height / 14))) {
                curlData();
            }
            listRoomNames(true);
            ImGui::SetCursorPos(ImVec2(5, height - (height / 15 + 50)));
            createStartButton(width, height);
            ImGui::SameLine(width / 8 + 10);
            createStopButton(width, height);
            ImGui::SameLine(2 * width / 8 + 15);
            createJoinButton("Teacher", width, height);
            ImGui::SameLine(3 * width / 8 + 20);
            if (ImGui::Button("Upload", ImVec2(width / 8, height / 15))) {
                ;
            }
            ImGui::SameLine(width - (width / 8 + 48));
            if (ImGui::Button("Back", ImVec2(width / 8, height / 15))) {
                mpMainMenuInfo.mainMenu = true;
                mpMainMenuInfo.adminMenu = false;
            }
        }
        else if (mpMainMenuInfo.uploadRoomGui) {
            uploadGUI(width, height);
        }
        else if (mpMainMenuInfo.settingsMenu) {
            mpMainMenuInfo.mainMenu = createSettingsMenu(width, height);

        }
        if (mpUpdateInfo->changedKey) {
            const float width = ImGui::GetIO().DisplaySize.x;
            const float height = ImGui::GetIO().DisplaySize.y;
            ImGui::SetNextWindowSize(ImVec2(200, 50));
            ImGui::SetNextWindowPos(ImVec2(width / 2 - 100, height / 2 - 25));
            ImGui::SetNextWindowFocus();
            ImGui::Begin("Press a key");
            ImGui::Text("Press a key to set the conrtol");
            ImGui::End();
        }
        mpMainMenuInfo.sideBarWidth = ImGui::GetWindowWidth();

        ImGui::End();
    }
}

void ape::VLFTImgui::loginGUI(){
    ImGui::GetStyle().Alpha = 0.8;
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(width-40, height-40));
    ImGui::Begin("Login", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove);
    ImGui::Checkbox("Teacher", &mpUpdateInfo->isAdmin);
    static char emailT[255] = u8"Teacher";
    static char emailS[255] = u8"Student";
    ImGui::SetCursorPos(ImVec2(width/2-105, height/2-120));
    if(mpUpdateInfo->wrongPassword){
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Incorrect password or email!");
    }
    ImGui::SetCursorPos(ImVec2(width/2-120, height/2-90));
    ImGui::PushItemWidth(200);
    if(mpUpdateInfo->isAdmin)
        ImGui::InputText(u8"email", emailT, IM_ARRAYSIZE(emailT));
    else
        ImGui::InputText(u8"email", emailS, IM_ARRAYSIZE(emailS));
    static char password[255] = u8"apertusvr2020";
    if(mpUpdateInfo->isAdmin){
        ImGui::SetCursorPos(ImVec2(width/2-120, height/2-60));
        ImGui::PushItemWidth(200);
        ImGui::InputText(u8"password", password, IM_ARRAYSIZE(password),ImGuiInputTextFlags_Password);
        ImGui::SetCursorPos(ImVec2(width/2-70, height/2-25));
        if(ImGui::Button("Log in", ImVec2(100,30))){
            if (mpUpdateInfo->isAdmin)
                mpUpdateInfo->userName = emailT;
            else
                mpUpdateInfo->userName = emailS;
            mpUpdateInfo->password = password;
            mpUpdateInfo->checkLogin = true;
        }
    }
    else{
        ImGui::SetCursorPos(ImVec2(width/2-70, height/2-25));
        if(ImGui::Button("Log in", ImVec2(100,30))){
            if (mpUpdateInfo->isAdmin)
                mpUpdateInfo->userName = emailT;
            else
                mpUpdateInfo->userName = emailS;
            mpUpdateInfo->checkLogin = true;
        }
    }
    if(mpUpdateInfo->logedIn){
        mpMainMenuInfo.loginMenu = false;
        mpMainMenuInfo.mainMenu = true;
    }
    
    
    ImGui::PopItemWidth();
    ImGui::End();
}

void ape::VLFTImgui::waitWindow(){
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(200, 50));
    ImGui::SetNextWindowPos(ImVec2(width/2-100, height/2-25));
    ImGui::Begin("Waiting", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoMove);
    std::string waitFor = "Leave room in ";
    std::stringstream posStream;
    double timeToWait = 1.0 - (mpUpdateInfo->now - mpUpdateInfo->leaveTime);
    posStream << std::fixed << std::setprecision(2) << timeToWait << " seconds";
    waitFor += posStream.str();
    ImGui::Text("%s", waitFor.c_str());
    ImGui::End();
    if (timeToWait <= 0.0) {
        if (!mpMainMenuInfo.inSinglePlayerMode) {
            mpSceneManager->createSceneNetwork();
            mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
        }
        mpUpdateInfo->leaveWait = false;
    }
}

void ape::VLFTImgui::update(){
    if(false && !mpUpdateInfo->resourcesUpdated){
        updateResources();
    }
    else{
      /*  if (mpUpdateInfo->loadingRoom) {
            loadingRoomGUI();
        }*/
        if (mpMainMenuInfo.connectToRoom) {
            connectToRoom();
            mpMainMenuInfo.current_selected = -1;
            mpMainMenuInfo.connectToRoom = false;
        }
        if(mpUpdateInfo->callLeave && (mpUpdateInfo->now - mpUpdateInfo->leaveTime) > 0.5){
            if (!mpMainMenuInfo.inSinglePlayerMode) {
                mpSceneManager->destroySceneNetwork();
            }
            else {
                mpPluginManager->stopPlugin("apeVLFTSceneLoaderPlugin");
                auto nodes = mpSceneManager->getNodes();
                for (auto node : nodes)
                {
                    mpSceneManager->deleteNode(node.first);
                }
                auto apeEntities = mpSceneManager->getEntities();
                for (auto apeEntity : apeEntities) {
                    mpSceneManager->deleteEntity(apeEntity.first);
                }
            }
            mpUpdateInfo->callLeave = false;
        }
        if(mpUpdateInfo->leaveWait)
            waitWindow();
        else if(mpMainMenuInfo.loginMenu)
            loginGUI();
        else if(mpUpdateInfo->isAdmin && (!mpMainMenuInfo.inRoomGui || mpUpdateInfo->loadingRoom))
            adminRoomGUI();
        else if(!mpUpdateInfo->isAdmin && (!mpMainMenuInfo.inRoomGui || mpUpdateInfo->loadingRoom))
            studentRoomGUI();
        else if(mpMainMenuInfo.inRoomGui && !mpUpdateInfo->loadingRoom){
            ImGui::GetStyle().Alpha = 0.65;
            const float width = ImGui::GetIO().DisplaySize.x;
            const float height = ImGui::GetIO().DisplaySize.y;
            ImGui::SetNextWindowPos(ImVec2(width-215, 3));
            ImGui::SetNextWindowSize(ImVec2(210, 210));
            ImGui::Begin("Map_of_players", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImU32 col_red = ImGui::GetColorU32(IM_COL32(255, 0, 0, 255));
            ImU32 col_blue = ImGui::GetColorU32(IM_COL32(0, 0 , 255, 255));
            const ImVec2 p = ImGui::GetCursorScreenPos();
            draw_list->AddNgonFilled(ImVec2(p.x+100,p.y+100),10, col_blue, 4);
            for (auto pos : mpUpdateInfo->playerMapPositions) {
                float posX = pos.second[0];
                float posY = -pos.second[1];
                float newX = posX * cos(-mpUpdateInfo->playerRotation) - posY * sin(-mpUpdateInfo->playerRotation);
                float newY = posX * sin(-mpUpdateInfo->playerRotation) + posY * cos(-mpUpdateInfo->playerRotation);
                if(newX > -120 && newY >-120 && newX < 120 && newY < 120)
                    draw_list->AddNgonFilled(ImVec2(p.x + newX * 5 + 100, p.y + newY * 5 + 100), 10, col_red, 4);
            }
            ImGui::End();

            ImGui::GetStyle().Alpha = 0.95;
            leftPanelGUI();
            if(mpUpdateInfo->isAdmin ||!mpMainMenuInfo.multiPlayer){
                animationPanelGUI();
                manipulatorPanelGUI();
                studentPanelGUI();
            }
            //screenshotPanelGUI();
            if(mpMainMenuInfo.showStates){
                statePanelGUI();
            }
            infoPanelGUI();
            drawUserNames();
        }
    }
    mpUpdateInfo->rePositionUI = false;
}

void ape::VLFTImgui::statePanelGUI(){
    ImGui::SetNextWindowPos(ImVec2(122, 0),ImGuiCond_Appearing);
    if (!mpUpdateInfo->isAdmin && mpMainMenuInfo.multiPlayer) {
        ImGui::SetNextWindowPos(ImVec2(255, 0), ImGuiCond_Appearing);
        if (mpUpdateInfo->rePositionUI) {
            ImGui::SetNextWindowPos(ImVec2(255, 0));
        }
    }
    else{
        ImGui::SetNextWindowPos(ImVec2(122, 0), ImGuiCond_Appearing);
        if (mpUpdateInfo->rePositionUI) {
            ImGui::SetNextWindowPos(ImVec2(122, 0));
        }
    }
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(270, 100), ImGuiCond_Once);
    ImGui::Begin("States", nullptr);
    float timeWidth = ImGui::CalcTextSize("10.0").x+20;
    float nameWidth = ImGui::CalcTextSize("This is that max length").x+10;
    
    ImGui::Text("time");
    ImGui::SameLine(timeWidth);
    ImGui::Text("Asset");
    ImGui::SameLine(140);
    ImGui::Text("State");
    for(size_t i = 0; i < mpUpdateInfo->nameOfState.size(); i++){
        std::stringstream posStream;
        posStream << std::fixed << std::setprecision(1) <<mpUpdateInfo->timeOfState[i];
        ImGui::Text("%s",posStream.str().c_str());
        ImGui::SameLine(timeWidth);
        ImGui::Text("%s", mpUpdateInfo->nameOfState[i].c_str());
        ImGui::SameLine(140);
        ImGui::Text("%s", mpUpdateInfo->stateOfObjects[i].c_str());
        statesMap[mpUpdateInfo->nameOfState[i]] = mpUpdateInfo->stateOfObjects[i];
        }
    if( mpMainMenuInfo.prevStateNum != mpUpdateInfo->nameOfState.size()){
        ImGui::SetScrollHere(1.0f);
        mpMainMenuInfo.prevStateNum = mpUpdateInfo->nameOfState.size();
    }
   
    ImGui::End();
}

void ape::VLFTImgui::leftPanelGUI() {
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(width-150, height/3-20),ImGuiCond_Appearing);
    if (mpUpdateInfo->rePositionUI) {
        ImGui::SetNextWindowPos(ImVec2(width - 135, height / 3 - 20));
    }
    ImGui::SetNextWindowSize(ImVec2(135, 170), ImGuiCond_Appearing);
    ImGui::Begin("Utilities", nullptr);
    ImGui::Checkbox("Show map", &mpUpdateInfo->isMapVisible);
    ImGui::Checkbox("Show headers", &mpMainMenuInfo.showStates);
    if (ImGui::Button("Screenshot", ImVec2(115, 25))) {
        mpUpdateInfo->takeScreenshot = true;
    }
    if (!mpUpdateInfo->screenCaptureOn) {
        if (ImGui::Button("Start screen cast", ImVec2(115, 25))) {
            mpUpdateInfo->screenCast = true;
        }
    }
    else {
        if (mpUpdateInfo->stoppingScreenCast) {
            if (ImGui::Button("Saving video", ImVec2(115, 25))) {
                ;
            }
        }
        else {
            if (ImGui::Button("Stop screen cast", ImVec2(115, 25))) {
                mpUpdateInfo->screenCast = true;
            }
        }
       
    }
    if(ImGui::Button("Settings",ImVec2(115,25))){
        mpMainMenuInfo.settingsMenu = true;
    }
    
    
    if(mpMainMenuInfo.settingsMenu){
        ImGui::SetNextWindowPos(ImVec2(5, 5));
        ImGui::SetNextWindowSize(ImVec2(width-10, height-10));
        ImGui::GetStyle().Alpha = 1;
        ImGui::SetNextWindowFocus();
        ImGui::Begin("SettingsWindow", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        createSettingsMenu(width, height);
        ImGui::End();
        ImGui::GetStyle().Alpha = 0.95;
    }
    if(mpUpdateInfo->changedKey){
        const float width = ImGui::GetIO().DisplaySize.x;
        const float height = ImGui::GetIO().DisplaySize.y;
        ImGui::SetNextWindowSize(ImVec2(200, 50));
        ImGui::SetNextWindowPos(ImVec2(width/2-100, height/2-25));
        ImGui::SetNextWindowFocus();
        ImGui::Begin("Press a key");
        ImGui::Text("Press a key to set the conrtol");
        ImGui::End();
    }
    ImGui::End();
}

void ape::VLFTImgui::studentPanelGUI(){
    if (!mpMainMenuInfo.inSinglePlayerMode) {
        const float width = ImGui::GetIO().DisplaySize.x;
        const float height = ImGui::GetIO().DisplaySize.y;
        ImGui::SetNextWindowPos(ImVec2(width - 135, height / 3 + 150),ImGuiCond_Appearing);
        if (mpUpdateInfo->rePositionUI) {
            ImGui::SetNextWindowPos(ImVec2(width - 135, height / 3 + 150));
        }
        ImGui::SetNextWindowSize(ImVec2(135, 105), ImGuiCond_Once);
        ImGui::Begin("Student control", nullptr);
        if (mpUpdateInfo->isAdmin || mpMainMenuInfo.inSinglePlayerMode) {
            if (!mpUpdateInfo->usersAttached) {
                if (ImGui::Button("Attach users", ImVec2(115, 25))) {
                    mpUpdateInfo->attachUsers = true;
                }
            }
            else {
                if (ImGui::Button("Detach users", ImVec2(115, 25))) {
                    mpUpdateInfo->attachUsers = true;
                }
            }
            if (!mpUpdateInfo->mIsStudentsMovementLogging) {
                if (ImGui::Button("Log movements", ImVec2(115, 25))) {
                    mpUpdateInfo->logMovements = true;
                }
            }
            else {
                if (ImGui::Button("Stop logging", ImVec2(115, 25))) {
                    mpUpdateInfo->logMovements = true;
                }
            }
        }
        ImGui::End();
    }
   
}

void ape::VLFTImgui::drawUserNames()
{
    const float winwidth = ImGui::GetIO().DisplaySize.x;
    const float winheight = ImGui::GetIO().DisplaySize.y;
    
    for (auto x : mpUpdateInfo->playerNamePositions) {
        auto wScale = winwidth/x.second[3];
        auto hScale = winheight/x.second[4];
        auto scale = x.second[2] > 0.6 ? x.second[2] : 0.6;
        auto width = ImGui::CalcTextSize(x.first.c_str()).x * (scale+0.2);
        auto height = ImGui::CalcTextSize(x.first.c_str()).y * (scale + 0.2);
        ImGui::SetNextWindowPos(ImVec2(x.second[0]*wScale-width/2*1.2, x.second[1]*hScale));
        ImGui::SetNextWindowSize(ImVec2(width*1.2, height*1.2));
        ImGui::Begin(x.first.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing);
        ImGui::SetWindowFontScale(scale);
        ImGui::Text(x.first.c_str());
        ImGui::SetWindowFontScale(1);
        ImGui::End();
    }
}

void ape::VLFTImgui::infoPanelGUI() {
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    if (!mpUpdateInfo->isAdmin && mpMainMenuInfo.multiPlayer) {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
        if (mpUpdateInfo->rePositionUI) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
        }
    }
    else {
        ImGui::SetNextWindowPos(ImVec2(0, 141), ImGuiCond_Appearing);
        if (mpUpdateInfo->rePositionUI) {
            ImGui::SetNextWindowPos(ImVec2(0,141));
        }
    }
  
    ImGui::SetNextWindowSize(ImVec2(250, 260), ImGuiCond_Once);
    ImGui::Begin("Info", nullptr);
    
    getInfoAboutObject(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
    ImGui::End();
    if (!mpUpdateInfo->inSinglePlayer) {
        if (mpUpdateInfo->newChatMessage) {
            chatMessages.insert(chatMessages.end(), mpUpdateInfo->newMessage.begin(), mpUpdateInfo->newMessage.end());
            mpUpdateInfo->newMessage.clear();
        }
        ImGui::SetNextWindowPos(ImVec2(width - 305, height - 210), ImGuiCond_Once);
        if (mpUpdateInfo->rePositionUI) {
            ImGui::SetNextWindowPos(ImVec2(width - 305, height - 210));
        }
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
        ImGui::SetNextWindowSizeConstraints(ImVec2(200, 150), ImVec2(500, 500));
        ImGui::GetStyle().Alpha = 0.6;
        ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImGui::GetStyle().Alpha = 1;
        ImGui::BeginChild("Chat region", ImVec2(ImGui::GetWindowWidth() - 18, ImGui::GetWindowHeight() - 45), ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushTextWrapPos(0.0f);
        if (mpUpdateInfo->sendMessage) {
            ImGui::SetScrollHere(1.0f);
        }
        std::string chatText = u8"";
        for (int i = 0; i < ((int)chatMessages.size() - 1); i++) {
            if (chatMessages[i].size() > 0)
                chatText += chatMessages[i] + "\n";
        }
        chatText += chatMessages[chatMessages.size() - 1];
        ImGui::TextWrapped("%s", chatText.c_str());
        if (messageInBuffer || mpUpdateInfo->newChatMessage) {
            ImGui::SetScrollHere(1.0f);
            messageInBuffer = false;
            mpUpdateInfo->newChatMessage = false;
        }
        ImGui::PopTextWrapPos();
        ImGui::EndChild();
        static char str0[255] = u8"";
        ImGui::SetCursorPos(ImVec2(8, ImGui::GetWindowHeight() - 30));
        ImGui::PushItemWidth(ImGui::GetWindowWidth() - 18);
        if (ImGui::InputText(u8"", str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue)) {

            if (strlen(str0) != 0) {
                std::string msg = str0;
                msg = mpUpdateInfo->userName + ": " + msg;
                chatMessages.push_back(msg);
                mpUpdateInfo->messageToSend = msg;
                mpUpdateInfo->sendMessage = true;
                messageInBuffer = true;
            }
            str0[0] = 0;
        }
        ImGui::End();
    }
    ImGui::GetStyle().Alpha = 0.8;
}

void ape::VLFTImgui::loadingRoomGUI()
{
    /*ImGui::GetStyle().Alpha = 1;
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(1,1));
    ImGui::SetNextWindowSize(ImVec2(width-1, height-1));
    ImGui::SetNextWindowFocus();

    ImGui::Begin("Loading_room", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    char* loadText = "Loading room";
    ImGui::SetCursorPos(ImVec2(width/2-40,height/2));
    ImGui::Text(loadText);
    ImGui::End();
    ImGui::GetStyle().Alpha = 0.8;*/
}

void ape::VLFTImgui::animationPanelGUI(){
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    if (mpUpdateInfo->rePositionUI) {
        ImGui::SetNextWindowPos(ImVec2(0,0));
    }
    ImGui::SetNextWindowSize(ImVec2(120, 140), ImGuiCond_Once);
    ImGui::Begin("Animation", nullptr);
    std::string playText="Play";
    if(mpUpdateInfo->isPlayRunning || mpUpdateInfo->pauseTime > 0){
        std::string time = std::to_string(mpUpdateInfo->pauseTime/1000.0);
        ImGui::Text("%s", time.c_str());
    }
    if(mpUpdateInfo->IsPlayClicked)
        playText = "Pause";
    ImGui::Checkbox("Show spaghetti", &mpUpdateInfo->spaghettiVisible);
    if(ImGui::Button(playText.c_str(),ImVec2(100,25)))
    {
        if (!mpUpdateInfo->isPlayRunning)
        {
            mpUpdateInfo->IsPlayClicked = true;
            mpUpdateInfo->ChoosedBookmarkedAnimationID = 0;
            mpUpdateInfo->ClickedBookmarkTime = 0;
            mpUpdateInfo->BookmarkID = -1;
            mpUpdateInfo->TimeToSleepFactor = 1.0f;
            mpUpdateInfo->IsPauseClicked = false;
           
        }else{
            mpUpdateInfo->IsPauseClicked = true;
            mpUpdateInfo->IsPlayClicked = false;
            mpUpdateInfo->isPlayRunning = false;
            
        }
    }
    if(ImGui::Button("Stop animation",ImVec2(100,25))){
        mpUpdateInfo->IsStopClicked = true;
        mpUpdateInfo->IsPlayClicked = false;
        mpUpdateInfo->isPlayRunning = false;
        mpUpdateInfo->IsPauseClicked = false;
        //mpUpdateInfo->pauseTime = 0;
    }
    //ImGui::Checkbox("show headers", &mpMainMenuInfo.showStates);
    ImGui::End();
}

void ape::VLFTImgui::screenshotPanelGUI(){
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(width-135, 0), ImGuiCond_Appearing);
    if (mpUpdateInfo->rePositionUI) {
        ImGui::SetNextWindowPos(ImVec2(width - 135, 0));
    }
    ImGui::SetNextWindowSize(ImVec2(130, 150), ImGuiCond_Appearing);
    ImGui::Begin("Screenshot", nullptr);
    if (ImGui::Button("Screenshot", ImVec2(115, 25))) {
        mpUpdateInfo->takeScreenshot = true;
    }
    if (!mpUpdateInfo->screenCaptureOn) {
        if (ImGui::Button("Start screen cast", ImVec2(115, 25))) {
            mpUpdateInfo->screenCast = true;
        }
    }
    else {
        if (ImGui::Button("Stop screen cast", ImVec2(115, 25))) {
            mpUpdateInfo->screenCast = true;
        }
    }
   
    ImGui::End();
}

void ape::VLFTImgui::openFileBrowser() {
    char* filePath;
    nfdresult_t result = NFD_OpenDialog( "gltf,glb", NULL, &filePath );
    if ( result == NFD_OKAY )
    {
        APE_LOG_DEBUG("Success! "<<filePath);
        std::string fileName = filePath;
        fileName = fileName.substr(fileName.find_last_of("/")+1);
        std::string nodeName = "gltfNode_"+fileName;
        std::string entityName = "gltfEntity_"+fileName;
        mpSceneManager->createNode(nodeName, true, mpCoreConfig->getNetworkGUID());
        //if(auto camNode = mCamNode.lock())
        if (auto node = mpSceneManager->getNode(nodeName).lock())
        {
            //camNode->setPosition(ape::Vector3(0.0, 0.0, 0.0));
            //node->setParentNode(camNode);
            if (auto gltfNode = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(entityName, ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
            {
                gltfNode->setFileName(fileName);
                gltfNode->setParentNode(node);
            }
            if (auto gltfNode = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->getEntity(entityName).lock())) {
                if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(nodeName, ape::Entity::Type::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
                {
                    geometryClone->setSourceGeometry(gltfNode);
                    node->setChildrenVisibility(true);
                }
            }
            node->setPosition(ape::Vector3(0.0, 0.0, 0.0));
        }

        if (auto node = mpSceneManager->getNode(nodeName).lock()) {
            if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->getEntity(nodeName).lock()))
            {
                geometryClone->setParentNode(node);
                node->setChildrenVisibility(true);
            }
        }
    }
    else if ( result == NFD_CANCEL )
    {
        puts("User pressed cancel.");
    }
    else
    {
        printf("Error: %s\n", NFD_GetError() );
    }
}

static std::string convertVecToString(std::vector<double> vec,int  precision){
    std::string convertedVec ="(";
    std::stringstream posStream;
    posStream << std::fixed << std::setprecision(precision) << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
    convertedVec += posStream.str();
    return convertedVec;
    
}

static std::string convertApeVec3ToString(ape::Vector3 vec,int  precision){
    std::string convertedVec ="(";
    std::stringstream posStream;
    posStream << std::fixed << std::setprecision(precision) << vec.getX() << ", " << vec.getY() << ", " << vec.getZ() << ")";
    convertedVec += posStream.str();
    return convertedVec;
    
}

static std::string convertApeQuaternionToString(ape::Quaternion vec,int  precision){
    std::string convertedVec ="(";
    std::stringstream posStream;
    posStream << std::fixed << std::setprecision(precision) << vec.getW() << ", " << vec.getX() << ", " << vec.getY() << ", "<< vec.getZ() << ")";
    convertedVec += posStream.str();
    return convertedVec;
    
}


void ape::VLFTImgui::getInfoAboutObject(float width, float height){
    std::string IDText = "ID: ";
    std::string positionText = "Position: ";
    std::string rotationText = "Rotation: ";
    std::string rootIDText = "ID: ";
    std::string typeText = "Type: ";
    std::string desrcText = "Desription: ";
    std::string rootPosText = "Position: ";
    std::string stateText = "State: ";
    std::string logText = "Log: ";
    bool foundSelected = false;
    bool foundRoot = false;
    if(mpUpdateInfo->selectedItem != "")
    {
        for(auto asset : mScene.get_assets()){
            if(asset.get_id() == mpUpdateInfo->rootOfSelected){
                auto position = asset.get_position();
                auto rotation = asset.get_rotation();
                
                IDText += mpUpdateInfo->selectedItem;
                if(position)
                    positionText += convertVecToString(*position, 2);
                if(rotation)
                    rotationText += convertVecToString(*rotation, 2);
                typeText += asset.get_type();
                if(asset.get_descr())
                    desrcText += *asset.get_descr();
                foundSelected = true;
            }
            else if(asset.get_id() == mpUpdateInfo->rootOfSelected){
                auto rootPos = asset.get_position();
                rootIDText += mpUpdateInfo->rootOfSelected;
                if(rootPos)
                    rootPosText += convertVecToString(*rootPos, 2);
                foundRoot = true;
            }
            if(foundSelected && foundRoot)
                break;
        }
        if(auto node = mpSceneManager->getNode(mpUpdateInfo->selectedItem).lock()){
            //APE_LOG_DEBUG(node->getName());
            stateText += statesMap[mpUpdateInfo->rootOfSelected];
            auto pos = node->getDerivedPosition();
            positionText = "Position: " + convertApeVec3ToString(pos, 2);
            auto ori = node->getDerivedOrientation();
            rotationText = "Rotation: " + convertApeQuaternionToString(ori, 2);
        }
    }
    ImGui::BeginChild("InfoboxSelected", ImVec2(width-25,height/3-30));
    if(mpUpdateInfo->pickedItem != ""){
        ImGui::Text("This item is in your hand");
    }
    ImGui::TextWrapped("%s", IDText.c_str());
    ImGui::Text("%s", positionText.c_str());
    ImGui::Text("%s", rotationText.c_str());
    ImGui::EndChild();
    ImGui::BeginChild("InfoboxRoot", ImVec2(width-25, height/3*2-40));
    const float textWidth = ImGui::CalcTextSize("Group Information").x;
    ImGui::SetCursorPosX((width-25-textWidth)/2);
    ImGui::Text("Group Information");
    //ImGui::Text("%s", rootIDText.c_str());
    //ImGui::Text("%s", rootPosText.c_str());
    ImGui::TextWrapped("%s", desrcText.c_str());
    ImGui::Text("%s", stateText.c_str());
    ImGui::Text("%s", logText.c_str());
    ImGui::SetCursorPosX(width/2-30);
    ImGui::Text("Links");
    for(auto link : mpUpdateInfo->animationLinks){
        ImGui::SetCursorPosX(15);
        ImGui::Text("%s: %s", link.first.c_str(),link.second.c_str());
    }
    ImGui::EndChild();
    
}

void ape::VLFTImgui::manipulatorPanelGUI(){
    ImGui::SetNextWindowPos(ImVec2(0, 400),ImGuiCond_Appearing);
    if (mpUpdateInfo->rePositionUI) {
        ImGui::SetNextWindowPos(ImVec2(0, 400));
    }
    ImGui::SetNextWindowSize(ImVec2(250, 245), ImGuiCond_Once);
    ImGui::Begin("Manipulator", nullptr);
    const float width = ImGui::GetWindowWidth();
    const float height = ImGui::GetWindowHeight();
    if(mpUpdateInfo->isAdmin ||  mpMainMenuInfo.inSinglePlayerMode){
        ImGui::PushItemWidth((width-80)/3);
        ImGui::InputDouble("x", &posX);
        ImGui::SameLine();
        ImGui::InputDouble("y", &posY);
        ImGui::SameLine();
        ImGui::InputDouble("z", &posZ);
        ImGui::PopItemWidth();
        if(ImGui::Button("Set position",ImVec2(110,25)) && mpUpdateInfo->selectedItem != ""){
            mpUpdateInfo->position = Vector3(posX, posY, posZ);
            mpUpdateInfo->setPosition = true;
        }
        ImGui::PushItemWidth((width-110)/2);
        ImGui::InputDouble("Rad", &rotAngle);
        ImGui::SameLine();
        ImGui::InputDouble("upX", &upX);
        ImGui::InputDouble("upY", &upY);
        ImGui::SameLine();
        ImGui::InputDouble("upZ", &upZ);
        ImGui::PopItemWidth();
        if(ImGui::Button("Set rotation",ImVec2(110,25)) && mpUpdateInfo->selectedItem != ""){
            mpUpdateInfo->orientation = ape::Quaternion(ape::Radian(rotAngle), Vector3(upX, upY, upZ));
            mpUpdateInfo->setRotation = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Rotate", ImVec2(110, 25)) && mpUpdateInfo->selectedItem != "") {
            mpUpdateInfo->orientation = ape::Quaternion(ape::Radian(rotAngle), Vector3(upX, upY, upZ));
            mpUpdateInfo->setRotation = true;
            mpUpdateInfo->rotate = true;
        }
    }
    
    if(ImGui::Button("Delete",ImVec2(110,25)) && mpUpdateInfo->pickedItem == "")
    {
        mpUpdateInfo->deleteSelected = true;
    }
    ImGui::SameLine(120);
    if(ImGui::Button("Browse files",ImVec2(110,25)))
    {
        openFileBrowser();
        
    }
    
    if(mpUpdateInfo->pickedItem == ""){
        if(ImGui::Button("Pick up",ImVec2(110,25)))
        {
            mpUpdateInfo->pickUp = true;
        }
    }
    else{
        if(ImGui::Button("Put down",ImVec2(110,25)))
        {
            mpUpdateInfo->pickUp = true;
        }
    }
    ImGui::SameLine(120);
    if(ImGui::Button("Drop", ImVec2(110,25)) && mpUpdateInfo->pickedItem != ""){
        mpUpdateInfo->drop = true;
    }
    ImGui::End();
}
