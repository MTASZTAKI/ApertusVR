/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <cstdlib>

namespace Dolphin
{
	template<typename  T>
	class Singleton
	{
	public:
		static T& Instance()
		{
			if (Singleton::instance_ == 0)
			{
				Singleton::instance_ = CreateInstance();
				ScheduleForDestruction(Singleton::Destroy);
			}
			return *(Singleton::instance_);
		}
		static void Destroy()
		{
			if (Singleton::instance_ != 0)
			{
				DestroyInstance(Singleton::instance_);
				Singleton::instance_ = 0;
			}
		}

	protected:
		Singleton()
		{
			assert(Singleton::instance_ == 0);
			Singleton::instance_ = static_cast<T*>(this);
		}
		~Singleton()
		{
			Singleton::instance_ = 0;
		}
	private:
		static T* CreateInstance()
		{
			return new T();
		}
		static void ScheduleForDestruction(void(*pFun)())
		{
			std::atexit(pFun);
		}
		static void DestroyInstance(T* p)
		{
			delete p;
		}
		
		static T* instance_;
	};

	template<typename  T>
	typename T* Singleton<T>::instance_ = 0;
}

#endif //EVENTMANAGER_H
