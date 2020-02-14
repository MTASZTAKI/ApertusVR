// SimpleSDKUsage.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <conio.h>

#define _3DRUDDER_SDK_STATIC
#include "3DRudderSDK.h"

typedef enum MyEnum
{
	ccBlack = 0,
	ccDarkBlue,
	ccGreen,
	ccGrayBlue,
	ccBrown,
	ccPurple,
	ccKaki,
	ccLightGray,
	ccGray,
	ccBlue,
	ccFluorescentGreen,
	ccTurquoise,
	ccRed,
	ccFluorescentPink,
	ccFluorescentYellow,
	ccWhite
}ConsoleColor;

void color(ConsoleColor t, ConsoleColor f)
{
	HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(H, WORD(f) * 16 + WORD(t));
}


class CEvent : public ns3dRudder::IEvent
{
	void OnConnect(uint32_t nDeviceNumber)
	{
		color(ccBlue, ccBlack);
		printf("\tOnConnect( %d )\n", nDeviceNumber);
		color(ccWhite, ccBlack);
	}
	void OnDisconnect(uint32_t nDeviceNumber)
	{
		color(ccBlue, ccBlack);
		printf("\tOnDisconnect(%d)\n", nDeviceNumber);
		color(ccWhite, ccBlack);
	}
};


int main()
{
	CEvent m_Event;
	ns3dRudder::ErrorCode errCodeLoad = ns3dRudder::LoadSDK(_3DRUDDER_SDK_LAST_COMPATIBLE_VERSION);
	if (errCodeLoad == ns3dRudder::ErrorCode::Success)
	{
		ns3dRudder::CSdk* pSdk = ns3dRudder::GetSDK();

		pSdk->SetEvent(&m_Event);
		pSdk->Init();

		color(ccFluorescentPink, ccBlack);		
		while (!pSdk->IsDeviceConnected(0))
			Sleep(100);
		uint16_t nVersion = pSdk->GetVersion(0);
		printf("Firmware Version %04x\n", nVersion);
		uint16_t nSdkVersion = pSdk->GetSDKVersion();
		printf("SDK Version %04x\n", nSdkVersion);
		color(ccWhite, ccBlack);

		color(ccRed, ccBlack);
		printf("Press 'Q' to quit\n");
		color(ccFluorescentYellow, ccBlack);
		printf("Press '+' and '-' to change mode\n");
		BOOL bFirst = TRUE;
		int nMode = 0;
		ns3dRudder::AxesParamDefault axesParamDefault = ns3dRudder::AxesParamDefault();
		ns3dRudder::AxesParamNormalizedLinear axesParamNormalizedLinear = ns3dRudder::AxesParamNormalizedLinear();
		ns3dRudder::ErrorCode errCodeGetAxes = ns3dRudder::ErrorCode::NotReady;



		while (1)
		{

			ns3dRudder::AxesValue lAxis;
			ns3dRudder::AxesValue lAxisUserOffset;


			if (_kbhit())
			{
				int nKey = _getch();
				if (nKey == 'q' || nKey == 'Q')
					break;
				if (nKey == '+' || nKey == '-')
				{
					if (nKey == '+')
						nMode += 1;
					if (nKey == '-')
						nMode -= 1;

					if (nMode < 0)
						nMode = 0;
					if (nMode > 2)
						nMode = 2;
				}
			}

			color(ccTurquoise, ccBlack);
			switch (nMode)
			{
			case 0:
				printf("AxesParam : UserRefAngle                                                                 \n");
				errCodeGetAxes = pSdk->GetAxes(0, NULL, &lAxis);
				break;
			case 1:
				printf("AxesParam : NormalizedLinear                                                              \n");
				errCodeGetAxes = pSdk->GetAxes(0, &axesParamNormalizedLinear, &lAxis);
				break;
			case 2:
				printf("AxesParam : Default                                                               \n");
				errCodeGetAxes = pSdk->GetAxes(0, &axesParamDefault, &lAxis);
				break;
			}
			color(ccWhite, ccBlack);

			if (errCodeGetAxes != ns3dRudder::NotReady)
			{
				color(ccFluorescentGreen, ccBlack);
				printf("Axes : ");
				color(ccWhite, ccBlack);
				printf("LeftRight: %2.5f ForwardBackward: %2.5f UpDown; %2.5f Rotation: %2.5f ", lAxis.Get(ns3dRudder::Axes::LeftRight),
					lAxis.Get(ns3dRudder::Axes::ForwardBackward), lAxis.Get(ns3dRudder::Axes::UpDown), lAxis.Get(ns3dRudder::Axes::Rotation));
				color(ccRed, ccBlack);
				ns3dRudder::Status status = pSdk->GetStatus(0);
				switch (status)
				{
				case ns3dRudder::NoFootStayStill:
					printf("Status : Don't put your Feet !!! Stay still 5s\r");
					bFirst = TRUE;
					break;
				case ns3dRudder::Initialization:
					printf("Status : Initialisation                       \r");
					bFirst = TRUE;
					break;
				case ns3dRudder::PutYourFeet:
					printf("Status : Please put your feet                 \r");
					bFirst = TRUE;
					break;
				case ns3dRudder::PutSecondFoot:
					printf("Status :  Put your second foot                \r");
					bFirst = TRUE;
					break;
				case ns3dRudder::StayStill:
					printf("Status : Stay still                           \r");

					pSdk->GetUserOffset(0, &lAxisUserOffset);
					color(ccFluorescentGreen, ccBlack);
					if (bFirst)
					{
						printf("GetUserOffset : ");
						color(ccWhite, ccBlack);
						printf("LeftRight: %2.5f ForwardBackward: %2.5f UpDown: %2.5f Rotation: %2.5f                                         \n ",
							lAxisUserOffset.Get(ns3dRudder::Axes::LeftRight), lAxisUserOffset.Get(ns3dRudder::Axes::ForwardBackward), lAxisUserOffset.Get(ns3dRudder::Axes::UpDown),
							lAxisUserOffset.Get(ns3dRudder::Axes::Rotation));
						bFirst = FALSE;
					}
					break;
				case ns3dRudder::InUse:
					printf("Status : 3DRudder in use                      \r");
					bFirst = TRUE;
					break;
				}
			}

			Sleep(100);
		}
		ns3dRudder::EndSDK();
	}
	else
	{
		printf("%s", ns3dRudder::GetErrorText(errCodeLoad));
	}


	return 0;
}

