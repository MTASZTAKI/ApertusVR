#include <iostream>
#include "apeSceneRecorderPlugin.h"

ape::apeSceneRecorderPlugin::apeSceneRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	mpScene = ape::ISceneManager::getSingletonPtr();
	mLastEventTimeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	mIsRecorder = false;
	mIsPlayer = true;
	mIsLooping = false;
	mFileName = "scene.bin";
	if (mIsRecorder) 
		mFileStreamOut.open(mFileName, std::ios::out | std::ios::binary);
	else if (mIsPlayer)
	{
		mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
	}
	mDelayToNextEvent = 0;
	APE_LOG_FUNC_LEAVE();
}

ape::apeSceneRecorderPlugin::~apeSceneRecorderPlugin()
{
	APE_LOG_FUNC_ENTER();
	if (mFileStreamOut.is_open())
	{
		mFileStreamOut.close();
	}
	APE_LOG_FUNC_LEAVE();
}


void ape::apeSceneRecorderPlugin::readEventData()
{
	mCurrentEventDataSizeInBytes = 0;
	mFileStreamIn.read(reinterpret_cast<char*>(&mCurrentEventDataSizeInBytes), sizeof(long));
	APE_LOG_DEBUG("currentEventDataSizeInBytes: " << mCurrentEventDataSizeInBytes);
}

void ape::apeSceneRecorderPlugin::readEventHeader(ape::Event& event)
{
	mFileStreamIn.read(reinterpret_cast<char*>(&mDelayToNextEvent), sizeof(long));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamIn.read(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	unsigned int subjectNameSize = 0;
	mFileStreamIn.read(reinterpret_cast<char*>(&subjectNameSize), sizeof(subjectNameSize));
	event.subjectName.resize(subjectNameSize);
	mFileStreamIn.read(&event.subjectName[0], subjectNameSize);
	APE_LOG_DEBUG("mDelayToNextEvent: " << mDelayToNextEvent << " event.subjectName: " << event.subjectName << " event.type:" << event.type);
}

void ape::apeSceneRecorderPlugin::readEventAndFire()
{
	ape::Event event;
	readEventHeader(event);
	readEventData();
	if (mDelayToNextEvent)
		std::this_thread::sleep_for(std::chrono::milliseconds(mDelayToNextEvent));
	if (event.group == ape::Event::Group::NODE)
	{
		if (event.type == ape::Event::Type::NODE_CREATE)
		{
			mpScene->createNode(event.subjectName);
		}
		else if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == ape::Event::Type::NODE_POSITION)
			{
				ape::Vector3 position;
				position.read(mFileStreamIn);
				node->setPosition(position);
			}
			else if (event.type == ape::Event::Type::NODE_ORIENTATION)
			{
				ape::Quaternion orientation;
				orientation.read(mFileStreamIn);
				node->setOrientation(orientation);
			}
			else if (event.type == ape::Event::Type::NODE_SCALE)
			{
				ape::Vector3 scale;
				scale.read(mFileStreamIn);
				node->setScale(scale);
			}
		}
	}
	else if (event.group == ape::Event::Group::MATERIAL_FILE)
	{
		if (event.type == ape::Event::Type::MATERIAL_FILE_CREATE)
		{
			mpScene->createEntity(event.subjectName, ape::Entity::Type::MATERIAL_FILE);
		}
		else if (auto materialFile = std::static_pointer_cast<ape::IFileMaterial>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (event.type == ape::Event::Type::MATERIAL_FILE_FILENAME)
			{
				materialFile->setFileName(readString());
			}
			else if (event.type == ape::Event::Type::MATERIAL_FILE_SETASSKYBOX)
			{
				materialFile->setAsSkyBox();
			}
		}
	}
	else if (event.group == ape::Event::Group::GEOMETRY_PLANE)
	{
		if (event.type == ape::Event::Type::GEOMETRY_PLANE_CREATE)
		{
			mpScene->createEntity(event.subjectName, ape::Entity::Type::GEOMETRY_PLANE);
		}
		else if (auto planeGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (event.type == ape::Event::Type::GEOMETRY_PLANE_PARENTNODE)
			{
				if (auto parentNode = mpScene->getNode(readString()).lock())
				{
					planeGeometry->setParentNode(parentNode);
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_PLANE_PARAMETERS)
			{
				ape::GeometryPlaneParameters geometryPlaneParameters;
				geometryPlaneParameters.read(mFileStreamIn);
				planeGeometry->setParameters(geometryPlaneParameters.numSeg, geometryPlaneParameters.size, geometryPlaneParameters.tile);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_PLANE_MATERIAL)
			{
				if (auto material = std::static_pointer_cast<ape::IManualMaterial>(mpScene->getEntity(readString()).lock()))
				{
					//planeGeometry->setMaterial(material);
				}
			}
		}
	}
	else if (event.group == ape::Event::Group::MATERIAL_MANUAL)
	{
		if (event.type == ape::Event::Type::MATERIAL_MANUAL_CREATE)
		{
			mpScene->createEntity(event.subjectName, ape::Entity::Type::MATERIAL_MANUAL);
		}
		else if (auto manualMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (event.type == ape::Event::Type::MATERIAL_MANUAL_DIFFUSE)
			{
				ape::Color color;
				color.read(mFileStreamIn);
				manualMaterial->setDiffuseColor(color);
			}
			else if (event.type == ape::Event::Type::MATERIAL_MANUAL_SPECULAR)
			{
				ape::Color color;
				color.read(mFileStreamIn);
				manualMaterial->setSpecularColor(color);
			}
		}
	}
}

void ape::apeSceneRecorderPlugin::writeEventHeader(ape::Event& event)
{
	auto timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	long timeToCallEventInMilliseconds = timeStamp.count() - mLastEventTimeStamp.count();
	mLastEventTimeStamp = timeStamp;
	mFileStreamOut.write(reinterpret_cast<char*>(&timeToCallEventInMilliseconds), sizeof(long));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.group), sizeof(unsigned int));
	mFileStreamOut.write(reinterpret_cast<char*>(&event.type), sizeof(unsigned int));
	unsigned int subjectNameSize = event.subjectName.size();
	mFileStreamOut.write(reinterpret_cast<char *>(&subjectNameSize), sizeof(subjectNameSize));
	mFileStreamOut.write(event.subjectName.c_str(), subjectNameSize);
	//APE_LOG_DEBUG(" event.subjectName: " << event.subjectName << " event.type:" << event.type);
}

void ape::apeSceneRecorderPlugin::writeEvent(ape::Event event)
{
	if (event.group == ape::Event::Group::NODE)
	{
		if (event.type == ape::Event::Type::NODE_CREATE)
		{
			writeEventHeader(event);
			writeZeroEventDataSize();
		}
		else if (auto node = mpScene->getNode(event.subjectName).lock())
		{
			if (event.type == ape::Event::Type::NODE_POSITION)
			{
				writeEventHeader(event);
				ape::Vector3 position = node->getPosition();
				position.write(mFileStreamOut);
			}
			else if (event.type == ape::Event::Type::NODE_ORIENTATION)
			{
				writeEventHeader(event);
				ape::Quaternion orientation = node->getOrientation();
				orientation.write(mFileStreamOut);
			}
			else if (event.type == ape::Event::Type::NODE_SCALE)
			{
				writeEventHeader(event);
				ape::Vector3 scale = node->getScale();
				scale.write(mFileStreamOut);
			}
		}
	}
	else if (event.group == ape::Event::Group::MATERIAL_FILE)
	{
		if (event.type == ape::Event::Type::MATERIAL_FILE_CREATE)
		{
			writeEventHeader(event);
			writeZeroEventDataSize();
		}
		else if (auto materialFile = std::static_pointer_cast<ape::IFileMaterial>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (event.type == ape::Event::Type::MATERIAL_FILE_FILENAME)
			{
				writeEventHeader(event);
				writeString(materialFile->getfFileName());
			}
			else if (event.type == ape::Event::Type::MATERIAL_FILE_SETASSKYBOX)
			{
				writeEventHeader(event);
				writeZeroEventDataSize();
			}
		}
	}
	else if (event.group == ape::Event::Group::GEOMETRY_PLANE)
	{
		if (event.type == ape::Event::Type::GEOMETRY_PLANE_CREATE)
		{
			writeEventHeader(event);
			writeZeroEventDataSize();
		}
		else if (auto planeGeometry = std::static_pointer_cast<ape::IPlaneGeometry>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (event.type == ape::Event::Type::GEOMETRY_PLANE_PARENTNODE)
			{
				if (auto parentNode = planeGeometry->getParentNode().lock())
				{
					writeEventHeader(event);
					writeString(parentNode->getName());
				}
			}
			else if (event.type == ape::Event::Type::GEOMETRY_PLANE_PARAMETERS)
			{
				writeEventHeader(event);
				ape::GeometryPlaneParameters geometryPlaneParameters = planeGeometry->getParameters();
				geometryPlaneParameters.write(mFileStreamOut);
			}
			else if (event.type == ape::Event::Type::GEOMETRY_PLANE_MATERIAL)
			{
				if (auto material = planeGeometry->getMaterial().lock())
				{
					writeEventHeader(event);
					writeString(material->getName());
				}
			}
		}
	}
	else if (event.group == ape::Event::Group::MATERIAL_MANUAL)
	{
		if (event.type == ape::Event::Type::MATERIAL_MANUAL_CREATE)
		{
			writeEventHeader(event);
			writeZeroEventDataSize();
		}
		else if (auto manualMaterial = std::static_pointer_cast<ape::IManualMaterial>(mpScene->getEntity(event.subjectName).lock()))
		{
			if (event.type == ape::Event::Type::MATERIAL_MANUAL_DIFFUSE)
			{
				writeEventHeader(event);
				ape::Color color = manualMaterial->getDiffuseColor();
				color.write(mFileStreamOut);
			}
			else if (event.type == ape::Event::Type::MATERIAL_MANUAL_SPECULAR)
			{
				writeEventHeader(event);
				ape::Color color = manualMaterial->getSpecularColor();
				color.write(mFileStreamOut);
			}
		}
	}
}

void ape::apeSceneRecorderPlugin::writeString(std::string string)
{
	unsigned int stringSize = string.size();
	mFileStreamOut.write(reinterpret_cast<char *>(&stringSize), sizeof(stringSize));
	mFileStreamOut.write(string.c_str(), stringSize);
}

void ape::apeSceneRecorderPlugin::writeZeroEventDataSize()
{
	long eventDataSizeInBytes = 0;
	mFileStreamOut.write(reinterpret_cast<char*>(&eventDataSizeInBytes), sizeof(long));
}

std::string ape::apeSceneRecorderPlugin::readString()
{
	std::string string;
	string.resize(mCurrentEventDataSizeInBytes);
	mFileStreamIn.read(&string[0], mCurrentEventDataSizeInBytes);
	return string;
}

void ape::apeSceneRecorderPlugin::eventCallBack(const ape::Event& event)
{
	if (mIsRecorder)
	{
		writeEvent(event);
	}
}

void ape::apeSceneRecorderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSceneRecorderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	bool isRun = true;
	while (isRun)
	{
		if (mIsPlayer)
		{
			readEventAndFire();
			if (!mFileStreamIn.good() && mIsLooping)
			{
				mFileStreamIn.close();
				mFileStreamIn.clear();
				mFileStreamIn.open(mFileName, std::ios::in | std::ios::binary);
				APE_LOG_DEBUG("looping the file");
			}
			else if (mFileStreamIn.eof())
			{
				mIsPlayer = false;
				APE_LOG_DEBUG("end of the file");
			}
		}
		else if (mIsRecorder)
		{
			APE_LOG_DEBUG("press any key to stop recording the scene");
			std::cin.get();
			isRun = false;
			APE_LOG_DEBUG("scene recording was stopped, flush into scene.bin file");
			if (mFileStreamOut.is_open())
			{
				mFileStreamOut.flush();
				mFileStreamOut.close();
				APE_LOG_DEBUG("scene.bin was closed");
			}
		}
	}
	mpEventManager->disconnectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&apeSceneRecorderPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeSceneRecorderPlugin::Step()
{

}

void ape::apeSceneRecorderPlugin::Stop()
{

}

void ape::apeSceneRecorderPlugin::Suspend()
{

}

void ape::apeSceneRecorderPlugin::Restart()
{

}

