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
    mpSceneManager = ape::ISceneManager::getSingletonPtr();
}

ape::VLFTImgui::~VLFTImgui()
{
}

void ape::VLFTImgui::init(updateInfo *updateinfo)
{
    std::stringstream fileFullPath;
    fileFullPath << mpCoreConfig->getConfigFolderPath() << "/apeVLFTSceneLoaderPlugin.json";
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
    if(mpUpdateInfo->isAdmin)
        mpMainMenuInfo.admin = true;
    else
        mpMainMenuInfo.student = true;
    chatMessages.push_back(u8"Elso uzenet bla bla");
    chatMessages.push_back(u8"Masodik uzenet bla bla");
    chatMessages.push_back(u8"Harmadik uzenet bla bla");
    chatMessages.push_back(u8"Makosteszte krumplipürével");
    chatMessages.push_back(u8"A hangya lassú sakkban");
    chatMessages.push_back(u8"Ha van 5 almád és nekem van 12 székem akkor hány palacsinta fér el a teton?");
    chatMessages.push_back(u8"Egyse mert az űrlények nem hordanak kalapot!");
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
    if(mpMainMenuInfo.inMainMenu){
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25));
        if (ImGui::Button("SinglePlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.singlePlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/12+12));
        if (ImGui::Button("MultiPlayer",ImVec2(width/5, height/12))){
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
        if (ImGui::Button("Refresh",ImVec2(width/6, height/14))) {
            curlData();
        }
        listRoomNames(false);
        ImGui::SetCursorPos(ImVec2(5,height-110));
        createJoinButton("Student", width, height);
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
            mpMainMenuInfo.inMainMenu = true;
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
    if(mpMainMenuInfo.inMainMenu){
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25));
        if (ImGui::Button("SinglePlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.singlePlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/12+12));
        if (ImGui::Button("MultiPlayer",ImVec2(width/5, height/12))){
            mpMainMenuInfo.multiPlayer = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/6+24));
        if (ImGui::Button("Admin Room",ImVec2(width/5, height/12))){
            mpMainMenuInfo.adminMenu = true;
            mpMainMenuInfo.inMainMenu = false;
            mpMainMenuInfo.namesLoaded = false;
        }
        ImGui::SetCursorPos(ImVec2(width/2-width/10, height*0.25+height/4+36));
        if (ImGui::Button("Settings",ImVec2(width/5, height/12))){
            mpMainMenuInfo.adminMenu = false;
            mpMainMenuInfo.inMainMenu = false;
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
            mpMainMenuInfo.inMainMenu = true;
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
            mpMainMenuInfo.inMainMenu = true;
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
            mpMainMenuInfo.inMainMenu = true;
            mpMainMenuInfo.adminMenu = false;
        }
    }
    else if(mpMainMenuInfo.settingsMenu){
        mpMainMenuInfo.inMainMenu = createSettingsMenu(width, height);
        
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
    ImGui::SetNextWindowPos(ImVec2(0, 0),ImGuiCond_Once);
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    ImGui::SetNextWindowSize(ImVec2(116, 250), ImGuiCond_Once);
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
        mpMainMenuInfo.inRoomGui = false;
        mpUpdateInfo->inRoom = false;
        mpMainMenuInfo.adminMenu = true;
        
    }
    if(ImGui::Button("Play",ImVec2(100,25)))
    {
        if (!mpUpdateInfo->isPlayRunning)
        {
            mpUpdateInfo->IsPlayClicked = true;
            mpUpdateInfo->ChoosedBookmarkedAnimationID = 0;
            mpUpdateInfo->ClickedBookmarkTime = 0;
            mpUpdateInfo->BookmarkID = -1;
            mpUpdateInfo->TimeToSleepFactor = 1.0f;
            mpUpdateInfo->IsPauseClicked = false;
            mpUpdateInfo->IsStopClicked = false;
        }else{
            mpUpdateInfo->IsPauseClicked = true;
            mpUpdateInfo->IsPlayClicked = false;
            mpUpdateInfo->isPlayRunning = false;
            
        }
    }
    if(ImGui::Button("Setting",ImVec2(100,25))){
        mpMainMenuInfo.settingsMenu = true;
    }
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
    
    
    ImGui::SetNextWindowPos(ImVec2(0, 320),ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 150), ImVec2(500, 500));
    ImGui::Begin("Chat panel", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
    ImGui::BeginChild("Chat region", ImVec2(ImGui::GetWindowWidth()-18, ImGui::GetWindowHeight()-45),ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushTextWrapPos(0.0f);
    if(mpUpdateInfo->newChatMessage){
        ImGui::SetScrollHere(1.0f);
    }
    std::string chatText = u8"";
    for(size_t i = 0; i < chatMessages.size()-1; ++i){
        chatText += chatMessages[i] + "\n";
    }
    chatText += chatMessages[chatMessages.size()-1];
    ImGui::TextWrapped(chatText.c_str());
    if(mpUpdateInfo->newChatMessage){
        ImGui::SetScrollHere(1.0f);
        mpUpdateInfo->newChatMessage = false;
    }
    ImGui::PopTextWrapPos();
    ImGui::EndChild();
    static char str0[255] = u8"";
    ImGui::SetCursorPos(ImVec2(8,ImGui::GetWindowHeight()-30));
    ImGui::PushItemWidth(ImGui::GetWindowWidth()-18);
    if(ImGui::InputText(u8"", str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue)){
        chatMessages.push_back(str0);
        mpUpdateInfo->newChatMessage = true;
        str0[0] = 0;
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
    }
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
    ImGui::EndChild();
    
}
