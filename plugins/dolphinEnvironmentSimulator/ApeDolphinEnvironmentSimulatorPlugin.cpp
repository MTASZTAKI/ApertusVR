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
#include "ApeDolphinEnvironmentSimulatorPlugin.h"


Ape::DolphinEnvironmentSimulatorPlugin::DolphinEnvironmentSimulatorPlugin()
{
	mpScene = Ape::IScene::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpMainWindow = Ape::IMainWindow::getSingletonPtr();
	mpdolphinImporter = nullptr;
	mdolphinScenes = std::vector<const aiScene*>();
	mdolphinAssetFileNames = std::vector<std::string>();
	std::srand(std::time(0));
	mMergeAndExportMeshes = false;
	mObjectCount = 0;
	mSceneUnitScale = 100;
}

Ape::DolphinEnvironmentSimulatorPlugin::~DolphinEnvironmentSimulatorPlugin()
{
	std::cout << "DolphinEnvironmentSimulatorPlugin dtor" << std::endl;
}

void Ape::DolphinEnvironmentSimulatorPlugin::Init()
{
	std::cout << "DolphinEnvironmentSimulatorPlugin::Init" << std::endl;
	std::cout << "DolphinEnvironmentSimulatorPlugin waiting for main window" << std::endl;
	while (mpMainWindow->getHandle() == nullptr)
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "DolphinEnvironmentSimulatorPlugin main window was found" << std::endl;
}

void Ape::DolphinEnvironmentSimulatorPlugin::Run()
{
	mpdolphinImporter = new dolphin::Importer();
	std::stringstream fileFullPath;
	fileFullPath << mpSystemConfig->getFolderPath() << "\\ApeDolphinEnvironmentSimulatorPlugin.json";
	FILE* apedolphinAssetLoaderConfigFile = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apedolphinAssetLoaderConfigFile)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apedolphinAssetLoaderConfigFile, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			rapidjson::Value& dolphinAssetFileNames = jsonDocument["assets"];
			for (auto& dolphinAssetFileName : dolphinAssetFileNames.GetArray())
			{
				std::stringstream dolphinAssetFileNamePath;
				dolphinAssetFileNamePath << APE_SOURCE_DIR << dolphinAssetFileName.GetString();
				std::string fileName = dolphinAssetFileNamePath.str().substr(dolphinAssetFileNamePath.str().find_last_of("/\\") + 1);
				std::string fileExtension = dolphinAssetFileNamePath.str().substr(dolphinAssetFileNamePath.str().find_last_of("."));
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
					mdolphinAssetFileNames.push_back(dolphinAssetFileNamePath.str());
					mdolphinScenes.push_back(mpdolphinImporter->ReadFile(dolphinAssetFileNamePath.str(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType));
				}
			}
			rapidjson::Value& mergeAndExportMeshes = jsonDocument["mergeAndExportMeshes"];
			mMergeAndExportMeshes = mergeAndExportMeshes.GetBool();
		}
		fclose(apedolphinAssetLoaderConfigFile);
	}
	for (int i = 0; i < mdolphinAssetFileNames.size(); i++)
	{
		const aiScene* dolphinScene = mdolphinScenes[i];
		if (!dolphinScene)
			std::cout << "DolphinEnvironmentSimulatorPlugin::loading the asset " << mdolphinAssetFileNames[i] << " was failed due to: " << mpdolphinImporter->GetErrorString() << std::endl;
		else if (dolphinScene->mRootNode)
		{
			std::cout << "DolphinEnvironmentSimulatorPlugin::mNumMeshes: " << dolphinScene->mNumMeshes << std::endl;
			createNode(i, dolphinScene->mRootNode);
			if (auto rootNode = mpScene->getNode(dolphinScene->mRootNode->mName.C_Str()).lock())
			{
				if (mMergeAndExportMeshes)
				{
					if (auto meshFile = std::static_pointer_cast<Ape::IFileGeometry>(mpScene->createEntity(mdolphinAssetFileNames[i], Ape::Entity::GEOMETRY_FILE).lock()))
					{
						meshFile->exportMesh();
						meshFile->setParentNode(rootNode);
						rootNode->setOrientation(Ape::Quaternion(1, 0, 0, 0));
					}
				}
				else
				{
					//TODO somehow detect the unit of the scene
					rootNode->setScale(Ape::Vector3(mSceneUnitScale, mSceneUnitScale, mSceneUnitScale));
				}
			}
		}
	}
}

void Ape::DolphinEnvironmentSimulatorPlugin::Step()
{
	
}

void Ape::DolphinEnvironmentSimulatorPlugin::Stop()
{
	
}

void Ape::DolphinEnvironmentSimulatorPlugin::Suspend()
{
	
}

void Ape::DolphinEnvironmentSimulatorPlugin::Restart()
{
	
}

void Ape::DolphinEnvironmentSimulatorPlugin::createNode(int dolphinSceneID, aiNode* dolphinNode)
{
	mObjectCount++;
	aiMatrix4x4 nodeTransformation = dolphinNode->mTransformation;
	aiVector3t<float> scaling;
	aiQuaterniont<float> rotation;
	aiVector3t<float> position;
	nodeTransformation.Decompose(scaling, rotation, position);
	//TODO get uuid generator
	std::stringstream nodeUniqueName;
	std::string dolphinNodeOriginalName = dolphinNode->mName.C_Str();
	nodeUniqueName << dolphinNode->mName.C_Str() << "_" << mObjectCount;
	dolphinNode->mName = nodeUniqueName.str();
	if (auto node = mpScene->createNode(nodeUniqueName.str()).lock())
	{
		auto parentNode = Ape::NodeWeakPtr();
		if (dolphinNode->mParent)
		{
			parentNode = mpScene->getNode(dolphinNode->mParent->mName.C_Str());
			if (parentNode.lock())
				node->setParentNode(parentNode);
		}
		node->setPosition(Ape::Vector3(position.x, position.y, position.z));
		node->setOrientation(Ape::Quaternion(rotation.w, rotation.x, rotation.y, rotation.z));
		node->setScale(Ape::Vector3(scaling.x, scaling.y, scaling.z));
		std::cout << "DolphinEnvironmentSimulatorPlugin::createNode " << node->getName() << std::endl;
		for (int i = 0; i < dolphinNode->mNumMeshes; i++)
		{
			mObjectCount++;
			aiMesh* dolphinMesh = mdolphinScenes[dolphinSceneID]->mMeshes[dolphinNode->mMeshes[i]];
			//TODO get uuid generator
			std::stringstream meshUniqueName;
			meshUniqueName << dolphinNodeOriginalName << "_" << mObjectCount;
			dolphinMesh->mName = meshUniqueName.str();
			if (auto mesh = std::static_pointer_cast<Ape::IIndexedFaceSetGeometry>(mpScene->createEntity(meshUniqueName.str(), Ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				Ape::GeometryCoordinates coordinates = Ape::GeometryCoordinates();
				for (int i = 0; i < dolphinMesh->mNumVertices; i++)
				{
					aiVector3D dolphinVertex = dolphinMesh->mVertices[i];
					Ape::Vector3 vertexPosition(dolphinVertex.x, dolphinVertex.y, dolphinVertex.z);
					if (mMergeAndExportMeshes)
						vertexPosition = (node->getDerivedPosition() + (node->getDerivedOrientation() * vertexPosition)) * mSceneUnitScale; //TODO somehow detect the unit of the scene
					coordinates.push_back(vertexPosition.x);
					coordinates.push_back(vertexPosition.y);
					coordinates.push_back(vertexPosition.z);
				}
				Ape::GeometryIndices indices = Ape::GeometryIndices();
				for (int i = 0; i < dolphinMesh->mNumFaces; i++)
				{
					aiFace dolphinFace = dolphinMesh->mFaces[i];
					for (int i = 0; i < dolphinFace.mNumIndices; i++)
						indices.push_back(dolphinFace.mIndices[i]);
					indices.push_back(-1);
				}
				aiMaterial* asssimpMaterial = mdolphinScenes[dolphinSceneID]->mMaterials[dolphinMesh->mMaterialIndex];
				aiString materialName;
				asssimpMaterial->Get(AI_MATKEY_NAME, materialName);
				std::string modifiedMaterialName = materialName.C_Str();
				modifiedMaterialName += mdolphinAssetFileNames[dolphinSceneID].substr(mdolphinAssetFileNames[dolphinSceneID].find_last_of("/\\"));
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
							std::cout << "DolphinEnvironmentSimulatorPlugin::blending TRANSPARENT_ALPHA " << opacity << std::endl;
							material->setPass(materialManualPass);
							std::cout << "DolphinEnvironmentSimulatorPlugin::createManualMaterial " << material->getName() << std::endl;
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
						std::cout << "DolphinEnvironmentSimulatorPlugin::colorTransparent " << colorTransparent.r << colorTransparent.g << colorTransparent.b << std::endl;
						int sceneBlendingType = 0;
						asssimpMaterial->Get(AI_MATKEY_BLEND_FUNC, sceneBlendingType);
						if (sceneBlendingType == aiBlendMode_Additive)
						{
							materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::ADD);
							std::cout << "DolphinEnvironmentSimulatorPlugin::blending ADD " << opacity << std::endl;
						}
						else if (sceneBlendingType == aiBlendMode_Default)
						{
							if (opacity < 0.99)
							{
								materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::TRANSPARENT_ALPHA);
								std::cout << "DolphinEnvironmentSimulatorPlugin::blending TRANSPARENT_ALPHA " << opacity << std::endl;
							}
							else
							{
								materialManualPass->setSceneBlending(Ape::Pass::SceneBlendingType::REPLACE);
								std::cout << "DolphinEnvironmentSimulatorPlugin::blending REPLACE " << opacity << std::endl;
							}
						}
						material->setPass(materialManualPass);
						std::cout << "DolphinEnvironmentSimulatorPlugin::createManualMaterial " << material->getName() << std::endl;
					}
				}
				Ape::GeometryNormals normals = Ape::GeometryNormals();
				if (dolphinMesh->HasNormals())
				{
					for (int i = 0; i < dolphinMesh->mNumFaces; i++)
					{
						aiVector3D dolphinNormal = dolphinMesh->mNormals[i];
						normals.push_back(dolphinNormal.x);
						normals.push_back(dolphinNormal.y);
						normals.push_back(dolphinNormal.z);
					}
					std::cout << "DolphinEnvironmentSimulatorPlugin::hasNormal " << dolphinMesh->mName.C_Str() << std::endl;
				}
				Ape::GeometryColors colors = Ape::GeometryColors();
				for (int colorSetIndex = 0; colorSetIndex < AI_MAX_NUMBER_OF_COLOR_SETS; colorSetIndex++)
				{
					if (dolphinMesh->HasVertexColors(colorSetIndex))
					{
						for (int vertexIndex = 0; vertexIndex < dolphinMesh->mNumVertices; vertexIndex++)
						{
							aiColor4D dolphinColor = dolphinMesh->mColors[colorSetIndex][vertexIndex];
							colors.push_back(dolphinColor.r);
							colors.push_back(dolphinColor.g);
							colors.push_back(dolphinColor.b);
							colors.push_back(dolphinColor.a);
						}
						std::cout << "DolphinEnvironmentSimulatorPlugin::hasVertexColors " << dolphinMesh->mName.C_Str() << std::endl;
					}
				}
				std::string groupName = std::string();
				if (mMergeAndExportMeshes)
					groupName = mdolphinAssetFileNames[dolphinSceneID];
				mesh->setParameters(groupName, coordinates, indices, normals, true, colors, Ape::GeometryTextureCoordinates(), material);
				if (!mMergeAndExportMeshes)
					mesh->setParentNode(node);
				std::cout << "DolphinEnvironmentSimulatorPlugin::createIndexedFaceSetGeometry " << mesh->getName() << std::endl;
			}
		}
	}
	for (int i = 0; i < dolphinNode->mNumChildren; i++)
		createNode(dolphinSceneID, dolphinNode->mChildren[i]);
}
