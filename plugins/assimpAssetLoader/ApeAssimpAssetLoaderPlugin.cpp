#include "ApeAssimpAssetLoaderPlugin.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Ape::AssimpAssetLoaderPlugin::AssimpAssetLoaderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpAssimpImporter = nullptr;
	mAssimpScenes = std::vector<const aiScene*>();
	mAssimpAssetConfigs = std::vector<AssetConfig>();
	std::srand(std::time(0));
	mObjectCount = 0;
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&AssimpAssetLoaderPlugin::eventCallBack, this, std::placeholders::_1));
	mAssetCount = 0;
	APE_LOG_FUNC_LEAVE();
}

Ape::AssimpAssetLoaderPlugin::~AssimpAssetLoaderPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&AssimpAssetLoaderPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	mpAssimpImporter = new Assimp::Importer();
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	loadConfig();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::eventCallBack(const Ape::Event & event)
{
	if (event.type == Ape::Event::Type::GEOMETRY_FILE_FILENAME)
	{
		if (auto fileGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->getEntity(event.subjectName).lock()))
		{
			//APE_LOG_DEBUG("GEOMETRY_FILE_FILENAME: subjectName: " << event.subjectName);
			//APE_LOG_DEBUG("GEOMETRY_FILE_FILENAME: fileName: " << fileGeometry->getFileName());

			readFile(fileGeometry->getFileName());
		}
	}
}

void Ape::AssimpAssetLoaderPlugin::createNode(int assimpSceneID, aiNode* assimpNode)
{
	APE_LOG_FUNC_ENTER();
	mObjectCount++;
	aiMatrix4x4 nodeTransformation = assimpNode->mTransformation;
	aiVector3t<float> scaling;
	aiQuaterniont<float> rotation;
	aiVector3t<float> position;
	nodeTransformation.Decompose(scaling, rotation, position);
	std::stringstream nodeUniqueName;
	std::string assimpNodeOriginalName = assimpNode->mName.C_Str();
	nodeUniqueName << assimpNode->mName.C_Str() << "_" << mObjectCount;
	std::string escapedNodeName = nodeUniqueName.str();
	escapedNodeName.erase(std::remove(escapedNodeName.begin(), escapedNodeName.end(), '<'), escapedNodeName.end());
	escapedNodeName.erase(std::remove(escapedNodeName.begin(), escapedNodeName.end(), '>'), escapedNodeName.end());
	assimpNode->mName = escapedNodeName;
	if (auto node = mpSceneManager->createNode(escapedNodeName).lock())
	{
		auto parentNode = Ape::NodeWeakPtr();
		if (assimpNode->mParent)
		{
			parentNode = mpSceneManager->getNode(assimpNode->mParent->mName.C_Str());
			if (parentNode.lock())
				node->setParentNode(parentNode);
		}
		node->setPosition(Ape::Vector3(position.x, position.y, position.z));
		node->setOrientation(Ape::Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
		node->setScale(Ape::Vector3(scaling.x, scaling.y, scaling.z));
		//APE_LOG_DEBUG("nodeName: " << node->getName());
		for (int i = 0; i < assimpNode->mNumMeshes; i++)
		{
			mObjectCount++;
			aiMesh* assimpMesh = mAssimpScenes[assimpSceneID]->mMeshes[assimpNode->mMeshes[i]];
			std::stringstream meshUniqueName;
			meshUniqueName << assimpNodeOriginalName << "_" << mObjectCount;
			assimpMesh->mName = meshUniqueName.str();
			if (auto mesh = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpSceneManager->createEntity(meshUniqueName.str(), Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				Ape::GeometryCoordinates coordinates = Ape::GeometryCoordinates();
				for (int i = 0; i < assimpMesh->mNumVertices; i++)
				{
					aiVector3D assimpVertex = assimpMesh->mVertices[i];
					Ape::Vector3 vertexPosition(assimpVertex.x, assimpVertex.y, assimpVertex.z);
					if (mAssimpAssetConfigs[assimpSceneID].mergeAndExportMeshes)
						vertexPosition = (node->getDerivedPosition() + (node->getDerivedOrientation() * vertexPosition)) * mAssimpAssetConfigs[assimpSceneID].scale;
					coordinates.push_back(vertexPosition.x);
					coordinates.push_back(vertexPosition.y);
					coordinates.push_back(vertexPosition.z);
				}
				Ape::GeometryIndices indices = Ape::GeometryIndices();
				for (int i = 0; i < assimpMesh->mNumFaces; i++)
				{
					aiFace assimpFace = assimpMesh->mFaces[i];
					for (int i = 0; i < assimpFace.mNumIndices; i++)
						indices.push_back(assimpFace.mIndices[i]);
					indices.push_back(-1);
				}
				aiMaterial* asssimpMaterial = mAssimpScenes[assimpSceneID]->mMaterials[assimpMesh->mMaterialIndex];
				aiString materialName;
				asssimpMaterial->Get(AI_MATKEY_NAME, materialName);
				std::string modifiedMaterialName = materialName.C_Str();
				modifiedMaterialName += mAssimpAssetConfigs[assimpSceneID].file.substr(mAssimpAssetConfigs[assimpSceneID].file.find_last_of("/\\"));
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), '/'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), '/\\'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), ':'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), ','), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), '<'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), '>'), modifiedMaterialName.end());

				auto material = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->getEntity(modifiedMaterialName).lock());
				std::string diffuseTextureFileName = std::string();
				if (!material)
				{
					material = std::static_pointer_cast<Ape::IManualMaterial>(mpSceneManager->createEntity(modifiedMaterialName, Ape::Entity::MATERIAL_MANUAL).lock());
					aiColor3D colorDiffuse(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);
					float opacity = 1.0f;
					asssimpMaterial->Get(AI_MATKEY_OPACITY, opacity);
					aiColor3D colorSpecular(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
					aiColor3D colorAmbient(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, colorAmbient);
					aiColor3D colorEmissive(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, colorEmissive);
					aiColor3D colorTransparent(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, colorTransparent);
					int sceneBlendingType = 0;
					asssimpMaterial->Get(AI_MATKEY_BLEND_FUNC, sceneBlendingType);
					if (asssimpMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
					{
						aiString path;
						asssimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
						diffuseTextureFileName = path.data;
					}
					if (!colorDiffuse.IsBlack())
						material->setDiffuseColor(Ape::Color(colorDiffuse.r, colorDiffuse.g, colorDiffuse.b, opacity));
					if (!colorSpecular.IsBlack())
						material->setSpecularColor(Ape::Color(colorSpecular.r, colorSpecular.g, colorSpecular.b, opacity));
					if (!colorAmbient.IsBlack())
						material->setAmbientColor(Ape::Color(colorAmbient.r, colorAmbient.g, colorAmbient.b));
					if (!colorEmissive.IsBlack())
						material->setEmissiveColor(Ape::Color(colorEmissive.r, colorEmissive.g, colorEmissive.b));


					if (sceneBlendingType == aiBlendMode_Additive)
					{
						material->setSceneBlending(Ape::Pass::SceneBlendingType::ADD);
						//APE_LOG_DEBUG("blending ADD: " << opacity);
					}
					else if (sceneBlendingType == aiBlendMode_Default)
					{
						if (opacity < 0.99)
						{
							material->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
							//APE_LOG_DEBUG("blending TRANSPARENT_ALPHA: " << opacity);
						}
						else
						{
							material->setSceneBlending(Ape::Pass::SceneBlendingType::REPLACE);
							//APE_LOG_DEBUG("blending REPLACE: " << opacity);
						}
					}
					//APE_LOG_DEBUG("createManualMaterial: " << material->getName());
				}
				Ape::GeometryNormals normals = Ape::GeometryNormals();
				if (assimpMesh->HasNormals() && !mAssimpAssetConfigs[assimpSceneID].regenerateNormals)
				{
					for (int i = 0; i < assimpMesh->mNumVertices; i++)
					{
						aiVector3D assimpNormal = assimpMesh->mNormals[i];
						normals.push_back(assimpNormal.x);
						normals.push_back(assimpNormal.y);
						normals.push_back(assimpNormal.z);
					}
					//APE_LOG_DEBUG("hasNormal: " << assimpMesh->mName.C_Str());
				}
				Ape::GeometryColors colors = Ape::GeometryColors();
				for (int colorSetIndex = 0; colorSetIndex < AI_MAX_NUMBER_OF_COLOR_SETS; colorSetIndex++)
				{
					if (assimpMesh->HasVertexColors(colorSetIndex))
					{
						for (int vertexIndex = 0; vertexIndex < assimpMesh->mNumVertices; vertexIndex++)
						{
							aiColor4D assimpColor = assimpMesh->mColors[colorSetIndex][vertexIndex];
							colors.push_back(assimpColor.r);
							colors.push_back(assimpColor.g);
							colors.push_back(assimpColor.b);
							colors.push_back(assimpColor.a);
						}
						//APE_LOG_DEBUG("hasVertexColors: " << assimpMesh->mName.C_Str());
					}
				}
				Ape::GeometryTextureCoordinates textureCoordinates = Ape::GeometryTextureCoordinates();
				for (int textureCoordinatesSetIndex = 0; textureCoordinatesSetIndex < AI_MAX_NUMBER_OF_TEXTURECOORDS; textureCoordinatesSetIndex++)
				{
					if (assimpMesh->HasTextureCoords(textureCoordinatesSetIndex))
					{
						for (int textureCoordinatesIndex = 0; textureCoordinatesIndex < assimpMesh->mNumVertices; textureCoordinatesIndex++)
						{
							aiVector3D assimpTerxtureCoordinates = assimpMesh->mTextureCoords[textureCoordinatesSetIndex][textureCoordinatesIndex];
							textureCoordinates.push_back(assimpTerxtureCoordinates.x);
							textureCoordinates.push_back(1.0f - assimpTerxtureCoordinates.y);
						}
						//APE_LOG_DEBUG("HasTextureCoords: " << assimpMesh->mName.C_Str());
					}
				}
				std::string groupName = std::string();
				if (mAssimpAssetConfigs[assimpSceneID].mergeAndExportMeshes)
					groupName = mAssimpAssetConfigs[assimpSceneID].file;
				mesh->setParameters(groupName, coordinates, indices, normals, mAssimpAssetConfigs[assimpSceneID].regenerateNormals, colors, textureCoordinates, material);
				if (textureCoordinates.size() && diffuseTextureFileName.length())
				{
					if (auto fileTexture = std::static_pointer_cast<Ape::IFileTexture>(mpSceneManager->createEntity(diffuseTextureFileName, Ape::Entity::Type::TEXTURE_FILE).lock()))
					{
						fileTexture->setFileName(diffuseTextureFileName);
						material->setPassTexture(fileTexture);
					}
				}
				if (!mAssimpAssetConfigs[assimpSceneID].mergeAndExportMeshes)
					mesh->setParentNode(node);
				//APE_LOG_DEBUG("createIndexedFaceSetGeometry: " << mesh->getName());
			}
		}
	}
	for (int i = 0; i < assimpNode->mNumChildren; i++)
		createNode(assimpSceneID, assimpNode->mChildren[i]);
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::loadConfig()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeAssimpAssetLoaderPlugin.json";
	FILE* apeAssimpAssetLoaderConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeAssimpAssetLoaderConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeAssimpAssetLoaderConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& assets = jsonDocument["assets"];
			for (auto& asset : assets.GetArray())
			{
				AssetConfig assetConfig;
				for (rapidjson::Value::MemberIterator assetMemberIterator = asset.MemberBegin(); assetMemberIterator != asset.MemberEnd(); ++assetMemberIterator)
				{
					if (assetMemberIterator->name == "mergeAndExportMeshes")
						assetConfig.mergeAndExportMeshes = assetMemberIterator->value.GetBool();
					if (assetMemberIterator->name == "regenerateNormals")
						assetConfig.regenerateNormals = assetMemberIterator->value.GetBool();
					if (assetMemberIterator->name == "rootNodeName")
					{
						assetConfig.rootNodeName = assetMemberIterator->value.GetString();
						mpSceneManager->createNode(assetConfig.rootNodeName);
						APE_LOG_DEBUG("rootNodeName " << assetConfig.rootNodeName);
					}
					if (assetMemberIterator->name == "scale")
					{
						rapidjson::Value& scale = assetMemberIterator->value;
						assetConfig.scale.x = scale.GetArray()[0].GetFloat();
						assetConfig.scale.y = scale.GetArray()[1].GetFloat();
						assetConfig.scale.z = scale.GetArray()[2].GetFloat();
					}
					if (assetMemberIterator->name == "position")
					{
						rapidjson::Value& position = assetMemberIterator->value;
						assetConfig.position.x = position.GetArray()[0].GetFloat();
						assetConfig.position.y = position.GetArray()[1].GetFloat();
						assetConfig.position.z = position.GetArray()[2].GetFloat();
					}
					if (assetMemberIterator->name == "orientation")
					{
						rapidjson::Value& orientation = assetMemberIterator->value;
						assetConfig.orientation.w = orientation.GetArray()[0].GetFloat();
						assetConfig.orientation.x = orientation.GetArray()[1].GetFloat();
						assetConfig.orientation.y = orientation.GetArray()[2].GetFloat();
						assetConfig.orientation.z = orientation.GetArray()[3].GetFloat();
					}
					if (assetMemberIterator->name == "file")
					{
						std::stringstream assimpAssetFileNamePath;
						assimpAssetFileNamePath << APE_SOURCE_DIR << assetMemberIterator->value.GetString();
						assetConfig.file = assimpAssetFileNamePath.str();
						std::string fileName = assimpAssetFileNamePath.str().substr(assimpAssetFileNamePath.str().find_last_of("/\\") + 1);
						std::string fileExtension = assimpAssetFileNamePath.str().substr(assimpAssetFileNamePath.str().find_last_of("."));
						if (fileExtension == ".mesh")
						{
							if (auto node = mpSceneManager->createNode("node").lock())
							{
								if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity(fileName, Ape::Entity::GEOMETRY_FILE).lock()))
								{
									meshFile->setFileName(fileName);
									//meshFile->mergeSubMeshes();
									//TODO_ApeAssimpAssetLoaderPlugin how to use it when static geomtery is created?
									meshFile->setParentNode(node);
									//TODO_ApeAssimpAssetLoaderPlugin how to export the optimized mesh when static geomtery is created?
									//std::this_thread::sleep_for(std::chrono::milliseconds(20000));
									//meshFile->exportMesh();
								}
							}
						}
					}
				}
				mAssimpAssetConfigs.push_back(assetConfig);
				readFile(assetConfig.file);
			}
		}
		fclose(apeAssimpAssetLoaderConfigFile);
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::readFile(std::string fileName)
{
	APE_LOG_FUNC_ENTER();
	std::lock_guard<std::mutex> guard(mMutex);
	if (mpAssimpImporter)
	{
		const aiScene* assimpScene = mpAssimpImporter->ReadFile(fileName, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if (!assimpScene)
		{
			APE_LOG_ERROR("Loading the asset " << fileName << " was failed due to: " << mpAssimpImporter->GetErrorString());
		}
		else
		{
			APE_LOG_DEBUG("Loading the asset " << fileName << " was started");
			mAssimpScenes.push_back(assimpScene);
			loadScene(assimpScene, mAssetCount);
			mAssetCount++;
		}
	}
	APE_LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::loadScene(const aiScene* assimpScene, int ID)
{
	APE_LOG_FUNC_ENTER();
	if (assimpScene->mRootNode)
	{
		//APE_LOG_DEBUG("mNumMeshes: " << assimpScene->mNumMeshes);
		createNode(ID, assimpScene->mRootNode);
		if (auto assimpSceneRootNode = mpSceneManager->getNode(assimpScene->mRootNode->mName.C_Str()).lock())
		{
			APE_LOG_DEBUG("assimpSceneRootNode: " << assimpSceneRootNode->getName());
			if (mAssimpAssetConfigs[ID].mergeAndExportMeshes)
			{
				if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpSceneManager->createEntity(mAssimpAssetConfigs[ID].file, Ape::Entity::GEOMETRY_FILE).lock()))
				{
					meshFile->exportMesh();
					meshFile->setParentNode(assimpSceneRootNode);
					assimpSceneRootNode->setOrientation(Ape::Quaternion(1, 0, 0, 0));
					APE_LOG_DEBUG("mergeAndExportMeshes: " << mAssimpAssetConfigs[ID].file);
				}
			}
			else
			{
				if (auto rootNode = mpSceneManager->getNode(mAssimpAssetConfigs[ID].rootNodeName).lock())
				{
					APE_LOG_DEBUG("setScale to " << mAssimpAssetConfigs[ID].scale.toString() << " setPosition to " << mAssimpAssetConfigs[ID].position.toString()
						<< " setOrientation to " << mAssimpAssetConfigs[ID].orientation.toString());
					rootNode->setScale(mAssimpAssetConfigs[ID].scale);
					rootNode->setOrientation(mAssimpAssetConfigs[ID].orientation);
					rootNode->setPosition(mAssimpAssetConfigs[ID].position);
					assimpSceneRootNode->setParentNode(rootNode);
				}
				else
				{
					APE_LOG_DEBUG("rootNode was not found by name: " << mAssimpAssetConfigs[ID].rootNodeName);
				}
			}
		}
	}
	APE_LOG_FUNC_LEAVE();
}
