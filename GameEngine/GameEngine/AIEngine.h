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


This is the Ai Engine. What it basicaly does is walk through all character components each game
update and decide, depending on the charater type(e.g. ally, enemy), what should it do. All the 
AI algorithms are declared and defined in the AIAlgorithms header.
*/
//##################################################################################################

#ifndef AI_ENGINE
#define AI_ENGINE




#include "AIAlgorithms.h"
#include "GlobalDefines.h"

#include "World.h"
#include "CharacterComponent.h"
#include "Entity.h"
#include "Observer.h"



class AIEngine : public Observer, public Subject
{
public:


	void intialize(World*);
	void onNotify(Message msg) override;
	void update(FLOAT_TYPE dt);

	
private:

	int iterations = 0;
	Algorithms AiSystem;
	World* world;
};






#endif // !AI_ENGINE

