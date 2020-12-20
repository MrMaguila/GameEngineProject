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


This header is part of a self made game engine. It declares the
TextureHandler class, that is used to load, store and manage all textures in the game.
*/
//#################################################################################

#ifndef TEXTURE_MANAGER
#define TEXTURE_MANAGER



#include <cassert>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>

#include "stb_image.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Texture.h"

#include "GlobalDefines.h"

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

//===============================================================================================
//The TextureHandler class(it loads and stores all textures): 

class TextureHandler
{
public:
	static TextureHandler& instance()
	{
		static TextureHandler instance = TextureHandler();
		return *(&instance);
	}

	~TextureHandler()
	{
		for (int i = 0; i < textures.size(); ++i) //clear textures from the GPU memory
		{
			textures[i].clearMemory();
		}
	}

	void addTexture(std::string); //construct and store a texture
	const Texture* get(int i);  //returns textures[i]; note that i < 0 will give a nullptr

private:
	TextureHandler(); //this class is a singleton
	std::vector<Texture> textures;
	
};




#endif // !TEXTURE_MANAGER