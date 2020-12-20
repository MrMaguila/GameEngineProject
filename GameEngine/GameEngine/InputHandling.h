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

This code is part of a self made game engine. It declares the InputHandler, that is a class 
used to update and manage all the inputComponents in a scene.
*/
//#############################################################################################
#ifndef INPUT_HANDLER
#define INPUT_HANDLER


#include <cassert>

#include "Observer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Entity.h"
#include "World.h"

#include "GlobalDefines.h"

class InputHandler : public Subject
{
public:

	static InputHandler& instance() //this class is a singleton
	{
		static InputHandler* instance = new InputHandler(); 
		return *instance;
	}

	static void keyCallBack(GLFWwindow*, int, int, int, int);
	void Initialize(World*, GLFWwindow*);
	void update();
	

private:

	InputHandler() {};
	void testKey(int);

	World* world = nullptr; //keep a ptr to the world to grant access to all the inputComponents in the scene
	GLFWwindow* window = nullptr;
};









#endif // !INPUT_HANDLER
