#include <iostream>
#include "ApeWebserverPlugin.h"

Ape::ApeWebserverPlugin::ApeWebserverPlugin()
{
	LOG_FUNC_ENTER();
	mpSystemConfig = Ape::ISystemConfig::getSingletonPtr();
	mpEventManager = Ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(Ape::Event::Group::NODE, std::bind(&ApeWebserverPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = Ape::ISceneManager::getSingletonPtr();
	LOG_FUNC_LEAVE();
}

Ape::ApeWebserverPlugin::~ApeWebserverPlugin()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeWebserverPlugin::eventCallBack(const Ape::Event& ev)
{
	for (auto u : mWebSocketUsers)
	{
		crow::json::wvalue respJson;
		respJson["group"] = ev.group;
		respJson["type"] = ev.type;
		respJson["subjectName"] = ev.subjectName;
		u->send_text(crow::json::dump(respJson));
	}
}

void Ape::ApeWebserverPlugin::Init()
{
	LOG_FUNC_ENTER();
	LOG_FUNC_LEAVE();
}

void Ape::ApeWebserverPlugin::Run()
{
	LOG_FUNC_ENTER();

	crow::SimpleApp app;
	std::mutex mtx;;

	CROW_ROUTE(app, "/ws")
	.websocket()
	.onopen([&](crow::websocket::connection& conn)
	{
		LOG(LOG_TYPE_DEBUG, "new websocket connection");
		std::lock_guard<std::mutex> _(mtx);
		mWebSocketUsers.insert(&conn);
	})
	.onclose([&](crow::websocket::connection& conn, const std::string& reason)
	{
		LOG(LOG_TYPE_DEBUG, "websocket connection closed: " << reason);
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

	app.loglevel(crow::LogLevel::Warning).port(40080).multithreaded().run();

	mpEventManager->disconnectEvent(Ape::Event::Group::NODE, std::bind(&ApeWebserverPlugin::eventCallBack, this, std::placeholders::_1));
	LOG_FUNC_LEAVE();
}

void Ape::ApeWebserverPlugin::Step()
{

}

void Ape::ApeWebserverPlugin::Stop()
{

}

void Ape::ApeWebserverPlugin::Suspend()
{

}

void Ape::ApeWebserverPlugin::Restart()
{

}
