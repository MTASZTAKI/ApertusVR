/*MIT License

Copyright (c) 2016 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#ifdef __linux__
#include <unistd.h>
#include <syslog.h>
#endif
#include "RakPeerInterface.h"
#include "RakSleep.h"
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "NatPunchthroughServer.h"
#include "SocketLayer.h"
#include "Getche.h"
#include "Gets.h"

int main(void)
{
	RakNet::RakPeerInterface* pPeer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SystemAddress addresses[MAXIMUM_NUMBER_OF_INTERNAL_IDS];
	int addressIndex = 0;
	for (int i = 0; i < MAXIMUM_NUMBER_OF_INTERNAL_IDS; i++)
	{
		addresses[i] = pPeer->GetLocalIP(i);
		if (addresses[i] == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
			addressIndex = i;
			break;
		}
	}
	int socketDescritproLength = 1;
	RakNet::SocketDescriptor socketDescriptor[2];
	socketDescriptor[0].port = 61666;
	if (addressIndex >= 2)
	{
		strcpy(socketDescriptor[0].hostAddress, addresses[0].ToString(false));
		socketDescriptor[1].port = 61666 + 1;
		strcpy(socketDescriptor[1].hostAddress, addresses[1].ToString(false));
		socketDescritproLength = 2;
	}
	if (pPeer->Startup(8096, socketDescriptor, socketDescritproLength) != RakNet::RAKNET_STARTED)
	{
		std::cout << "Unable to start peer..." << std::endl;
		RakNet::RakPeerInterface::DestroyInstance(pPeer);
		return 0;
	}
	pPeer->SetTimeoutTime(5000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	std::cout << "Start peer on " << pPeer->GetMyBoundAddress().ToString(true) << std::endl;
	pPeer->SetMaximumIncomingConnections(8096);
	auto natServer = new RakNet::NatPunchthroughServer;
	pPeer->AttachPlugin(natServer);
	RakNet::Packet* pPacket;											    	
	while (true)
	{
		for (pPacket = pPeer->Receive(); pPacket; pPeer->DeallocatePacket(pPacket), pPacket = pPeer->Receive())
			;
		RakSleep(30);
	}
}

