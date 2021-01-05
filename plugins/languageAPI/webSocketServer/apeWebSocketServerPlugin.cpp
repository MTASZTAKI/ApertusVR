#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/writer.h"
#include "apeWebSocketServerPlugin.h"

ape::WebSocketServerPlugin::WebSocketServerPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::WebSocketServerPlugin::~WebSocketServerPlugin()
{
	APE_LOG_FUNC_ENTER();
	for (auto eventGroup : mEventGroups)
	{
		mpEventManager->disconnectEvent(ape::Event::Group(eventGroup), std::bind(&WebSocketServerPlugin::eventCallBack, this, std::placeholders::_1));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::WebSocketServerPlugin::eventCallBack(const ape::Event& ev)
{
	for (auto u : mWebSocketUsers)
	{
		crow::json::wvalue respJson;
		respJson["group"] = ev.group;
		respJson["type"] = ev.type;
		respJson["subjectName"] = ev.subjectName;
		for (auto eventType : mEventTypes)
		{
			if (eventType == ev.type)
			{
				u->send_text(crow::json::dump(respJson));
			}
		}
	}
}

void ape::WebSocketServerPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	std::stringstream fileFullPath;
	fileFullPath << mpCoreConfig->getConfigFolderPath() << "\\apeWebSocketServerPlugin.json";
	FILE* apeWebSocketServerPluginConfig = std::fopen(fileFullPath.str().c_str(), "r");
	char readBuffer[65536];
	if (apeWebSocketServerPluginConfig)
	{
		rapidjson::FileReadStream jsonFileReaderStream(apeWebSocketServerPluginConfig, readBuffer, sizeof(readBuffer));
		rapidjson::Document jsonDocument;
		jsonDocument.ParseStream(jsonFileReaderStream);
		if (jsonDocument.IsObject())
		{
			if (jsonDocument.HasMember("port"))
			{
				rapidjson::Value& input = jsonDocument["port"];
				mPortNumber = input.GetInt();
			}
			if (jsonDocument.HasMember("eventGroups"))
			{
				rapidjson::Value& eventGroups = jsonDocument["eventGroups"];
				for (int i = 0; i < eventGroups.GetArray().Size(); i++)
				{
					mEventGroups.push_back(eventGroups.GetArray()[i].GetInt());
				}
			}
			if (jsonDocument.HasMember("eventTypes"))
			{
				rapidjson::Value& eventTypes = jsonDocument["eventTypes"];
				for (int i = 0; i < eventTypes.GetArray().Size(); i++)
				{
					mEventTypes.push_back(eventTypes.GetArray()[i].GetInt());
				}
			}
		}
	}
	for (auto eventGroup : mEventGroups)
	{
		mpEventManager->connectEvent(ape::Event::Group(eventGroup), std::bind(&WebSocketServerPlugin::eventCallBack, this, std::placeholders::_1));
	}
	APE_LOG_FUNC_LEAVE();
}

void ape::WebSocketServerPlugin::Run()
{
	APE_LOG_FUNC_ENTER();

	crow::SimpleApp app;
	std::mutex mtx;;

	CROW_ROUTE(app, "/ws")
	.websocket()
	.onopen([&](crow::websocket::connection& conn)
	{
		APE_LOG_DEBUG("new websocket connection");
		std::lock_guard<std::mutex> _(mtx);
		mWebSocketUsers.insert(&conn);
	})
	.onclose([&](crow::websocket::connection& conn, const std::string& reason)
	{
		APE_LOG_DEBUG("websocket connection closed: " << reason);
		std::lock_guard<std::mutex> _(mtx);
		mWebSocketUsers.erase(&conn);
	})
	.onmessage([&](crow::websocket::connection& /*conn*/, const std::string& data, bool is_binary)
	{
		std::lock_guard<std::mutex> _(mtx);
		for (auto u : mWebSocketUsers)
			if (is_binary)
				u->send_binary(data);
			else
				u->send_text(data);
	});

	CROW_ROUTE(app, "/")
	([]
	{
		char name[256];
		gethostname(name, 256);
		crow::mustache::context x;
		x["servername"] = name;

		crow::mustache::set_base("webserver/templates");
		auto page = crow::mustache::load("ws.html");
		return page.render(x);
	});
	app.loglevel(crow::LogLevel::Warning).port(mPortNumber).multithreaded().run();

	APE_LOG_FUNC_LEAVE();
}

void ape::WebSocketServerPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::WebSocketServerPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::WebSocketServerPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::WebSocketServerPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
