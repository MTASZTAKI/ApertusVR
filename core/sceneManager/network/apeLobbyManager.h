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


#ifndef APE_LOBBYMANAGER_H
#define APE_LOBBYMANAGER_H

#include <vector>
#include <string>
#include <iostream>
#include "apeISceneNetwork.h"
#include "apeILogManager.h"
#include "apeHttpManager.h"

namespace ape
{
	struct LobbyResponseErrorItem
	{
		int code;
		std::string name;
		std::string message;
	};

	struct LobbySession
	{
		std::string name;
		std::string guid;
	};

	struct LobbyResponse
	{
		bool success;
		std::vector<LobbyResponseErrorItem> errors;
		LobbySession data;
	};

	class LobbyManager
	{
	private:
		HttpManager mHttpManager;

		std::string mIp;
		
		std::string mPort;

		std::string mSessionName;
		
		std::string mApiEndPointUrl;
		
	public:
		LobbyManager(const std::string& ip, const std::string& port);
		LobbyManager(const std::string& ip, const std::string& port, const std::string& sessionName);
		
		~LobbyManager();

		bool parseResponse(const std::string& httpResponse, LobbyResponse& resp);

		bool createSession(const std::string& sessionName, std::string guid);

		bool removeSession(const std::string& sessionName);
		
		bool getSessionHostGuid(std::string& sessionName, std::string& guid);

		bool downloadResources(const std::string& url, const std::string& location, const std::string& md5 = "");
	};
}

#endif 
