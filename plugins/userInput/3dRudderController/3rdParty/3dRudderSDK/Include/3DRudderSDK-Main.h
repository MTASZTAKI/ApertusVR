/************************************************************************************

	Copyright © 2014-2019, 3dRudder SA, All rights reserved
	For terms of use: https://3drudder-dev.com/docs/introduction/sdk_licensing_agreement/

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met :

	* Redistributions of source code must retain the above copyright notice, and
	this list of conditions.
	* Redistributions in binary form must reproduce the above copyright
	notice and this list of conditions.
	* The name of 3dRudder may not be used to endorse or promote products derived from
	this software without specific prior written permission.

************************************************************************************/

/**
 * \file	3DRudderSDK-Main.h
 *
 * \brief	The c++ 3dRudder SDK containing the main features
 */

#pragma once

#if defined(_WINDOWS) && ! defined ( _3DRUDDER_SDK_STATIC)  
#ifdef _USRDLL

#define _3DRUDDER_SDK_EXPORT __declspec(dllexport)  
#else

#define _3DRUDDER_SDK_EXPORT __declspec(dllimport)  
#endif
#else
#define _3DRUDDER_SDK_EXPORT			/// used for the dynamic library exported functions
#endif

#define  _3DRUDDER_SDK_MAX_DEVICE 4			/// definite the maximum of supported devices
#define  _3DRUDDER_SDK_VERSION 0x0205		/// definite the version of the SDK of this .h
#define  _3DRUDDER_SDK_LAST_COMPATIBLE_VERSION 0x0200		/// definite the last compatible version of the SDK with this .h

#include <stdint.h>
#include <math.h>


#if defined(__GNUC__)
#define     _3DR_UNUSED(a)          		({__typeof__ (&a) __attribute__ ((unused)) __tmp = &a; })
#else
#define     _3DR_UNUSED(a)          		(a) /// to avoid warning in non used parameters of functions 
#endif

/**
 * \namespace	ns3dRudder
 *
 * \brief	Namespace used for the 3dRudder SDK
 *
 */

namespace ns3dRudder
{
	/// Values that represent error codes.
	enum ErrorCode
	{
		/// The command had been successful
		Success = 0,				
		/// The 3dRudder is not connected
		NotConnected,				
		/// The device fail to execute the command
		Fail,						
		/// Incorrect intern command
		IncorrectCommand,			
		/// Timeout communication with the 3dRudder
		Timeout,					
		/// Device not supported by the SDK
		DeviceNotSupported,			
		/// The new connected 3dRudder did an error at the Initialization
		DeviceInitError,			
		/// The security of the 3dRudder had not been validated.
		ValidationError,			
		/// The security of the 3dRudder did a timeout : it could append when you stop the thread when debugging. 
		ValidationTimeOut,			
		/// The 3dRudder isn't ready
		NotReady,					
		/// Indicated that the Firmware must be updated
		FirmwareNeedToBeUpdated,	
		/// The 3dRudder's SDK isn't initialized
		NotInitialized,				
		/// This command is not supported in this version  of the SDK (or plateform).
		NotSupported,
		/// The dashboard is not installed
		DashboardInstallError,
		/// The dashboard need to be updated
		DashboardUpdateError,
		/// Other Errors.
		Other = 0xFF
	};

	/// Values that represent status.
	enum Status
	{

		/// While the 3dRudder initializes.
		NoStatus,

		/// Puts the 3dRudder on the floor, curved side below, without putting your feet on the device. The user waits for approx. 5 seconds for the 3dRudder to boot up until 3 short beeps are heard.
		NoFootStayStill,

		/// The 3dRudder initialize for about 2 seconds. Once done a long beep will be heard from the device. The 3dRudder is then operational.
		Initialization,

		/// Put your first feet on the 3dRudder.
		PutYourFeet,

		/// Put your second Foot on the 3dRudder.
		PutSecondFoot,

		/// The user must wait still for half a second for user calibration until a last short beep is heard from the device. The 3dRudder is then ready to be used.
		StayStill,

		/// The 3dRudder is in use.
		InUse,

		/// The 3dRudder is frozen.
		Frozen = 253,

		/// The 3dRudder is not connected.
		IsNotConnected=254,

		/// Call GetLastError function to get the error code
		Error = 255,
	};

	/// Values that represent axes returned by GetAxes.
	enum Axes
	{
		/// Left / Right Axis resulting from the physical action on the Roll angle of the 3dRudder. 
		LeftRight = 0,				
		/// Forward / Backward Axis resulting from the physical action on the Pitch angle of the 3dRudder.
		ForwardBackward,			
		/// Up / Down Axis resulting on the pressure sensor action on the 3dRudder.
		UpDown,						
		/// Horizontal Rotation Axis resulting from the physical action on the Yaw Angle of the 3dRudder.
		Rotation,					
		/// The Maximum possible axes
		MaxAxes,		
	};

	/// Values that represent tone values.
	enum NoteFrequency
	{
		/// C Octave 0 (DO Octave 0) Note Frequency
		Note_C0 = 16,
		/// C# Octave 0 (DO # Octave 0) Note Frequency
		Note_C_Sharp0 = 17,
		/// D Octave 0 (RE Octave 0) Note Frequency
		Note_D0 = 18,
		/// D# Octave 0 (RE # Octave 0) Note Frequency
		Note_D_Sharp0 = 19,
		/// E Octave 0 (MI Octave 0) Note Frequency
		Note_E0 = 20,
		/// F Octave 0 (FA Octave 0) Note Frequency
		Note_F0 = 21,
		/// F# Octave 0 (FA # Octave 0) Note Frequency
		Note_F_Sharp0 = 23,
		/// G Octave 0 (SOL Octave 0) Note Frequency
		Note_G0 = 24,
		/// G# Octave 0 (SOL # Octave 0) Note Frequency
		Note_G_Sharp0 = 25,
		/// A Octave 0 (LA Octave 0) Note Frequency
		Note_A0 = 27,
		/// A# Octave 0 (LA # Octave 0) Note Frequency
		Note_A_Sharp0 = 29,
		/// B Octave 0 (SI Octave 0) Note Frequency
		Note_B0 = 30,
		/// C Octave 1 (DO Octave 1) Note Frequency
		Note_C1 = 32,
		/// C# Octave 1 (DO # Octave 1) Note Frequency
		Note_C_Sharp1 = 34,
		/// D Octave 1 (RE Octave 1) Note Frequency
		Note_D1 = 36,
		/// D# Octave 1 (RE # Octave 1) Note Frequency
		Note_D_Sharp1 = 38,
		/// E Octave 1 (MI Octave 1) Note Frequency
		Note_E1 = 41,
		/// F Octave 1 (FA Octave 1) Note Frequency
		Note_F1 = 43,
		/// F# Octave 1 (FA # Octave 1) Note Frequency
		Note_F_Sharp1 = 46,
		/// G Octave 1 (SOL Octave 1) Note Frequency
		Note_G1 = 49,
		/// G# Octave 1 (SOL # Octave 1) Note Frequency
		Note_G_Sharp1 = 51,
		/// A Octave 1 (LA Octave 1) Note Frequency
		Note_A1 = 55,
		/// A# Octave 1 (LA # Octave 1) Note Frequency
		Note_A_Sharp1 = 58,
		/// B Octave 1 (SI Octave 1) Note Frequency
		Note_B1 = 61,
		/// C Octave 2 (DO Octave 2) Note Frequency
		Note_C2 = 65,
		/// C# Octave 2 (DO # Octave 2) Note Frequency
		Note_C_Sharp2 = 69,
		/// D Octave 2 (RE Octave 2) Note Frequency
		Note_D2 = 73,
		/// D# Octave 2 (RE # Octave 2) Note Frequency
		Note_D_Sharp2 = 77,
		/// E Octave 2 (MI Octave 2) Note Frequency
		Note_E2 = 82,
		/// F Octave 2 (FA Octave 2) Note Frequency
		Note_F2 = 87,
		/// F# Octave 2 (FA # Octave 2) Note Frequency
		Note_F_Sharp2 = 92,
		/// G Octave 2 (SOL Octave 2) Note Frequency
		Note_G2 = 98,
		/// G# Octave 2 (SOL # Octave 2) Note Frequency
		Note_G_Sharp2 = 103,
		/// A Octave 2 (LA Octave 2) Note Frequency
		Note_A2 = 110,
		/// A# Octave 2 (LA # Octave 2) Note Frequency
		Note_A_Sharp2 = 116,
		/// B Octave 2 (SI Octave 2) Note Frequency
		Note_B2 = 123,
		/// C Octave 3 (DO Octave 3) Note Frequency
		Note_C3 = 130,
		/// C# Octave 3 (DO # Octave 3) Note Frequency
		Note_C_Sharp3 = 138,
		/// D Octave 3 (RE Octave 3) Note Frequency
		Note_D3 = 146,
		/// D# Octave 3 (RE # Octave 3) Note Frequency
		Note_D_Sharp3 = 155,
		/// E Octave 3 (MI Octave 3) Note Frequency
		Note_E3 = 164,
		/// F Octave 3 (FA Octave 3) Note Frequency
		Note_F3 = 174,
		/// F# Octave 3 (FA # Octave 3) Note Frequency
		Note_F_Sharp3 = 185,
		/// G Octave 3 (SOL Octave 3) Note Frequency
		Note_G3 = 196,
		/// G# Octave 3 (SOL # Octave 3) Note Frequency
		Note_G_Sharp3 = 207,
		/// A Octave 3 (LA Octave 3) Note Frequency
		Note_A3 = 220,
		/// A# Octave 3 (LA # Octave 3) Note Frequency
		Note_A_Sharp3 = 233,
		/// B Octave 3 (SI Octave 3) Note Frequency
		Note_B3 = 246,
		/// C Octave 4 (DO Octave 4) Note Frequency
		Note_C4 = 261,
		/// C# Octave 4 (DO # Octave 4) Note Frequency
		Note_C_Sharp4 = 277,
		/// D Octave 4 (RE Octave 4) Note Frequency
		Note_D4 = 293,
		/// D# Octave 4 (RE # Octave 4) Note Frequency
		Note_D_Sharp4 = 311,
		/// E Octave 4 (MI Octave 4) Note Frequency
		Note_E4 = 329,
		/// F Octave 4 (FA Octave 4) Note Frequency
		Note_F4 = 349,
		/// F# Octave 4 (FA # Octave 4) Note Frequency
		Note_F_Sharp4 = 369,
		/// G Octave 4 (SOL Octave 4) Note Frequency
		Note_G4 = 392,
		/// G# Octave 4 (SOL # Octave 4) Note Frequency
		Note_G_Sharp4 = 415,
		/// A Octave 4 (LA Octave 4) Note Frequency
		Note_A4 = 440,
		/// A# Octave 4 (LA # Octave 4) Note Frequency
		Note_A_Sharp4 = 466,
		/// B Octave 4 (SI Octave 4) Note Frequency
		Note_B4 = 493,
		/// C Octave 5 (DO Octave 5) Note Frequency
		Note_C5 = 523,
		/// C# Octave 5 (DO # Octave 5) Note Frequency
		Note_C_Sharp5 = 554,
		/// D Octave 5 (RE Octave 5) Note Frequency
		Note_D5 = 587,
		/// D# Octave 5 (RE # Octave 5) Note Frequency
		Note_D_Sharp5 = 622,
		/// E Octave 5 (MI Octave 5) Note Frequency
		Note_E5 = 659,
		/// F Octave 5 (FA Octave 5) Note Frequency
		Note_F5 = 698,
		/// F# Octave 5 (FA # Octave 5) Note Frequency
		Note_F_Sharp5 = 739,
		/// G Octave 5 (SOL Octave 5) Note Frequency
		Note_G5 = 783,
		/// G# Octave 5 (SOL # Octave 5) Note Frequency
		Note_G_Sharp5 = 830,
		/// A Octave 5 (LA Octave 5) Note Frequency
		Note_A5 = 880,
		/// A# Octave 5 (LA # Octave 5) Note Frequency
		Note_A_Sharp5 = 932,
		/// B Octave 5 (SI Octave 5) Note Frequency
		Note_B5 = 987,
		/// C Octave 6 (DO Octave 6) Note Frequency
		Note_C6 = 1046,
		/// C# Octave 6 (DO # Octave 6) Note Frequency
		Note_C_Sharp6 = 1108,
		/// D Octave 6 (RE Octave 6) Note Frequency
		Note_D6 = 1174,
		/// D# Octave 6 (RE # Octave 6) Note Frequency
		Note_D_Sharp6 = 1244,
		/// E Octave 6 (MI Octave 6) Note Frequency
		Note_E6 = 1318,
		/// F Octave 6 (FA Octave 6) Note Frequency
		Note_F6 = 1396,
		/// F# Octave 6 (FA # Octave 6) Note Frequency
		Note_F_Sharp6 = 1479,
		/// G Octave 6 (SOL Octave 6) Note Frequency
		Note_G6 = 1567,
		/// G# Octave 6 (SOL # Octave 6) Note Frequency
		Note_G_Sharp6 = 1661,
		/// A Octave 6 (LA Octave 6) Note Frequency
		Note_A6 = 1760,
		/// A# Octave 6 (LA # Octave 6) Note Frequency
		Note_A_Sharp6 = 1864,
		/// B Octave 6 (SI Octave 6) Note Frequency
		Note_B6 = 1975,
		/// C Octave 7 (DO Octave 7) Note Frequency
		Note_C7 = 2093,
		/// C# Octave 7 (DO # Octave 7) Note Frequency
		Note_C_Sharp7 = 2217,
		/// D Octave 7 (RE Octave 7) Note Frequency
		Note_D7 = 2349,
		/// D# Octave 7 (RE # Octave 7) Note Frequency
		Note_D_Sharp7 = 2489,
		/// E Octave 7 (MI Octave 7) Note Frequency
		Note_E7 = 2637,
		/// F Octave 7 (FA Octave 7) Note Frequency
		Note_F7 = 2793,
		/// F# Octave 7 (FA # Octave 7) Note Frequency
		Note_F_Sharp7 = 2959,
		/// G Octave 7 (SOL Octave 7) Note Frequency
		Note_G7 = 3135,
		/// G# Octave 7 (SOL # Octave 7) Note Frequency
		Note_G_Sharp7 = 3322,
		/// A Octave 7 (LA Octave 7) Note Frequency
		Note_A7 = 3520,
		/// A# Octave 7 (LA # Octave 7) Note Frequency
		Note_A_Sharp7 = 3729,
		/// B Octave 7 (SI Octave 7) Note Frequency
		Note_B7 = 3951,
		/// C Octave 8 (DO Octave 8) Note Frequency
		Note_C8 = 4186,
		/// C# Octave 8 (DO # Octave 8) Note Frequency
		Note_C_Sharp8 = 4434,
		/// D Octave 8 (RE Octave 8) Note Frequency
		Note_D8 = 4698,
		/// D# Octave 8 (RE # Octave 8) Note Frequency
		Note_D_Sharp8 = 4978,
		/// E Octave 8 (MI Octave 8) Note Frequency
		Note_E8 = 5274,
		/// F Octave 8 (FA Octave 8) Note Frequency
		Note_F8 = 5587,
		/// F# Octave 8 (FA # Octave 8) Note Frequency
		Note_F_Sharp8 = 5919,
		/// G Octave 8 (SOL Octave 8) Note Frequency
		Note_G8 = 6271,
		/// G# Octave 8 (SOL # Octave 8) Note Frequency
		Note_G_Sharp8 = 6644,
		/// A Octave 8 (LA Octave 8) Note Frequency
		Note_A8 = 7040,
		/// A# Octave 8 (LA # Octave 8) Note Frequency
		Note_A_Sharp8 = 7458,
		/// B Octave 8 (SI Octave 8) Note Frequency
		Note_B8 = 7902,

	};

	/**
	 * \class	Tone
	 *
	 * \brief	__Containe one tone__
	 * 			
	 * 			Usable in CSdk::PlaySndEx
	 */

	class Tone
	{
	public:

		/**
		 * \fn	Tone::Tone()
		 *
		 * \brief	Default constructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */


		Tone()
		{
			m_nFrequency = Note_A4;
			m_nDurationOfTone = 500 / 10;
			m_nPauseAfterTone = 500 / 10;
		}

		/**
		 * \fn	Tone::Tone(uint16_t nFrequency, uint16_t nDurationOfTone, uint16_t nPauseAfterTone)
		 *
		 * \brief	Constructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nFrequency	   	The frequency of the sound to be played
		 * \param	nDurationOfTone	The duration of the played tone
		 * \param	nPauseAfterTone	The pause before playing the next tones
		 *
		 * \return  Void
		 */

		Tone(uint16_t nFrequency, uint16_t nDurationOfTone, uint16_t nPauseAfterTone)
		{
			m_nFrequency = nFrequency;
			m_nDurationOfTone =uint8_t(nDurationOfTone);
			m_nPauseAfterTone = uint8_t(nPauseAfterTone);
		}

		/**
		 * \fn	static float Tone::NoteToFrequency(uint16_t nNote);
		 *
		 * \brief	Note to frequency
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nNote	The note.
		 *
		 * \return	A float.
		 */

		static float NoteToFrequency(uint16_t nNote)
		{
			if (nNote >= 0 && nNote <= 119)
			{
				return float(440.0f*pow(2.0, (double(nNote) - 57.0) / 12.0));
			}
			else
				return -1;
		}

		/**
		 * \fn	uint16_t Tone::GetFrequency() const
		 *
		 * \brief	Gets the frequency
		 *
		 *    ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The frequency.
		 */

		uint16_t GetFrequency() const { return m_nFrequency; }

		/**
		 * \fn	void Tone::SetFrequency(uint16_t nF)
		 *
		 * \brief	Sets a frequency
		 *
		 *    ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nF	The note frequency (Hz).
		 */

		void  SetFrequency(uint16_t nF) { m_nFrequency=nF; }

		/**
		 * \fn	uint8_t Tone::GetDurationOfTone() const
		 *
		 * \brief	Gets duration of tone
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The duration of tone.
		 */

		uint8_t GetDurationOfTone() const { return m_nDurationOfTone; }

		/**
		 * \fn	void Tone::SetDurationOfTone(uint8_t nD)
		 *
		 * \brief	Sets duration of tone
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nD	The duration of the tone (ms)
		 */

		void SetDurationOfTone(uint8_t nD) { m_nDurationOfTone = nD; }

		/**
		 * \fn	uint8_t Tone::GetPauseAfterTone() const
		 *
		 * \brief	Gets pause after tone
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The pause after tone.
		 */

		uint8_t GetPauseAfterTone() const { return  m_nPauseAfterTone; }

		/**
		 * \fn	void Tone::SetPauseAfterTone(uint8_t nP)
		 *
		 * \brief	Sets pause after tone
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nP	The pause after tone (ms).
		 */

		void SetPauseAfterTone(uint8_t nP) { m_nPauseAfterTone = nP; }

	protected:
		uint16_t m_nFrequency;
		uint8_t m_nDurationOfTone;
		uint8_t m_nPauseAfterTone;
	};

	/**
	 * \class	Curve
	 *
	 * \brief	A curve object.
	 */

	class Curve
	{

	protected:
		float m_fDeadZone;
		float m_fxSat;
		float m_fExp;
	public:

		/**
		 * \fn	Curve::Curve()
		 *
		 * \brief	Default constructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */

		Curve()
		{
			m_fDeadZone = 0.0f;
			m_fxSat = 1.0f;
			m_fExp = 1.0f;
		}

		/**
		 * \fn	Curve::Curve(float fxSat, float fDeadZone,float fExp)
		 *
		 * \brief	Constructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	fxSat	 	The effects sat.
		 * \param	fDeadZone	The dead zone.
		 * \param	fExp	 	The exponent.
		 * 						.
		 * \return  Void
		 */

		Curve(float fxSat, float fDeadZone,float fExp)
		{
			m_fDeadZone = fDeadZone; m_fxSat = fxSat;  m_fExp = fExp;

		}

		/**
		 * \fn Curve::~Curve()
		 *
		 * \brief	Destructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */

		virtual ~Curve() {}

		/**
		 * \fn	float Curve::GetDeadZone() const
		 *
		 * \brief	Gets dead zone
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The deadzone's value.
		 * 			.
		 */

		float GetDeadZone()  const { return m_fDeadZone; }

		/**
		 * \fn	float Curve::GetXSat() const
		 *
		 * \brief	Get x coordinate saturation (eq. Sensivity)
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The x coordinate saturation.
		 * 			.
		 */

		float GetXSat()		 const { return m_fxSat; }

		/**
		 * \fn	float Curve::GetExp() const
		 *
		 * \brief	Gets the exponent
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The exponent.
		 * 			.
		 */

		float GetExp()		 const { return m_fExp; }

		/**
		 * \fn	void Curve::SetDeadZone(float fV)
		 *
		 * \brief	Sets dead zone
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	fV	The f v.
		 * 				.
		 */

		void SetDeadZone(float fV)	{ m_fDeadZone=fV; }

		/**
		 * \fn	void Curve::SetXSat(float fV)
		 *
		 * \brief	Sets x coordinate sat
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	fV	: Value of the new XSat.
		 * 				.
		 *
		 */

		void SetXSat(float fV)		{ m_fxSat = fV; }

		/**
		 * \fn	void Curve::SetExp(float fV)
		 *
		 * \brief	Sets an exponent
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	fV :	The new value of the Exp.
		 * 				.
		 */

		void SetExp(float fV)		{ m_fExp = fV; }

		/**
		 * \fn float Curve::CalcCurveValue(float fValue) const;
		 *
		 * \brief	Calculates the curve value.
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	fValue	The value.
		 *
		 * \return	The calculated curve value.
		 * 			.
		 */

		virtual float CalcCurveValue(float fValue)  const;

	};

	/**
	 * \class	DeviceInformation
	 *
	 * \brief	Information about the device.
	 *
	 */

	class DeviceInformation
	{

	protected:

		char m_sDeviceName[32];
		uint32_t m_nSerialNumber;
		float m_fMaxRoll;
		float m_fMaxPitch;
		float m_fMaxYaw;
		float m_fUserRoll;
		float m_fUserPitch;
		float m_fUserYaw;
		float m_DefaultRoll2YawCompensation;

	public:

		/**
		 * \fn	DeviceInformation::DeviceInformation();
		 *
		 * \brief	Default constructor
		 */
		DeviceInformation();

		/**
		 * \fn	const char * DeviceInformation::GetName() const
		 *
		 * \brief	Get model's name of the 3dRudder
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \return	Null if it fails, else the name. The default name is “3dRudder 1”
		 * 			.
		 */

		const char * GetName() const { return m_sDeviceName;	}

		/**
		 * \fn	uint32_t DeviceInformation::GetSerialNumber() const
		 *
		 * \brief	Gets serial number
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \return	The serial number. By default the number is 0x00010000
		 * 			.
		 */

		uint32_t GetSerialNumber() const {	return m_nSerialNumber; 	}

		/**
		 * \fn	void DeviceInformation::SetUserRoll(float nVal)
		 *
		 * \brief	Sets user roll
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nVal	The value.
		 * 					.
		 */

		void	SetUserRoll(float nVal) { m_fUserRoll = nVal; }

		/**
		 * \fn	void DeviceInformation::SetUserPitch(float nVal)
		 *
		 * \brief	Sets user pitch
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nVal	The value.
		 * 					.
		 */

		void	SetUserPitch(float nVal) { m_fUserPitch = nVal; }

		/**
		 * \fn	void DeviceInformation::SetUserYaw(float nVal)
		 *
		 * \brief	Sets user yaw
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nVal	The value.
		 *
		 */

		void	SetUserYaw(float nVal) { m_fUserYaw = nVal; }

		/**
		 * \fn	float DeviceInformation::GetMaxRoll() const
		 *
		 * \brief	Gets maximum roll. 
		 * \brief	Get physical specification of the 3dRudder (angle °)
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The maximum roll. 3dRudder R5 Edition | 3dRudder for PS4™ : 18°
		 */

		float	GetMaxRoll() const { return m_fMaxRoll; }

		/**
		 * \fn	float DeviceInformation::GetMaxPitch() const
		 *
		 * \brief	Gets maximum pitch.
		 * 	Get physical specification of the 3dRudder (angle °)
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The maximum pitch. 3dRudder R5 Edition | 3dRudder for PS4™ : 18°
		 * 			.
		 */

		float	GetMaxPitch() const { return m_fMaxPitch; }

		/**
		 * \fn	float DeviceInformation::GetMaxYaw() const
		 *
		 * \brief	Gets maximum yaw.
		 *  	Get physical specification of the 3dRudder (angle °)
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The maximum yaw. 3dRudder R5 Edition | 3dRudder for PS4™ : 25°
		 * 			.
		 */

		float	GetMaxYaw() const { return m_fMaxYaw; }

		/**
		 * \fn	float DeviceInformation::GetUserRoll() const
		 *
		 * \brief	Gets user roll
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The user roll.
		 * 			.
		 */

		float	GetUserRoll() const { return m_fUserRoll; }

		/**
		 * \fn	float DeviceInformation::GetUserPitch() const
		 *
		 * \brief	Gets user pitch
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The user pitch.
		 * 			.
		 */


		float	GetUserPitch() const { return m_fUserPitch; }

		/**
		 * \fn	float DeviceInformation::GetUserYaw() const
		 *
		 * \brief	Gets user yaw
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The user yaw.
		 * 			.
		 */

		float	GetUserYaw() const { return m_fUserYaw; }

		/**
		 * \fn	float DeviceInformation::GetDefaultRoll2YawCompensation() const
		 *
		 * \brief	Gets default roll 2 yaw compensation
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The default roll 2 yaw compensation. By default the value is 0.15
		 * 			.
		 */

		float	GetDefaultRoll2YawCompensation() const { return m_DefaultRoll2YawCompensation; }

	};

	/**
	 * \class	IAxesParam
	 *
	 * \brief	The axes parameter. This allow you to define the settings of each axis of the 3dRudder.
	 */

	class IAxesParam
	{
	public:

		/**
		 * \fn	IAxesParam::IAxesParam()
		 *
		 * \brief	Constructor. 
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */

		IAxesParam()
		{
			for (int i = 0; i < MaxAxes; i++)
				m_pCurve[i] = nullptr;

			m_fRoll2YawCompensation = 0.15f;
			m_bNonSymmetrical = false;
		}
		virtual ~IAxesParam() {}


		/**
		 * \fn	ErrorCode IAxesParam::UpdateParam(uint32_t nPortNumber) const
		 *
		 * \brief	virtual pure UpdateParam
		 *
		 * give the parameteres used to calculated the axes.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The device number.
		 *
		 * \return	the error code depending of the usage.
		 * 			.
		 */
		

		virtual ErrorCode UpdateParam(uint32_t nPortNumber) = 0;

		/**
		 * \fn	Curve * IAxesParam::GetCurve(Axes nCurve) const
		 *
		 * \brief	Gets a curve
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nCurve	The curve.
		 *
		 * \return	Null if it fails, else the curve.
		 * 			.
		 */

		Curve *	GetCurve(Axes nCurve)  const
		{
			if (nCurve >= LeftRight && nCurve < MaxAxes)
				return m_pCurve[nCurve];
			else
				return nullptr;
		}

		/**
		 * \fn	void IAxesParam::SetCurve(Axes nCurve, Curve *pCurve)
		 *
		 * \brief	Sets a curve.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param 		  	nCurve	The curve.
		 * \param [in,out]	pCurve	If non-null, the curve.
		 * 							.
		 */

		void	SetCurve(Axes nCurve, Curve *pCurve)
		{
			if (nCurve >= LeftRight && nCurve < MaxAxes)
				m_pCurve[nCurve] = pCurve;
		}
		
		/**
		 * \fn	bool IAxesParam::IsNonSymmetrical() const
		 *
		 * \brief	Query if this object is non Symmetrical
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	True if non Symmetrical, false if not.
		 * 			.
		 */

		bool	IsNonSymmetrical() const { return m_bNonSymmetrical; }

		/**
		 * \fn	void IAxesParam::SetNonSymmetrical(bool bVal)
		 *
		 * \brief	Sets non Symmetrical.
		 *  Enable/Disable Non Symmetrical pitch calculation. 
		 *
		 * **By default, this process is activated.**
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	bEnable	True : Enable the nonSymmetrical process, False : Disable the nonSymmetrical calculation
		 * 					.
		 */

		void	SetNonSymmetrical(bool bEnable) { m_bNonSymmetrical = bEnable; }

		/**
		 * \fn	float IAxesParam::GetRoll2YawCompensation() const
		 *
		 * \brief	Gets roll 2 yaw compensation
		 *
		 * The default value is 0.15
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The roll 2 yaw compensation.
		 * 			.
		 */

		float	GetRoll2YawCompensation() const { return m_fRoll2YawCompensation; }

		/**
		 * \fn	void IAxesParam::SetRoll2YawCompensation(float fVal)
		 *
		 * \brief	Sets params of  the processing roll 2 yaw compensation
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	fVal	Coefficient of the compensation.
		 * 					.
		 */

		void	SetRoll2YawCompensation(float fVal) { m_fRoll2YawCompensation = fVal; }

		/**
		 * \fn	void IAxesParam::NoUpDown()
		 *
		 * \brief	Disable the upDown of the 3dRudder.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \attention  When you call NoUpdown, the value of upDown stay at 0.0
		 *
		 */

		void NoUpDown()
		{
			SetCurve(UpDown, nullptr);
		}
		
	protected:
		Curve *m_pCurve[MaxAxes];
		float m_fRoll2YawCompensation;
		bool m_bNonSymmetrical;
	};

	/**
	 * \class	AxesValue
	 *
	 * \brief	The axes value.
	 */

	class AxesValue
	{
	protected:
		float m_Axes[MaxAxes];
	public:

		/**
		 * \fn	AxesValue::AxesValue()
		 *
		 * \brief	Default constructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */

		AxesValue()
		{
			for(int i = 0; i < MaxAxes; i++)
				m_Axes[i] = 0.0f;
		}

		/**
		 * \fn	float AxesValue::Get(Axes nAxis) const
		 *
		 * \brief	Gets a float using the given n axis
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nAxis	The axis to get.
		 *
		 * \return	The value of the specific axe
		 * 			.
		 */

		float Get(Axes nAxis)		const
		{
			if (nAxis >= LeftRight && nAxis < MaxAxes)
				return m_Axes[nAxis];
			else
				return 0.0f;
		}
	};

	/**
	 * \class	IEvent
	 *
	 * \brief	An event. You have to AddEvent before initializing the SDK.
	 */

	class IEvent
	{
	public:

		/**
		 * \fn IEvent::~IEvent()
		 *
		 * \brief	Destructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 */

		virtual ~IEvent() {}

		/**
		 * \fn virtual IEvent::OnConnect(uint32_t nPortNumber)
		 *
		 * \brief	Recieves the connect action.
		 *
		 * Event Sent when a 3dRudder is connected. The 3dRudder is identified by the nPortNumber. This function is a Virtual function.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 */

		virtual void OnConnect(uint32_t nPortNumber) { _3DR_UNUSED(nPortNumber); };

		/**
		 * \fn void IEvent::OnDisconnect(uint32_t nPortNumber)
		 *
		 * \brief	Recieves the disconnect action.
		 *
		 * Event Sent when a 3dRudder is disconnected. The 3dRudder is identified by the nPortNumber. This function is a Virtual function.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 * 							.
		 */

		virtual void OnDisconnect(uint32_t nPortNumber) { _3DR_UNUSED(nPortNumber); };

		/**
		 * \fn void IEvent::OnEndSound(uint32_t nPortNumber)
		 *
		 * \brief	Receives an event at the end of the sound
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 * 							.
		 */

		virtual void OnEndSound(uint32_t nPortNumber) { _3DR_UNUSED(nPortNumber); };
	};

	/**
	 * \class	CSdk
	 *
	 * \brief	This SDK allows you to integrate the 3dRudder in your Game or experience.
	 *
	 * All the SDK is defined in the class ns3dRudder::CSdk.
	 *
	 * With this SDK it's possible to manage up to four 3dRudder _3DRUDDER_SDK_MAX_DEVICE defines the max ports number (from 0 to 3).
	 */

	class CSdk
	{
	public:

		/**
		 * \fn	CSdk::CSdk();
		 *
		 * \brief	Default constructor. 
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */

		CSdk();

		/**
		 * \fn CSdk::~CSdk();
		 *
		 * \brief	Destructor
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 */

		virtual ~CSdk();

		/**
		 * \fn	virtual ErrorCode CSdk::Init() const;
		 *
		 * \brief	Initializes this object
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #Timeout
		 * \possibleret #NotReady
		 * \possibleret #IncorrectCommand	 
		 *                  	.
		 */

		virtual ErrorCode   Init()  const;

		/**
		 * \fn	virtual ErrorCode CSdk::Stop() const;
		 *
		 * \brief	Stop all Threads of the SDK and will free the sdk from memory.
		 *
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #NotReady
		 * 			.
		 */

		virtual ErrorCode   Stop()  const;

		/**
		 * \fn	virtual uint16_t CSdk::GetSDKVersion() const;
		 *
		 * \brief	Gets the From SDK version.
		 *
		 * Return the From Firmware version of the library, it's possible to compare this version with the _3DRUDDER_SDK_VERSION. The returned value is coded in BCD (binary coded decimal), so 0x0200 means version 2.0
		 * 
		 * define included in the 3dRdudderSDK.h to compare if the library and the .h match.
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \attention  It's not necessary to use Init() function before.
		 * 
		 * \return	The From Firmware version. The version The returned value is coded in BCD (binary coded decimal), so 0x1410 means version 1.4.1.0
		 * 			.
		 */

		virtual uint16_t	GetSDKVersion() const;

		/**
		 * \fn	virtual int32_t CSdk::GetNumberOfConnectedDevice() const;
		 *
		 * \brief	Gets number of connected device. Maximum 4
		 *
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \return	The number of connected device.
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * 			.
		 */

		virtual int32_t		GetNumberOfConnectedDevice() const;

		/**
		 * \fn	virtual bool CSdk::IsDeviceConnected(uint32_t nPortNumber) const;
		 *
		 * \brief	Query if 'nPortNumber' is device connected
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	True if device connected, false or error if not. Check GetLastError for possible error.  \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * 			.
		 */


		virtual bool		IsDeviceConnected(uint32_t nPortNumber) const;

		/**
		 * \fn	virtual uint16_t CSdk::GetVersion(uint32_t nPortNumber) const;
		 *
		 * \brief	Gets the firmware's version.
		 *
		 * \brief	Return version number of the firmware of the 3dRudder connected to the nPortNumber port.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	The version. 
		 * \possibleret The version is a fixed point unsigned short in hexadecimal: 0x1318 means version 1.3.1.8. 
		 * \possibleret Return 0xFFFF in case of error. 
		 * \possibleret Return 0 if the 3dRudder isn't Ready. \n \n \n <b> Possible error codes returned by this method : </b>
		 *		 
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * 			.
		 */

		virtual uint16_t	GetVersion(uint32_t nPortNumber)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::HideSystemDevice(uint32_t nPortNumber, bool bHide) const;
		 *
		 * \brief	Hides the system device.
		 *
		 * By default the 3dRudder is seen by the system as a Directinput device, a mouse or a keyboard (this can be changed thanks to the dashboard). 
		 *
		 * The function HideSystemDevice allows to hide the 3dRudder from the system, so your game will not see it as a DirectInput device. 
		 *
		 * **Please think to put it back in standard mode when you exit your game !**
		 *
		 *
		 * ¤compatible_plateforme Win¤
		 * ¤compatible_firm From 1.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 * \param	bHide	   	True to hide, False to show.
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes under Windows :</b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 * 
		 */
		 
		virtual ErrorCode	HideSystemDevice(uint32_t nPortNumber, bool bHide)  const;

		/**
		 * \fn	virtual bool CSdk::IsSystemDeviceHidden(uint32_t nPortNumber) const;
		 *
		 * \brief	Query if 'nPortNumber' is system device hidden
		 *
		 *
		 *	  ¤compatible_plateforme Win¤
		 *    ¤compatible_firm From 1.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	True if system device hidden, false or error if not. Check GetLastError for possible error.  \n \n \n <b> Possible error codes : </b>
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 * 
		 */

		virtual bool		IsSystemDeviceHidden(uint32_t nPortNumber)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::PlaySnd(uint32_t nPortNumber, uint16_t nFrequency, uint16_t nDuration) const;
		 *
		 * \brief	Play sound
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 * \param	nFrequency 	The frequency of the sound in Hz (440 is a A).
		 * \param	nDuration  	The duration.
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 * 			.
		 */
		virtual ErrorCode	PlaySnd(uint32_t nPortNumber, uint16_t nFrequency, uint16_t nDuration)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::PlaySndEx(uint32_t nPortNumber, uint8_t nSize, Tone *pTones, bool bAddToPlayedList = true) const;
		 *
		 * \brief	Play sound list in Tone.
		 * 
		 * Play a sequence of sound on a 3dRudder connected to the nPortNumber port defined by pTones array with the size nSize.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.0.8¤
		 *    ¤compatible_sdk From 2.00¤
		 * 
		 * \attention Usable after the firmware version X.4.0.8. 
		 * 
		 * \param	    nPortNumber 		The port number of the 3dRudder [0,3]
		 * \param 	  	nSize				The size.
		 * \param [in]	pTones				The tones list.
		 * \param 	  	bAddToPlayedList	(Optional) Add to played List (true: default) or Replace the played list.
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *		.
		 */

		virtual ErrorCode	PlaySndEx(uint32_t nPortNumber, uint32_t nSize, Tone *pTones, bool bAddToPlayedList = true)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::PlaySndEx(uint32_t nPortNumber, char *sTones, bool bAddToPlayedList = true) const;
		 * 
		 * \brief	Play sound list in text, using a string
		 *
		 *	  ¤sample Example_PlaySndEx_string¤
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.0.8¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \attention Usable only after the firmware version X.4.0.8. 
		 * \attention **Be careful ! PlaySndEx(uint32_t nPortNumber, uint32_t nSize, Tone *pTones, bool bAddToPlayedList = true)  const; & PlaySndEx(uint32_t nPortNumber, char *sTones, bool bAddToPlayedList = true)  const; ARE NOT THE SAME !** One uses a pointer, and the other one a String as arguments.
		 * \attention To use those functions, don't forget to use : **#define NB_ITEM(_Array) (sizeof(_Array) / sizeof(_Array[0]))**
		 *
		 * \param		nPortNumber 		The port number of the 3dRudder [0,3]
		 * \param [in]	sTones				The tones list in text.
		 * \param 	  	bAddToPlayedList	(Optional) Add to played List (true: default) or Replace the played list.
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes : </b>
		 * 
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *.
		 */

		virtual ErrorCode	PlaySndEx(uint32_t nPortNumber, char *sTones, bool bAddToPlayedList = true)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::RePlaySnd(uint32_t nPortNumber) const;
		 *
		 * \brief	Replay the last tones list.
		 *	
		 *	You have to use PlaySnd() function before starting RePlaySnd. If this isn't the case, will show ErrorCode : #NotReady
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success.
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *
		 */

		virtual ErrorCode	RePlaySnd(uint32_t nPortNumber)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::GetUserOffset(uint32_t nPortNumber, AxesValue *pAxis) const;
		 *
		 * \brief	Gets user offset
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param			nPortNumber The port number of the 3dRudder [0,3]
		 * \param [in,out]	pAxis	   	If non-null, the axis.
		 *
		 * \return	The user offset.
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *
		 */

		virtual ErrorCode	GetUserOffset(uint32_t nPortNumber, AxesValue *pAxis)  const;

		/**
		 * \fn	virtual ErrorCode CSdk::GetAxes(uint32_t nPortNumber,IAxesParam *pAxesParam, AxesValue *pAxis) ;
		 *
		 * \brief	Get the current value for each axis define by AxesValue. Axe's angle is given in degree.
		 *
		 * Values are valid only if the status is "InUse".
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param			nPortNumber The port number of the 3dRudder [0,3]
		 * \param 		  	pAxesParam 	The axes parameter.
		 * \param [in,out]	pAxis	   	If non-null, the axis.
		 *
		 * \return	or the axes. If pAxesParams is null, GetAxes return directly the angle (Degree) of each axis. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *
		 */

		virtual ErrorCode	GetAxes(uint32_t nPortNumber, IAxesParam *pAxesParam, AxesValue *pAxis) const;

		/**
		 * \fn	virtual Status CSdk::GetStatus(uint32_t nPortNumber) const;
		 *
		 * \brief	Gets the status.
		 * This function read the current status of the 3dRudder connected to nPortNumber
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	The status.
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *
		 */

		virtual Status		GetStatus(uint32_t nPortNumber)  const;

		/**
		 * \fn	virtual uint16_t CSdk::GetSensor(uint32_t nPortNumber, uint32_t nIndex) const;
		 *
		 * \brief	Gets a sensor, and the values of the force sensors. The value is in grams.
		 *
		 * This function reads the values of the 6 force sensors indexed by nIndex of the 3dRudder connected on nPortNumber.
		 *
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 * \param	nIndex	   	Zero-based index of the 3dRudder
		 *
		 * \return	The sensor. The unit of 16 bits returned value is given in grams. If 0 , it's possible an error, please check GetLastError for possible error.
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 *
		 */

		virtual uint16_t	GetSensor(uint32_t nPortNumber, uint32_t nIndex) const;

		/**
		 * \fn	virtual DeviceInformation CSdk::*GetDeviceInformation(uint32_t nPortNumber) const;
		 *
		 * \brief	Gets device information of the 3dRudder connected to nPortNumber
		 *
		 *	  ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	Null if it fails, else the device information.
		 * 			.
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 *
		 */

		virtual DeviceInformation *GetDeviceInformation(uint32_t nPortNumber) const;

		/**
		 * \fn	virtual ErrorCode CSdk::SetFreeze(uint32_t nPortNumber, bool bEnable)const;
		 *
		 * \brief	do a freeze of the 3dRudder value
		 * 
		 * It may be useful to temporarily desactivate and reactivate the 3dRudder connected to nPortNumber without necessarily removing and replacing the feet. 
		 *
		 * This makes it possible, for example, to freeze the displacement in the phases when they are not required in the 3D Universe, without risk of drifting, 			 
		 * and to avoid to freeze the user in his initial position, and thus relocate the device or move the legs to relax.
		 * 
		 * In Freeze mode, the values returned by the __3dRudder__ are identical to those returned when the device waits for the 2nd foot : the outputs are 0. 
		 *
		 * During an "unfreeze" : 
		 *	- The device switches directly to the "InUse" mode, without going through the required immobility step required in standard mode. This makes it possible to freeze the * 		displacements and to restore them without latency, for a more fluid operation. 
		 *	- The user offsets are recalculated when unfreezing : thus, during the freeze, the user can change his rest position. As a summary, the freeze/unfreeze function allows * 		you to reposition yourself without creating unintentional movements in the game. bEnable must be set to 'False' to unfreeze, and to 'True' to freeze.
		 *
		 *    ¤compatible_plateforme Win, PS4¤
		 *    ¤compatible_firm From x.4.x.x¤
		 *    ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]		 
		 * \param	bEnable	   	True to enable, false to disable.
		 *
		 * \return	The possible error code returned by this method. ErrorCode can also be a success.
		 * 	
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *.
		 */

		virtual ErrorCode	SetFreeze(uint32_t nPortNumber, bool bEnable)const;

		/**
		* \fn virtual ErrorCode CSdk::IsFrozen(uint32_t nPortNumber)const;
		*
		* \brief	return the frozen status of the device. 
		*
		* Check if the 3dRudder had been frozen by the comment CSdk::SetFreeze 
		*
		*    ¤compatible_plateforme Win, PS4¤
		*    ¤compatible_firm From x.4.1.0¤
		*    ¤compatible_sdk From 1.91¤		
		* \param	nPortNumber The port number of the 3dRudder [0,3]
		*
		* \return	frozen status
		*
		*/

		virtual bool IsFrozen(uint32_t nPortNumber);

		/**
		 * \fn virtual ErrorCode CSdk::GetLastError() const;
		 *
		 * \brief	Gets the last error
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \param	nPortNumber The port number of the 3dRudder [0,3]
		 *
		 * \return	The last error. The possible error code returned by this method. ErrorCode can also be a success. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotInitialized
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #DeviceNotSupported
		 * \possibleret #ValidationError
		 * \possibleret #DeviceInitError
		 * \possibleret #NotConnected
		 * \possibleret #NotReady
		 * \possibleret #ValidationTimeOut
		 * \possibleret #IncorrectCommand
		 * \possibleret #Fail
		 * \possibleret #Timeout
		 *		.
		 */

		virtual ErrorCode	GetLastError() const;

	

		/**
		 * \fn	virtual void CSdk::SetEvent(IEvent *pEvent) const;
		 *
		 * \brief	The SDK manages events.
		 *
		 * Set Event must be called before Init. To use it, you should create a class derived from IEvent and define method from the virtual. 
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \attention It's important to use it before the Init() function
		 *
		 * \param [in,out]	pEvent	If non-null, the event.
		 * 							.
		 */

		virtual void		SetEvent(IEvent *pEvent)  const;

		/**
		 * \fn	virtual float CSdk::CalcCurveValue(float fDeadZone, float fxSat, float fExp, float fValue) const;
		 *
		 * \brief	Calculates the curve value. Used when you're using your own response curve for each axes of the 3dRudder.
		 *
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \param	fDeadZone	The dead zone.
		 * \param	fxSat	 	The effects sat.
		 * \param	fExp	 	The exponent.
		 * \param	fValue   	The raw value.
		 *
		 * \return	The calculated curve value.
		 * 			.
		 */

		virtual float		CalcCurveValue(float fDeadZone, float fxSat, float fExp, float fValue)  const;

	};

	

	
}
