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

#include "Sprite.h"

//definitions:

void Sprite::initialize(const Texture* tex, int nr, int nc)
{
	if (!tex)
		throw std::logic_error("ERROR::NULL TEXTURE PASSED AS ARGUMENT OF Sprite::initialize;\n");

	setTexture(tex);


	rows = (nr == 0) ? 1 : nr;
	columns = (nc == 0) ? 1 : nc;

	width = tex->getWidth() / columns;
	height = tex->getHeight() / rows;
}



//-----------------------------------------------------------------------------------------------------


const Texture* Sprite::getTexture() const
{
	if (texture) return texture;
	
	throw std::logic_error("ERROR::Sprite::getTexture RETURNED A NULL TEXTURE;\n");
	return nullptr;
}

//-----------------------------------------------------------------------------------------------------


void Sprite::setTexture(const Texture* tex) noexcept
{
	texture = tex;
}