#include "SBFaceShiftManager.h"
#include "external/faceshift/fsbinarystream.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAttribute.h>
#include <vhcl.h>
#include <iostream>
#include <sstream>

#if WIN32
#include <WS2tcpip.h>
#include <tchar.h>
#include <Windows.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "33433"
#define DEFAULT_IPADDR "127.0.0.1"
#define DEFAULT_BUFLEN 1024

namespace SmartBody {

SBFaceShiftManager::SBFaceShiftManager() : SBService()
{
	setEnable(false);
	setName("FaceShift");
	connectSocket = INVALID_SOCKET;
	createStringAttribute("defaultIP", "127.0.0.1", true, "Basic", 60, false, false, false, "IP address for connection with FaceShift software.");
	createIntAttribute("defaultPort", 33433, true, "Basic", 60, false, false, false, "Port No. for connection with FaceShift software.");
	createBoolAttribute("debug", false, true, "Basic", 60, false, false, false, "Show debugging information.");
	createActionAttribute("dumpData", true, "Basic", 60, false, false, false, "Dump the faceshift data.");
}

SBFaceShiftManager::~SBFaceShiftManager()
{
	
}

void SBFaceShiftManager::setEnable( bool val )
{	
	SBService::setEnable(val);	
	if (val)
		start();
	else
		stop();
}

bool SBFaceShiftManager::isEnable()
{
	return SBService::isEnable();
}

void SBFaceShiftManager::start()
{
	coeffTable.clear();

	initConnection();

	fs::fsMsgSendBlendshapeNames msg;
	std::string dataToSend;
	fs::fsBinaryStream parserOut;
	parserOut.encode_message(dataToSend, msg);
	send(connectSocket,dataToSend.data(),dataToSend.size(),0);

}

void SBFaceShiftManager::stop()
{
	stopConnection();
}


void SBFaceShiftManager::update( double time )
{
	if (!this->isEnable())
		false;

	int iResult;
	fs::fsBinaryStream parserIn, parserOut;
	fs::fsMsgPtr msg;
	static char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
	if ( iResult > 0 ){
		parserIn.received(iResult, recvbuf);
		while(msg=parserIn.get_message()) 
		{			
			if(dynamic_cast<fs::fsMsgTrackingState*>(msg.get())) 
			{
				
				fs::fsMsgTrackingState *ts = dynamic_cast<fs::fsMsgTrackingState*>(msg.get());
				const fs::fsTrackingData &data = ts->tracking_data();
				
				for (unsigned int i=0;i<data.m_coeffs.size();i++)
				{
					if (shapeNames.size() > i)
					{
						std::map<std::string, double>::iterator coeffTableiter = coeffTable.find(shapeNames[i]);
						if (coeffTableiter == coeffTable.end())
						{
							coeffTable.insert(std::pair<std::string, double>(shapeNames[i], data.m_coeffs[i]));
						}
						else
						{
							coeffTable[shapeNames[i]] = data.m_coeffs[i];						
						}

					}					
					else
					{
						if (this->getBoolAttribute("debug"))
						{
							LOG("Shape names (%d) cannot handle data for shape %d", shapeNames.size(), i);
						}
					}
						
				}
				headRotation = SrQuat(data.m_headRotation.w, data.m_headRotation.x, data.m_headRotation.y, data.m_headRotation.z);				
				// Do something with the Tracking Data (change controllers in DAZ or record into the timeline, depending 
				// on the state of the plugin
				//LOG("head rotation = %f %f %f %f",data.m_headRotation.w, data.m_headRotation.x, data.m_headRotation.y, data.m_headRotation.z);
				//LOG("head translation: %f %f %f",data.m_headTranslation.x,data.m_headTranslation.y,data.m_headTranslation.z);						
			} 

			if (dynamic_cast<fs::fsMsgBlendshapeNames*>(msg.get()))
			{
				fs::fsMsgBlendshapeNames *bsnames = dynamic_cast<fs::fsMsgBlendshapeNames*>(msg.get());
				
				std::vector<std::string>& names = bsnames->blendshape_names();
				shapeNames.clear();
				LOG("Shapes sent from FaceShift:");
				for (std::vector<std::string>::iterator iter = names.begin();
					 iter != names.end();
					 iter++)
				{
					LOG("[%s]", (*iter).c_str());
					
					shapeNames.push_back(*iter);
				}
			}
		}
	}
	if(!parserIn.valid()) {
		printf("parser in invalid state\n");
		parserIn.clear();
	}
}

void SBFaceShiftManager::initConnection()
{
	int err = 0;
	WSADATA wsaData;
	WORD wVersionRequested;
	
	struct sockaddr_in clientService;
	
	int iResult;	
	wVersionRequested = MAKEWORD(2,2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return;
	}


	// Create a SOCKET for connecting to server
	connectSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET) {	
		WSACleanup();
		return;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port of the server to be connected to.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( getStringAttribute("defaultIP").c_str() );
	clientService.sin_port = htons( getIntAttribute("defaultPort") );


	iResult = connect( connectSocket, (SOCKADDR*) &clientService, sizeof(clientService));
	if ( iResult == SOCKET_ERROR) {
		closesocket (connectSocket);
		WSACleanup();
		return;
	}

	if (connectSocket == INVALID_SOCKET) {
		closesocket(connectSocket);
		WSACleanup();
	}
}

void SBFaceShiftManager::stopConnection()
{
	closesocket(connectSocket);
	WSACleanup();
}

double SBFaceShiftManager::getCoeffValue( const std::string& blendName )
{
	double coeff = 0.0;
	if (coeffTable.find(blendName) != coeffTable.end())
	{
		coeff = coeffTable[blendName];
	}
	else
	{
		if (this->getBoolAttribute("debug"))
			LOG("Cannot find shape named: %s", blendName.c_str());
	}
	if (coeff < 0.0) coeff = 0.0;
	return coeff;
}

SrQuat SBFaceShiftManager::getHeadRotation()
{
	return headRotation;
}

std::vector<std::string>& SBFaceShiftManager::getShapeNames()
{
	return shapeNames;
}

void SBFaceShiftManager::notify(SBSubject* subject)
{
	SBService::notify(subject);

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "dumpData")
		{
			for (std::map<std::string, double>::iterator iter =  coeffTable.begin();
				 iter != coeffTable.end();
				 iter++)
			{
				LOG("[%s] = %f", (*iter).first.c_str(), (*iter).second);
			}
		}
	}

}

}


#endif