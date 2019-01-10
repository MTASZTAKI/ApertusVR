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
#include <memory>
#include "ApePluginAPI.h"
#include "ApeIEventManager.h"
#include "ApeILogManager.h"
#include "ApeIScene.h"
#include "ApeINode.h"
#include "ApeILight.h"
#include "ApeITextGeometry.h"
#include "ApeIFileGeometry.h"
#include "ApeIFileMaterial.h"
#include "ApeISystemConfig.h"

#define THIS_PLUGINNAME "UserInputMacro"

namespace Ape
{

    class UserInputMacro
    {
	private:
		Ape::IEventManager* mpEventManager;

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::NodeWeakPtr mUserNode;

		Ape::NodeWeakPtr mHeadNode;

		Ape::NodeWeakPtr mDummyNode;

		void eventCallBack(const Ape::Event& event);

	public:
		UserInputMacro();

		~UserInputMacro();

		void translateUserNode(Ape::Vector3 axis, Ape::Node::TransformationSpace transformationSpace);

		void rotateUserNode(Ape::Degree angle, Ape::Vector3 axis, Ape::Node::TransformationSpace transformationSpace);
    };
}

#endif
