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

This code is part of a self made game engine. For now, the game engine assumes the os is Microsoft Windows.
This header declares the NetworkHandler class, which is responsible for abstracting all
socket comunication between the game in different computers.
*/
//##################################################################################################


#ifndef NETWORK_HANDLER
#define NETWORK_HANDLER


#include <cassert>
#include "Observer.h"


#include <iostream>
#include <string>
#include "Entity.h"
#include "World.h"

//#ifndef NOMINMAX
//#define NOMINMAX //prevent the creation of some macros that may cause errors
//#endif //!NOMINMAX
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "GlobalDefines.h"

//#include <limits>
#include <WinSock2.h>
#include <WS2tcpip.h>







struct EndSystem //hold the address of some endSystem and the socket used to comunicate with it
{
	~EndSystem();

	std::string address = ""; 
	SOCKET sock;
	bool On = false;
	bool initialized = false;
};



class NetworkHandler// : public Subject
{
public:
	
	~NetworkHandler();


	//Functions:
	//void initialize(World*); //initializes the data needed to start comunication with other players
	void initializeWinsock();
	void initializeSocket(std::string port); //initializes the windows socket api and the udp socket
	void host(std::string); //allow other players to join this game(it basically initializes thisPcTCP's socket, set it as non blocking 
					//socket, and put it to listen for incoming connection requests)
	void connectTo(std::string address, std::string port); //join the game of another player

	void update(); //send and receive data from all hosts


private:

	World* world;

	bool hosting = false;
	bool active = false;
	
	WSAData wsaData;
	EndSystem thisPcUDP; //contains a socket that will be used to send udp packets to all other hosts.
	EndSystem thisPcTCP; //contains a tcp socket used to connect to the game host(it is used only when the player
							//connects to the world of another player).
	EndSystem hosts[3]; //these are used to hold a tcp conection to other players(note that they are only used
							//when the player is hosting the game, and that the max number of players is 4, including the 
							//one who is hosting the game).
};





#endif