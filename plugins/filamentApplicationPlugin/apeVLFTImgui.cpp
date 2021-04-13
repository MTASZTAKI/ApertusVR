#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "apeVLFTImgui.h"
#include "NativeWindowHelper.h"
#ifdef __APPLE__
#include <sys/stat.h>
#endif

ape::VLFTImgui::VLFTImgui( )
{
    mpSceneNetwork = ape::ISceneNetwork::getSingletonPtr();
    mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
    mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
    mpPluginManager = ape::IPluginManager::getSingletonPtr();
    mpSceneManager = ape::ISceneManager::getSingletonPtr();
}

ape::VLFTImgui::~VLFTImgui()
{
}

void ape::VLFTImgui::init(updateInfo *updateinfo)
{
    std::stringstream fileFullPath;
    fileFullPath << "C:/ApertusVR/samples/virtualLearningFactory/apeVLFTSceneLoaderPlugin.json";
    mApeVLFTSceneLoaderPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
    mScene = nlohmann::json::parse(mApeVLFTSceneLoaderPluginConfigFile);
    mpUpdateInfo = updateinfo;
    mpUpdateInfo->keyLabel["w"] = "w";
    mpUpdateInfo->keyLabel["s"] = "s";
    mpUpdateInfo->keyLabel["a"] = "a";
    mpUpdateInfo->keyLabel["d"] = "d";
    mpUpdateInfo->keyLabel["e"] = "e";
    mpUpdateInfo->keyLabel["q"] = "q";
    for(int i = 0; i < 500; i++){
        mpUpdateInfo->playedAnimation.push_back(false);
    }
    mpMainMenuInfo.admin = mpUpdateInfo->isAdmin;
    chatMessages.push_back(u8"Elso uzenet bla bla");
    chatMessages.push_back(u8"Masodik uzenet bla bla");
    chatMessages.push_back(u8"Harmadik uzenet bla bla");
    chatMessages.push_back(u8"Makosteszte krumplipürével");
    chatMessages.push_back(u8"A hangya lassú sakkban");
    chatMessages.push_back(u8"Ha van 5 almád és nekem van 12 székem akkor hány palacsinta fér el a teton?");
    chatMessages.push_back(u8"Egyse mert az űrlények nem hordanak kalapot!");
    std::fclose(mApeVLFTSceneLoaderPluginConfigFile);
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
                connectToRoom();
                mpMainMenuInfo.current_selected = -1;
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

void ape::VLFTImgui::connectToRoom(){
    std::string roomName = mpMainMenuInfo.roomNames[mpMainMenuInfo.current_selected];
    if((mpMainMenuInfo.multiPlayer || (mpMainMenuInfo.adminMenu && mpMainMenuInfo.running_rooms[mpMainMenuInfo.current_selected])) && !mpMainMenuInfo.inRoomGui){
        std::string urlAnimationConfig = "http://srv.mvv.sztaki.hu/temp/vlft/virtualLearningFactory/rooms/" + roomName + "/apeVLFTSceneLoaderPlugin.json";
        std::string locationAnimationConfig = mpCoreConfig->getConfigFolderPath() + "/apeVLFTSceneLoaderPlugin.json";
        std::vector<std::string> urls;
        std::vector<std::string> locations;
        urls.push_back(urlAnimationConfig);
        locations.push_back(locationAnimationConfig);
        mpSceneNetwork->connectToRoom(roomName, urls, locations);
        //mpSceneNetwork->
        mpMainMenuInfo.inRoomGui = true;
        mpUpdateInfo->inRoom = true;
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
        
        mpMainMenuInfo.inRoomGui = true;
        mpUpdateInfo->inRoom = true;
    }
    mpUpdateInfo->leftRoom = false;
    mpUpdateInfo->setUpRoom = true;
}

void ape::VLFTImgui::createJoinButton(std::string userType,int width, int height){
    if (ImGui::Button("Join",ImVec2(width/8, height/15))){
        connectToRoom();
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
    if(mpMainMenuInfo.mainMenu){
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25));
        if (ImGui::Button("SinglePlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.singlePlayer = true;
            mpMainMenuInfo.mainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/12+12));
        if (ImGui::Button("MultiPlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.multiPlayer = true;
            mpMainMenuInfo.mainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
    }
    else if(mpMainMenuInfo.singlePlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(width/6, height/14))) {
            curlData();
        }
        listRoomNames(false);
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Student", width, height);
        ImGui::SameLine(ImGui::GetWindowWidth()-205);
        if (ImGui::Button("Back",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = false;
            mpMainMenuInfo.mainMenu = true;
        }
    }
    else if(mpMainMenuInfo.multiPlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(width/6, height/14))) {
            curlData();
        }
        for (int n = 0; n < mpMainMenuInfo.roomNames.size(); n++)
        {
            if (ImGui::Selectable(mpMainMenuInfo.roomNames[n].c_str(), mpMainMenuInfo.current_selected == n))
                mpMainMenuInfo.current_selected = n;
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)){
                    connectToRoom();
                    mpMainMenuInfo.current_selected = -1;
                }
        }
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Student", width, height);
        ImGui::SameLine(ImGui::GetWindowWidth()-205);
        if (ImGui::Button("Back",ImVec2(200, 50))){
            mpMainMenuInfo.singlePlayer = false;
            mpMainMenuInfo.mainMenu = true;
        }
    }
    mpMainMenuInfo.sideBarWidth = ImGui::GetWindowWidth();
    ImGui::End();
}

bool ape::VLFTImgui::createSettingsMenu(int width, int height){
    mpUpdateInfo->inSettings = true;
    ImGui::SetCursorPos(ImVec2(8,40));
    ImGui::Text("Forward");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button(mpUpdateInfo->keyLabel["w"].c_str(),ImVec2(40, 20)) && !mpUpdateInfo->changedKey){
        mpUpdateInfo->changeKeyCode = "w";
        mpUpdateInfo->changedKey = true;
    }
    ImGui::Text("Backward");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button(mpUpdateInfo->keyLabel["s"].c_str(),ImVec2(40, 20)) && !mpUpdateInfo->changedKey){
        mpUpdateInfo->changeKeyCode = "s";
        mpUpdateInfo->changedKey = true;
    }
    ImGui::Text("Left");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button(mpUpdateInfo->keyLabel["a"].c_str(),ImVec2(40, 20)) && !mpUpdateInfo->changedKey){
        mpUpdateInfo->changeKeyCode = "a";
        mpUpdateInfo->changedKey = true;
    }
    ImGui::Text("Right");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button(mpUpdateInfo->keyLabel["d"].c_str(),ImVec2(40, 20)) && !mpUpdateInfo->changedKey){
        mpUpdateInfo->changeKeyCode = "d";
        mpUpdateInfo->changedKey = true;
    }
    ImGui::Text("Up");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button(mpUpdateInfo->keyLabel["e"].c_str(),ImVec2(40, 20)) && !mpUpdateInfo->changedKey){
        mpUpdateInfo->changeKeyCode = "e";
        mpUpdateInfo->changedKey = true;
    }
    ImGui::Text("Down");
    ImGui::SameLine(width/3*2);
    if(ImGui::Button(mpUpdateInfo->keyLabel["q"].c_str(),ImVec2(40, 20)) && !mpUpdateInfo->changedKey){
        mpUpdateInfo->changeKeyCode = "q";
        mpUpdateInfo->changedKey = true;
    }
    ImGui::SetCursorPos(ImVec2(width-(width/8+48),height-(height/15+50)));
    if (ImGui::Button("Back",ImVec2(width/8, height/15))){
        mpMainMenuInfo.settingsMenu = false;
        mpUpdateInfo->inSettings = false;
        return true;
    }
    return false;
}

void ape::VLFTImgui::adminRoomGUI(){
    
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(width-40, height-40));
    ImGui::Begin("Admin", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
   
    ImGui::GetStyle().Alpha = 0.95;
    if(mpMainMenuInfo.mainMenu){
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25));
        if (ImGui::Button("SinglePlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.singlePlayer = true;
            mpMainMenuInfo.mainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/12+12));
        if (ImGui::Button("MultiPlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.multiPlayer = true;
            mpMainMenuInfo.mainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/6+24));
        if (ImGui::Button("Admin Room",ImVec2(width/5, height/12))){
            mpMainMenuInfo.adminMenu = true;
            mpMainMenuInfo.mainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/4+36));
        if (ImGui::Button("Settings",ImVec2(width/5, height/12))){
            mpMainMenuInfo.adminMenu = false;
            mpMainMenuInfo.mainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
            mpMainMenuInfo.settingsMenu = true;
        }
    }
    else if(mpMainMenuInfo.singlePlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(width/6, height/14))) {
            curlData();
        }
        listRoomNames(false);
        ImGui::SetCursorPos(ImVec2(5,height-(height/15+50)));
        createJoinButton("Teacher", width, height);
        ImGui::SameLine(width-(width/8+48));
        if (ImGui::Button("Back",ImVec2(width/8, height/15))){
            mpMainMenuInfo.singlePlayer = false;
            mpMainMenuInfo.mainMenu = true;
        }
    }
    else if(mpMainMenuInfo.multiPlayer){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(width/6, height/14))) {
            curlData();
        }
        listRoomNames(true);
        ImGui::SetCursorPos(ImVec2(5,height-(height/15+50)));
        createJoinButton("Teacher", width, height);
        ImGui::SameLine(width-(width/8+48));
        if (ImGui::Button("Back",ImVec2(width/8, height/15))){
            mpMainMenuInfo.mainMenu = true;
            mpMainMenuInfo.multiPlayer = false;
        }
    }
    else if(mpMainMenuInfo.adminMenu){
        if(!mpMainMenuInfo.namesLoaded){
            curlData();
            mpMainMenuInfo.namesLoaded = true;
        }
        if (ImGui::Button("Refresh",ImVec2(width/6, height/14))) {
            curlData();
        }
        listRoomNames(true);
        ImGui::SetCursorPos(ImVec2(5,height-(height/15+50)));
        createStartButton(width, height);
        ImGui::SameLine(width/8+10);
        createStopButton(width, height);
        ImGui::SameLine(2*width/8+15);
        createJoinButton("Teacher", width, height);
        ImGui::SameLine(3*width/8+20);
        if (ImGui::Button("Upload",ImVec2(width/8, height/15))){
            ;
        }
        ImGui::SameLine(width-(width/8+48));
        if (ImGui::Button("Back",ImVec2(width/8, height/15))){
            mpMainMenuInfo.mainMenu = true;
            mpMainMenuInfo.adminMenu = false;
        }
    }
    else if(mpMainMenuInfo.settingsMenu){
        mpMainMenuInfo.mainMenu = createSettingsMenu(width, height);
        
    }
    if(mpUpdateInfo->changedKey){
        const float width = ImGui::GetIO().DisplaySize.x;
        const float height = ImGui::GetIO().DisplaySize.y;
        ImGui::SetNextWindowSize(ImVec2(200, 50));
        ImGui::SetNextWindowPos(ImVec2(width/2-100, height/2-25));
        ImGui::Begin("Press a key");
        ImGui::Text("Press a key to set the conrtol");
        ImGui::End();
    }
    mpMainMenuInfo.sideBarWidth = ImGui::GetWindowWidth();
    ImGui::End();
}
void ape::VLFTImgui::loginGUI(){
    ImGui::SetNextWindowPos(ImVec2(20, 20));
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(width-40, height-40));
    ImGui::Begin("Login", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::Checkbox("Teacher", &mpUpdateInfo->isAdmin);
    static char email[255] = u8"";
    ImGui::SetCursorPos(ImVec2(width/2-120, height/2-90));
    ImGui::PushItemWidth(200);
    ImGui::InputText(u8"email", email, IM_ARRAYSIZE(email));
    static char password[255] = u8"";
    if(mpUpdateInfo->isAdmin){
        ImGui::SetCursorPos(ImVec2(width/2-120, height/2-60));
        ImGui::PushItemWidth(200);
        ImGui::InputText(u8"password", password, IM_ARRAYSIZE(password),ImGuiInputTextFlags_Password);
        ImGui::SetCursorPos(ImVec2(width/2-70, height/2-25));
        if(ImGui::Button("Log in", ImVec2(100,30))){
            mpUpdateInfo->userName = email;
            mpUpdateInfo->password = password;
            mpUpdateInfo->checkLogin = true;
        }
    }
    else{
        ImGui::SetCursorPos(ImVec2(width/2-70, height/2-25));
        if(ImGui::Button("Log in", ImVec2(100,30))){
            mpUpdateInfo->userName = email;
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

void ape::VLFTImgui::update(){
  
    if(mpMainMenuInfo.loginMenu)
        loginGUI();
    else if(mpMainMenuInfo.admin && !mpMainMenuInfo.inRoomGui)
        adminRoomGUI();
    else if(!mpMainMenuInfo.admin && !mpMainMenuInfo.inRoomGui)
        studentRoomGUI();
    else if(mpMainMenuInfo.inRoomGui){
        leftPanelGUI();
        if(mpMainMenuInfo.showStates){
            statePanelGUI();
        }
        rightPanelGUI();
    }
}


void ape::VLFTImgui::statePanelGUI(){
    ImGui::SetNextWindowPos(ImVec2(0, 0),ImGuiCond_Once);
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(270, 100), ImGuiCond_Once);
    ImGui::Begin("State panel", nullptr);
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
        }
    ImGui::SetScrollHere(1.0f);
    ImGui::End();
}

void ape::VLFTImgui::leftPanelGUI() {
    ImGui::SetNextWindowPos(ImVec2(0, 0),ImGuiCond_Once);
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(116, 325), ImGuiCond_Once);
    ImGui::Begin("Left panel", nullptr);
    if(ImGui::Button("Delete",ImVec2(100,25)) && mpUpdateInfo->pickedItem == "")
    {
        mpUpdateInfo->deleteSelected = true;
    }
    if(mpUpdateInfo->pickedItem == ""){
        if(ImGui::Button("Pick up",ImVec2(100,25)))
        {
            mpUpdateInfo->pickUp = true;
        }
    }
    else{
        if(ImGui::Button("Put down",ImVec2(100,25)))
        {
            mpUpdateInfo->pickUp = true;
        }
    }
    if(ImGui::Button("Drop", ImVec2(100,30)) && mpUpdateInfo->pickedItem != ""){
        mpUpdateInfo->drop = true;
    }
    if(ImGui::Button("Browse files",ImVec2(100,25)))
    {
        openFileBrowser();
        
    }
    if(ImGui::Button("Leave room",ImVec2(100,25)))
    {
       //TODO delete all object;
        mpSceneNetwork->leave();
        mpUpdateInfo->leftRoom = true;
        mpMainMenuInfo.inRoomGui = false;
        mpUpdateInfo->inRoom = false;
        mpMainMenuInfo.adminMenu = true;
        
    }
    std::string playText="Play";
    if(mpUpdateInfo->IsPlayClicked)
        playText = "Pause";
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
        mpUpdateInfo->pauseTime = 0;
    }
    if(ImGui::Button("Setting",ImVec2(100,25))){
        mpMainMenuInfo.settingsMenu = true;
    }
    if(!mpUpdateInfo->usersAttached){
        if(ImGui::Button("Attach users",ImVec2(100,25))){
            mpUpdateInfo->attachUsers = true;
        }
    }
    else{
        if(ImGui::Button("Detach users",ImVec2(100,25))){
            mpUpdateInfo->attachUsers = true;
        }
    }
    ImGui::Checkbox("show_headers", &mpMainMenuInfo.showStates);
    if(mpMainMenuInfo.settingsMenu){
        ImGui::SetNextWindowPos(ImVec2(5, 5));
        ImGui::SetNextWindowSize(ImVec2(width-10, height-10));
        void  SetNextWindowFocus();
        ImGui::Begin("SettingsWindow", nullptr,ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        createSettingsMenu(width, height);
        ImGui::End();
    }
    if(mpUpdateInfo->changedKey){
        const float width = ImGui::GetIO().DisplaySize.x;
        const float height = ImGui::GetIO().DisplaySize.y;
        ImGui::SetNextWindowSize(ImVec2(200, 50));
        ImGui::SetNextWindowPos(ImVec2(width/2-100, height/2-25));
        ImGui::Begin("Press a key");
        ImGui::Text("Press a key to set the conrtol");
        ImGui::End();
    }
    ImGui::End();
}

void ape::VLFTImgui::rightPanelGUI() {
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowPos(ImVec2(width-251, 0),ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Once);
    ImGui::Begin("Right panel", nullptr);
    
    getInfoAboutObject(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
    ImGui::End();
    
    if(mpUpdateInfo->newChatMessage){
        chatMessages.insert(chatMessages.end(), mpUpdateInfo->newMessage.begin(), mpUpdateInfo->newMessage.end());
        mpUpdateInfo->newMessage.clear();
        mpUpdateInfo->newChatMessage = false;
    }
    ImGui::SetNextWindowPos(ImVec2(0, 320),ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 150), ImVec2(500, 500));
    ImGui::Begin("Chat panel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
    ImGui::BeginChild("Chat region", ImVec2(ImGui::GetWindowWidth()-18, ImGui::GetWindowHeight()-45),ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushTextWrapPos(0.0f);
    if(mpUpdateInfo->sendMessage){
        ImGui::SetScrollHere(1.0f);
    }
    std::string chatText = u8"";
    for(size_t i = 0; i < chatMessages.size()-1; ++i){
        chatText += chatMessages[i] + "\n";
    }
    chatText += chatMessages[chatMessages.size()-1];
    ImGui::TextWrapped("%s", chatText.c_str());
    if(messageInBuffer){
        ImGui::SetScrollHere(1.0f);
        messageInBuffer= false;
    }
    ImGui::PopTextWrapPos();
    ImGui::EndChild();
    static char str0[255] = u8"";
    ImGui::SetCursorPos(ImVec2(8,ImGui::GetWindowHeight()-30));
    ImGui::PushItemWidth(ImGui::GetWindowWidth()-18);
    if(ImGui::InputText(u8"", str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue)){
        
        if(strlen(str0)!=0){
            chatMessages.push_back(str0);
            mpUpdateInfo->messageToSend = str0;
            mpUpdateInfo->sendMessage = true;
            messageInBuffer = true;
        }
        str0[0] = 0;
    }
    ImGui::End();

}

void ape::VLFTImgui::openFileBrowser() {
    /*char* filePath;
    nfdresult_t result = NFD_OpenDialog( "gltf,glb", NULL, &filePath );
    if ( result == NFD_OKAY )
    {
        APE_LOG_DEBUG("Success! "<<filePath);
        std::string fileName = filePath;
        fileName = fileName.substr(fileName.find_last_of("/")+1);
        std::string nodeName = "gltfNode_"+fileName;
        std::string entityName = "gltfEntity_"+fileName;
        auto mNode = mpSceneManager->createNode(nodeName, true, mpCoreConfig->getNetworkGUID());
        if (auto node = mNode.lock())
        {
            if (auto gltfMeshFile = std::static_pointer_cast<ape::IFileGeometry>(mpSceneManager->createEntity(entityName, ape::Entity::GEOMETRY_FILE, true, mpCoreConfig->getNetworkGUID()).lock()))
            {
                gltfMeshFile->setFileName(filePath);
                gltfMeshFile->setParentNode(node);
            }
        }
        std::string cloneName = "Clone1_"+fileName;
        std::string cloneEntityName = "Clone1Entity_"+fileName;
        auto mCloneNode = mpSceneManager->createNode(cloneName, true, mpCoreConfig->getNetworkGUID());
        if(auto cloneNode = mCloneNode.lock()){
            if (auto geometryClone = std::static_pointer_cast<ape::ICloneGeometry>(mpSceneManager->createEntity(cloneEntityName, ape::Entity::GEOMETRY_CLONE, true, mpCoreConfig->getNetworkGUID()).lock()))
            {
                geometryClone->setSourceGeometryGroupName(entityName);
                geometryClone->setParentNode(cloneNode);
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
    }*/
}

static std::string convertVecToString(std::vector<double> vec,int  precision){
    std::string convertedVec ="(";
    std::stringstream posStream;
    posStream << std::fixed << std::setprecision(precision) << vec[0] << ", " << vec[1] << ", " << vec[2] << ")";
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
            if(asset.get_id() == mpUpdateInfo->selectedItem){
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
    ImGui::Text("%s", rootIDText.c_str());
    ImGui::Text("%s", rootPosText.c_str());
    ImGui::PushItemWidth((width-90)/3);
    ImGui::InputDouble("x", &posX);
    ImGui::SameLine();
    ImGui::InputDouble("y", &posY);
    ImGui::SameLine();
    ImGui::InputDouble("z", &posZ);
    ImGui::PopItemWidth();
    if(ImGui::Button("Set position") && mpUpdateInfo->selectedItem != ""){
        mpUpdateInfo->position = Vector3(posX, posY, posZ);
        mpUpdateInfo->setPosition = true;
    }
    ImGui::PushItemWidth((width)/3);
    ImGui::InputDouble("Radian", &rotAngle);
    ImGui::InputDouble("upX", &upX);
    //ImGui::SameLine();
    ImGui::InputDouble("upY", &upY);
    // ImGui::SameLine();
    ImGui::InputDouble("upZ", &upZ);
    ImGui::PopItemWidth();
    if(ImGui::Button("Set rotation") && mpUpdateInfo->selectedItem != ""){
        mpUpdateInfo->orientation = ape::Quaternion(ape::Radian(rotAngle), Vector3(upX, upY, upZ));
        mpUpdateInfo->setRotation = true;
    }
    ImGui::TextWrapped("%s", desrcText.c_str());
    ImGui::Text("%s", stateText.c_str());
    ImGui::Text("%s", logText.c_str());
    ImGui::SetCursorPosX(width/2-30);
    ImGui::Text("Links");
    for(auto link : mpUpdateInfo->animationLinks){
        ImGui::SetCursorPosX(15);
        ImGui::Text("%s: %s", link.first.c_str(),link.second.c_str());
    }
    if(!mpUpdateInfo->mIsStudentsMovementLogging){
        if(ImGui::Button("Log movements")){
            mpUpdateInfo->logMovements = true;
        }
    }else{
        if(ImGui::Button("Stop movement logging")){
            mpUpdateInfo->logMovements = true;
        }
    }
    ImGui::EndChild();
    
}
