#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include <time.h>
#include "apeVLFTAnimationPlayerPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD wndPid;
	GetWindowThreadProcessId(hwnd, &wndPid);
	int len = GetWindowTextLength(hwnd) + 1;
	std::string s;
	s.reserve(len);
	GetWindowText(hwnd, const_cast<char*>(s.c_str()), len - 1);
	if (wndPid == (DWORD)lParam)
	{
		::PostMessage(hwnd, WM_CLOSE, 0, 0);
		return false;
	}
	else
	{
		return true;
	}
}

bool compareAnimationTime(ape::VLFTAnimationPlayerPlugin::Animation animation1, ape::VLFTAnimationPlayerPlugin::Animation animation2)
{
	return (animation1.time < animation2.time);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) 
{
	using namespace Gdiplus;
	UINT  num = 0;
	UINT  size = 0;
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return 0;
}

void gdiscreen() 
{
	using namespace Gdiplus;
	IStream* istream;
	HRESULT res = CreateStreamOnHGlobal(NULL, true, &istream);
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(0);
		int Height = GetSystemMetrics(SM_CYSCREEN);
		int Width = GetSystemMetrics(SM_CXSCREEN);
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
		Gdiplus::Bitmap bitmap(membit, NULL);
		CLSID clsid;
		GetEncoderClsid(L"image/jpeg", &clsid);
		std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		auto fileName = "../../screenshots/" + std::to_string(uuid.count()) + ".jpg";
		std::wstring wFileName = std::wstring(fileName.begin(), fileName.end());
		bitmap.Save(wFileName.c_str(), &clsid, NULL);
	}
	GdiplusShutdown(gdiplusToken);
	istream->Release();
}

ape::VLFTAnimationPlayerPlugin::VLFTAnimationPlayerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&VLFTAnimationPlayerPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::BROWSER, std::bind(&VLFTAnimationPlayerPlugin::eventCallBack, this, std::placeholders::_1));
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpSceneMakerMacro = new ape::SceneMakerMacro();
	mParsedAnimations = std::vector<Animation>();
	mClickedNode = ape::NodeWeakPtr();
	mParsedBookmarkTimes = std::vector<unsigned long long>();
	mChoosedBookmarkedAnimationID = 0;
	mBookmarkID = -1;
	mClickedBookmarkTime = 0;
	mTimeToSleepFactor = 1.0f;
	mIsPauseClicked = false;
	mIsStopClicked = false;
	mIsPlayRunning = false;
	mAnimatedNodeNames = std::vector<std::string>();
	mAttachedUsers = std::vector<ape::NodeWeakPtr>();
	mAttach2NewAnimationNode = std::vector<ape::NodeWeakPtr>();
	mIsStudentsMovementLogging = false;
	mStudents = std::vector<ape::NodeWeakPtr>();
	mStudentsMovementLoggingFile = std::ofstream();
	APE_LOG_FUNC_LEAVE();
}

ape::VLFTAnimationPlayerPlugin::~VLFTAnimationPlayerPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::drawSpaghettiSection(const ape::Vector3& startPosition, const ape::NodeSharedPtr& node, std::string& spaghettiSectionName)
{
	if (auto spaghettiNode = mpSceneManager->getNode(node->getName() + "_spaghettiNode").lock())
	{
		auto currentPosition = node->getDerivedPosition();
		//APE_LOG_DEBUG("startPosition: " << startPosition.toString());
		//APE_LOG_DEBUG("currentPosition: " << currentPosition.toString());
		std::chrono::nanoseconds uuid = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
		if (auto spagetthiLineSection = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity(std::to_string(uuid.count()), ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
		{
			ape::GeometryCoordinates coordinates = {
				startPosition.x, startPosition.y, startPosition.z,
				currentPosition.x, currentPosition.y, currentPosition.z, };
			ape::GeometryIndices indices = { 0, 1, -1 };
			ape::Color color(1, 0, 0);
			spagetthiLineSection->setParameters(coordinates, indices, color);
			spagetthiLineSection->setParentNode(spaghettiNode);
			spaghettiSectionName = spagetthiLineSection->getName();
		}
	}
}

bool ape::VLFTAnimationPlayerPlugin::attach2NewAnimationNode(const std::string& parentNodeName, const ape::NodeSharedPtr& node)
{
	if (auto newParentNode = mpSceneManager->getNode(parentNodeName).lock())
	{
		auto currentParentNode = node->getParentNode().lock();
		if (newParentNode != currentParentNode)
		{
			node->setParentNode(newParentNode);
			mAttach2NewAnimationNode.push_back(node);
			return true;
		}
	}
	return false;
}

void ape::VLFTAnimationPlayerPlugin::playAnimation()
{
	mIsPlayRunning = true;
	std::vector<std::string> spaghettiNodeNames;
	std::vector<std::string> spaghettiLineNames;
	for (auto animatedNodeName : mAnimatedNodeNames)
	{
		if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
		{
			node->setOwner(mpCoreConfig->getNetworkGUID());
			if (auto spaghettiNode = mpSceneManager->createNode(animatedNodeName + "_spaghettiNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				spaghettiNodeNames.push_back(spaghettiNode->getName());
			}
		}
	}
	unsigned long long previousTimeToSleep = 0;
	for (int i = 0; i < mParsedAnimations.size(); i++)
	{
		if (i > mChoosedBookmarkedAnimationID)
		{
			while (mIsPauseClicked)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				if (mIsStopClicked)
					break;
			}
		}
		if (mIsStopClicked)
			break;
		unsigned long long timeToSleep = mParsedAnimations[i].time - previousTimeToSleep;
		if (i > mChoosedBookmarkedAnimationID)
		{
			auto sendAnimationTimeDuringSleepThread = std::thread(&VLFTAnimationPlayerPlugin::sendAnimationTimeDuringSleep, this, mParsedAnimations[i - 1].time, mParsedAnimations[i].time);
			sendAnimationTimeDuringSleepThread.detach();
			std::this_thread::sleep_for(std::chrono::milliseconds((int)round(timeToSleep * mTimeToSleepFactor)));
		}
		mpUserInputMacro->sendOverlayBrowserMessage(std::to_string(mParsedAnimations[i].time));
		previousTimeToSleep = mParsedAnimations[i].time;
		if (auto node = mpSceneManager->getNode(mParsedAnimations[i].nodeName).lock())
		{
			if (mParsedAnimations[i].type == quicktype::EventType::SHOW)
			{
				attach2NewAnimationNode(mParsedAnimations[i].parentNodeName, node);
				node->setVisible(true);
			}
			if (mParsedAnimations[i].type == quicktype::EventType::HIDE)
			{
				node->setVisible(false);
			}
			if (mParsedAnimations[i].type == quicktype::EventType::ANIMATION)
			{
				auto previousPosition = node->getDerivedPosition();
				if (!attach2NewAnimationNode(mParsedAnimations[i].parentNodeName, node))
				{
					previousPosition = node->getDerivedPosition();
				}
				node->setPosition(mParsedAnimations[i].position);
				node->setOrientation(mParsedAnimations[i].orientation);
				std::string spaghettiSectionName;
				drawSpaghettiSection(previousPosition, node, spaghettiSectionName);
				spaghettiLineNames.push_back(spaghettiSectionName);
			}
		}
	}
	while (!mIsStopClicked)
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	for (auto spaghettiLineName : spaghettiLineNames)
	{
		mpSceneManager->deleteEntity(spaghettiLineName);
	}
	for (auto spaghettiNodeName : spaghettiNodeNames)
	{
		mpSceneManager->deleteNode(spaghettiNodeName);
	}
	for (auto animatedNodeName : mAnimatedNodeNames)
	{
		if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
		{
			for (auto attach2NewAnimationNode : mAttach2NewAnimationNode)
			{
				if (node == attach2NewAnimationNode.lock())
				{
					node->detachFromParentNode();
					node->setPosition(ape::Vector3(0, 0, 0));
					node->setOrientation(ape::Quaternion(1, 0, 0, 0));
				}
			}
			node->setOwner(node->getCreator());
		}
	}
	mAttach2NewAnimationNode = std::vector<ape::NodeWeakPtr>();
	mIsPlayRunning = false;
}

void ape::VLFTAnimationPlayerPlugin::startPlayAnimationThread()
{
	mAnimationThread = std::thread(&VLFTAnimationPlayerPlugin::playAnimation, this);
	mAnimationThread.detach();
}

void ape::VLFTAnimationPlayerPlugin::screenCast()
{
	std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::stringstream command;
	command << "/c ffmpeg -f gdigrab -framerate 30 -i desktop ../../screencasts/" << uuid.count() << ".mkv";
	STARTUPINFO info = { sizeof(info) };
	CreateProcess("C:\\windows\\system32\\cmd.exe", LPTSTR((LPCTSTR)command.str().c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &info, &mScreenCastProcessInfo);
}

void ape::VLFTAnimationPlayerPlugin::sendAnimationTimeDuringSleep(unsigned long long startTime, unsigned long long stopTime)
{
	unsigned long long time = startTime;
	int sleepTime = 100;
	while (time < stopTime)
	{
		time = time + sleepTime;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		mpUserInputMacro->sendOverlayBrowserMessage(std::to_string(time));
	}
}

void ape::VLFTAnimationPlayerPlugin::eventCallBack(const ape::Event & event)
{
	if (event.type == ape::Event::Type::BROWSER_ELEMENT_CLICK)
	{
		if (auto browser = std::static_pointer_cast<ape::IBrowser>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			APE_LOG_DEBUG("BROWSER_ELEMENT_CLICK: " << browser->getClickedElementName());
			if (browser->getClickedElementName() == "play")
			{
				if (!mIsPlayRunning)
				{
					mChoosedBookmarkedAnimationID = 0;
					mClickedBookmarkTime = 0;
					mBookmarkID = -1;
					mTimeToSleepFactor = 1.0f;
					mIsPauseClicked = false;
					mIsStopClicked = false;
					startPlayAnimationThread();
				}
			}
			else if (browser->getClickedElementName() == "skip_backward")
			{
				mTimeToSleepFactor += 0.5f;
			}
			else if (browser->getClickedElementName() == "skip_forward")
			{
				mTimeToSleepFactor -= 0.5f;
			}
			else if (browser->getClickedElementName() == "backward")
			{
				mIsStopClicked = true;
				while (mIsPlayRunning)
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				mChoosedBookmarkedAnimationID = 0;
				mTimeToSleepFactor = 1.0f;
				mIsPauseClicked = true;
				mIsStopClicked = false;
				startPlayAnimationThread();
			}
			else if (browser->getClickedElementName() == "forward")
			{
				mIsStopClicked = true;
				while (mIsPlayRunning)
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				mChoosedBookmarkedAnimationID = mParsedAnimations.size() - 1;
				mTimeToSleepFactor = 1.0f;
				mIsPauseClicked = true;
				mIsStopClicked = false;
				startPlayAnimationThread();
			}
			else if (browser->getClickedElementName() == "pause")
			{
				if (!mIsPauseClicked)
					mIsPauseClicked = true;
				else
					mIsPauseClicked = false;
			}
			else if (browser->getClickedElementName() == "stop")
			{
				if (!mIsStopClicked)
				{
					mIsStopClicked = true;
					if (mIsPauseClicked)
						mIsPauseClicked = false;
					while (mIsPlayRunning)
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}
			else if (browser->getClickedElementName().find("@") != std::string::npos)
			{
				mIsStopClicked = true;
				while (mIsPlayRunning)
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				auto bookmark = browser->getClickedElementName();
				auto atPosition = bookmark.find("@");
				mClickedBookmarkTime = atoi(bookmark.substr(atPosition + 1, bookmark.length()).c_str());
				for (int i = 0; i < mParsedAnimations.size(); i++)
				{
					if (mParsedAnimations[i].time == mClickedBookmarkTime)
					{
						mChoosedBookmarkedAnimationID = i;
						mTimeToSleepFactor = 1.0f;
						mIsPauseClicked = true;
						mIsStopClicked = false;
						startPlayAnimationThread();
						break;
					}
				}
			}
			else if (browser->getClickedElementName() == "spaghetti")
			{
				if (auto clickedNode = mClickedNode.lock())
				{
					if (auto spaghettiNode = mpSceneManager->getNode(clickedNode->getName() + "_spaghettiNode").lock())
					{
						if (spaghettiNode->isVisible())
							spaghettiNode->setVisible(false);
						else
							spaghettiNode->setVisible(true);
					}
				}
			}
			else if (browser->getClickedElementName() == "view")
			{
			}
			else if (browser->getClickedElementName() == "edit")
			{
			}
			else if (browser->getClickedElementName() == "performance")
			{
			}
			else if (browser->getClickedElementName() == "screenshot")
			{
				gdiscreen();
			}
			else if (browser->getClickedElementName() == "logUsers")
			{
				mIsStudentsMovementLogging = true;	
				std::stringstream fileName;
				std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
				fileName << "../../studentsMovementLog/" << uuid.count() << ".txt";
				mStudentsMovementLoggingFile.open(fileName.str());
				auto nodes = mpSceneManager->getNodes();
				for (auto node : nodes)
				{
					if (auto nodeSP = node.second.lock())
					{
						std::string nodeName = nodeSP->getName();
						std::size_t pos = nodeName.find("_HeadNode");
						if (pos != std::string::npos)
						{
							if (auto userNode = nodeSP->getParentNode().lock())
							{
								if (userNode->getName() != mpUserInputMacro->getUserNode().lock()->getName())
								{
									mStudents.push_back(userNode);
								}
							}
						}
					}
				}
			}
			else if (browser->getClickedElementName() == "logUsersStop")
			{
				mIsStudentsMovementLogging = false;
				mStudents.clear();
				mStudents.resize(0);
				mStudentsMovementLoggingFile.close();
			}
			else if (browser->getClickedElementName() == "screencast")
			{
				auto screenCastThread = std::thread(&VLFTAnimationPlayerPlugin::screenCast, this);
				screenCastThread.detach();
			}
			else if (browser->getClickedElementName() == "screencastStop")
			{
				HANDLE ps = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, mScreenCastProcessInfo.dwProcessId);
				EnumWindows(EnumWindowsProc, mScreenCastProcessInfo.dwProcessId);
				CloseHandle(ps);
			}
			else if (browser->getClickedElementName() == "attachUsers")
			{
				auto nodes = mpSceneManager->getNodes();
				for (auto node : nodes)
				{
					if (auto nodeSP = node.second.lock())
					{
						std::string nodeName = nodeSP->getName();
						std::size_t pos = nodeName.find("_HeadNode");
						if (pos != std::string::npos)
						{
							if (auto userNode = nodeSP->getParentNode().lock())
							{
								if (userNode->getName() != mpUserInputMacro->getUserNode().lock()->getName())
								{
									userNode->setOwner(mpCoreConfig->getNetworkGUID());
									userNode->setParentNode(mpUserInputMacro->getUserNode());
									userNode->setPosition(ape::Vector3(0, 0, 0));
									userNode->setOrientation(ape::Quaternion(1, 0, 0, 0));
									mAttachedUsers.push_back(userNode);
								}
							}
						}
					}
				}
			}
			else if (browser->getClickedElementName() == "freeUsers")
			{
				for (auto attachedUserWP : mAttachedUsers)
				{
					if (auto attachedUser = attachedUserWP.lock())
					{
						if (attachedUser->getOwner() == mpCoreConfig->getNetworkGUID())
						{
							attachedUser->detachFromParentNode();
							attachedUser->setPosition(ape::Vector3(0, 100, 0));
							attachedUser->setOrientation(ape::Quaternion(0, 0, 1, 0));
							attachedUser->setOwner(attachedUser->getCreator());
						}
					}
				}
				mAttachedUsers = std::vector<ape::NodeWeakPtr>();
			}
			else if (browser->getClickedElementName() == "freeMe")
			{
				if (auto userNode = mpUserInputMacro->getUserNode().lock())
				{
					userNode->setOwner(mpCoreConfig->getNetworkGUID());
					userNode->detachFromParentNode();
					userNode->setPosition(ape::Vector3(0, 100, 0));
					userNode->setOrientation(ape::Quaternion(0, 0, 1, 0));
				}
			}
		}
	}
	else if (event.type == ape::Event::Type::NODE_SHOWBOUNDINGBOX)
	{
		if (auto clickedNode = mpSceneManager->getNode(event.subjectName).lock())
		{
			//APE_LOG_DEBUG("NODE_SHOWBOUNDINGBOX");
			mClickedNode = clickedNode;
		}
	}
	else if (event.type == ape::Event::Type::NODE_POSITION || event.type == ape::Event::Type::NODE_ORIENTATION)
	{
		for (auto studentWP : mStudents)
		{
			if (auto student = studentWP.lock())
			{
				if (event.subjectName == student->getName())
				{
					std::stringstream data;
					data << student->getName() << student->getDerivedPosition().toString() << student->getDerivedOrientation().toString();
					mStudentsMovementLoggingFile << data.str();
				}
			}
		}
	}
}

void ape::VLFTAnimationPlayerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpUserInputMacro = ape::UserInputMacro::getSingletonPtr();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTAnimationPlayerPlugin.json";
	FILE* apeVLFTAnimationPlayerPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mAnimations = nlohmann::json::parse(apeVLFTAnimationPlayerPluginConfigFile);
	for (const auto& node : mAnimations.get_nodes())
	{
		mAnimatedNodeNames.push_back(node.get_name());
		for (const auto& action : node.get_actions())
		{
			if (action.get_trigger().get_type() == quicktype::TriggerType::TIMESTAMP)
			{
				if (action.get_event().get_type() == quicktype::EventType::SHOW)
				{
					Animation animation;
					animation.type = action.get_event().get_type();
					animation.nodeName = node.get_name();
					animation.parentNodeName = *action.get_event().get_placement_rel_to();
					animation.time = (atoi(action.get_trigger().get_data().c_str()) * 1000);
					mParsedAnimations.push_back(animation);
				}
				if (action.get_event().get_type() == quicktype::EventType::HIDE)
				{
					Animation animation;
					animation.type = action.get_event().get_type();
					animation.nodeName = node.get_name();
					animation.parentNodeName = "";
					animation.time = (atoi(action.get_trigger().get_data().c_str()) * 1000);
					mParsedAnimations.push_back(animation);
				}
				if (action.get_event().get_type() == quicktype::EventType::ANIMATION)
				{
					std::string fileNamePath = mpCoreConfig->getConfigFolderPath().substr(0, mpCoreConfig->getConfigFolderPath().find("virtualLearningFactory") + 23) + *action.get_event().get_data();
					std::ifstream file(fileNamePath);
					std::string dataCount;
					std::getline(file, dataCount);
					std::string fps;
					std::getline(file, fps);
					std::vector<Animation> currentAnimations;
					for (int i = 0; i < atoi(dataCount.c_str()); i++)
					{
						std::string postionData;
						std::getline(file, postionData);
						auto posX = postionData.find_first_of(",");
						float x = atof(postionData.substr(1, posX).c_str());
						postionData = postionData.substr(posX + 1, postionData.length());
						auto posY = postionData.find_first_of(",");
						float y = atof(postionData.substr(0, posY).c_str());
						postionData = postionData.substr(posY + 1, postionData.length());
						auto posZ = postionData.find_first_of("]");
						float z = atof(postionData.substr(0, posZ).c_str());
						Animation animation;
						animation.type = action.get_event().get_type();
						animation.nodeName = node.get_name();
						if (action.get_event().get_placement_rel_to())
							animation.parentNodeName = *action.get_event().get_placement_rel_to();
						else
							animation.parentNodeName = "";
						animation.time = (atoi(action.get_trigger().get_data().c_str()) * 1000) + ((1.0f / atoi(fps.c_str()) * 1000) * i);
						animation.position = ape::Vector3(x, y, z);
						currentAnimations.push_back(animation);
					}
					for (auto currentAnimation : currentAnimations)
					{
						std::string orientationData;
						std::getline(file, orientationData);
						auto posW = orientationData.find_first_of(",");
						float w = atof(orientationData.substr(1, posW).c_str());
						orientationData = orientationData.substr(posW + 1, orientationData.length());
						auto posX = orientationData.find_first_of(",");
						float x = atof(orientationData.substr(0, posX).c_str());
						orientationData = orientationData.substr(posX + 1, orientationData.length());
						auto posY = orientationData.find_first_of(",");
						float y = atof(orientationData.substr(0, posY).c_str());
						orientationData = orientationData.substr(posY + 1, orientationData.length());
						auto posZ = orientationData.find_first_of("]");
						float z = atof(orientationData.substr(0, posZ).c_str());
						currentAnimation.orientation = ape::Quaternion(w, x, y, z);
						mParsedAnimations.push_back(currentAnimation);
					}
				}
			}
		}
	}
	for (const auto& bookmark : mAnimations.get_bookmarks())
	{
		mParsedBookmarkTimes.push_back(atoi(bookmark.get_time().c_str()));
	}
	std::sort(mParsedBookmarkTimes.begin(), mParsedBookmarkTimes.end());
	std::sort(mParsedAnimations.begin(), mParsedAnimations.end(), compareAnimationTime);
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	if (!mIsStopClicked)
	{
		mIsStopClicked = true;
		if (mIsPauseClicked)
			mIsPauseClicked = false;
		while (mIsPlayRunning)
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	for (auto attachedUserWP : mAttachedUsers)
	{
		if (auto attachedUser = attachedUserWP.lock())
		{
			if (attachedUser->getOwner() == mpCoreConfig->getNetworkGUID())
			{
				attachedUser->detachFromParentNode();
				attachedUser->setPosition(ape::Vector3(0, 0, 0));
				attachedUser->setOrientation(ape::Quaternion(1, 0, 0, 0));
				attachedUser->setOwner(attachedUser->getCreator());
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
