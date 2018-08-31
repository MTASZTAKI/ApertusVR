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

#ifndef APE_ILOGMANAGER_H
#define APE_ILOGMANAGER_H

#ifdef _WIN32
	#ifdef BUILDING_APE_LOGMANAGER_DLL
		#define APE_LOGMANAGER_DLL_EXPORT __declspec(dllexport)
	#else
		#define APE_LOGMANAGER_DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define APE_LOGMANAGER_DLL_EXPORT
#endif

#include <functional>
#include <iomanip>
#include <iostream>
#include <istream>
#include <map>
#include <mutex>
#include <ostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include "ApeSingleton.h"

#ifndef __FUNCTION_NAME__
	#ifdef WIN32   //WINDOWS
		#define __FUNCTION_NAME__   __FUNCTION__
	#else          //*NIX
		#define __FUNCTION_NAME__   __func__
	#endif
#endif

#define LOG_ENABLE_INFO
#define LOG_ENABLE_DEBUG
//#define LOG_ENABLE_TRACE
#define LOG_ENABLE_WARNING
#define LOG_ENABLE_ERROR
#define LOG_ENABLE_COLORIZE

#define COLOR_ORANGE "\033[0;33m"
#define COLOR_LIGHT_RED "\033[0;91m"
#define COLOR_LIGHT_GREEN "\033[0;92m"
#define COLOR_LIGHT_YELLOW "\033[0;93m"
#define COLOR_LIGHT_BLUE "\033[0;94m"
#define COLOR_LIGHT_MAGENTA "\033[0;95m"
#define COLOR_LIGHT_CYAN "\033[0;96m"
#define COLOR_TERM "\033[0m"

#define LOG_TYPE_INFO 0
#define LOG_TYPE_DEBUG 1
#define LOG_TYPE_TRACE 2
#define LOG_TYPE_WARNING 3
#define LOG_TYPE_ERROR 4

#define LOG_DELIMITER ": "
#define LOG_LINE_END std::endl

#define LOG_FILE_WIDTH 31
#define LOG_LINE_WIDTH 5
#define LOG_FUNC_WIDTH 45
#define LOG_FILL(WIDTH) std::setfill(' ') << std::setw(WIDTH)

#define LOG_WRITE(LEVEL, SS) { std::stringstream superStringStream; superStringStream << SS << LOG_LINE_END; Ape::ILogManager::getSingletonPtr()->log(superStringStream, LEVEL); }

#ifdef LOG_ENABLE_COLORIZE
	#define LOG(LEVEL, SS) \
		LOG_WRITE(LEVEL, COLOR_LIGHT_CYAN << "LN " << LOG_FILL(LOG_LINE_WIDTH) << __LINE__ << COLOR_TERM \
				  << LOG_DELIMITER << COLOR_LIGHT_YELLOW << LOG_FILL(LOG_FUNC_WIDTH) << __FUNCTION_NAME__ << COLOR_TERM \
				  << LOG_DELIMITER << SS \
		);
#else
	#define LOG(LEVEL, SS) \
		LOG_WRITE(LEVEL, "LN " << LOG_FILL(LOG_LINE_WIDTH) << __LINE__ \
				  << LOG_DELIMITER << LOG_FILL(LOG_FUNC_WIDTH) << __FUNCTION_NAME__ \
				  << LOG_DELIMITER << SS \
		);
#endif

#ifdef LOG_ENABLE_INFO
	#define LOG_INFO(SS) LOG(LOG_TYPE_INFO, SS);
#else
	#define LOG_INFO(SS) do { } while(0);
#endif

#ifdef LOG_ENABLE_DEBUG
	#define LOG_DEBUG(SS) LOG(LOG_TYPE_DEBUG, SS);
#else
	#define LOG_DEBUG(SS) do { } while(0);
#endif

#ifdef LOG_ENABLE_TRACE
	#define LOG_FUNC_ENTER() LOG(LOG_TYPE_TRACE, "Enter");
	#define LOG_FUNC_LEAVE() LOG(LOG_TYPE_TRACE, "Leave");
	#define LOG_TRACE(SS) LOG(LOG_TYPE_TRACE, SS);
#else
	#define LOG_FUNC_ENTER() do { } while(0);
	#define LOG_FUNC_LEAVE() do { } while(0);
	#define LOG_TRACE(SS) do { } while(0);
#endif

#ifdef LOG_ENABLE_WARNING
	#define LOG_WARNING(SS) LOG(LOG_TYPE_WARNING, SS);
#else
	#define LOG_WARNING(SS) do { } while(0);
#endif

#ifdef LOG_ENABLE_ERROR
	#define LOG_ERROR(SS) LOG(LOG_TYPE_ERROR, SS);
#else
	#define LOG_ERROR(SS) do { } while(0);
#endif

namespace Ape
{
	class APE_LOGMANAGER_DLL_EXPORT ILogManager : public Singleton<ILogManager>
	{

	protected:
		virtual ~ILogManager() {};

	public:
		virtual std::string getFileNameFromPath(const std::string& path) = 0;
		virtual void setLevel(int level) = 0;
		virtual void registerStream(std::ostream& stream) = 0;
		virtual void log(std::stringstream& ss, int level = LOG_TYPE_DEBUG) = 0;
	};
}

#endif
