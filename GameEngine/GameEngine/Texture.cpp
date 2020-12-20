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

#include "Texture.h"



//Texture definitions:

Texture::Texture(std::string path, bool flipUVs)
{
	stbi_set_flip_vertically_on_load(flipUVs);

	std::cout << "Loading Texture: " << path << '\n';

	//generate a texture:
	glGenTextures(1, &glId);
	glBindTexture(GL_TEXTURE_2D, glId);

	//configure it:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//FLOAT_TYPE borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//get it's data from file:
	unsigned char* data = stbi_load(path.c_str(), &xSize, &ySize, &numOfChannels, 0);
	if (data)
	{
		//get the image format:
		switch (numOfChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			std::cerr << "->ERROR::CANNOT DETERMINE TEXTURE FILE FORMAT WHEN LOADING; TEXTURE ID: " << glId << ";\n";
			myAssert(false);
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, xSize, ySize, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//free image data:
		stbi_image_free(data);
	}
	else {
		stbi_image_free(data);
		std::cerr << "->ERROR::CANNOT LOAD THE TEXTURE FROM FILE; TEXTURE ID: " << glId << ";\n";
		glDeleteTextures(1, &glId);
		myAssert(false);
		throw std::logic_error("ERROR::FAILED TO LOAD TEXTURE FROM FILE IN Texture::Texture();\n");
		
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------------------------------------------

unsigned int Texture::getGlId() const noexcept
{
	return glId;
}

//-----------------------------------------------------------------------------------------------------------------

int Texture::getWidth() const noexcept
{
	return xSize;
}

//-----------------------------------------------------------------------------------------------------------------

int Texture::getHeight() const noexcept
{
	return ySize;
}

//-----------------------------------------------------------------------------------------------------------------
 
int Texture::getNumOfChannels() const noexcept
{
	return numOfChannels;
}

//-----------------------------------------------------------------------------------------------------------------

GLenum Texture::getType() const noexcept
{
	return format;
}


void Texture::clearMemory()
{
	if(glId)
		glDeleteTextures(1, &glId);
}