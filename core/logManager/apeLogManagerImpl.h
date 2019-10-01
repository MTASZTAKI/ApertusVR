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

#ifndef APE_LOGMANAGERIMPL_H
#define APE_LOGMANAGERIMPL_H

#ifdef _WIN32
#ifdef BUILDING_APE_LOGMANAGER_DLL
#define APE_LOGMANAGER_DLL_EXPORT __declspec(dllexport)
#else
#define APE_LOGMANAGER_DLL_EXPORT __declspec(dllimport)
#endif
#else
#define APE_LOGMANAGER_DLL_EXPORT
#endif

#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include "apePlatform.h"
#include "apeILogManager.h"

namespace ape
{
	class APE_LOGMANAGER_DLL_EXPORT LogManagerImpl : public ILogManager
	{
	private:
		std::mutex g_pages_mutex;

		int mLevel = APE_LOG_TYPE_DEBUG;

		std::ostream* mStream;

		std::ofstream mOutFileStream;

	public:
		LogManagerImpl();

		~LogManagerImpl();

		std::string getFileNameFromPath(const std::string& path);

		void setLevel(int level);

		void registerStream(std::ostream& stream);

		void log(std::stringstream& ss, int level = APE_LOG_TYPE_DEBUG);
	};
}

#endif
