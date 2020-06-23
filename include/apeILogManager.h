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
#include "apeSingleton.h"

#ifndef __FUNCTION_NAME__
	#ifdef WIN32   //WINDOWS
		#define __FUNCTION_NAME__   __FUNCTION__
	#else          //*NIX
		#define __FUNCTION_NAME__   __func__
	#endif
#endif

#define APE_LOG_ENABLE_INFO
#define APE_LOG_ENABLE_DEBUG
//#define APE_LOG_ENABLE_TRACE
#define APE_LOG_ENABLE_WARNING
#define APE_LOG_ENABLE_ERROR
#ifndef __ANDROID__
#define APE_LOG_ENABLE_COLORIZE
#endif

#define COLOR_ORANGE "\033[0;33m"
#define COLOR_LIGHT_RED "\033[0;91m"
#define COLOR_LIGHT_GREEN "\033[0;92m"
#define COLOR_LIGHT_YELLOW "\033[0;93m"
#define COLOR_LIGHT_BLUE "\033[0;94m"
#define COLOR_LIGHT_MAGENTA "\033[0;95m"
#define COLOR_LIGHT_CYAN "\033[0;96m"
#define COLOR_TERM "\033[0m"

#define APE_LOG_TYPE_INFO 0
#define APE_LOG_TYPE_DEBUG 1
#define APE_LOG_TYPE_TRACE 2
#define APE_LOG_TYPE_WARNING 3
#define APE_LOG_TYPE_ERROR 4

#define APE_LOG_DELIMITER ": "
#define APE_LOG_LINE_END std::endl

#define APE_LOG_FILE_WIDTH 31
#define APE_LOG_LINE_WIDTH 5
#define APE_LOG_FUNC_WIDTH 45
#define APE_LOG_FILL(WIDTH) std::setfill(' ') << std::setw(WIDTH)

#define APE_LOG_WRITE(LEVEL, SS) { std::stringstream superStringStream; superStringStream << SS << APE_LOG_LINE_END; ape::ILogManager::getSingletonPtr()->log(superStringStream, LEVEL); }

#ifdef LOG
	#undef LOG
#endif

#ifdef APE_LOG_ENABLE_COLORIZE
	#define APE_LOG(LEVEL, SS) \
		APE_LOG_WRITE(LEVEL, COLOR_LIGHT_CYAN << "LN " << APE_LOG_FILL(APE_LOG_LINE_WIDTH) << __LINE__ << COLOR_TERM \
				  << APE_LOG_DELIMITER << COLOR_LIGHT_YELLOW << APE_LOG_FILL(APE_LOG_FUNC_WIDTH) << __FUNCTION_NAME__ << COLOR_TERM \
				  << APE_LOG_DELIMITER << SS \
		);
#else
	#define APE_LOG(LEVEL, SS) \
		APE_LOG_WRITE(LEVEL, "LN " << APE_LOG_FILL(APE_LOG_LINE_WIDTH) << __LINE__ \
				  << APE_LOG_DELIMITER << APE_LOG_FILL(APE_LOG_FUNC_WIDTH) << __FUNCTION_NAME__ \
				  << APE_LOG_DELIMITER << SS \
		);
#endif

#ifdef APE_LOG_ENABLE_INFO
	#define APE_LOG_INFO(SS) APE_LOG(APE_LOG_TYPE_INFO, SS);
#else
	#define APE_LOG_INFO(SS) do { } while(0);
#endif

#ifdef APE_LOG_ENABLE_DEBUG
	#define APE_LOG_DEBUG(SS) APE_LOG(APE_LOG_TYPE_DEBUG, SS);
#else
	#define APE_LOG_DEBUG(SS) do { } while(0);
#endif

#ifdef APE_LOG_ENABLE_TRACE
	#define APE_LOG_FUNC_ENTER() APE_LOG(APE_LOG_TYPE_TRACE, "Enter");
	#define APE_LOG_FUNC_LEAVE() APE_LOG(APE_LOG_TYPE_TRACE, "Leave");
	#define APE_LOG_TRACE(SS) APE_LOG(APE_LOG_TYPE_TRACE, SS);
#else
	#define APE_LOG_FUNC_ENTER() do { } while(0);
	#define APE_LOG_FUNC_LEAVE() do { } while(0);
	#define APE_LOG_TRACE(SS) do { } while(0);
#endif

#ifdef APE_LOG_ENABLE_WARNING
	#define APE_LOG_WARNING(SS) APE_LOG(APE_LOG_TYPE_WARNING, SS);
#else
	#define APE_LOG_WARNING(SS) do { } while(0);
#endif

#ifdef APE_LOG_ENABLE_ERROR
	#define APE_LOG_ERROR(SS) APE_LOG(APE_LOG_TYPE_ERROR, SS);
#else
	#define APE_LOG_ERROR(SS) do { } while(0);
#endif

namespace ape
{
	class APE_LOGMANAGER_DLL_EXPORT ILogManager : public Singleton<ILogManager>
	{

	protected:
		virtual ~ILogManager() {};

	public:
		virtual std::string getFileNameFromPath(const std::string& path) = 0;
		virtual void setLevel(int level) = 0;
		virtual void registerStream(std::ostream& stream) = 0;
		virtual void log(std::stringstream& ss, int level = APE_LOG_TYPE_DEBUG) = 0;
	};
}

#endif
