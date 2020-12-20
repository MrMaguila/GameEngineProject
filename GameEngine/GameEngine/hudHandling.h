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



This code is part of a self made game engine. It declares all the classes used 
to handle the player interface, e.g. the inventory energy and life bars.
*/
//###########################################################################################


#ifndef HUD_COMPONENT
#define HUD_COMPONENT


#include <cassert>

#include <glm/glm.hpp>
#include "Entity.h"
#include "Sprite.h"
#include "Texture.h"

#include "GlobalDefines.h"

//########################################################################################################
//utility classes:


struct Button
{
private:
	
	friend class GameplayHandler;


	bool isPointInside(glm::vec2 hudPos, glm::vec2 point) const noexcept; //the hud position should be passed as 
													//argumment here because it is stored in a transformComponent and
													//the buttom has no information about it's hud. The two vectors
													//are in normalized device cordinates

	glm::vec2 size = glm::vec2(1.0f, 1.0f);
	glm::vec2 pos = glm::vec2(0.0f, 0.0f); //top left corner of the buttom in ndc, relative to the position(also in ndc) of the
										//hud that this button belongs to
	Sprite spt;
};



//######################################################################################################



template<int NUMBER_OF_BUTTONS, int NUMBER_OF_IMAGES>
class Hud
{
public:
	const int numberOfButtons = NUMBER_OF_BUTTONS;
	const int numberOfImages = NUMBER_OF_IMAGES;

	const Button* getButton(int) const;
	const Sprite* getImage(int) const;
	glm::vec2 getSpritePos(int) const;
	FLOAT_TYPE getSpriteHorizontalPercent(int) const;

private:

	friend class GameplayHandler; //the GameplayHandler is responsible to initialize and update the hud data.

	Button buttons[NUMBER_OF_BUTTONS + 1]; //the last button will not be considered. It is used just to allow
										//the numberOfButtons be 0.
	Sprite images[NUMBER_OF_IMAGES];
	glm::vec2 imagesPositions[NUMBER_OF_IMAGES];//top left corner of the image in ndc, relative to the position(also in ndc) of the
										//hud that this image belongs to.
	FLOAT_TYPE horizontalPercent[NUMBER_OF_IMAGES] = { 1.0f }; //percentage of the each sprite to be draw on the x axis(this is used for 
										//things like life bars, that the sprite size must be adjusted according to a certain 
										//value). If positive, this causes the sprite to grow from left to right, otherwise it
										//grow from right to left.
};


#endif // !HUD_COMPONENT
