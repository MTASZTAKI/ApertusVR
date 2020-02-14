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

class CEventSnd : public ns3dRudder::IEvent
{
public:
	CEventSnd()
	{
	}
	ns3dRudder::CSdk *m_pSdk;
	void OnEndSound(uint32_t nPortNumber)
	{
		m_pSdk->RePlaySnd(nPortNumber);
	}
};


int main()
{
	char *g_pSequenceText_StarWars = "a4(500, 0)a4(500, 0)a4(500, 0)f4(350, 0)c5(150, 0)a4(500, 0)f4(350, 0)c5(150, 0)a4(650, 50)e5(500, 0)e5(500, 0)e5(500, 0)f5(350, 0)c5(150, 0)g#4(500, 0)f4(350, 0)c5(150, 0)a4(650, 50)a5(500, 0)a4(300, 0)a4(150, 0)a5(500, 0)g#5(325, 0)g5(175, 0)f#5(125, 0)f5(125, 0)f#5(250, 325)a#4(250, 0)d#5(500, 0)d5(325, 0)c#5(175,0)c5(125,0)b4(125,0)c5(250,350)f4(250, 0)g#4(500, 0)f4(350, 0)a4(125, 0)c5(500, 0)a4(375, 0)c5(125, 0)e5(650, 500)a5(500, 0)a4(300, 0)a4(150, 0)a5(500, 0)g#5(325, 0)g5(175, 0)f#5(125, 0)f5(125, 0)f#5(250, 325)a#4(250, 0)d#5(500, 0)d5(325, 0)c#5(175,0)c5(125,0)b4(125,0)c5(250,350)f4(250, 0)g#4(500, 0)f4(375, 0)c5(125, 0)a4(500, 0)f4(375, 0)c5(125, 0)a4(650, 650)";
	CEventSnd m_EventSnd;
	ns3dRudder::ErrorCode errCodeLoad = ns3dRudder::LoadSDK(_3DRUDDER_SDK_LAST_COMPATIBLE_VERSION);

	if (errCodeLoad == ns3dRudder::ErrorCode::Success)
	{
		ns3dRudder::CSdk* pSdk = ns3dRudder::GetSDK();
		m_EventSnd.m_pSdk = pSdk;
		pSdk->SetEvent(&m_EventSnd);
		pSdk->Init();

		color(ccFluorescentPink, ccBlack);
		uint16_t nVersion = pSdk->GetVersion(0);
		printf("Firmware Version %04x\n", nVersion);
		uint16_t nSdkVersion = pSdk->GetSDKVersion();
		printf("SDK Version %04x\n", nSdkVersion);
		color(ccWhite, ccBlack);


		while (1)
		{
			if (pSdk->IsDeviceConnected(0))
			{
				pSdk->PlaySndEx(0, g_pSequenceText_StarWars);
			}
			else
				Sleep(100);

			color(ccFluorescentGreen, ccBlack);
			printf("press any key to play again, 'Q' to quit\n");
			color(ccWhite, ccBlack);

			int nKey = _getch();
			if (nKey == 'Q' || nKey == 'q')
				break;
		}

		ns3dRudder::EndSDK();
	}

	

	return 0;
}

