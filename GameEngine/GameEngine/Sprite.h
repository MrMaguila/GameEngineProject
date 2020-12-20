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

#ifndef SPRITE_IMPLEMENTATION
#define SPRITE_IMPLEMENTATION


#include <cassert>
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GlobalDefines.h"

/*
	the Sprite is a class used for storing data to be draw,
	it is build to work like a sprite sheet so that animations can be
	made with just one Sprite. They are stored in components with graphical
	propertities and are drawn on the screen by the DrawingSystem.
	In an Sprite sheet, each row represents an animation, and each column
	represents a different frame. When constructed by the GraphicalSystem, the widht and height are set
	according to the number of rows and columns, so if the sprite is not
	meant to be an sprite sheet, just let the rows and columns arguments at 0 or 1.

*/
class Sprite
	
{
public:

	//Functions:
	void initialize(const Texture* tex, int nr, int nc);
	const Texture* getTexture() const;

	/*
		setTexture - set the sprite texture. Note that a NULL texture can be set
	*/
	void setTexture(const Texture*) noexcept;

	//Public Data:
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	int width = 0;
	int height = 0;
	
	int rows = 0;
	int columns = 0;

	int currentRow = 0;
	int currentColumn = 0;
	//note: The drawing data, like vertex arrays are stored by the GraphicalSystem
private:
	const Texture* texture = nullptr;
};



#endif // !SPRITE_IMPLEMENTATION
