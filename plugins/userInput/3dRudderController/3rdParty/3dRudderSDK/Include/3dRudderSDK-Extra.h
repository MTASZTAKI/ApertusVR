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
* \file	3DRudderSDK-Extra.h
*
* \brief	The c++ 3dRudder SDK containing the extra features
*/
#pragma once

/**
* \namespace	ns3dRudder
*
* \brief	Namespace used for the 3dRudder SDK
*
*/
namespace ns3dRudder
{
	const float c_coeff_deadZone = 0.15f;
	/**
	* \fn	inline float Curve::CalcCurveValue(float fValue) const
	*
	* \brief	Calculates the curve value.
	*
	* ¤compatible_plateforme Win, PS4¤
	* ¤compatible_firm From x.4.x.x¤
	* ¤compatible_sdk From 2.00¤
	*
	* \param	fValue	The value.
	*
	* \return	The calculated curve value.
	* 			.
	*/

	inline float Curve::CalcCurveValue(float fValue)  const
	{
		CSdk* pSdk = GetSDK();
		return pSdk->CalcCurveValue(m_fDeadZone, m_fxSat, m_fExp, fValue);
	}

	/**
	* \class	AxesParamDefault
	*
	* \brief	The axes parameter default.
	*/

	class AxesParamDefault : public IAxesParam
	{
	public:

		/**
		* \fn	AxesParamDefault::AxesParamDefault()
		*
		* \brief	Constructor
		*
		*
		* ¤compatible_plateforme Win, PS4¤
		* ¤compatible_firm From x.4.x.x¤
		* ¤compatible_sdk From 2.00¤
		*
		* 						.
		*/
		AxesParamDefault() : IAxesParam() 
		{
			m_Curve[LeftRight] = Curve(1.0f, c_coeff_deadZone, 2.0f);
			m_Curve[ForwardBackward] = Curve(1.0f, c_coeff_deadZone, 2.0f);
			m_Curve[UpDown] = Curve(0.6f, 0.08f, 4.0f);
			m_Curve[Rotation] = Curve(1.0f, c_coeff_deadZone, 2.0f);

			SetCurve(UpDown, &m_Curve[UpDown]);
			SetCurve(LeftRight, &m_Curve[LeftRight]);
			SetCurve(ForwardBackward, &m_Curve[ForwardBackward]);
			SetCurve(Rotation, &m_Curve[Rotation]);

			SetNonSymmetrical(true);
		}

		virtual ~AxesParamDefault() {}

		/**
		* \fn	ErrorCode AxesParamDefault::UpdateParam(uint32_t nPortNumber)
		*
		*
		*
		* ¤compatible_plateforme Win, PS4¤
		* ¤compatible_firm From x.4.x.x¤
		* ¤compatible_sdk From 2.00¤
		*
		* \param	nPortNumber	The port number.
		* 						.
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
		*
		*/
		ErrorCode UpdateParam(uint32_t nPortNumber)
		{
			ErrorCode nError = Success;
			float fxSatLeftRight = 1.0f;
			float fxSatForwardBackward = 1.0f;
			float fxSatRotation = 1.0f;
			float fRollToYaw = 0.15f;

			DeviceInformation *pInfo = GetSDK()->GetDeviceInformation(nPortNumber);
			if (pInfo)
			{
				fxSatLeftRight = pInfo->GetUserRoll() / pInfo->GetMaxRoll();
				fxSatForwardBackward = pInfo->GetUserPitch() / pInfo->GetMaxPitch();
				fxSatRotation = pInfo->GetUserYaw() / pInfo->GetMaxYaw();
				fRollToYaw = pInfo->GetDefaultRoll2YawCompensation();
			}
			else
				nError = GetSDK()->GetLastError();

			m_Curve[LeftRight].SetXSat(fxSatLeftRight);
			m_Curve[LeftRight].SetDeadZone(0.15f * fxSatLeftRight);
			m_Curve[ForwardBackward].SetXSat(fxSatForwardBackward);
			m_Curve[ForwardBackward].SetDeadZone(0.15f * fxSatForwardBackward);
			m_Curve[Rotation].SetXSat(fxSatRotation);
			m_Curve[Rotation].SetDeadZone(0.15f * fxSatRotation);

			SetRoll2YawCompensation(fRollToYaw);

			return nError;
		}

	private:
		Curve m_Curve[MaxAxes];		
	};

	/**
	* \class	AxesParamNormalizedLinear
	*
	* \brief	The axes parameter used to calculate the axes in Normalized Linear.
	*/

	class AxesParamNormalizedLinear : public IAxesParam
	{
	public:

		/**
		* \fn	AxesParamNormalizedLinear::AxesParamNormalizedLinear()
		*
		* \brief	Constructor
		*
		*	  ¤compatible_plateforme Win, PS4¤
		*    ¤compatible_firm From x.4.x.x¤
		*    ¤compatible_sdk From 2.00¤
		*
		*/
		AxesParamNormalizedLinear() : IAxesParam() 
		{
			m_Curve[LeftRight] = Curve(1.0f, 0.0f, 1.0f);
			m_Curve[ForwardBackward] = Curve(1.0f, 0.0f, 1.0f);
			m_Curve[UpDown] = Curve(1.0f, 0.0f, 1.0f);
			m_Curve[Rotation] = Curve(1.0f, 0.0f, 1.0f);

			SetCurve(LeftRight, &m_Curve[LeftRight]);
			SetCurve(ForwardBackward, &m_Curve[ForwardBackward]);
			SetCurve(UpDown, &m_Curve[UpDown]);
			SetCurve(Rotation, &m_Curve[Rotation]);

			SetNonSymmetrical(false);
			SetRoll2YawCompensation(0.0f);
		}
		virtual ~AxesParamNormalizedLinear() {}

		/**
		* \fn	ErrorCode AxesParamNormalizedLinear::UpdateParam(uint32_t nPortNumber)
		*
		* \brief	Update curves data
		*
		*	 ¤compatible_plateforme Win, PS4¤
		*    ¤compatible_firm From x.4.x.x¤
		*    ¤compatible_sdk From 2.00¤
		*
		* \param	nPortNumber	The port number.
		* 						.
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
		*/

		ErrorCode UpdateParam(uint32_t nPortNumber)
		{
			GetSDK()->GetDeviceInformation(nPortNumber);
			return GetSDK()->GetLastError();
		}


	private:
		Curve m_Curve[MaxAxes];
	};

	/**
	* \class	AxesParamCustom
	*
	* \brief	The axes parameter custom.
	*/

	class AxesParamCustom : public IAxesParam
	{
	public:		
		/**
		* \fn	AxesParamCustom::AxesParamCustom()
		*
		* \brief	Constructor
		*
		*
		* ¤compatible_plateforme Win, PS4¤
		* ¤compatible_firm From x.4.x.x¤
		* ¤compatible_sdk From 2.00¤
		* 
		*/
		AxesParamCustom() : IAxesParam()
		{		
			m_CustomCurve[LeftRight] = Curve(1.0f, c_coeff_deadZone, 2.0f);
			m_CustomCurve[ForwardBackward] = Curve(1.0f, c_coeff_deadZone, 2.0f);
			m_CustomCurve[UpDown] = Curve(0.6f, 0.08f, 4.0f);
			m_CustomCurve[Rotation] = Curve(1.0f, c_coeff_deadZone, 2.0f);

			SetCurve(UpDown, &m_Curve[UpDown]);
			SetCurve(LeftRight, &m_Curve[LeftRight]);
			SetCurve(ForwardBackward, &m_Curve[ForwardBackward]);
			SetCurve(Rotation, &m_Curve[Rotation]);

			SetNonSymmetrical(true);
		}

		virtual ~AxesParamCustom() {}

		/**
		* \fn	Curve * AxesParamCustom::GetCustomCurve(Axes nCurve) const
		*
		* \brief	Gets a custom curve
		* You can use this methode to get the custom cruve to update it the way you need. 
		*
		* ¤compatible_plateforme Win, PS4¤
		* ¤compatible_firm From x.4.x.x¤
		* ¤compatible_sdk From 2.00¤
		*
		* \param	nCurve	The curve.
		*
		* \return	Null if it fails, else the curve.
		* 			.
		*/
		Curve *	GetCustomCurve(Axes nCurve)
		{
			if (nCurve >= LeftRight && nCurve < MaxAxes)
				return &m_CustomCurve[nCurve];
			else
				return nullptr;
		}

		/**
		* \fn	ErrorCode AxesParamCustom::UpdateParam(uint32_t nPortNumber)
		*
		*
		*
		* ¤compatible_plateforme Win, PS4¤
		* ¤compatible_firm From x.4.x.x¤
		* ¤compatible_sdk From 2.00¤
		*
		* \param	nPortNumber	The port number.
		* 						.
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
		*
		*/
		ErrorCode UpdateParam(uint32_t nPortNumber)
		{
			ErrorCode nError = Success;
			float fxSatLeftRight = m_CustomCurve[LeftRight].GetXSat();
			float fxSatForwardBackward = m_CustomCurve[ForwardBackward].GetXSat();
			float fxSatUpDown = m_CustomCurve[UpDown].GetXSat();
			float fxSatRotation = m_CustomCurve[Rotation].GetXSat();			

			DeviceInformation *pInfo = GetSDK()->GetDeviceInformation(nPortNumber);
			if (pInfo)
			{
				fxSatLeftRight *= pInfo->GetUserRoll() / pInfo->GetMaxRoll();
				fxSatForwardBackward *= pInfo->GetUserPitch() / pInfo->GetMaxPitch();				
				fxSatRotation *= pInfo->GetUserYaw() / pInfo->GetMaxYaw();
			}
			else
				nError = GetSDK()->GetLastError();

			m_Curve[LeftRight] = Curve(fxSatLeftRight, fxSatLeftRight * m_CustomCurve[LeftRight].GetDeadZone(), m_CustomCurve[LeftRight].GetExp());
			m_Curve[ForwardBackward] = Curve(fxSatForwardBackward, fxSatForwardBackward * m_CustomCurve[ForwardBackward].GetDeadZone(), m_CustomCurve[ForwardBackward].GetExp());
			m_Curve[UpDown] = Curve(fxSatUpDown, fxSatUpDown * m_CustomCurve[UpDown].GetDeadZone(), m_CustomCurve[UpDown].GetExp());
			m_Curve[Rotation] = Curve(fxSatRotation, fxSatRotation * m_CustomCurve[Rotation].GetDeadZone(), m_CustomCurve[Rotation].GetExp());			

			return nError;
		}

	private:
		Curve m_Curve[MaxAxes];
		Curve m_CustomCurve[MaxAxes];
	};
}