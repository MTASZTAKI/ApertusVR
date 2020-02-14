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
* \file	3DRudderSDK.h.
*
* \brief	The c++ 3dRudder SDK
*/
#pragma once
#include "3DRudderSDK-Main.h"

/**
* \namespace	ns3dRudder
*
* \brief	Namespace used for the 3dRudder SDK
*
*/
namespace ns3dRudder
{
	/**
	* \fn	ErrorCode LoadSDK(uint16_t nVersion);
	*
	* \brief	Load the SDK version corresponding to nVersion.
	*
	*	 ¤compatible_plateforme Win, PS4¤
	*    ¤compatible_firm From x.4.x.x¤
	*    ¤compatible_sdk From 2.00¤
	*
	* \param	nVersion The SDK version, we advice to use _3DRUDDER_SDK_LAST_COMPATIBLE_VERSION
	*
	* \return	Success if the SDK with the asked version is correctly loaded, else check the error code.  \n \n \n <b> Possible error codes : </b>
	*
	* \possibleret #Success
	* \possibleret #ValidationError
	* \possibleret #IncorrectCommand
	* \possibleret #DashboardInstallError
	* \possibleret #DashboardUpdateError
	* 			
	*/
	_3DRUDDER_SDK_EXPORT ErrorCode LoadSDK(uint16_t nVersion = _3DRUDDER_SDK_LAST_COMPATIBLE_VERSION);

	/**
	* \fn	CSdk* GetSDK();
	*
	* \brief	Get access of the SDK.
	*
	*	 ¤compatible_plateforme Win, PS4¤
	*    ¤compatible_firm From x.4.x.x¤
	*    ¤compatible_sdk From 2.00¤
	*
	* \return	pointer to access the 3dRudder SDK, Null if it fails.
	* 			
	*/
	_3DRUDDER_SDK_EXPORT CSdk *GetSDK();

	/**
	* \fn	void EndSDK();
	*
	* \brief	Ends a sdk.
	*
	*	 ¤compatible_plateforme Win, PS4¤
	*    ¤compatible_firm From x.4.x.x¤
	*    ¤compatible_sdk From 2.00¤
	*/
	_3DRUDDER_SDK_EXPORT void EndSDK();

	/**
		 * \fn	virtual const char CSdk::*GetErrorText(ErrorCode nError) 
		 *
		 * \brief Translates the code to human readable value.
		 *
		 *  If the error code isn't defined, GetErrorText return "Other : Default".
		 *
		 * | Code | Value ||
		 * | --- | --- ||
		 * | ns3dRudder::Success | Success ||
		 * | ns3dRudder::NotSupported | Not Supported ||
		 * | ns3dRudder::Other| Other ||
		 *
		 * ¤compatible_plateforme Win, PS4¤
		 * ¤compatible_firm From x.4.x.x¤
		 * ¤compatible_sdk From 2.00¤
		 *
		 * \param	nError	The error code to be translated
		 *
		 *
		 * \return A char* which describe the code. \n \n \n <b> Possible error codes : </b>
		 *
		 * \possibleret #Success
		 * \possibleret #NotConnected
		 * \possibleret #Fail
		 * \possibleret #IncorrectCommand
		 * \possibleret #DeviceNotSupported
		 * \possibleret #NotSupported
		 * \possibleret #Timeout
		 * \possibleret #FirmwareNeedToBeUpdated
		 * \possibleret #NotInitialized
		 * \possibleret #NotReady
		 * \possibleret #Other
		 * 			.
		 */

	_3DRUDDER_SDK_EXPORT const char *GetErrorText(ns3dRudder::ErrorCode nError);
}

#include "3dRudderSDK-Extra.h"






