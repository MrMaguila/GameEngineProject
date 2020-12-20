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


This header declares the Game class, that is responsible for initializing
the game and coupling all the engines. 

*/
//##########################################################################
#ifndef GAME_HEADER
#define GAME_HEADER


#define NOMINMAX

#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>


#define _WINSOCKAPI_

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "window.h"

#include "TextureHandler.h"
#include "ModelHandler.h"
#include "TransformComponent.h"
#include "World.h"
#include "GraphicalSystem.h"
#include "PhysicsEngine.h"
#include "InputHandling.h"
#include "GameplayHandler.h"
#include "AIEngine.h"
#include "NetworkHandler.h"

#include "GlobalDefines.h"

#define STARTING_WINDOW_WIDTH 1024
#define STARTING_WINDOW_HEIGHT 576

class Game
{
public:

	void initializeWindow();
	void initializeGame();
	void gameLoop();
	void handleMultiplayer(); //ask the player if it wants to play offline, open to lan or
										//connect to the world of another player and then initializes the networkHandler 
										//according to it's answer

	World world;
	
private:
	GLFWwindow* window; 
	bool running = true;

	//Game systems:
	GraphicalSystem graphicsEngine;
	PhysicsEngine physicsEngine;
	GameplayHandler gameplayHandler;
	AIEngine aiEngine;
	NetworkHandler networkHandler;

	//Private funcions:
	
};



#endif // !GAME_HEADER
