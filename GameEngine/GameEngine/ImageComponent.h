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


#ifndef IMAGE_COMPONENT
#define IMAGE_COMPONENT

#include <cassert>
#include "Texture.h"
#include "Entity.h"
#include "Sprite.h"

#include "GlobalDefines.h"

class ImageComponent : public Component
{
public:
	ImageComponent();

	void update() noexcept; //should be called each frame
	void play(int dur, int row); //turn on playing, set duration of each frame to dur gameloop frames, and change the current row
	void stop(bool reset) noexcept; //stop playing. if reset is true, the current collum will be set to 0

	Sprite spt;
	bool instanced = false;
private:

	friend class GraphicalSystem;

	ImageComponent(Entity);

	
	int playing = 0;
	int frameCounter = 0;
	const Texture* normalMap;
	const Texture* emissionMap;
};



#endif // !IMAGE_COMPONENT
