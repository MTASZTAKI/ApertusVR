/*
The zlib/libpng License

Copyright (c) 2005-2007 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that
		you wrote the original software. If you use this software in a product,
		an acknowledgment in the product documentation would be appreciated but is
		not required.

    2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#include "win32/Win32JoyStick.h"
#include "win32/Win32InputManager.h"
#include "win32/Win32ForceFeedback.h"
#include "OISEvents.h"
#include "OISException.h"

#include <cassert>

//DX Only defines macros for the JOYSTICK not JOYSTICK2, so fix it
#undef DIJOFS_BUTTON
#undef DIJOFS_POV

#define DIJOFS_BUTTON(n)  (FIELD_OFFSET(DIJOYSTATE2, rgbButtons) + (n))
#define DIJOFS_POV(n)     (FIELD_OFFSET(DIJOYSTATE2, rgdwPOV)+(n)*sizeof(DWORD))
#define DIJOFS_SLIDER0(n) (FIELD_OFFSET(DIJOYSTATE2, rglSlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER1(n) (FIELD_OFFSET(DIJOYSTATE2, rglVSlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER2(n) (FIELD_OFFSET(DIJOYSTATE2, rglASlider)+(n) * sizeof(LONG))
#define DIJOFS_SLIDER3(n) (FIELD_OFFSET(DIJOYSTATE2, rglFSlider)+(n) * sizeof(LONG))

using namespace OIS;

//--------------------------------------------------------------------------------------------------//
Win32JoyStick::Win32JoyStick( InputManager* creator, IDirectInput8* pDI,
	bool buffered, DWORD coopSettings, const JoyStickInfo &info )
	: JoyStick(info.vendor, buffered, info.devId, creator)
{
	mDirectInput = pDI;
	coopSetting = coopSettings;
	mJoyStick = 0;

	mJoyInfo = info; //mJoyInfo.deviceID = info.deviceID;

	mFfDevice = 0;
}

//--------------------------------------------------------------------------------------------------//
Win32JoyStick::~Win32JoyStick()
{
	delete mFfDevice;

	if(mJoyStick)
	{
		mJoyStick->Unacquire();
		mJoyStick->Release();
		mJoyStick = 0;
	}

	//Return joystick to pool
	static_cast<Win32InputManager*>(mCreator)->_returnJoyStick(mJoyInfo);
}

//--------------------------------------------------------------------------------------------------//
void Win32JoyStick::_initialize()
{
	//Clear old state
	mState.mAxes.clear();

	delete mFfDevice;
	mFfDevice = 0;

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = JOYSTICK_DX_BUFFERSIZE;

	if(FAILED(mDirectInput->CreateDevice(mJoyInfo.deviceID, &mJoyStick, NULL)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_initialize() >> Could not initialize joy device!");

	if(FAILED(mJoyStick->SetDataFormat(&c_dfDIJoystick2)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_initialize() >> data format error!");

	HWND hwin = ((Win32InputManager*)mCreator)->getWindowHandle();

	if(FAILED(mJoyStick->SetCooperativeLevel( hwin, coopSetting)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_initialize() >> failed to set cooperation level!");

	if( FAILED(mJoyStick->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)) )
		OIS_EXCEPT( E_General, "Win32Mouse::Win32Mouse >> Failed to set buffer size property" );

	//Enumerate all axes/buttons/sliders/etc before aquiring
	_enumerate();

	mState.clear();

	capture();
}

//--------------------------------------------------------------------------------------------------//
void Win32JoyStick::_enumerate()
{
	//We can check force feedback here too
	mDIJoyCaps.dwSize = sizeof(DIDEVCAPS);
	mJoyStick->GetCapabilities(&mDIJoyCaps);

	mPOVs = (short)mDIJoyCaps.dwPOVs;

	mState.mButtons.resize(mDIJoyCaps.dwButtons);
	mState.mAxes.resize(mDIJoyCaps.dwAxes);

	//Reset the axis mapping enumeration value
	_AxisNumber = 0;

	//Enumerate Force Feedback (if any)
	mJoyStick->EnumEffects(DIEnumEffectsCallback, this, DIEFT_ALL);

	//Enumerate and set axis constraints (and check FF Axes)
	mJoyStick->EnumObjects(DIEnumDeviceObjectsCallback, this, DIDFT_AXIS);
}

//--------------------------------------------------------------------------------------------------//
BOOL CALLBACK Win32JoyStick::DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	Win32JoyStick* _this = (Win32JoyStick*)pvRef;

	//Setup mappings
	DIPROPPOINTER diptr;
	diptr.diph.dwSize       = sizeof(DIPROPPOINTER);
	diptr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diptr.diph.dwHow        = DIPH_BYID;
	diptr.diph.dwObj        = lpddoi->dwType;
	//Add the high bit in so that an axis value of zero does not mean a null userdata
	diptr.uData             = 0x80000000 | _this->_AxisNumber;

	//Check if axis is slider, if so, do not treat as regular axis
	if(GUID_Slider == lpddoi->guidType)
	{
		++_this->mSliders;

		//Decrease Axes, since this slider shows up in a different place
		_this->mState.mAxes.pop_back();
	}
	else if (FAILED(_this->mJoyStick->SetProperty(DIPROP_APPDATA, &diptr.diph)))
	{	//If for some reason we could not set needed user data, just ignore this axis
		return DIENUM_CONTINUE;
	}

	//Increase for next time through
	_this->_AxisNumber += 1;

	//Set range
	DIPROPRANGE diprg;
	diprg.diph.dwSize       = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwHow        = DIPH_BYID;
	diprg.diph.dwObj        = lpddoi->dwType;
	diprg.lMin              = MIN_AXIS;
	diprg.lMax              = MAX_AXIS;

	if (FAILED(_this->mJoyStick->SetProperty(DIPROP_RANGE, &diprg.diph)))
		OIS_EXCEPT( E_General, "Win32JoyStick::_DIEnumDeviceObjectsCallback >> Failed to set min/max range property" );

	//Check if FF Axes
	if((lpddoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
	{
		if( _this->mFfDevice )
		{
			//todo - increment force feedback axis count
		}
	}

	return DIENUM_CONTINUE;
}

//--------------------------------------------------------------------------------------------------//
BOOL CALLBACK Win32JoyStick::DIEnumEffectsCallback(LPCDIEFFECTINFO pdei, LPVOID pvRef)
{
	Win32JoyStick* _this = (Win32JoyStick*)pvRef;

	//Create the FF class after we know there is at least one effect type
	if( _this->mFfDevice == 0 )
		_this->mFfDevice = new Win32ForceFeedback(_this->mJoyStick, &_this->mDIJoyCaps);

	_this->mFfDevice->_addEffectSupport( pdei );

	return DIENUM_CONTINUE;
}

//--------------------------------------------------------------------------------------------------//
void Win32JoyStick::capture()
{
	DIDEVICEOBJECTDATA diBuff[JOYSTICK_DX_BUFFERSIZE];
	DWORD entries = JOYSTICK_DX_BUFFERSIZE;

	// Poll the device to read the current state
	HRESULT hr = mJoyStick->Poll();
	if( hr == DI_OK )
		hr = mJoyStick->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );

	if( hr != DI_OK )
	{
		hr = mJoyStick->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = mJoyStick->Acquire();

		// Poll the device to read the current state
	    mJoyStick->Poll();
		hr = mJoyStick->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );
		//Perhaps the user just tabbed away
		if( FAILED(hr) )
			return;
	}

	bool axisMoved[24] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
						  false,false,false,false,false,false,false,false};
	bool sliderMoved[4] = {false,false,false,false};

	//Loop through all the events
	for(unsigned int i = 0; i < entries; ++i)
	{
		//First check to see if event entry is a Axis we enumerated earlier
		if( diBuff[i].uAppData != 0xFFFFFFFF && diBuff[i].uAppData > 0 )
		{
			int axis = (int)(0x7FFFFFFF & diBuff[i].uAppData); //Mask out the high bit
			assert( axis >= 0 && axis < (int)mState.mAxes.size() && "Axis out of range!");
			mState.mAxes[axis].abs = diBuff[i].dwData;
			axisMoved[axis] = true;
		}
		else
		{
			//This may seem outof order, but is in order of the way these variables
			//are declared in the JoyStick State 2 structure.
			switch(diBuff[i].dwOfs)
			{
			//------ slider -//
			case DIJOFS_SLIDER0(0):
				sliderMoved[0] = true;
				mState.mSliders[0].abX = diBuff[i].dwData;
				break;
			case DIJOFS_SLIDER0(1):
				sliderMoved[0] = true;
				mState.mSliders[0].abY = diBuff[i].dwData;
				break;
			//----- Max 4 POVs Next ---------------//
			case DIJOFS_POV(0):
				if(!_changePOV(0,diBuff[i]))
					return;
				break;
			case DIJOFS_POV(1):
				if(!_changePOV(1,diBuff[i]))
					return;
				break;
			case DIJOFS_POV(2):
				if(!_changePOV(2,diBuff[i]))
					return;
				break;
			case DIJOFS_POV(3):
				if(!_changePOV(3,diBuff[i]))
					return;
				break;
			case DIJOFS_SLIDER1(0):
				sliderMoved[1] = true;
				mState.mSliders[1].abX = diBuff[i].dwData;
				break;
			case DIJOFS_SLIDER1(1):
				sliderMoved[1] = true;
				mState.mSliders[1].abY = diBuff[i].dwData;
				break;
			case DIJOFS_SLIDER2(0):
				sliderMoved[2] = true;
				mState.mSliders[2].abX = diBuff[i].dwData;
				break;
			case DIJOFS_SLIDER2(1):
				sliderMoved[2] = true;
				mState.mSliders[2].abY = diBuff[i].dwData;
				break;
			case DIJOFS_SLIDER3(0):
				sliderMoved[3] = true;
				mState.mSliders[3].abX = diBuff[i].dwData;
				break;
			case DIJOFS_SLIDER3(1):
				sliderMoved[3] = true;
				mState.mSliders[3].abY = diBuff[i].dwData;
				break;
			//-----------------------------------------//
			default:
				//Handle Button Events Easily using the DX Offset Macros
				if( diBuff[i].dwOfs >= DIJOFS_BUTTON(0) && diBuff[i].dwOfs < DIJOFS_BUTTON(128) )
				{
					if(!_doButtonClick((diBuff[i].dwOfs - DIJOFS_BUTTON(0)), diBuff[i]))
						return;
				}
				break;
			} //end case
		} //End else
	} //end for

	//Check to see if any of the axes values have changed.. if so send events
	if( mBuffered && mListener && entries > 0 )
	{
		JoyStickEvent temp(this, mState);

		//Update axes
		for( int i = 0; i < 24; ++i )
			if( axisMoved[i] )
				if( mListener->axisMoved( temp, i ) == false )
					return;

		//Now update sliders
		for( int i = 0; i < 4; ++i )
			if( sliderMoved[i] )
				if( mListener->sliderMoved( temp, i ) == false )
					return;
	}
}

//--------------------------------------------------------------------------------------------------//
bool Win32JoyStick::_doButtonClick( int button, DIDEVICEOBJECTDATA& di )
{
	if( di.dwData & 0x80 )
	{
		mState.mButtons[button] = true;
		if( mBuffered && mListener )
			return mListener->buttonPressed( JoyStickEvent( this, mState ), button );
	}
	else
	{
		mState.mButtons[button] = false;
		if( mBuffered && mListener )
			return mListener->buttonReleased( JoyStickEvent( this, mState ), button );
	}

	return true;
}

//--------------------------------------------------------------------------------------------------//
bool Win32JoyStick::_changePOV( int pov, DIDEVICEOBJECTDATA& di )
{
	//Some drivers report a value of 65,535, instead of �1,
	//for the center position
	if(LOWORD(di.dwData) == 0xFFFF)
	{
		mState.mPOV[pov].direction = Pov::Centered;
	}
	else
	{
		switch(di.dwData)
		{
			case 0: mState.mPOV[pov].direction = Pov::North; break;
			case 4500: mState.mPOV[pov].direction = Pov::NorthEast; break;
			case 9000: mState.mPOV[pov].direction = Pov::East; break;
			case 13500: mState.mPOV[pov].direction = Pov::SouthEast; break;
			case 18000: mState.mPOV[pov].direction = Pov::South; break;
			case 22500: mState.mPOV[pov].direction = Pov::SouthWest; break;
			case 27000: mState.mPOV[pov].direction = Pov::West; break;
			case 31500: mState.mPOV[pov].direction = Pov::NorthWest; break;
		}
	}

	if( mBuffered && mListener )
		return mListener->povMoved( JoyStickEvent( this, mState ), pov );

	return true;
}

//--------------------------------------------------------------------------------------------------//
void Win32JoyStick::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//--------------------------------------------------------------------------------------------------//
Interface* Win32JoyStick::queryInterface(Interface::IType type)
{
	//Thought about using covariant return type here.. however,
	//some devices may allow LED light changing, or other interface stuff

	if( mFfDevice && type == Interface::ForceFeedback )
		return mFfDevice;
	else
		return 0;
}
