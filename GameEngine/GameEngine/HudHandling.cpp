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


#include "hudHandling.h"


//buttom definitions:

bool Button::isPointInside(glm::vec2 hudPos, glm::vec2 point) const noexcept
{
	if (point.x < hudPos.x + pos.x || point.x > hudPos.x + pos.x + size.x) 
		return false; //not overlaped in the x axis
	if (point.y < hudPos.y + pos.y || point.y > hudPos.y + pos.y + size.y)
		return false; //not overlaped in the y axis

}


//#####################################################################################
//Hud definitions:

template<int NUMBER_OF_BUTTONS, int NUMBER_OF_IMAGES>
const Button* Hud<NUMBER_OF_BUTTONS, NUMBER_OF_IMAGES>::getButton(int i) const
{
	if (i < numberOfButtons && i >= 0)
		return &buttons[i];

	myAssert(false);
	return nullptr;
}


//---------------------------------------------------------------------------------


template<int NUMBER_OF_BUTTONS, int NUMBER_OF_IMAGES>
const Sprite* Hud<NUMBER_OF_BUTTONS, NUMBER_OF_IMAGES>::getImage(int i) const
{
	if (i < numberOfImages && i >= 0)
		return &images[i];
	myAssert(false);
	return nullptr;
}


//---------------------------------------------------------------------------------


template<int NUMBER_OF_BUTTONS, int NUMBER_OF_IMAGES>
glm::vec2 Hud<NUMBER_OF_BUTTONS, NUMBER_OF_IMAGES>::getSpritePos(int i) const
{
	if (i < numberOfImages && i >= 0)
		return imagesPositions[i];
	myAssert(false);
	return glm::vec2(0.0f, 0.0f);
}


//---------------------------------------------------------------------------------


template<int NUMBER_OF_BUTTONS, int NUMBER_OF_IMAGES>
FLOAT_TYPE Hud<NUMBER_OF_BUTTONS, NUMBER_OF_IMAGES>::getSpriteHorizontalPercent(int i) const
{
	if (i < numberOfImages && i >= 0)
		return horizontalPercent[i];
	myAssert(false);
	return 1.0f;
}

//Explicit template initialization:
template class Hud<0, 4>;