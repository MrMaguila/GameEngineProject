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

#include "InputHandling.h"



//InputHandler definitions:



void InputHandler::keyCallBack(GLFWwindow* win, int key, int scancode, int action, int mods)
{
	return;
}

//-------------------------------------------------------------------------------------------


void InputHandler::Initialize(World* w, GLFWwindow* win)
{
	myAssert(w && win);
	world = w;
	window = win;
}


//-------------------------------------------------------------------------------------------



void InputHandler::testKey(int key)
{
	if (glfwGetKey(window, key) == GLFW_PRESS)
		instance().storeMessage(Message(MessageType::KEY_PRESSED, key));
	else if (glfwGetKey(window, key) == GLFW_RELEASE)
		instance().storeMessage(Message(MessageType::KEY_RELEASED, key));
}



//-------------------------------------------------------------------------------------------

void InputHandler::update()
{

	//get keyboard keys states:
	//testKey(GLFW_KEY_UP);
	//testKey(GLFW_KEY_DOWN);
	//testKey(GLFW_KEY_LEFT);
	//testKey(GLFW_KEY_RIGHT);

	testKey(GLFW_KEY_W);
	testKey(GLFW_KEY_S);
	testKey(GLFW_KEY_A);
	testKey(GLFW_KEY_D);
	testKey(GLFW_KEY_E);

	testKey(GLFW_KEY_P);
	testKey(GLFW_KEY_SPACE);
	testKey(GLFW_KEY_LEFT_SHIFT);


	//get mouse buttoms state:
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		instance().storeMessage(Message(MessageType::MOUSE_BUTTOM_PRESSED, GLFW_MOUSE_BUTTON_LEFT));
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		instance().storeMessage(Message(MessageType::MOUSE_BUTTOM_RELEASED, GLFW_MOUSE_BUTTON_LEFT));

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		instance().storeMessage(Message(MessageType::MOUSE_BUTTOM_PRESSED, GLFW_MOUSE_BUTTON_RIGHT));
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		instance().storeMessage(Message(MessageType::MOUSE_BUTTOM_RELEASED, GLFW_MOUSE_BUTTON_RIGHT));


}


