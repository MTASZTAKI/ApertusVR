#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include <curl/curl.h>
#include <curl/easy.h>
#ifdef __APPLE__
#include <sys/stat.h>
#include "NativeWindowHelper.h"
#include "apeVLFTImgui.h"
#endif

ape::VLFTImgui::VLFTImgui( )
{
    mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
    mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
    mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
    mpPluginManager = ape::IPluginManager::getSingletonPtr();
}

ape::VLFTImgui::~VLFTImgui()
{
}

void ape::VLFTImgui::init(bool isAdmin)
{
    if(isAdmin)
        mpMainMenuInfo.admin = true;
    else
        mpMainMenuInfo.student = true;
}


void ape::VLFTImgui::tooltipFloat(std::string tooltipText){
    if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
        ImGui::SetTooltip(tooltipText.c_str());
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
        if (ImGui::Selectable(mpMainMenuInfo.roomNames[n].c_str(), mpMainMenuInfo.current_selected == n))
            mpMainMenuInfo.current_selected = n;
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

void ape::VLFTImgui::createStopButton(){
    if (ImGui::Button("Stop",ImVec2(185, 50))){
        if(mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected]){
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
void ape::VLFTImgui::createJoinButton(std::string userType){
    if (ImGui::Button("Join",ImVec2(200, 50))){
        std::string roomName = mpMainMenuInfo.roomNames[mpMainMenuInfo.current_selected];
        if(mpMainMenuInfo.multiPlayer){
            std::string urlAnimationConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
            std::string locationAnimationConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
            std::vector<std::string> urls;
            std::vector<std::string> locations;
            urls.push_back(urlAnimationConfig);
            locations.push_back(locationAnimationConfig);
            mpSceneNetwork->connectToRoom(roomName, urls, locations);
            mpMainMenuInfo.inRoomGui = true;
        }
        else if(mpMainMenuInfo.singlePlayer){
            std::string urlSceneConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
            std::string locationSceneConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
            std::vector<std::string> urls;
            std::vector<std::string> locations;
            urls.push_back(urlSceneConfig);
            locations.push_back(locationSceneConfig);
            mpSceneNetwork->downloadConfigs(urls, locations);
            mpPluginManager->loadPlugin("apeVLFTSceneLoaderPlugin");
            
        }
    }
}
void ape::VLFTImgui::createStartButton(){
    if (ImGui::Button("Start",ImVec2(185, 50))){
        if(!mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected]){
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
    return false;
}

void ape::VLFTImgui::uploadGUI(){
//    const float width = ImGui::GetIO().DisplaySize.x;
//    const float height = ImGui::GetIO().DisplaySize.y;
//    ImGui::SetNextWindowPos(ImVec2(width-300, 0),ImGuiCond_FirstUseEver);
//    ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);
//    ImGui::SetNextWindowSizeConstraints(ImVec2(150, 70), ImVec2(width, height));
//    ImGui::Begin("Upload", nullptr);
//    ImGui::Text("Enter the name of the room for uploading");
//    ImGui::InputTextWithHint("", "write room name here", mpMainMenuInfo.uploadRoomName, IM_ARRAYSIZE(mpMainMenuInfo.uploadRoomName));
//    ImGui::End();
}

void ape::VLFTImgui::studentRoomGUI(){
       
    ImGui::SetNextWindowPos(ImVec2(20, 20));

    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(width-50, height-50));
    ImGui::Begin("Student", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::GetStyle().Alpha = 0.95;
    if(mpMainMenuInfo.inMainMenu){
        ImGui::SetCursorPos(ImVec2(width/2-100, height*0.25));
        if (ImGui::Button("SinglePlayer",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-100, height*0.25+70));
        if (ImGui::Button("MultiPlayer",ImVec2(200, 50))){
            mpMainMenuInfo.multiPlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
    }
    else if(mpMainMenuInfo.singlePlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(200, 50))) {
            curlData();
        }
        for (int n = 0; n < mpMainMenuInfo.roomNames.size(); n++)
        {
            if (ImGui::Selectable(mpMainMenuInfo.roomNames[n].c_str(), mpMainMenuInfo.current_selected == n))
                mpMainMenuInfo.current_selected = n;
        }
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Student");
        ImGui::SameLine(ImGui::GetWindowWidth()-205);
        if (ImGui::Button("Back",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = false;
            mpMainMenuInfo.inMainMenu = true;
        }
    }
    else if(mpMainMenuInfo.multiPlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(200, 50))) {
            curlData();
        }
        for (int n = 0; n < mpMainMenuInfo.roomNames.size(); n++)
        {
            if (ImGui::Selectable(mpMainMenuInfo.roomNames[n].c_str(), mpMainMenuInfo.current_selected == n))
                mpMainMenuInfo.current_selected = n;
        }
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Student");
        ImGui::SameLine(ImGui::GetWindowWidth()-205);
        if (ImGui::Button("Back",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = false;
            mpMainMenuInfo.inMainMenu = true;
        }
    }
    mpMainMenuInfo.sideBarWidth = ImGui::GetWindowWidth();
    ImGui::End();
}

void ape::VLFTImgui::adminRoomGUI(){
    
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(width-40, height-40));
    ImGui::Begin("Admin", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::GetStyle().Alpha = 0.95;
    if(mpMainMenuInfo.inMainMenu){
        ImGui::SetCursorPos(ImVec2(width/2-100, height*0.25));
        if (ImGui::Button("SinglePlayer",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-100, height*0.25+70));
        if (ImGui::Button("MultiPlayer",ImVec2(200, 50))){
            mpMainMenuInfo.multiPlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-100, height*0.25+140));
        if (ImGui::Button("Admin Room",ImVec2(200, 50))){
            mpMainMenuInfo.adminMenu = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
    }
    else if(mpMainMenuInfo.singlePlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(200, 50))) {
            curlData();
        }
        listRoomNames(false);
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Teacher");
        ImGui::SameLine(width-235);
        if (ImGui::Button("Back",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = false;
            mpMainMenuInfo.inMainMenu = true;
        }
    }
    else if(mpMainMenuInfo.multiPlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(200, 50))) {
            curlData();
        }
        listRoomNames(true);
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Teacher");
        ImGui::SameLine(width-235);
        if (ImGui::Button("Back",ImVec2(200, 50))){
            mpMainMenuInfo.inMainMenu = true;
            mpMainMenuInfo.multiPlayer = false;
        }
    }
    else{
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(200, 50))) {
            curlData();
        }
        listRoomNames(true);
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createStartButton();
        ImGui::SameLine(195);
        createStopButton();
        ImGui::SameLine(385);
        if (ImGui::Button("Upload Room",ImVec2(185, 50))){
            ;
        }
        ImGui::SameLine(575);
        if (ImGui::Button("Upload Resources",ImVec2(185, 50))){
            ;
        }
        ImGui::SameLine(width-233);
        if (ImGui::Button("Back",ImVec2(185, 50))){
            mpMainMenuInfo.inMainMenu = true;
            mpMainMenuInfo.adminMenu = false;
        }
    }
    mpMainMenuInfo.sideBarWidth = ImGui::GetWindowWidth();
    ImGui::End();
}

void ape::VLFTImgui::update(){
    if(mpMainMenuInfo.admin && !mpMainMenuInfo.inRoomGui)
        adminRoomGUI();
    else if(mpMainMenuInfo.student && !mpMainMenuInfo.inRoomGui)
        studentRoomGUI();
    else if(mpMainMenuInfo.inRoomGui){
        leftPanelGUI();
        rightPanelGUI();
    }
}
void ape::VLFTImgui::leftPanelGUI() {
    ;
}


void ape::VLFTImgui::rightPanelGUI() { 
    ;
}

