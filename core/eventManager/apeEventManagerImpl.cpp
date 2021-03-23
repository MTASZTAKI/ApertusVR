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

#include "apeEventManagerImpl.h"

ape::EventManagerImpl::EventManagerImpl()
{
	msSingleton = this;
}

ape::EventManagerImpl::~EventManagerImpl()
{

}

void ape::EventManagerImpl::connectEvent(ape::Event::Group group, std::function<void(const ape::Event&)> cb)
{
	mEventMap[group].push_back(cb);
}

void ape::EventManagerImpl::disconnectEvent(ape::Event::Group group, std::function<void(const ape::Event&)> callback)
{
	if (mEventMap.find(group) != mEventMap.end())
	{
		for (auto it = mEventMap[group].begin(); it != mEventMap[group].end();)
		{
			if ((*it).target_type() == callback.target_type())
				it = mEventMap[group].erase(it);
			else
				++it;
		}
	}
}

void ape::EventManagerImpl::fireEvent(const ape::Event& event)
{
	if (mEventMap.find(event.group) != mEventMap.end())
	{
		for (auto it : mEventMap[event.group])
			it(event);
	}
}
