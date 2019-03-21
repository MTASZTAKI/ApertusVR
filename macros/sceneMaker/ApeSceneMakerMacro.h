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

#ifndef APE_SCENEMAKERMACRO_H
#define APE_SCENEMAKERMACRO_H

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
#include "sceneelements/ApeIPlaneGeometry.h"
#include "sceneelements/ApeITubeGeometry.h"
#include "sceneelements/ApeIIndexedLineSetGeometry.h"
#include "sceneelements/ApeIIndexedFaceSetGeometry.h"
#include "sceneelements/ApeIConeGeometry.h"
#include "sceneelements/ApeIFileMaterial.h"
#include "sceneelements/ApeIManualMaterial.h"
#include "sceneelements/ApeICamera.h"
#include "sceneelements/ApeIBrowser.h"
#include "sceneelements/ApeIUnitTexture.h"
#include "managers/ApeISystemConfig.h"
#include "utils/ApeInterpolator.h"
#include "utils/ApeSingleton.h"

namespace Ape
{
    class SceneMakerMacro
    {
	private:
		Ape::IEventManager* mpEventManager;

		Ape::ISceneManager* mpSceneManager;

		Ape::ISystemConfig* mpSystemConfig;

		void eventCallBack(const Ape::Event& event);

	public:
		SceneMakerMacro();

		~SceneMakerMacro();

		void makeLit();

		void makeGround();

		void makeModel(std::string fileName);

		void makeBrowser();

		void makeSky();

		void makeBackground();

		void makeWater();

		void makeTerrain();

		void makeCoordinateSystem();

		void makeBox(std::string name);

		void makeOverlayBrowser();

		void interpolate(Ape::NodeWeakPtr node, Ape::Vector3 position, Ape::Quaternion orientation, unsigned int milliseconds);

    };
}

#endif
