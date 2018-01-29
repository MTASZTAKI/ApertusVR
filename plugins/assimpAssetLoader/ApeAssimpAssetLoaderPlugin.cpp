/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "ApeAssimpAssetLoaderPlugin.h"


Ape::AssimpAssetLoaderPlugin::AssimpAssetLoaderPlugin()
{
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
	mSceneUnitScale = 1;
	mRegenerateNormals = false;
}

Ape::AssimpAssetLoaderPlugin::~AssimpAssetLoaderPlugin()
{
	std::cout << "AssimpAssetLoaderPlugin dtor" << std::endl;
}

void Ape::AssimpAssetLoaderPlugin::Init()
{
	std::cout << "AssimpAssetLoaderPlugin::Init" << std::endl;
	std::cout << "AssimpAssetLoaderPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "AssimpAssetLoaderPlugin main window was found" << std::endl;
}

void Ape::AssimpAssetLoaderPlugin::Run()
{
	mpAssimpImporter = new Assimp::Importer();
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
							meshFile->mergeSubMeshes();
							//TODO how to use it when static geomtery is created?
							//meshFile->setParentNode(node);
							//TODO how to export the optimized mesh when static geomtery is created?
							//std::this_thread::sleep_for(std::chrono::milliseconds(20000));
							//meshFile->exportMesh();
						}
					}
				}
				//TODO end
				else
				{
					mAssimpAssetFileNames.push_back(assimpAssetFileNamePath.str());
					mAssimpScenes.push_back(mpAssimpImporter->ReadFile(assimpAssetFileNamePath.str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType));
				}
			}
			rapidjson::Value& mergeAndExportMeshes = jsonDocument["mergeAndExportMeshes"];
			mMergeAndExportMeshes = mergeAndExportMeshes.GetBool();
			rapidjson::Value& scale = jsonDocument["scale"];
			mSceneUnitScale = scale.GetFloat();
			rapidjson::Value& regenerateNormals = jsonDocument["regenerateNormals"];
			mRegenerateNormals = regenerateNormals.GetBool();
			std::cout << "AssimpAssetLoaderPlugin::run regenerateNormals? " << mRegenerateNormals << std::endl;
		}
		fclose(apeAssimpAssetLoaderConfigFile);
	}
	for (int i = 0; i < mAssimpAssetFileNames.size(); i++)
	{
		const aiScene* assimpScene = mAssimpScenes[i];
		if (!assimpScene)
			std::cout << "AssimpAssetLoaderPlugin::loading the asset " << mAssimpAssetFileNames[i] << " was failed due to: " << mpAssimpImporter->GetErrorString() << std::endl;
		else if (assimpScene->mRootNode)
		{
			std::cout << "AssimpAssetLoaderPlugin::mNumMeshes: " << assimpScene->mNumMeshes << std::endl;
			createNode(i, assimpScene->mRootNode);
			if (auto rootNode = mpScene->getNode(assimpScene->mRootNode->mName.C_Str()).lock())
			{
				if (mMergeAndExportMeshes)
				{
					if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(mAssimpAssetFileNames[i], Ape::Entity::GEOMETRY_FILE).lock()))
					{
						meshFile->exportMesh();
						meshFile->setParentNode(rootNode);
						rootNode->setOrientation(Ape::Quaternion(1, 0, 0, 0));
					}
				}
				else
				{
					//TODO somehow detect the unit of the scene
					std::cout << "AssimpAssetLoaderPlugin::run setScale to " << mSceneUnitScale << std::endl;
					rootNode->setScale(Ape::Vector3(mSceneUnitScale, mSceneUnitScale, mSceneUnitScale));
				}
			}
		}
	}
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

void Ape::AssimpAssetLoaderPlugin::createNode(int assimpSceneID, aiNode* assimpNode)
{
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
		std::cout << "AssimpAssetLoaderPlugin::createNode " << node->getName() << std::endl;
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
				if (meshUniqueName.str().find("window") != std::string::npos || meshUniqueName.str().find("Window") != std::string::npos)
				{
					modifiedMaterialName += "window";
					material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(modifiedMaterialName).lock());
					if (!material)
					{
						material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(modifiedMaterialName, Ape::Entity::MATERIAL_MANUAL).lock());
						std::stringstream manualPassName;
						manualPassName << modifiedMaterialName << "ManualPass" << std::rand();
						if (auto materialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity(manualPassName.str(), Ape::Entity::PASS_MANUAL).lock()))
						{
							float opacity = 0.12f;
							materialManualPass->setShininess(0.0f);
							materialManualPass->setDiffuseColor(Ape::Color(0.058053, 0.0753292, 0.0675212, opacity));
							materialManualPass->setSpecularColor(Ape::Color(0.58053, 0.753292, 0.675212, opacity));
							materialManualPass->setAmbientColor(Ape::Color(0, 0, 0));
							materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
							std::cout << "AssimpAssetLoaderPlugin::blending TRANSPARENT_ALPHA " << opacity << std::endl;
							material->setPass(materialManualPass);
							std::cout << "AssimpAssetLoaderPlugin::createManualMaterial " << material->getName() << std::endl;
						}
					}
				}
				//TODO end
				if (!material)
				{
					material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(modifiedMaterialName, Ape::Entity::MATERIAL_MANUAL).lock());
					std::stringstream manualPassName;
					manualPassName << modifiedMaterialName << "ManualPass" << std::rand();
					if (auto materialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity(manualPassName.str(), Ape::Entity::PASS_MANUAL).lock()))
					{
						float shininess = 0.0f;
						asssimpMaterial->Get(AI_MATKEY_SHININESS, shininess);
						materialManualPass->setShininess(shininess);
						aiColor3D colorDiffuse(0.0f, 0.0f, 0.0f);
						asssimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colorDiffuse);
						float opacity = 1.0f;
						asssimpMaterial->Get(AI_MATKEY_OPACITY, opacity);
						materialManualPass->setDiffuseColor(Ape::Color(colorDiffuse.r, colorDiffuse.g, colorDiffuse.b, opacity));
						aiColor3D colorSpecular(0.0f, 0.0f, 0.0f);
						asssimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, colorSpecular);
						materialManualPass->setSpecularColor(Ape::Color(colorSpecular.r, colorSpecular.g, colorSpecular.b, opacity));
						aiColor3D colorAmbient(0.0f, 0.0f, 0.0f);
						asssimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, colorAmbient);
						materialManualPass->setAmbientColor(Ape::Color(colorAmbient.r, colorAmbient.g, colorAmbient.b));
						aiColor3D colorEmissive(0.0f, 0.0f, 0.0f);
						asssimpMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, colorEmissive);
						materialManualPass->setEmissiveColor(Ape::Color(colorEmissive.r, colorEmissive.g, colorEmissive.b));
						aiColor3D colorTransparent(0.0f, 0.0f, 0.0f);
						asssimpMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, colorTransparent);
						std::cout << "AssimpAssetLoaderPlugin::colorTransparent " << colorTransparent.r << colorTransparent.g << colorTransparent.b << std::endl;
						int sceneBlendingType = 0;
						asssimpMaterial->Get(AI_MATKEY_BLEND_FUNC, sceneBlendingType);
						if (sceneBlendingType == aiBlendMode_Additive)
						{
							materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::ADD);
							std::cout << "AssimpAssetLoaderPlugin::blending ADD " << opacity << std::endl;
						}
						else if (sceneBlendingType == aiBlendMode_Default)
						{
							if (opacity < 0.99)
							{
								materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
								std::cout << "AssimpAssetLoaderPlugin::blending TRANSPARENT_ALPHA " << opacity << std::endl;
							}
							else
							{
								materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::REPLACE);
								std::cout << "AssimpAssetLoaderPlugin::blending REPLACE " << opacity << std::endl;
							}
						}
						material->setPass(materialManualPass);
						std::cout << "AssimpAssetLoaderPlugin::createManualMaterial " << material->getName() << std::endl;
					}
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
					std::cout << "AssimpAssetLoaderPlugin::hasNormal " << assimpMesh->mName.C_Str() << std::endl;
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
						std::cout << "AssimpAssetLoaderPlugin::hasVertexColors " << assimpMesh->mName.C_Str() << std::endl;
					}
				}
				std::string groupName = std::string();
				if (mMergeAndExportMeshes)
					groupName = mAssimpAssetFileNames[assimpSceneID];
				mesh->setParameters(groupName, coordinates, indices, normals, mRegenerateNormals, colors, Ape::GeometryTextureCoordinates(), material);
				if (!mMergeAndExportMeshes)
					mesh->setParentNode(node);
				std::cout << "AssimpAssetLoaderPlugin::createIndexedFaceSetGeometry " << mesh->getName() << std::endl;
			}
		}
	}
	for (int i = 0; i < assimpNode->mNumChildren; i++)
		createNode(assimpSceneID, assimpNode->mChildren[i]);
}
