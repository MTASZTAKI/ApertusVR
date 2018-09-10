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

#ifndef APE_EVENTMANAGERIMPL_H
#define APE_EVENTMANAGERIMPL_H

#ifdef _WIN32
#ifdef BUILDING_APE_EVENTMANAGER_DLL
#define APE_EVENTMANAGER_DLL_EXPORT __declspec(dllexport)
#else
#define APE_EVENTMANAGER_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_EVENTMANAGER_DLL_EXPORT 
#endif

#include <map>
#include <vector>
#include "managers/ApeIEventManager.h"
#include "managers/ApeILogManager.h"

namespace Ape
{ 
	class APE_EVENTMANAGER_DLL_EXPORT EventManagerImpl : public IEventManager
	{
	private:
		std::map<Ape::Event::Group, std::vector< std::function<void(const Ape::Event&)>>> mEventMap;

	public:
		EventManagerImpl();

		~EventManagerImpl();

		void connectEvent(Ape::Event::Group group, std::function<void(const Ape::Event&)> callback) override;

		void disconnectEvent(Ape::Event::Group group, std::function<void(const Ape::Event&)> callback) override;

		void fireEvent(const Ape::Event& event);
	};
}


#endif
