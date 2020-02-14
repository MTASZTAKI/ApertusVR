// MarioTheme_PlaySndEx_Tone_Array.cpp : Defines the entry point for the console application.
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

#define NB_ITEM(_Array) (sizeof(_Array) / sizeof(_Array[0]))

void PlayMarioTheme(ns3dRudder::CSdk* pSdk,float fPitch)
{
	ns3dRudder::Tone Mario[] = 
	{ 
		{ 660,10,15 },{ 660,10,30 },{ 660,10,30 },{ 510,10,10 },{ 660,10,30 },{ 770,10,55 },{ 380,10,57 },{ 510,10,45 },{ 380,10,40 },{ 320,10,50 },{ 440,10,30 },{ 480, 8,33 },
		{ 450,10,15 },{ 430,10,30 },{ 380,10,20 },{ 660, 8,20 },{ 760, 5,15 },{ 860,10,30 },{ 700, 8,15 },{ 760, 5,35 },{ 660, 8,30 },{ 520, 8,15 },{ 580, 8,15 },{ 480, 8,50 },
		{ 510,10,45 },{ 380,10,40 },{ 320,10,50 },{ 440,10,30 },{ 480, 8,33 },{ 450,10,15 },{ 430,10,30 },{ 380,10,20 },{ 660, 8,20 },{ 760, 5,15 },{ 860,10,30 },{ 700, 8,15 },
		{ 760, 5,35 },{ 660, 8,30 },{ 520, 8,15 },{ 580, 8,15 },{ 480, 8,50 },{ 500,10,30 },{ 760,10,10 },{ 720,10,15 },{ 680,10,15 },{ 620,15,30 },{ 650,15,30 },{ 380,10,15 },
		{ 430,10,15 },{ 500,10,30 },{ 430,10,15 },{ 500,10,10 },{ 570,10,22 },{ 500,10,30 },{ 760,10,10 },{ 720,10,15 },{ 680,10,15 },{ 620,15,30 },{ 650,20,30 },{ 1020,8,30 },
		{ 1020,8,15 },{ 1020,8,30 },{ 380,10,30 },{ 500,10,30 },{ 760,10,10 },{ 720,10,15 },{ 680,10,15 },{ 620,15,30 },{ 650,15,30 },{ 380,10,15 },{ 430,10,15 },{ 500,10,30 },
		{ 430,10,15 },{ 500,10,10 },{ 570,10,42 },{ 585,10,45 },{ 550,10,42 },{ 500,10,36 },{ 380,10,30 },{ 500,10,30 },{ 500,10,15 },{ 500,10,30 },{ 500,10,30 },{ 760,10,10 },
		{ 720,10,15 },{ 680,10,15 },{ 620,15,30 },{ 650,15,30 },{ 380,10,15 },{ 430,10,15 },{ 500,10,30 },{ 430,10,15 },{ 500,10,10 },{ 570,10,22 },{ 500,10,30 },{ 760,10,10 },
		{ 720,10,15 },{ 680,10,15 },{ 620,15,30 },{ 650,20,30 },{ 1020,8,30 },{ 1020,8,15 },{ 1020,8,30 },{ 380,10,30 },{ 500,10,30 },{ 760,10,10 },{ 720,10,15 },{ 680,10,15 },
		{ 620,15,30 },{ 650,15,30 },{ 380,10,15 },{ 430,10,15 },{ 500,10,30 },{ 430,10,15 },{ 500,10,10 },{ 570,10,42 },{ 585,10,45 },{ 550,10,42 },{ 500,10,36 },{ 380,10,30 },
		{ 500,10,30 },{ 500,10,15 },{ 500,10,30 },{ 500, 6,15 },{ 500, 8,30 },{ 500, 6,35 },{ 500, 8,15 },{ 580, 8,35 },{ 660, 8,15 },{ 500, 8,30 },{ 430, 8,15 },{ 380, 8,60 },
		{ 500, 6,15 },{ 500, 8,30 },{ 500, 6,35 },{ 500, 8,15 },{ 580, 8,15 },{ 660, 8,55 },{ 870, 8,32 },{ 760, 8,60 },{ 500, 6,15 },{ 500, 8,30 },{ 500, 6,35 },{ 500, 8,15 },
		{ 580, 8,35 },{ 660, 8,15 },{ 500, 8,30 },{ 430, 8,15 },{ 380, 8,60 },{ 660,10,15 },{ 660,10,30 },{ 660,10,30 },{ 510,10,10 },{ 660,10,30 },{ 770,10,55 },{ 380,10,57 } 
	};

	if (pSdk->IsDeviceConnected(0))
	{
		int nNbTones = NB_ITEM(Mario);
		if(fPitch!=0)
			for (int i = 0; i < nNbTones; i++)
				Mario[i].SetFrequency(uint16_t(float(Mario[i].GetFrequency()) * 2.0 * fPitch));

		pSdk->PlaySndEx(0, nNbTones, Mario);
	}
}


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
		uint16_t nVersion = pSdk->GetVersion(0);
		printf("Firmware Version %04x\n", nVersion);
		uint16_t nSdkVersion = pSdk->GetSDKVersion();
		printf("SDK Version %04x\n", nSdkVersion);
		color(ccWhite, ccBlack);

		float fPitch = 0.0;
		int nPitch = 0;
		while (1)
		{
			color(ccFluorescentGreen, ccBlack);
			printf("press any key to play, 'Q' to quit, '+' or '-' to change the pitch\n");
			color(ccWhite, ccBlack);
			int nKey = 0;
			while (1)
			{
				nKey = _getch();
				if (nKey == '+' || nKey == '-')
				{
					if (nKey == '+')
						nPitch++;
					if (nKey == '-')
						nPitch--;
					if (nPitch >= 0)
					{
						fPitch = float(nPitch);
						printf("Pitch %d         \r", nPitch);
					}
					if (nPitch < 0)
					{
						fPitch = 1.0f / float(-nPitch);
						printf("Pitch 1/%d\r", nPitch);
					}
				}
				else
					break;
			}
			if (nKey == 'Q' || nKey == 'q')
				break;
			PlayMarioTheme(pSdk, fPitch);
		}

		ns3dRudder::EndSDK();
	}
	
	
	return 0;
}

