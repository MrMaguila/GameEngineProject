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

This header defines all functions used in the window management and initialition.
*/
//################################################################################################################



#ifndef WINDOW_HANDLING
#define WINDOW_HANDLING



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GlobalDefines.h"


//=================================================================================================
//Error handling:



inline void error_callback(int error, const char* description)
{
	fprintf(stderr, "ERROR: %s\n", description);
}

//=================================================================================================
//initializating functions:

inline void initializeGraphicsAPI(GLFWwindow* window)
/*set the OpenGL context and initialize glad*/
{

	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		std::cerr << "->GLFW ERROR: CONTEXT CREATION FAILED!!!\n";
	}
	glfwSwapInterval(1);
}

//===================================================================================================
//resize callback:


inline void resize_callback(GLFWwindow*, int width, int height)
{
	//keep the 16 : 9 aspect ratio:
	int offset = (-16 * height / 18) + (width / 2);
	glViewport(offset, 0, width - (2 * offset), height);
}


#endif // !WINDOW_HANDLING
