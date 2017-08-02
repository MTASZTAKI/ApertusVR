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
				mAssimpAssetFileNames.push_back(assimpAssetFileNamePath.str());
				mAssimpScenes.push_back(mpAssimpImporter->ReadFile(assimpAssetFileNamePath.str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType));
			}
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
			createNode(i, assimpScene->mRootNode);
			//TODO somehow detect the unit of the scene
			if (auto rootNode = mpScene->getNode(assimpScene->mRootNode->mName.C_Str()).lock())
			{
				rootNode->setScale(Ape::Vector3(100, 100, 100));
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
	aiMatrix4x4 nodeTransformation = assimpNode->mTransformation;
	aiVector3t<float> scaling;
	aiQuaterniont<float> rotation;
	aiVector3t<float> position;
	nodeTransformation.Decompose(scaling, rotation, position);
	//TODO get uuid generator
	std::stringstream nodeUniqueName;
	nodeUniqueName << assimpNode->mName.C_Str() << std::rand();
	assimpNode->mName = nodeUniqueName.str();
	if (auto node = mpScene->createNode(nodeUniqueName.str()).lock())
	{
		if (assimpNode->mParent)
		{
			if (auto parentNode = mpScene->getNode(assimpNode->mParent->mName.C_Str()).lock())
				node->setParentNode(parentNode);
		}
		node->setPosition(Ape::Vector3(position.x, position.y, position.z));
		node->setOrientation(Ape::Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
		node->setScale(Ape::Vector3(scaling.x, scaling.y, scaling.z));
		std::cout << "AssimpAssetLoaderPlugin::createNode " << node->getName() << std::endl;
		for (int i = 0; i < assimpNode->mNumMeshes; i++)
		{
			aiMesh* assimpMesh = mAssimpScenes[assimpSceneID]->mMeshes[assimpNode->mMeshes[i]];
			//TODO get uuid generator
			std::stringstream meshUniqueName;
			meshUniqueName << assimpMesh->mName.C_Str() << std::rand();
			assimpMesh->mName = meshUniqueName.str();
			if (auto mesh = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpScene->createEntity(meshUniqueName.str(), Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				Ape::GeometryCoordinates coordinates = Ape::GeometryCoordinates();
				for (int i = 0; i < assimpMesh->mNumVertices; i++)
				{
					aiVector3D assimpVertex = assimpMesh->mVertices[i];
					coordinates.push_back(assimpVertex.x);
					coordinates.push_back(assimpVertex.y);
					coordinates.push_back(assimpVertex.z);
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
				auto material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->getEntity(materialName.C_Str()).lock());
				if (!material)
				{
					material = std::static_pointer_cast<Ape::IManualMaterial>(mpScene->createEntity(materialName.C_Str(), Ape::Entity::MATERIAL_MANUAL).lock());
					std::stringstream manualPassName;
					manualPassName << materialName.C_Str() << "ManualPass" << std::rand();
					if (auto materialManualPass = std::static_pointer_cast<Ape::IManualPass>(mpScene->createEntity(manualPassName.str(), Ape::Entity::PASS_MANUAL).lock()))
					{
						float shininess;
						asssimpMaterial->Get(AI_MATKEY_SHININESS, shininess);
						materialManualPass->setShininess(shininess);
						aiColor3D color(0.f, 0.f, 0.f);
						asssimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
						materialManualPass->setDiffuseColor(Ape::Color(color.r, color.g, color.b));
						asssimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
						materialManualPass->setSpecularColor(Ape::Color(color.r, color.g, color.b));
						material->setPass(materialManualPass);
						std::cout << "AssimpAssetLoaderPlugin::createManualMaterial " << material->getName() << std::endl;
					}
				}
				mesh->setParameters("", coordinates, indices, Ape::GeometryNormals(), true, Ape::GeometryColors(), Ape::GeometryTextureCoordinates(), material);
				mesh->setParentNode(node);
				std::cout << "AssimpAssetLoaderPlugin::createIndexedFaceSetGeometry " << mesh->getName() << std::endl;
			}
		}
	}
	for (int i = 0; i < assimpNode->mNumChildren; i++)
		createNode(assimpSceneID, assimpNode->mChildren[i]);
}
