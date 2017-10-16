/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef HUDDEV_H
#define HUDDEV_H

namespace OIS
{
	class KeyEvent;
}

namespace Dolphin
{
	class HudDev
	{
	public:
		HudDev();
		virtual ~HudDev();
		void CreateHudDev();
		void UpdateHudDev();
	private:
		void ToggleAdvancedFrameStats(const OIS::KeyEvent arg);
		void InitiateShutDown(const OIS::KeyEvent arg);
		void ToggleDebuggingDetails(const OIS::KeyEvent arg);
	};
}
#endif HUDDEV_H
