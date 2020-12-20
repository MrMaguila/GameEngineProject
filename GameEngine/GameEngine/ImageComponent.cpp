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

#include "ImageComponent.h"

ImageComponent::ImageComponent()
	:Component{ -1 }
{
}

//definitions:
ImageComponent::ImageComponent(Entity id)
	:Component{ id }
{
}


void ImageComponent::update() noexcept
{
	//std::cout << frameCounter << '\n';
	if (playing > 0)
	{
		//std::cout << "HAAAA " << playing << "\n";
		++frameCounter;
		if (frameCounter >= playing)
		{
			spt.currentColumn = (spt.currentColumn + 1) % spt.columns;
			frameCounter = 0;
		}
	}
}


void ImageComponent::play(int dur, int row)
{
	//if (row > 3) std::cout << "haa\n";
	if ((spt.columns < 2 && spt.rows < 2) || (spt.rows < row) ) myAssert(false); //the sprite isn't a spritesheet
	playing = dur;
	if (spt.currentRow != row) { playing = dur; frameCounter = 0; spt.currentColumn = 0; } //reset current frame if the animation was changed
	spt.currentRow = row;
}


void ImageComponent::stop(bool reset) noexcept
{
	playing = 0;
	
	//if (reset) spt.currentCollum = 0;
}