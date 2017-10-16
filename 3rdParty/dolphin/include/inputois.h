/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef _INPUTOIS_H_
#define _INPUTOIS_H_

#include <OISKeyboard.h>
#include <OISMouse.h>

namespace OIS
{
	class KeyEvent;
	class MouseEvent;
	enum MouseButtonID;
}

namespace Ogre
{
	class RenderWindow;
}

namespace Dolphin
{
	class InputOis : public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		static InputOis& GetSingleton();
		InputOis(InputOis const&) = delete;
		void operator=(InputOis const&);
		void InitializeOIS(Ogre::RenderWindow* mWindow);
		void CaptureMouse();
		void CaptureKeyboard();
		void DestroyInputs();
		OIS::InputManager* GetInputManager();
		OIS::Mouse* GetMouse();
		OIS::Keyboard* GetKeyboard();
		void HIA(const OIS::KeyEvent arg);
		void HIA2(const OIS::KeyEvent arg);
		void HIA3(const OIS::KeyEvent arg, int x);
	protected:
		InputOis() {};
		// OIS Input devices
		OIS::InputManager* mInputManager;
		OIS::Mouse*    mMouse;
		OIS::Keyboard* mKeyboard;

		// OIS::KeyListener
		virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		// OIS::MouseListener
		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);


	};
}
#endif //_INPUTOIS_H_