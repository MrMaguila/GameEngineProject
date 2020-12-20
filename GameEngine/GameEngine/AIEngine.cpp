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


#include "AIEngine.h"


void AIEngine::intialize(World* w) 
{
	world = w;
}

//===================================================================================================

void AIEngine::onNotify(Message msg)
{

}

//===================================================================================================

void AIEngine::update(FLOAT_TYPE dt)
{
	if (iterations > 0) { --iterations; return; };
	iterations = 15;

	CharacterComponent* player = world->currentScene->getCharacterComponent(world->playerId);
	RigidBodyComponent<Box>* playerPhysicalComp = world->currentScene->getBoxRigidBodyComponent(world->playerId);

	//update each character component in the scene(only the ones controlled by AI)
	for (int i = 0; i < world->currentScene->characterComponents.getSize(); ++i)
	{
		CharacterComponent* charComp = &(world->currentScene->characterComponents[i]);
		if (charComp->charType == CharacterType::Player) continue;

		//resultant moving vector(this will be sent to the physics engine to move the character)
		glm::vec3 result = 
			AiSystem.seek(*charComp, playerPhysicalComp->linearVelocity,
				world->currentScene->getTransformComponent(charComp->getEntityId())->getPosition(),
				playerPhysicalComp->getPosition(), 40, 85);//binomialWandering(*iter);; //movement to be applied to the character:
		FLOAT_TYPE len = glm::length(result);
		charComp->speed = len;

		if(len > 0.01f) result = glm::normalize(result);
		charComp->direction.x = result.x;
		charComp->direction.y = result.z;
	
		switch (charComp->charState)
		{
		case CharacterState::Standing:
		{
			if (len < 0.01f) break;
			(result.x > 0.1f) ? charComp->WalkLeft(true) : charComp->WalkRight(true);
			(result.z > 0.1f) ? charComp->WalkUp(true) : charComp->WalkDown(true);
		}
		case CharacterState::Walking:
		{
			charComp->WalkUp(result.z > 0.1f);
			charComp->WalkDown(result.z < -0.1f);
			charComp->WalkLeft(result.x > 0.1f);
			charComp->WalkRight(result.x < -0.1f);
		}
		default:
			break;
		}
		
		//iter->WalkLeft(result.x > 0.01f);//, iter->WalkRight(false)) : (iter->WalkRight(true), iter->WalkLeft(false));
		//iter->WalkRight(result.x < 0.01f);

		//iter->WalkUp(result.z > 0.01f);//, iter->WalkDown(false)) : (iter->WalkDown(true), iter->WalkUp(false));
		//iter->WalkDown(result.z < 0.01f);
		//else { iter->WalkDown(false);  iter->WalkUp(false); }
	}
}