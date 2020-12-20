//#############################################################################################
/*
Copyright[2020][Gabriel G. Fernandes]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissionsand
limitations under the License.

by Gabriel G. Fernandes 19/12/2020
gabrielgf6000@gmail.com
*/
//#############################################################################################

#include "NetworkHandler.h"



EndSystem::~EndSystem()
{
	if (initialized) closesocket(sock);
}


//======================================================================================================================


NetworkHandler::~NetworkHandler()
{
	if(active) WSACleanup();
}


//======================================================================================================================


void NetworkHandler::initializeWinsock()
{
	WORD versionWanted = MAKEWORD(2, 2); //version of the windows socket api

	if (WSAStartup(versionWanted, &wsaData) != 0)
	{
		std::cerr << "ERROR:FAILED TO INITIALIZE THE WINDOWS SOCKET API\n" << "\n";
		myAssert(false);
		return;
	}
	else std::cout << "successfully initialized the windows socket api\n";

	//set the NetworkHandler as active
	active = true;
}




//======================================================================================================================


void NetworkHandler::initializeSocket(std::string port)
{
	//initialize the udp socket:

	int status;
	addrinfo* info = nullptr;

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;


	if ((status = getaddrinfo(0, port.c_str(), &hints, &info)) == -1)
	{
		std::cerr << "ERROR::getaddring FUNCTION RETURNED NON ZERO VALUE; ERROR STRING: " << gai_strerror(status) << '\n';
	}

	addrinfo* thisPcInfo = nullptr; 
	//iterate over the list returned in thisPcUDP.address to find a valid adress info to be used by the socket
	for (thisPcInfo = info; thisPcInfo != nullptr; thisPcInfo = thisPcInfo->ai_next)
	{
		if (thisPcInfo->ai_family != AF_INET) continue; //accept only ipv4 addresses

		thisPcUDP.sock = socket(thisPcInfo->ai_family, thisPcInfo->ai_socktype, thisPcInfo->ai_protocol);
		status = bind(thisPcUDP.sock, thisPcInfo->ai_addr, thisPcInfo->ai_addrlen);

		if (thisPcUDP.sock == INVALID_SOCKET || status == -1)
		{
			std::cout << "!Failed to create a socket using one of the addresses in the list. Trying next address...\n";
			if (thisPcUDP.sock != INVALID_SOCKET && status == -1) closesocket(thisPcUDP.sock); //if created but failed to bind
		}
		else
		{
			//print the address
			sockaddr_in* ipv4Addr = (sockaddr_in*) thisPcInfo->ai_addr;
			char ipStr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET, &(ipv4Addr->sin_addr), ipStr, sizeof(ipStr));
			std::cout << "Socket successfully created; Ipv4: ";
			printf("%s on port: %s\n", ipStr, port.c_str());

			//save the address
			thisPcUDP.address = std::string(ipStr);

			//set the socket as On
			thisPcUDP.On = true;
			thisPcUDP.initialized = true; //this makes sure this socket is closed on destruction

			//make the socket a non blocking socket
			u_long mode = 1; //1 for use non blocking sockets
			ioctlsocket(thisPcUDP.sock, FIONBIO, &mode); 

			//and clear memory
			freeaddrinfo(info);
			 
			return;
		}

	}

	freeaddrinfo(info);

	myAssert(false);
	
}



//===================================================================================================================


void NetworkHandler::host(std::string port)
{
	//create the tcp socket

	int status;
	addrinfo* info;
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(0, port.c_str(), &hints, &info)) == -1)
	{
		std::cerr << "ERROR::getaddring FUNCTION RETURNED NON ZERO VALUE; ERROR STRING: " << gai_strerror(status) << '\n';
	}

	bool created = false;
	addrinfo* servInfo = nullptr;
	//iterate over the list returned in thisPcTCP.address and connect to the first we can
	for (servInfo = info; servInfo != nullptr; servInfo = servInfo->ai_next)
	{
		if (servInfo->ai_family != AF_INET) continue; //accept only ipv4 addresses

		thisPcTCP.sock = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol); //create the socket
		status = bind(thisPcTCP.sock, servInfo->ai_addr, servInfo->ai_addrlen);


		if (thisPcUDP.sock == INVALID_SOCKET || status == -1)
		{
			std::cout << "!Failed to create a socket using one of the addresses in the list. Trying next address...\n";
			if (thisPcUDP.sock != INVALID_SOCKET && status == -1) closesocket(thisPcTCP.sock); //if created but not bound
		}
		else
		{
			//print the address:
			sockaddr_in* ipv4Addr = (sockaddr_in*)servInfo->ai_addr;
			char ipStr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET, &(ipv4Addr->sin_addr), ipStr, sizeof(ipStr));
			std::cout << "Socket successfully created; address: ";
			printf("%s port: %s", ipStr, port.c_str());

			//save the address:
			thisPcTCP.address = std::string(ipStr);

			//set the socket as on
			thisPcTCP.On = true;
			thisPcTCP.initialized = true; //this makes sure this socket is closed on NetworkHandler's destructor

			//make the socket a non blocking socket
			u_long mode = 1;
			ioctlsocket(thisPcTCP.sock, FIONBIO, &mode);

			
			//set the hosting flag(it indicates to the update function that it must accept incoming requests)
			hosting = true;
			created = true;
		}

	}

	//clear memory
	freeaddrinfo(info);
	if(created == false) myAssert(false);
	

	//------------------------------------------------------------
	//put thisPcTCP's socket to listen for incoming connections
	int result = listen(thisPcTCP.sock, 3); //this assumes that thisPcTCP.sock is initialized and is a non blocking socket
	if (result == -1)
	{
		std::cerr << "ERROR::FAILED TO HOST THE GAME; listen() returned -1;\n";
	}
}



//===================================================================================================================


void NetworkHandler::connectTo(std::string address, std::string port)
//this function creates and connects a non blocking tcp socket to the host defined by the
//specified address and port number.
{
	int status;

	addrinfo* info = nullptr;
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	

	if ((status = getaddrinfo("127.0.0.1", port.c_str(), &hints, &info)) == -1)
	{
		std::cerr << "ERROR::getaddring FUNCTION RETURNED NON ZERO VALUE; ERROR STRING: " << gai_strerror(status) << '\n';
		myAssert(false);
	}
	myAssert(info);

	addrinfo* servInfo = nullptr;
	//iterate over the list returned in thisPcTCP.address and connect to the first we can
	for (servInfo = info; servInfo != nullptr; servInfo = servInfo->ai_next)
	{
		if (servInfo->ai_family != AF_INET) continue; //accept only ipv4 addresses
		
		thisPcTCP.sock = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol); //create the socket
		status = connect(thisPcTCP.sock, servInfo->ai_addr, servInfo->ai_addrlen); //and connect it
		//note that the previous line will block this thread for some time(and yes, for now, that was intended)

		if (thisPcUDP.sock == INVALID_SOCKET || status == -1)
		{
			std::cout << "!Failed to create a socket using one of the addresses in the list. Trying next address...\n";
			if (thisPcUDP.sock != INVALID_SOCKET && status == -1) closesocket(thisPcTCP.sock); //if created but not connected
		}
		else
		{
			//print the address:
			sockaddr_in* ipv4Addr = (sockaddr_in*)servInfo->ai_addr;
			char ipStr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET, &(ipv4Addr->sin_addr), ipStr, sizeof(ipStr));
			std::cout << "Socket successfully created; Connected to: ";
			printf("%s on port: %s", ipStr, port.c_str());

			//save the address:
			thisPcTCP.address = std::string(ipStr);

			//set the socket as on
			thisPcTCP.On = true;
			thisPcTCP.initialized = true; //this makes sure this socket is freed on NetworkHandler's destructor

			//make the socket a non blocking socket
			u_long mode = 1;
			ioctlsocket(thisPcTCP.sock, FIONBIO, &mode);

			//and clear memory
			freeaddrinfo(info);
			return;
		}

	}

	freeaddrinfo(info);
	myAssert(false);
}



//===================================================================================================================


void NetworkHandler::update()
{
	
	if (!hosting) return;
	//see if there is incoming connection requests
	for (int i = 0; i < 3; ++i) //find an unitialized host
	{
		if (hosts[i].initialized == false)
		{
			myAssert(thisPcTCP.initialized); //assumes the tcp socket is initialized and listening
			hosts[i].sock = accept(thisPcTCP.sock, nullptr, nullptr);
			if (hosts[i].sock == INVALID_SOCKET)
			{
				closesocket(hosts[i].sock);
				//std::cout << "No connection requests\n";
			}
			else
			{
				hosts[i].initialized = true;
				hosts[i].On = true;
				std::cout << "\n\n->Host Successfully connected\n\n\n";
			}
			break;
		}
	}
	
}