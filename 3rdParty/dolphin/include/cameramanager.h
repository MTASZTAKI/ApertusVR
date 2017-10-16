/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

namespace OgreBites
{
	class SdkCameraMan;
}

namespace OIS
{
	class KeyEvent;
	class MouseEvent;
	enum MouseButtonID;
}

namespace Dolphin
{
	class CameraManager
	{
	public:
		CameraManager(OgreBites::SdkCameraMan* ogrecamera);
		virtual ~CameraManager();
		void SendKeyDown(const OIS::KeyEvent arg);
		void SendKeyUp(const OIS::KeyEvent arg);
		void SendPointerMove(const OIS::MouseEvent arg);
		void SendPointerButtonDown(const OIS::MouseEvent arg, OIS::MouseButtonID id);
		void SendPointerButtonUp(const OIS::MouseEvent arg, OIS::MouseButtonID id);
	private:
		OgreBites::SdkCameraMan* ogrecameraman;
	};
}
#endif //CAMERAMANAGER_H

