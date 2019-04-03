#include "apeWebserverPlugin.h"

ape::apeWebserverPlugin::apeWebserverPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
	mpEventManager = ape::IEventManager::getSingletonPtr();
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&apeWebserverPlugin::eventCallBack, this, std::placeholders::_1));
	mpSceneManager = ape::ISceneManager::getSingletonPtr();
	APE_LOG_FUNC_LEAVE();
}

ape::apeWebserverPlugin::~apeWebserverPlugin()
{
	APE_LOG_FUNC_ENTER();
	mpEventManager->disconnectEvent(ape::Event::Group::NODE, std::bind(&apeWebserverPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWebserverPlugin::eventCallBack(const ape::Event& ev)
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

void ape::apeWebserverPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWebserverPlugin::Run()
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
	app.loglevel(crow::LogLevel::Warning).port(40080).multithreaded().run();

	APE_LOG_FUNC_LEAVE();
}

void ape::apeWebserverPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWebserverPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWebserverPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::apeWebserverPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}
