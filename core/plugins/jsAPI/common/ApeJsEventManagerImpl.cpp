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

#include "ApeJsEventManagerImpl.h"
#include <iostream>

template<> Ape::IJsEventManager* Ape::Singleton<Ape::IJsEventManager>::msSingleton = 0;

Ape::JsEventManagerImpl::JsEventManagerImpl()
{
	msSingleton = this;
}

Ape::JsEventManagerImpl::~JsEventManagerImpl()
{

}

void Ape::JsEventManagerImpl::connectEvent(Ape::Event::Group group, v8::Persistent<v8::Function> cb)
{
	std::cout << "Ape::JsEventManagerImpl::connectEvent" << std::endl;
	mEventMap[group].push_back(cb);
}

void Ape::JsEventManagerImpl::onEvent(const Ape::Event& e)
{
	std::cout << "Ape::JsEventManagerImpl::onEvent()" << std::endl;

	/* TODO
	v8::Local<v8::Object> obj = v8::Object::New();

	v8::Handle<v8::String> js_str = v8::String::New(e.subjectName.data(), e.subjectName.size());
	obj->Set(v8::String::NewSymbol("subjectName"), js_str);

	v8::Local<v8::Number> type = v8::Number::New(e.type);
	obj->Set(v8::String::NewSymbol("type"), type);

	v8::Local<v8::Number> group = v8::Number::New(e.group);
	obj->Set(v8::String::NewSymbol("group"), group);

	std::cout << "Ape::JsEventManagerImpl::onEvent(): e.subjectName: " << e.subjectName << std::endl;
	std::cout << "Ape::JsEventManagerImpl::onEvent(): e.group: " << e.group << std::endl;

	// put event object into args array
	v8::Local<v8::Value> argv[] = {
		obj
	};

	// fire events with args array
	if (mEventMap.find(e.group) != mEventMap.end())
	{
		for (auto it : mEventMap[e.group])
			it->Call(it, 1, argv);
	}
	*/
}

int Ape::JsEventManagerImpl::size()
{
	return mEventMap.size();
}
