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

#include "TextureHandler.h"





//#############################################################################################################
//TextureHandler definitions:


TextureHandler::TextureHandler()
{
	textures.reserve(100);
}


void TextureHandler::addTexture(std::string path)
//path should be relative to the project folder
{
	Texture texture(path);
	textures.push_back(texture); 
}

const Texture* TextureHandler::get(int i)
{
	if (i < 0)
	{
		std::cout << "->WARNING::NULL TEXTURE POINTER PASSED BY TextureHandler::get();\n";
		return nullptr;
	}
	myAssert(!(i >= textures.size()));
	
	return &textures[i];
}