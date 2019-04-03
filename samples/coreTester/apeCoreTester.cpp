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

#include <sstream>
#include <iostream>
#include <thread>
#include "ape.h"
#include "system/apeSystem.h"
#include "managers/apeISceneManager.h"
#include "managers/apeICoreConfig.h"
#include "sceneElements/apeIManualMaterial.h"
#include "sceneElements/apeIIndexedFaceSetGeometry.h"

void myApeMakeBox()
{
	std::string myApeBoxName = "myApeBox";
	auto apeCoreConfig = ape::ICoreConfig::getSingletonPtr();
	auto apeSceneManager = ape::ISceneManager::getSingletonPtr();
	if (auto material = std::static_pointer_cast<ape::IManualMaterial>(apeSceneManager->createEntity(myApeBoxName + "Material", ape::Entity::MATERIAL_MANUAL).lock()))
	{
		material->setDiffuseColor(ape::Color(1.0f, 0.0f, 0.0f));
		material->setSpecularColor(ape::Color(1.0f, 0.0f, 0.0f));
		if (auto node = apeSceneManager->createNode(myApeBoxName + "Node").lock())
		{
			if (auto box = std::static_pointer_cast<ape::IIndexedFaceSetGeometry>(apeSceneManager->createEntity(myApeBoxName, ape::Entity::GEOMETRY_INDEXEDFACESET).lock()))
			{
				ape::GeometryCoordinates coordinates = {
					10,  10, -10,
					10, -10, -10,
					-10, -10, -10,
					-10,  10, -10,
					10,  10,  10,
					10, -10,  10,
					-10, -10,  10,
					-10,  10,  10
				};
				ape::GeometryIndices indices = {
					0, 1, 2, 3, -1,
					4, 7, 6, 5, -1,
					0, 4, 5, 1, -1,
					1, 5, 6, 2, -1,
					2, 6, 7, 3, -1,
					4, 0, 3, 7, -1 };
				box->setParameters("", coordinates, indices, ape::GeometryNormals(), true, ape::GeometryColors(), ape::GeometryTextureCoordinates(), material);
				box->setParentNode(node);
				std::cout << "myApeBox was created in myApeThread" << std::endl;
			}
		}
	}
}

int main(int argc, char** argv)
{
	ape::System::Start("./", false, std::bind(myApeMakeBox));
	std::cout << "Press ENTER to stop Apertus Core..." << std::endl;
	std::cin.ignore(std::numeric_limits <std::streamsize> ::max(), '\n');
	ape::System::Stop();
	return 0;
}
