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

#include <iostream>
#include "apeFileGeometryImpl.h"

ape::FileGeometryImpl::FileGeometryImpl(std::string name, bool replicate, std::string ownerID, bool isHost) : ape::IFileGeometry(name, replicate, ownerID), ape::Replica("FileGeometry", name, ownerID, isHost)
{
	mpEventManagerImpl = ((ape::EventManagerImpl*)ape::IEventManager::getSingletonPtr());
	mFileName = std::string();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	mIsExportMesh = false;
	mIsSubMeshesMerged = false;
	mVisibilityFlag = 0;
	mIsAnimationRunning = false;
	mRunningAnimation = "";
	mUnitScale = 1.0f;
}

ape::FileGeometryImpl::~FileGeometryImpl()
{
	
}

std::string ape::FileGeometryImpl::getFileName()
{
	return mFileName;
}

void ape::FileGeometryImpl::setFileName(std::string fileName)
{
	mFileName = fileName;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_FILENAME));
}

void ape::FileGeometryImpl::setParentNode(ape::NodeWeakPtr parentNode)
{
	if (auto parentNodeSP = parentNode.lock())
	{
		mParentNode = parentNode;
		mParentNodeName = parentNodeSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_PARENTNODE));
	}
	else
		mParentNode = ape::NodeWeakPtr();
}

void ape::FileGeometryImpl::setMaterial(ape::MaterialWeakPtr material)
{
	if (auto materialSP = material.lock())
	{
		mMaterial = material;
		mMaterialName = materialSP->getName();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_MATERIAL));
	}
	else
		mMaterial = ape::MaterialWeakPtr();
}

ape::MaterialWeakPtr ape::FileGeometryImpl::getMaterial()
{
	return mMaterial;
}

void ape::FileGeometryImpl::exportMesh()
{
	mIsExportMesh = true;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_EXPORT));
}

bool ape::FileGeometryImpl::isExportMesh()
{
	return mIsExportMesh;
}

void ape::FileGeometryImpl::mergeSubMeshes()
{
	mIsSubMeshesMerged = true;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES));
}

bool ape::FileGeometryImpl::isMergeSubMeshes()
{
	return mIsSubMeshesMerged;
}

void ape::FileGeometryImpl::setVisibilityFlag(unsigned int flag)
{
	mVisibilityFlag = flag;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_VISIBILITY));
}

unsigned int ape::FileGeometryImpl::getVisibilityFlag()
{
	return mVisibilityFlag;
}

void ape::FileGeometryImpl::setOwner(std::string ownerID)
{
	mOwnerID = ownerID;
}

std::string ape::FileGeometryImpl::getOwner()
{
	return mOwnerID;
}

void ape::FileGeometryImpl::setUnitScale(float unitScale)
{
	mUnitScale = unitScale;
}

float ape::FileGeometryImpl::getUnitScale()
{
	return mUnitScale;
}

void ape::FileGeometryImpl::playAnimation(std::string animationID)
{
	mRunningAnimation = animationID;
	mIsAnimationRunning = true;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_PLAYANIMATION));
}

void ape::FileGeometryImpl::stopAnimation(std::string animationID)
{
	mStoppedAnimation = animationID;
	mIsAnimationRunning = false;
	mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_STOPANIMATION));
}

std::string ape::FileGeometryImpl::getRunningAnimation()
{
	return mRunningAnimation;
}

std::string ape::FileGeometryImpl::getStoppedAnimation()
{
	return mStoppedAnimation;
}

void ape::FileGeometryImpl::WriteAllocationID(RakNet::Connection_RM3 *destinationConnection, RakNet::BitStream *allocationIdBitstream) const
{
	allocationIdBitstream->Write(mObjectType);
	allocationIdBitstream->Write(RakNet::RakString(mName.c_str())); allocationIdBitstream->Write(RakNet::RakString(mOwnerID.c_str()));
}

RakNet::RM3SerializationResult ape::FileGeometryImpl::Serialize(RakNet::SerializeParameters *serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	serializeParameters->pro[0].reliability = RELIABLE_ORDERED;
	mVariableDeltaSerializer.BeginIdenticalSerialize(&serializationContext, serializeParameters->whenLastSerialized == 0, &serializeParameters->outputBitstream[0]);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mUnitScale);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mFileName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mParentNodeName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mMaterialName.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsExportMesh);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsSubMeshesMerged);
	//APE_LOG_DEBUG("mIsAnimationRunning " << std::to_string(mIsAnimationRunning));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, mIsAnimationRunning);
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mRunningAnimation.c_str()));
	mVariableDeltaSerializer.SerializeVariable(&serializationContext, RakNet::RakString(mStoppedAnimation.c_str()));
	mVariableDeltaSerializer.EndSerialize(&serializationContext);
	return RakNet::RM3SR_BROADCAST_IDENTICALLY_FORCE_SERIALIZATION;
}

void ape::FileGeometryImpl::Deserialize(RakNet::DeserializeParameters *deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	mVariableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mUnitScale);
	RakNet::RakString fileName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, fileName))
	{
		mFileName = fileName.C_String();
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_FILENAME));
	}
	RakNet::RakString parentName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, parentName))
	{
		if (auto parentNode = mpSceneManager->getNode(parentName.C_String()).lock())
		{
			mParentNode = parentNode;
			mParentNodeName = parentName.C_String();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_PARENTNODE));
		}
	}
	RakNet::RakString materialName;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, materialName))
	{
		if (auto material = std::static_pointer_cast<ape::Material>(mpSceneManager->getEntity(materialName.C_String()).lock()))
		{
			mMaterial = material;
			mMaterialName = material->getName();
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_MATERIAL));
		}
	}
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsExportMesh))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_EXPORT));
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsSubMeshesMerged))
		mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES));

	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, mIsAnimationRunning))
	{
		//APE_LOG_DEBUG("DESERIALIZE isAnimationRunning");
		if (mIsAnimationRunning)
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_PLAYANIMATION));
		else
			mpEventManagerImpl->fireEvent(ape::Event(mName, ape::Event::Type::GEOMETRY_FILE_STOPANIMATION));
	}

	RakNet::RakString runningAnimation;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, runningAnimation))
	{	
		mRunningAnimation = runningAnimation.C_String();	
	}

	RakNet::RakString stoppedAnimation;
	if (mVariableDeltaSerializer.DeserializeVariable(&deserializationContext, stoppedAnimation))
	{
		mStoppedAnimation = stoppedAnimation.C_String();
	}
	
	mVariableDeltaSerializer.EndDeserialize(&deserializationContext);
}






