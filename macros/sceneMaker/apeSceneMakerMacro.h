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
#include "apePluginAPI.h"
#include "apeIEventManager.h"
#include "apeILogManager.h"
#include "apeISceneManager.h"
#include "apeINode.h"
#include "apeILight.h"
#include "apeITextGeometry.h"
#include "apeIFileGeometry.h"
#include "apeIPlaneGeometry.h"
#include "apeITubeGeometry.h"
#include "apeIIndexedLineSetGeometry.h"
#include "apeIIndexedFaceSetGeometry.h"
#include "apeIConeGeometry.h"
#include "apeIFileMaterial.h"
#include "apeIManualMaterial.h"
#include "apeICamera.h"
#include "apeIBrowser.h"
#include "apeIUnitTexture.h"
#include "apeICoreConfig.h"
#include "apeInterpolator.h"
#include "apeSingleton.h"

namespace ape
{
    class SceneMakerMacro
    {
	private:
		ape::IEventManager* mpEventManager;

		ape::ISceneManager* mpSceneManager;

		ape::ICoreConfig* mpCoreConfig;

		void eventCallBack(const ape::Event& event);

	public:
		SceneMakerMacro();

		~SceneMakerMacro();

		void makeLit(ape::Color color);

		void makeGround();

		void makeModel(std::string fileName);

		void makeBrowser(std::string name, std::string url, ape::Vector3 position, ape::Quaternion orientation, float width, float height, float resoultionVertical, float resolutionHorizontal);

		void makeSky();

		void makeBackground();

		void makeWater();

		void makeTerrain();

		void makeCoordinateSystem();

		void makeBox(std::string name);

		void makeOverlayBrowser(std::string url);

		void interpolate(ape::NodeWeakPtr node, ape::Vector3 position, ape::Quaternion orientation, unsigned int milliseconds);

    };
}

#endif
