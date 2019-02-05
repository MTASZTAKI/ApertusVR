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

#ifndef APE_USERINPUTMACRO_H
#define APE_USERINPUTMACRO_H

#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <memory>
#include "plugin/ApePluginAPI.h"
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"
#include "managers/ApeISceneManager.h"
#include "sceneelements/ApeINode.h"
#include "sceneelements/ApeILight.h"
#include "sceneelements/ApeITextGeometry.h"
#include "sceneelements/ApeIFileGeometry.h"
#include "sceneelements/ApeIConeGeometry.h"
#include "sceneelements/ApeIFileMaterial.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeICamera.h"
#include "managers/ApeISystemConfig.h"

namespace Ape
{
    class UserInputMacro
    {
	public: 
		struct Pose
		{
			Ape::Vector3 position;

			Ape::Quaternion orientation;

			Ape::Vector3 translate;

			Ape::Radian rotateAngle;
			
			Ape::Vector3 rotateAxis;

			Pose()
			{

			}

			Pose(Ape::Vector3 position, Ape::Quaternion orientation, Ape::Vector3 translate, Ape::Radian rotateAngle, Ape::Vector3 rotateAxis)
			{
				this->position = position;
				this->orientation = orientation;
				this->translate = translate;
				this->rotateAngle = rotateAngle;
				this->rotateAxis = rotateAxis;
			}
		};

	private:
		Ape::IEventManager* mpEventManager;

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::NodeWeakPtr mUserNode;

		std::string mUniqueUserNodeName;

		Ape::NodeWeakPtr mHeadNode;

		Ape::Vector3 mTranslate;

		std::map<std::string, Ape::CameraWeakPtr> mCameras;

		Ape::ManualMaterialWeakPtr mUserMaterial;

		void eventCallBack(const Ape::Event& event);

	public:
		UserInputMacro();

		~UserInputMacro();

		void updatePose(Pose pose); 

		Ape::CameraWeakPtr createCamera(std::string name);
    };
}

#endif
