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

#ifndef APE_ISCENENETWORK_H
#define APE_ISCENENETWORK_H

#ifdef _WIN32
#ifdef BUILDING_APE_SCENEMANAGER_DLL
#define APE_SCENEMANAGER_DLL_EXPORT __declspec(dllexport)
#else
#define APE_SCENEMANAGER_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_SCENEMANAGER_DLL_EXPORT 
#endif

#include <functional>
#include <map>
#include <vector>
#include "apeSingleton.h"
#include "apeSystem.h"

namespace ape
{
	namespace SceneNetwork
	{
		enum ParticipantType
		{
			HOST,
			GUEST,
			NONE,
			INVALID
		};
	}

	class APE_SCENEMANAGER_DLL_EXPORT ISceneNetwork : public Singleton<ISceneNetwork>
	{
	protected:
		virtual ~ISceneNetwork() {};

	public:
		virtual ape::SceneNetwork::ParticipantType getParticipantType() = 0;

		virtual bool isRoomRunning(std::string roomName) = 0;

		virtual void connectToRoom(std::string roomName, std::vector<std::string> configURLs, std::vector<std::string> configLocations) = 0;

		virtual void downloadConfigs(std::vector<std::string> configURLs, std::vector<std::string> configLocations) = 0;

        virtual void updateResources() = 0;

		virtual void updateRoomResources(std::string roomName) = 0;
        
		virtual std::string getCurrentRoomName() = 0;
        
        virtual void leave() = 0;
	};
}

#endif
