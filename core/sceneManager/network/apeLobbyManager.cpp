#include "apeLobbyManager.h"
#include "rapidjson/document.h"

#include <string>
#include <sstream>

ape::LobbyManager::LobbyManager(const std::string& ip, const std::string& port)
{
	mIp = ip;
	mPort = port;
	mApiEndPointUrl = mIp + ":" + mPort + "/sessions";
}

ape::LobbyManager::LobbyManager(const std::string& ip, const std::string& port, const std::string& sessionName)
{
	mIp = ip;
	mPort = port;
	mSessionName = sessionName;
	mApiEndPointUrl = mIp + ":" + mPort + "/sessions";
}

ape::LobbyManager::~LobbyManager()
{
	
}

bool ape::LobbyManager::parseResponse(const std::string& httpResponse, LobbyResponse& resp)
{
	if (httpResponse.empty())
		return false;

	rapidjson::StringStream in(httpResponse.c_str());
	rapidjson::Document jsonDocument;
	jsonDocument.ParseStream(in);

	if (!jsonDocument.IsObject())
		return false;

	if (!(jsonDocument.HasMember("result") && jsonDocument.HasMember("errors") && jsonDocument.HasMember("data")))
		return false;

	rapidjson::Value& result = jsonDocument["result"];
	if (result.IsString())
		resp.success = std::string(result.GetString()) == "success";

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
					APE_LOG_DEBUG("errorCode:" << item["code"].GetInt());
					errItem.code = item["code"].GetInt();
				}

				if (item.HasMember("name"))
				{
					APE_LOG_DEBUG("errorName:" << item["name"].GetString());
					errItem.name = item["name"].GetString();
				}

				if (item.HasMember("message"))
				{
					APE_LOG_DEBUG("errorMessage:" << item["message"].GetString());
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
					APE_LOG_DEBUG("sessionGuid: " << item["host_guid"].GetString());
					resp.data.guid = item["host_guid"].GetString();
				}

				if (item.HasMember("session_name"))
				{
					APE_LOG_DEBUG("sessionName: " << item["session_name"].GetString());
					resp.data.name = item["session_name"].GetString();
				}
			}
		}
	}

	return true;
}

bool ape::LobbyManager::createSession(const std::string& sessionName, std::string guid)
{
	std::string response;
	LobbyResponse resp;
	std::string data = "{ \"sessionName\": \"" + sessionName + "\", \"sessionGuid\": \"" + guid + "\" }";
	APE_LOG_DEBUG("Sending HTTP POST request to: " << mApiEndPointUrl << " with data: " << data);

	try
	{
		response = mHttpManager.post(mApiEndPointUrl, data);
		APE_LOG_DEBUG("response: " << response);

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				APE_LOG_DEBUG("result: succeeded");
				return true;
			}
			else
			{
				APE_LOG_DEBUG("result: failed");
			}
		}
	}
	catch (std::exception &e)
	{
		APE_LOG_ERROR("exception: " << e.what());
	}

	return false;
}

bool ape::LobbyManager::removeSession(const std::string& sessionName)
{
	std::string response;
	LobbyResponse resp;
	std::string url = mApiEndPointUrl + "/" + sessionName;
	APE_LOG_DEBUG("Sending HTTP POST request to: " << url);

	try
	{
		response = mHttpManager.del(url, "");
		APE_LOG_DEBUG("response: " << response);

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				APE_LOG_DEBUG("result: succeeded");
				return true;
			}
			else
			{
				APE_LOG_DEBUG("result: failed");
			}
		}
	}
	catch (std::exception &e)
	{
		APE_LOG_ERROR("exception: " << e.what());
	}

	return false;
}

bool ape::LobbyManager::getSessionHostGuid(std::string& sessionName, std::string& guid)
{
	std::string response;
	LobbyResponse resp;
	std::string url = mApiEndPointUrl + "/" + sessionName;
	APE_LOG_DEBUG("Sending HTTP GET request to: " << url);

	try
	{
		response = mHttpManager.download(url);
		APE_LOG_DEBUG("response: " << response);

		if (parseResponse(response, resp) && resp.success)
		{
			guid = resp.data.guid;
			APE_LOG_DEBUG("result: succeeded");
			return true;
		}
		else
		{
			APE_LOG_DEBUG("result: failed");
		}
	}
	catch (std::exception &e)
	{
		APE_LOG_ERROR("exception: " << e.what());
	}

	return false;
}

bool ape::LobbyManager::downloadResources(const std::string& url, const std::string& location, const std::string& md5)
{
	return mHttpManager.downloadResources(url, location, md5);
}

bool ape::LobbyManager::downloadRoomResources(const std::string& url, const std::string& location, const std::string& roomName, const std::string& md5)
{
	return mHttpManager.downloadRoomResources(url, location, roomName, md5);
}

bool ape::LobbyManager::downloadConfig(const std::string & url, const std::string & location)
{
	return mHttpManager.downloadConfig(url, location);
}
