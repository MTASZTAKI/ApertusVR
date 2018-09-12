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

#if _WIN32
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif
#include "ApePluginInstance.h"

class Ape::PluginInstance::Impl
{
public:
	std::string mFileName;
	std::string mDisplayName;
#if _WIN32
	HINSTANCE handle;

	typedef FARPROC PluginFunc;

	bool Load()
	{
		handle = LoadLibrary(mFileName.c_str());
		return (handle != NULL);
	}

	bool Unload()
	{
		if (!handle)
			return true;
		return FreeLibrary(handle) != 0;
	}

	PluginFunc GetFunction(const std::string &name)
	{
		PluginFunc fptr = GetProcAddress(handle, name.c_str());
		if (! fptr)
			return NULL;
		return fptr;
	}
#else
	void *handle;

	typedef void(*PluginFunc)();

	bool Load()
	{
		handle = dlopen(mFileName.c_str(), RTLD_LOCAL | RTLD_LAZY);
		return (handle != NULL);
	}

	bool Unload()
	{
		if (!handle)
			return true;
		return dlclose(handle) == 0;
	}

	PluginFunc GetFunction(const std::string &name)
	{
		dlerror();
		PluginFunc fptr = (PluginFunc) dlsym(handle, name.c_str());
		if (dlerror())
			return NULL;
		return fptr;
	}
#endif


};

Ape::PluginInstance::PluginInstance(const std::string &name)
{
	mImpl = new Impl;
	mImpl->mDisplayName = name;

#if _WIN32
	mImpl->mFileName = name + ".dll";
#elif __APPLE__
	mImpl->mFileName = "lib" + name + ".dylib";
#else
	mImpl->mFileName = "lib" + name + ".so";
#endif
}

Ape::PluginInstance::~PluginInstance()
{
	delete mImpl;
}

bool Ape::PluginInstance::Load()
{
	if (! mImpl->Load())
		return false;

	Impl::PluginFunc init_func = mImpl->GetFunction("ApePluginInit");
	if (!init_func)
		return false;

	(*init_func)();

	Impl::PluginFunc name_string = mImpl->GetFunction("ApePluginDisplayName");
	if (name_string)
	{
		const char **ptr = (const char **) name_string;
		mImpl->mDisplayName = *ptr;
	}

	return true;
}

bool Ape::PluginInstance::Unload()
{
	return mImpl->Unload();
}

bool Ape::PluginInstance::IsLoaded()
{
	return (mImpl->handle != NULL);
}

std::string Ape::PluginInstance::GetFileName()
{
	return mImpl->mFileName;
}

std::string Ape::PluginInstance::GetDisplayName()
{
	return mImpl->mDisplayName;
}
