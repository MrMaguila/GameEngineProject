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
Texture class. It does not store a itself, they are put on the gpu when the
non-default constructor is used. The memory management is done by the TextureHandler.
This allows textures to be copied
*/
//#################################################################################

#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER



#include <cassert>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include "stb_image.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "GlobalDefines.h"

//=========================================================================================
//the Texture class:

class Texture
{
public:
	Texture(std::string path, bool flipUVs = true); //note: this function may throw exceptions and allocate GPU memory
	Texture() {};
	
	//note that the Texture::~Texture() does not clear the memory allocated by the constructor, this is 
	//assumed to be done by the class holding the Texture by calling clearMemory()

	//Functions:
	unsigned int getGlId() const noexcept;
	int getWidth() const noexcept;
	int getHeight() const noexcept;
	int getNumOfChannels() const noexcept;
	GLenum getType() const noexcept;
	void clearMemory();

private:

	//Data:
	int xSize = 0;
	int ySize = 0;
	int numOfChannels = 0;
	GLenum format;
	unsigned int glId = 0;

};




#endif // !TEXTURE_HEADER
