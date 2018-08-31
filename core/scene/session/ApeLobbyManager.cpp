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
					LOG(LOG_TYPE_DEBUG, "errorCode:" << item["code"].GetInt());
					errItem.code = item["code"].GetInt();
				}

				if (item.HasMember("name"))
				{
					LOG(LOG_TYPE_DEBUG, "errorName:" << item["name"].GetString());
					errItem.name = item["name"].GetString();
				}

				if (item.HasMember("message"))
				{
					LOG(LOG_TYPE_DEBUG, "errorMessage:" << item["message"].GetString());
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
					LOG(LOG_TYPE_DEBUG, "sessionGuid: " << item["host_guid"].GetString());
					resp.data.guid = item["host_guid"].GetString();
				}

				if (item.HasMember("session_name"))
				{
					LOG(LOG_TYPE_DEBUG, "sessionName: " << item["session_name"].GetString());
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
	LOG(LOG_TYPE_DEBUG, "Sending HTTP POST request to: " << mApiEndPointUrl << " with data: " << data);

	try
	{
		response = mHttpManager.post(mApiEndPointUrl, data);
		LOG(LOG_TYPE_DEBUG, "response: " << response);

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				LOG(LOG_TYPE_DEBUG, "result: succeeded");
				return true;
			}
			else
			{
				LOG(LOG_TYPE_DEBUG, "result: failed");
			}
		}
	}
	catch (std::exception &e)
	{
		LOG(LOG_TYPE_ERROR, "exception: " << e.what());
	}

	return false;
}

bool Ape::LobbyManager::removeSession(const std::string& sessionName)
{
	std::string response;
	LobbyResponse resp;
	std::string url = mApiEndPointUrl + "/" + sessionName;
	LOG(LOG_TYPE_DEBUG, "Sending HTTP POST request to: " << mApiEndPointUrl);

	try
	{
		response = mHttpManager.del(mApiEndPointUrl, "");
		LOG(LOG_TYPE_DEBUG, "response: " << response);

		if (parseResponse(response, resp))
		{
			if (resp.success)
			{
				LOG(LOG_TYPE_DEBUG, "result: succeeded");
				return true;
			}
			else
			{
				LOG(LOG_TYPE_DEBUG, "result: failed");
			}
		}
	}
	catch (std::exception &e)
	{
		LOG(LOG_TYPE_ERROR, "exception: " << e.what());
	}

	return false;
}

bool Ape::LobbyManager::getSessionHostGuid(std::string& sessionName, SceneSessionUniqueID& guid)
{
	std::string response;
	LobbyResponse resp;
	std::string url = mApiEndPointUrl + "/" + sessionName;
	LOG(LOG_TYPE_DEBUG, "Sending HTTP GET request to: " << url);

	try
	{
		response = mHttpManager.download(url);
		LOG(LOG_TYPE_DEBUG, "response: " << response);

		if (parseResponse(response, resp) && resp.success)
		{
			guid = resp.data.guid;
			LOG(LOG_TYPE_DEBUG, "result: succeeded");
			return true;
		}
		else
		{
			LOG(LOG_TYPE_DEBUG, "result: failed");
		}
	}
	catch (std::exception &e)
	{
		LOG(LOG_TYPE_ERROR, "exception: " << e.what());
	}

	return false;
}
