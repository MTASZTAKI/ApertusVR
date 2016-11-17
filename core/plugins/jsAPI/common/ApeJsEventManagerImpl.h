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

#ifndef APE_JSEVENTMANAGERIMPL_H
#define APE_JSEVENTMANAGERIMPL_H

#ifdef _WIN32
#ifdef BUILDING_APE_JSEVENTMANAGER_DLL
#define APE_JSEVENTMANAGER_DLL_EXPORT __declspec(dllexport)
#else
#define APE_JSEVENTMANAGER_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_EVENTMANAGER_DLL_EXPORT
#endif

#include <map>
#include <vector>
#include "node.h"
#include "v8.h"
#include "uv.h"
#include "ApeEvent.h"
#include "ApeIJsEventManager.h"

namespace Ape
{
	class APE_JSEVENTMANAGER_DLL_EXPORT JsEventManagerImpl : public IJsEventManager
	{
	private:
		std::map<Ape::Event::Group, std::vector<v8::Persistent<v8::Function>>> mEventMap;

	public:
		JsEventManagerImpl();

		~JsEventManagerImpl();

		void connectEvent(Ape::Event::Group group, v8::Persistent<v8::Function> cb) override;

		void onEvent(const Ape::Event& e);

		int size();
	};
}


#endif
