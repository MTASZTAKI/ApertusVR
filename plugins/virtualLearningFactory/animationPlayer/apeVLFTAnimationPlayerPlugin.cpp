#include <fstream>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <gdiplus.h>
#include <time.h>
#include "apeVLFTAnimationPlayerPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

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
	APE_LOG_FUNC_LEAVE();
}

ape::VLFTAnimationPlayerPlugin::~VLFTAnimationPlayerPlugin()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::VLFTAnimationPlayerPlugin::playBinFile(std::string name, quicktype::Action action)
{
	auto nodes = mpSceneManager->getNodes();
	auto entities = mpSceneManager->getEntities();
	if (auto node = mpSceneManager->getNode(name).lock())
	{
		std::ifstream binFileStream;
		std::string binFileNamePath = mpCoreConfig->getConfigFolderPath() + action.get_event().get_data();
		binFileStream.open(binFileNamePath, std::ios::in | std::ios::binary);
		long frameCount = 0;
		binFileStream.read(reinterpret_cast<char*>(&frameCount), sizeof(long));
		if (!binFileStream.badbit)
		{
			std::vector<ape::Vector3> positions;
			positions.resize(frameCount);
			std::vector<ape::Quaternion> orientations;
			orientations.resize(frameCount);
			binFileStream.read(reinterpret_cast<char*>(&positions[0]), frameCount * 3 * sizeof(float));
			if (!binFileStream.badbit)
			{
				binFileStream.read(reinterpret_cast<char*>(&orientations[0]), frameCount * 4 * sizeof(float));
				if (!binFileStream.badbit)
				{
					APE_LOG_DEBUG(name << " animation was timed to start at " << action.get_trigger().get_data().c_str() << " seconds after the startup signal");
					std::this_thread::sleep_for(std::chrono::milliseconds(atoi(action.get_trigger().get_data().c_str()) * 1000));
					for (long i = 0; i < frameCount; i++)
					{
						node->setPosition(positions[i]);
						node->setOrientation(orientations[i]);
						std::this_thread::sleep_for(std::chrono::milliseconds(16));
					}
					APE_LOG_DEBUG(name << " animation was played with a frame count " << frameCount);
				}
				else
				{
					APE_LOG_DEBUG("wrong orientations data: " << name);
				}
			}
			else
			{
				APE_LOG_DEBUG("wrong positions data: " << name);
			}
		}
		else
		{
			APE_LOG_DEBUG("wrong bin header: " << name);
		}
	}
}

void ape::VLFTAnimationPlayerPlugin::playAnimation()
{
	mIsPlayRunning = true;
	std::vector<std::string> previousOwnerNames;
	std::vector<std::string> spaghettiNodeNames;
	for (auto animatedNodeName : mAnimatedNodeNames)
	{
		if (auto node = mpSceneManager->getNode(animatedNodeName).lock())
		{
			previousOwnerNames.push_back(node->getOwner());
			node->setOwner(mpCoreConfig->getNetworkGUID());
			node->setVisible(true);
			if (auto spaghettiLineNode = mpSceneManager->createNode(animatedNodeName + "_spaghettiLineNode", true, mpCoreConfig->getNetworkGUID()).lock())
			{
				spaghettiNodeNames.push_back(spaghettiLineNode->getName());
			}
		}
	}
	std::vector<std::string> spaghettiLineNames;
	unsigned long long previousTimeToSleep = 0;
	for (int i = 0; i < mParsedAnimations.size(); i++)
	{
		if (i > mChoosedBookmarkedAnimationID)
		{
			while (mIsPauseClicked)
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		if (mIsStopClicked)
			break;
		unsigned long long timeToSleep = mParsedAnimations[i].time - previousTimeToSleep;
		if (i > mChoosedBookmarkedAnimationID)
			std::this_thread::sleep_for(std::chrono::milliseconds((int)round(timeToSleep * mTimeToSleepFactor)));
		previousTimeToSleep = mParsedAnimations[i].time;
		if (auto node = mpSceneManager->getNode(mParsedAnimations[i].nodeName).lock())
		{
			if (auto spaghettiLineNode = mpSceneManager->getNode(mParsedAnimations[i].nodeName + "_spaghettiLineNode").lock())
			{
				auto previousPosition = node->getDerivedPosition();
				node->setPosition(mParsedAnimations[i].position);
				node->setOrientation(mParsedAnimations[i].orientation);
				auto currentPosition = node->getDerivedPosition();
				std::chrono::microseconds uuid = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
				if (auto spagetthiLineSection = std::static_pointer_cast<ape::IIndexedLineSetGeometry>(mpSceneManager->createEntity(spaghettiLineNode->getName() + std::to_string(uuid.count()), ape::Entity::GEOMETRY_INDEXEDLINESET, true, mpCoreConfig->getNetworkGUID()).lock()))
				{
					ape::GeometryCoordinates coordinates = {
						previousPosition.x, previousPosition.y, previousPosition.z,
						currentPosition.x, currentPosition.y, currentPosition.z, };
					ape::GeometryIndices indices = { 0, 1, -1 };
					ape::Color color(1, 0, 0);
					spagetthiLineSection->setParameters(coordinates, indices, color);
					spagetthiLineSection->setParentNode(spaghettiLineNode);
					spaghettiLineNames.push_back(spagetthiLineSection->getName());
				}
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
	for (int i = 0; i < mAnimatedNodeNames.size(); i++)
	{
		if (auto node = mpSceneManager->getNode(mAnimatedNodeNames[i]).lock())
		{
			node->setPosition(ape::Vector3(0, 0, 0));
			node->setOrientation(ape::Quaternion(1, 0, 0, 0));
			node->setVisible(false);
			node->setOwner(previousOwnerNames[i]);
		}
	}
	mIsPlayRunning = false;
}

void ape::VLFTAnimationPlayerPlugin::startPlayAnimationThread()
{
	mAnimationThread = std::thread(&VLFTAnimationPlayerPlugin::playAnimation, this);
	mAnimationThread.detach();
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
				if (mBookmarkID > -1)
				{
					mIsStopClicked = true;
					while (mIsPlayRunning)
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
					mBookmarkID--;
					mClickedBookmarkTime = mParsedBookmarkTimes[mBookmarkID];
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
			}
			else if (browser->getClickedElementName() == "forward")
			{
				if (mBookmarkID < (int)mParsedBookmarkTimes.size())
				{
					mIsStopClicked = true;
					while (mIsPlayRunning)
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
					mBookmarkID++;
					mClickedBookmarkTime = mParsedBookmarkTimes[mBookmarkID];
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
					if (auto spaghettiNode = mpSceneManager->getNode(clickedNode->getName() + "_spaghettiLineNode").lock())
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
			else if (browser->getClickedElementName() == "screencast")
			{
				std::chrono::milliseconds uuid = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
				std::stringstream command;
				command << "ffmpeg -f gdigrab -framerate 30 -i desktop ../../screencasts/" << uuid.count() << ".mkv";
				system(command.str().c_str());
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
}

void ape::VLFTAnimationPlayerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeVLFTAnimationPlayerPlugin.json";
	FILE* apeVLFTAnimationPlayerPluginConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	mAnimations = nlohmann::json::parse(apeVLFTAnimationPlayerPluginConfigFile);
	for (const auto& node : mAnimations.get_nodes())
	{
		mAnimatedNodeNames.push_back(node.get_name());
		for (const auto& action : node.get_actions())
		{
			if (action.get_trigger().get_type() == "timestamp")
			{
				std::string fileNamePath = mpCoreConfig->getConfigFolderPath().substr(0, mpCoreConfig->getConfigFolderPath().find("virtualLearningFactory") + 23) + action.get_event().get_data();
				std::ifstream file(fileNamePath);
				std::string dataCount;
				std::getline(file, dataCount);
				std::string fps;
				std::getline(file, fps);
				std::vector<Animation> mCurrentAnimations;
				for (int i = 0; i < atoi(dataCount.c_str()); i++)
				{
					std::string postionData;
					std::getline(file, postionData);
					auto posX = postionData.find_first_of(",") - 1;
					float x = atof(postionData.substr(1, posX).c_str());
					postionData = postionData.substr(posX + 2, postionData.length());
					auto posY = postionData.find_first_of(",") - 1;
					float y = atof(postionData.substr(0, posY).c_str());
					postionData = postionData.substr(posY + 2, postionData.length());
					auto posZ = postionData.find_first_of("]") - 1;
					float z = atof(postionData.substr(0, posZ).c_str());
					Animation animation;
					animation.nodeName = node.get_name();
					animation.time = (atoi(action.get_trigger().get_data().c_str()) * 1000) + ((1.0f / atoi(fps.c_str()) * 1000) * i);
					animation.position = ape::Vector3(x, y, z);
					mCurrentAnimations.push_back(animation);
				}
				for (auto currentAnimation : mCurrentAnimations)
				{
					std::string orientationData;
					std::getline(file, orientationData);
					auto posW = orientationData.find_first_of(",") - 1;
					float w = atof(orientationData.substr(1, posW).c_str());
					orientationData = orientationData.substr(posW + 2, orientationData.length());
					auto posX = orientationData.find_first_of(",") - 1;
					float x = atof(orientationData.substr(0, posX).c_str());
					orientationData = orientationData.substr(posX + 2, orientationData.length());
					auto posY = orientationData.find_first_of(",") - 1;
					float y = atof(orientationData.substr(0, posY).c_str());
					orientationData = orientationData.substr(posY + 2, orientationData.length());
					auto posZ = orientationData.find_first_of("]" - 1);
					float z = atof(orientationData.substr(0, posZ).c_str());
					currentAnimation.orientation = ape::Quaternion(w, x, y, z);
					mParsedAnimations.push_back(currentAnimation);
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
