/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <queue>
#include <vector>
#include <map>
#include <functional>

namespace Dolphin
{
	template <typename T, typename... V>
	class EventManager
	{
	public:
		static EventManager& GetSingleton()
		{
			static Dolphin::EventManager<T, V...> instance;
			return instance;
		}
		EventManager(EventManager<T, V...> const&) = delete;
		void operator=(EventManager<T, V...> const&);

		void SubscribeEvent(T do_event, std::function<void(V...)> eventcallback)
		{
			callbackmap[do_event].push_back(eventcallback);
		}
		void TriggerEvent(T do_event, V... arg)
		{
			for (auto const& value : callbackmap[do_event])
			{
				(value)(arg...);
			}
		}
	private:
		EventManager<T, V...>() {};
		std::map<T, std::vector<std::function<void(V...)>>> callbackmap;
	};
}

#endif //EVENTMANAGER_H