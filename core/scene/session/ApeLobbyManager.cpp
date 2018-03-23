#include "ApeLobbyManager.h"
#include "rapidjson/document.h"
#include <string>
#include <sstream>

Ape::LobbyManager::LobbyManager(const std::string& ip, const std::string& port)
{
	mIp = ip;
	mPort = port;
	mApiEndPointUrl = mIp + ":" + mPort + "/sessions";
}

Ape::LobbyManager::LobbyManager(const std::string& ip, const std::string& port, const std::string& sessionName)
{
	mIp = ip;
	mPort = port;
	mSessionName = sessionName;
	mApiEndPointUrl = mIp + ":" + mPort + "/sessions";
}

Ape::LobbyManager::~LobbyManager()
{
	
}

bool Ape::LobbyManager::parseResponse(const std::string& httpResponse, LobbyResponse& resp)
{
	rapidjson::StringStream in(httpResponse.c_str());
	rapidjson::Document jsonDocument;
	jsonDocument.ParseStream(in);

	if (!jsonDocument.IsObject())
		return false;

	if (!(jsonDocument.HasMember("result") && jsonDocument.HasMember("errors") && jsonDocument.HasMember("data")))
		return false;

	rapidjson::Value& result = jsonDocument["result"];
	if (result.IsString())
		resp.success = (std::strcmp(result.GetString(), "success") == 0);

	rapidjson::Value& errors = jsonDocument["errors"];
	if (errors.HasMember("items"))
	{
		rapidjson::Value& items = errors["items"];
		if (items.IsArray())
		{
			for (auto& item : items.GetArray())
			{
				LobbyResponseErrorItem errItem;

				if (item.HasMember("code"))
				{
					std::cout << "errorCode:" << item["code"].GetInt() << std::endl;
					errItem.code = item["code"].GetInt();
				}

				if (item.HasMember("name"))
				{
					std::cout << "errorName:" << item["name"].GetString() << std::endl;
					errItem.name = item["name"].GetString();
				}

				if (item.HasMember("message"))
				{
					std::cout << "errorMessage:" << item["message"].GetString() << std::endl;
					errItem.message = item["message"].GetString();
				}

				resp.errors.push_back(errItem);
			}
		}
	}

	rapidjson::Value& data = jsonDocument["data"];
	if (data.HasMember("items"))
	{
		rapidjson::Value& items = data["items"];
		if (items.IsArray())
		{
			for (auto& item : items.GetArray())
			{
				if (item.HasMember("host_guid"))
				{
					std::cout << "sessionGuid: " << item["host_guid"].GetString() << std::endl;
					resp.data.guid = item["host_guid"].GetString();
				}

				if (item.HasMember("session_name"))
				{
					std::cout << "sessionName: " << item["session_name"].GetString() << std::endl;
					resp.data.name = item["session_name"].GetString();
				}
			}
		}
	}

	return true;
}

bool Ape::LobbyManager::createSession(const std::string& sessionName, SceneSessionUniqueID guid)
{
	std::string response;
	LobbyResponse resp;
	std::string data = "{ \"sessionName\": \"" + sessionName + "\", \"sessionGuid\": \"" + guid + "\" }";
	std::cout << "createSession(): " << "Sending HTTP POST request to: " << mApiEndPointUrl << " with data: " << data << std::endl;

	try
	{
		response = mHttpManager.post(mApiEndPointUrl, data);
		std::cout << response << std::endl;

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				std::cout << "createSession succeeded" << std::endl;
				return true;
			}
			else
			{
				std::cout << "createSession failed" << std::endl;
			}
		}
	}
	catch (std::exception &e)
	{
		std::cout << "Got an exception: " << e.what() << std::endl;
	}

	return false;
}

bool Ape::LobbyManager::removeSession(const std::string& sessionName)
{
	std::string response;
	LobbyResponse resp;
	std::string url = mApiEndPointUrl + "/" + sessionName;
	std::cout << "removeSession(): " << "Sending HTTP POST request to: " << mApiEndPointUrl << std::endl;

	try
	{
		response = mHttpManager.del(mApiEndPointUrl, "");
		std::cout << response << std::endl;

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				std::cout << "removeSession succeeded" << std::endl;
				return true;
			}
			else
			{
				std::cout << "removeSession failed" << std::endl;
			}
		}
	}
	catch (std::exception &e)
	{
		std::cout << "Got an exception: " << e.what() << std::endl;
	}

	return false;
}

bool Ape::LobbyManager::getSessionHostGuid(std::string& sessionName, SceneSessionUniqueID& guid)
{
	std::string response;
	LobbyResponse resp;
	std::string url = mApiEndPointUrl + "/" + sessionName;
	std::cout << "getSessionHostGuid(): " << "Sending HTTP GET request to: " << url << std::endl;

	try
	{
		response = mHttpManager.download(url);
		std::cout << response << std::endl;

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				guid = resp.data.guid;
				std::cout << "getSessionHostGuid succeeded" << std::endl;
				return true;
			}
			else
			{
				std::cout << "getSessionHostGuid failed" << std::endl;
			}
		}
	}
	catch (std::exception &e)
	{
		std::cout << "Got an exception: " << e.what() << std::endl;
	}

	return false;
}
