#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "ApeAssimpAssetLoaderPlugin.h"

Ape::AssimpAssetLoaderPlugin::AssimpAssetLoaderPlugin()
{
	LOG_FUNC_ENTER();
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpAssimpImporter = nullptr;
	mAssimpScenes = std::vector<const aiScene*>();
	mAssimpAssetFileNames = std::vector<std::string>();
	std::srand(std::time(0));
	mMergeAndExportMeshes = false;
	mObjectCount = 0;
	mSceneUnitScale = Ape::Vector3();
	mSceneUnitPosition = Ape::Vector3();
	mRegenerateNormals = false;
	mRootNode = Ape::NodeWeakPtr();
	mpEventManager->connectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&AssimpAssetLoaderPlugin::eventCallBack, this, std::placeholders::_1));
	mAssetCount = 0;
	LOG_FUNC_LEAVE();
}

Ape::AssimpAssetLoaderPlugin::~AssimpAssetLoaderPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Init()
{
	LOG_FUNC_ENTER();
	LOG(LOG_TYPE_DEBUG, "AssimpAssetLoaderPlugin waiting for main window");
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	LOG(LOG_TYPE_DEBUG, "AssimpAssetLoaderPlugin main window was found");
	mpAssimpImporter = new Assimp::Importer();
	LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Run()
{
	LOG_FUNC_ENTER();
	loadConfig();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	mpEventManager->disconnectEvent(Ape::Event::Group::GEOMETRY_FILE, std::bind(&AssimpAssetLoaderPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::Step()
{
	
}

void Ape::AssimpAssetLoaderPlugin::Stop()
{
	
}

void Ape::AssimpAssetLoaderPlugin::Suspend()
{
	
}

void Ape::AssimpAssetLoaderPlugin::Restart()
{
	
}

void Ape::AssimpAssetLoaderPlugin::eventCallBack(const Ape::Event & event)
{
	if (event.type == Ape::Event::Type::GEOMETRY_FILE_FILENAME)
	{
		if (auto fileGeometry = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->getEntity(event.subjectName).lock()))
		{
			//LOG(LOG_TYPE_DEBUG, "GEOMETRY_FILE_FILENAME: subjectName: " << event.subjectName);
			//LOG(LOG_TYPE_DEBUG, "GEOMETRY_FILE_FILENAME: fileName: " << fileGeometry->getFileName());

			readFile(fileGeometry->getFileName());
		}
	}
}

void Ape::AssimpAssetLoaderPlugin::createNode(int assimpSceneID, aiNode* assimpNode)
{
	LOG_FUNC_ENTER();
	mObjectCount++;
	aiMatrix4x4 nodeTransformation = assimpNode->mTransformation;
	aiVector3t<float> scaling;
	aiQuaterniont<float> rotation;
	aiVector3t<float> position;
	nodeTransformation.Decompose(scaling, rotation, position);
	//TODO get uuid generator
	std::stringstream nodeUniqueName;
	std::string assimpNodeOriginalName = assimpNode->mName.C_Str();
	nodeUniqueName << assimpNode->mName.C_Str() << "_" << mObjectCount;
	assimpNode->mName = nodeUniqueName.str();
	if (auto node = mpScene->createNode(nodeUniqueName.str()).lock())
	{
		auto parentNode = Ape::NodeWeakPtr();
		if (assimpNode->mParent)
		{
			parentNode = mpScene->getNode(assimpNode->mParent->mName.C_Str());
			if (parentNode.lock())
				node->setParentNode(parentNode);
		}
		node->setPosition(Ape::Vector3(position.x, position.y, position.z));
		node->setOrientation(Ape::Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
		node->setScale(Ape::Vector3(scaling.x, scaling.y, scaling.z));
		//LOG(LOG_TYPE_DEBUG, "nodeName: " << node->getName());
		for (int i = 0; i < assimpNode->mNumMeshes; i++)
		{
			mObjectCount++;
			aiMesh* assimpMesh = mAssimpScenes[assimpSceneID]->mMeshes[assimpNode->mMeshes[i]];
			//TODO get uuid generator
			std::stringstream meshUniqueName;
			meshUniqueName << assimpNodeOriginalName << "_" << mObjectCount;
			assimpMesh->mName = meshUniqueName.str();
			if (auto mesh = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpScene->createEntity(meshUniqueName.str(), Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				Ape::GeometryCoordinates coordinates = Ape::GeometryCoordinates();
				for (int i = 0; i < assimpMesh->mNumVertices; i++)
				{
					aiVector3D assimpVertex = assimpMesh->mVertices[i];
					Ape::Vector3 vertexPosition(assimpVertex.x, assimpVertex.y, assimpVertex.z);
					if (mMergeAndExportMeshes)
						vertexPosition = (node->getDerivedPosition() + (node->getDerivedOrientation() * vertexPosition)) * mSceneUnitScale; //TODO somehow detect the unit of the scene
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
				modifiedMaterialName += mAssimpAssetFileNames[assimpSceneID].substr(mAssimpAssetFileNames[assimpSceneID].find_last_of("/\\"));
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), '/'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), '/\\'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), ':'), modifiedMaterialName.end());
				modifiedMaterialName.erase(std::remove(modifiedMaterialName.begin(), modifiedMaterialName.end(), ','), modifiedMaterialName.end());
				auto material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(modifiedMaterialName).lock());
				//TODO just a hotfix for window mesh where no transparent material is created
				//if (meshUniqueName.str().find("window") != std::string::npos || meshUniqueName.str().find("Window") != std::string::npos)
				//{
				//	modifiedMaterialName += "window";
				//	material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(modifiedMaterialName).lock());
				//	if (!material)
				//	{
				//		material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(modifiedMaterialName, Ape::Entity::MATERIAL_MANUAL).lock());
				//		float opacity = 0.12f;
				//		material->setDiffuseColor(Ape::Color(0.058053, 0.0753292, 0.0675212, opacity));
				//		material->setSpecularColor(Ape::Color(0.58053, 0.753292, 0.675212, opacity));
				//		material->setAmbientColor(Ape::Color(0, 0, 0));
				//		material->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
				//		//LOG(LOG_TYPE_DEBUG, "blending TRANSPARENT_ALPHA: " << opacity);
				//	}
				//}
				//TODO end
				if (!material)
				{
					material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(modifiedMaterialName, Ape::Entity::MATERIAL_MANUAL).lock());
					aiColor3D colorDiffuse(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);
					float opacity = 1.0f;
					asssimpMaterial->Get(AI_MATKEY_OPACITY, opacity);
					material->setDiffuseColor(Ape::Color(colorDiffuse.r, colorDiffuse.g, colorDiffuse.b, opacity));
					aiColor3D colorSpecular(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
					material->setSpecularColor(Ape::Color(colorSpecular.r, colorSpecular.g, colorSpecular.b, opacity));
					aiColor3D colorAmbient(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, colorAmbient);
					material->setAmbientColor(Ape::Color(colorAmbient.r, colorAmbient.g, colorAmbient.b));
					aiColor3D colorEmissive(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, colorEmissive);
					material->setEmissiveColor(Ape::Color(colorEmissive.r, colorEmissive.g, colorEmissive.b));
					aiColor3D colorTransparent(0.0f, 0.0f, 0.0f);
					asssimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, colorTransparent);
					//LOG(LOG_TYPE_DEBUG, "colorTransparent: " << colorTransparent.r << colorTransparent.g << colorTransparent.b);
					int sceneBlendingType = 0;
					asssimpMaterial->Get(AI_MATKEY_BLEND_FUNC, sceneBlendingType);
					if (sceneBlendingType == aiBlendMode_Additive)
					{
						material->setSceneBlending(Ape::Pass::SceneBlendingType::ADD);
						//LOG(LOG_TYPE_DEBUG, "blending ADD: " << opacity);
					}
					else if (sceneBlendingType == aiBlendMode_Default)
					{
						if (opacity < 0.99)
						{
							material->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
							//LOG(LOG_TYPE_DEBUG, "blending TRANSPARENT_ALPHA: " << opacity);
						}
						else
						{
							material->setSceneBlending(Ape::Pass::SceneBlendingType::REPLACE);
							//LOG(LOG_TYPE_DEBUG, "blending REPLACE: " << opacity);
						}
					}
					//LOG(LOG_TYPE_DEBUG, "createManualMaterial: " << material->getName());
				}
				Ape::GeometryNormals normals = Ape::GeometryNormals();
				if (assimpMesh->HasNormals() && !mRegenerateNormals)
				{
					for (int i = 0; i < assimpMesh->mNumFaces; i++)
					{
						aiVector3D assimpNormal = assimpMesh->mNormals[i];
						normals.push_back(assimpNormal.x);
						normals.push_back(assimpNormal.y);
						normals.push_back(assimpNormal.z);
					}
					//LOG(LOG_TYPE_DEBUG, "hasNormal: " << assimpMesh->mName.C_Str());
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
						//LOG(LOG_TYPE_DEBUG, "hasVertexColors: " << assimpMesh->mName.C_Str());
					}
				}
				std::string groupName = std::string();
				if (mMergeAndExportMeshes)
					groupName = mAssimpAssetFileNames[assimpSceneID];
				mesh->setParameters(groupName, coordinates, indices, normals, mRegenerateNormals, colors, Ape::GeometryTextureCoordinates(), material);
				if (!mMergeAndExportMeshes)
					mesh->setParentNode(node);
				//LOG(LOG_TYPE_DEBUG, "createIndexedFaceSetGeometry: " << mesh->getName());
			}
		}
	}
	for (int i = 0; i < assimpNode->mNumChildren; i++)
		createNode(assimpSceneID, assimpNode->mChildren[i]);
	LOG_FUNC_LEAVE();
}

void Ape::AssimpAssetLoaderPlugin::loadConfig()
{
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
			rapidjson::Value& mergeAndExportMeshes = jsonDocument["mergeAndExportMeshes"];
			mMergeAndExportMeshes = mergeAndExportMeshes.GetBool();
			mSceneUnitScale.x = jsonDocument["scale"].GetArray()[0].GetFloat();
			mSceneUnitScale.y = jsonDocument["scale"].GetArray()[1].GetFloat();
			mSceneUnitScale.z = jsonDocument["scale"].GetArray()[2].GetFloat();
			mSceneUnitPosition.x = jsonDocument["position"].GetArray()[0].GetFloat();
			mSceneUnitPosition.y = jsonDocument["position"].GetArray()[1].GetFloat();
			mSceneUnitPosition.z = jsonDocument["position"].GetArray()[2].GetFloat();
			mSceneUnitOrientation.w = jsonDocument["orientation"].GetArray()[0].GetFloat();
			mSceneUnitOrientation.x = jsonDocument["orientation"].GetArray()[1].GetFloat();
			mSceneUnitOrientation.y = jsonDocument["orientation"].GetArray()[2].GetFloat();
			mSceneUnitOrientation.z = jsonDocument["orientation"].GetArray()[3].GetFloat();
			rapidjson::Value& regenerateNormals = jsonDocument["regenerateNormals"];
			mRegenerateNormals = regenerateNormals.GetBool();
			//LOG(LOG_TYPE_DEBUG, "regenerateNormals? " << mRegenerateNormals);
			rapidjson::Value& rootNodeName = jsonDocument["rootNodeName"];
			mRootNode = mpScene->createNode(rootNodeName.GetString());
			//LOG(LOG_TYPE_DEBUG, "mRootNode: " << rootNodeName.GetString());

			rapidjson::Value& assimpAssetFileNames = jsonDocument["assets"];
			for (auto& assimpAssetFileName : assimpAssetFileNames.GetArray())
			{
				std::stringstream assimpAssetFileNamePath;
				assimpAssetFileNamePath << APE_SOURCE_DIR << assimpAssetFileName.GetString();
				std::string fileName = assimpAssetFileNamePath.str().substr(assimpAssetFileNamePath.str().find_last_of("/\\") + 1);
				std::string fileExtension = assimpAssetFileNamePath.str().substr(assimpAssetFileNamePath.str().find_last_of("."));
				//TODO be careful maybe should impelent the pluginManager interface and get infomration about ogreRender plugin  (native format when ogrePlugin is the renderer)
				if (fileExtension == ".mesh")
				{
					if (auto node = mpScene->createNode("node").lock())
					{
						if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(fileName, Ape::Entity::GEOMETRY_FILE).lock()))
						{
							meshFile->setFileName(fileName);
							//meshFile->mergeSubMeshes();
							//TODO how to use it when static geomtery is created?
							meshFile->setParentNode(node);
							//TODO how to export the optimized mesh when static geomtery is created?
							//std::this_thread::sleep_for(std::chrono::milliseconds(20000));
							//meshFile->exportMesh();
						}
					}
				}
				//TODO end
				else
				{
					readFile(assimpAssetFileNamePath.str());
				}
			}
		}
		fclose(apeAssimpAssetLoaderConfigFile);
	}
}

void Ape::AssimpAssetLoaderPlugin::readFile(std::string fileName)
{
	std::lock_guard<std::mutex> guard(mMutex);
	if (mpAssimpImporter)
	{
		const aiScene* assimpScene = mpAssimpImporter->ReadFile(fileName, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if (!assimpScene)
		{
			LOG(LOG_TYPE_ERROR, "Loading the asset " << fileName << " was failed due to: " << mpAssimpImporter->GetErrorString());
		}
		else
		{
			LOG(LOG_TYPE_DEBUG, "Loading the asset " << fileName << " was started");
			mAssimpAssetFileNames.push_back(fileName);
			mAssimpScenes.push_back(assimpScene);
			loadScene(assimpScene, mAssetCount);
			mAssetCount++;
		}
	}
}

void Ape::AssimpAssetLoaderPlugin::loadScene(const aiScene* assimpScene, int ID)
{
	LOG_FUNC_ENTER();
	if (assimpScene->mRootNode)
	{
		//LOG(LOG_TYPE_DEBUG, "mNumMeshes: " << assimpScene->mNumMeshes);
		createNode(ID, assimpScene->mRootNode);
		if (auto rootNode = mpScene->getNode(assimpScene->mRootNode->mName.C_Str()).lock())
		{
			if (mMergeAndExportMeshes)
			{
				if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(mAssimpAssetFileNames[ID], Ape::Entity::GEOMETRY_FILE).lock()))
				{
					meshFile->exportMesh();
					meshFile->setParentNode(rootNode);
					rootNode->setOrientation(Ape::Quaternion(1, 0, 0, 0));
				}
			}
			else
			{
				if (auto node = mRootNode.lock())
				{
					//TODO somehow detect the unit of the scene
					LOG(LOG_TYPE_DEBUG, "setScale to " << mSceneUnitScale.toString() << " setPosition to " << mSceneUnitPosition.toString() << " setOrientation to " << mSceneUnitOrientation.toString());
					node->setScale(mSceneUnitScale);
					node->setOrientation(mSceneUnitOrientation);
					node->setPosition(mSceneUnitPosition);
					rootNode->setParentNode(node);
				}
			}
		}
	}
	LOG_FUNC_LEAVE();
}
