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

#include "GameplayHandler.h"

//####################################################
//GameplayHandler definitions:



void GameplayHandler::initialize(World* w, GLFWwindow* win)
{
	world = w;
	window = win;

	
	//----------------------------------------------------------------
	//initialize the huds:

	//initialize the player status bar
	w->playerStatusBar.images[0].initialize(TextureHandler::instance().get(9), 1, 1);
	w->playerStatusBar.imagesPositions[0] = glm::vec2(-172.0f / 240.0f, 121.0f / 135.0f);
	w->playerStatusBar.horizontalPercent[0] = 1.0f;

	w->playerStatusBar.images[1].initialize(TextureHandler::instance().get(10), 1, 1);
	w->playerStatusBar.imagesPositions[1] = glm::vec2(-172.0f / 240.0f, 106.0f / 135.0f);
	w->playerStatusBar.horizontalPercent[1] = 1.0f;

	w->playerStatusBar.images[2].initialize(TextureHandler::instance().get(11), 1, 1);
	w->playerStatusBar.imagesPositions[2] = glm::vec2(-172.0f / 240.0f, 91.0f / 135.0f);
	w->playerStatusBar.horizontalPercent[2] = 1.0f;

	w->playerStatusBar.images[3].initialize(TextureHandler::instance().get(12), 1, 1);
	w->playerStatusBar.imagesPositions[3] = glm::vec2(-170.0f / 240.0f, 110.0f / 135.0f);
	w->playerStatusBar.horizontalPercent[3] = 1.0f;


	//---------------------------------------------------------------------------------
	//load effects from file:

	loadEffects();
}


//======================================================================================================


void GameplayHandler::update()
{
	//Update the player hud data:
	myAssert(world->currentScene->getEntity(playerId));
	CharacterComponent* player = world->currentScene->getCharacterComponent(playerId);
	myAssert(player);

	world->playerStatusBar.horizontalPercent[0] = FLOAT_TYPE(player->health) / player->maxHealth;
	world->playerStatusBar.horizontalPercent[1] = FLOAT_TYPE(player->mana) / player->maxMana;
	world->playerStatusBar.horizontalPercent[2] = FLOAT_TYPE(player->energy) / player->maxEnergy;


	FLOAT_TYPE deltaTime = glfwGetTime() - timer;

	//update every character in the scene:
	for (int i = 0; i < world->currentScene->characterComponents.getSize(); ++i)
	{
		CharacterComponent* charComp = &(world->currentScene->characterComponents[i]);
		//update character internal data:
		charComp->update();
		charComp->updateModel();

		//-----------
		//Handle character movement:
		glm::vec3 resultantForce = charComp->getMovingVector(); //get the character moving direction and intensity

		if (charComp->mustJump) //if it was ordened to jump, add a vertical force:
		{ 
			resultantForce.y += charComp->jumpForce;
			charComp->mustJump = false;
			charComp->jumpCooldown = 30;
			//charComp->AddEnergy(-400);
		}

		if (charComp->running && !charComp->IsIdle() && charComp->onGround)  //if the character is running, scale the size of the resultant force
		{
			if (charComp->energy > 1)
			{
				resultantForce.x *= charComp->runSpeedMultiplier;
				resultantForce.z *= charComp->runSpeedMultiplier;
				resultantForce.y *= charComp->runSpeedMultiplier * 0.9f;
				charComp->AddEnergy(-5);
			}
		}

		if (glm::length(resultantForce) > 0.0001f) //send a message to the physics engine, saying to add a force to this object
		{
			Message msg;
			msg.type = MessageType::APPLY_FORCE;
			msg.data = charComp->getEntityId();
			msg.fdata[0] = resultantForce.x;
			msg.fdata[1] = resultantForce.y;
			msg.fdata[2] = resultantForce.z;
			storeMessage(msg);
		}

		//----------
		//now, handle character animation:
		
		charComp->updateModel();
		const Model* charModel = charComp->getModel();

		//increase the animation time by the time passed since last update
		charComp->animationTime = fmod(charComp->animationTime + deltaTime, charComp->getAnimationDuration());

		//myAssert(charModelComp);

		//std::pair<int, int> animation = charComp->getAnimation(); //a duration and a row(duration is the frequency at which the animation is played)
		//charComp->play(animation.first, animation.second);
	}

	
	//---------------------------

	handleInteractableObjects();


	timer = glfwGetTime();

	//---------------------------
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	Message mousePosMsg;
	mousePosMsg.type = MessageType::MOUSE_POSITION_NOTIFICATION;
	mousePosMsg.fdata[0] = x;
	mousePosMsg.fdata[1] = y;
	storeMessage(mousePosMsg);
}


//======================================================================================================


void GameplayHandler::setPlayer(int id)
{
	myAssert(world->currentScene->getEntity(id));
	for(int i = 0; i < world->currentScene->characterComponents.getSize(); ++i)
		if (world->currentScene->characterComponents[i].getEntityId() == id) { world->playerId = id; playerId = id; return; }

	myAssert(false);
}


//======================================================================================================

void GameplayHandler::onNotify(Message msg)
{
	if (msg.type == MessageType::KEY_PRESSED || msg.type == MessageType::KEY_RELEASED || msg.type == MessageType::KEY_HOLD_ON)
		handlePlayerInput(msg);
	
	if (msg.type == MessageType::MOUSE_BUTTOM_PRESSED || msg.type == MessageType::MOUSE_BUTTOM_RELEASED)
		handlerMouseInput(msg);

	if (msg.type == MessageType::COLLISION_OCCURRED)
	{
		CharacterComponent* playerCharComp = world->currentScene->getCharacterComponent(playerId);
		myAssert(playerCharComp);
		if (msg.fdata[0] != playerId && msg.fdata[1] != playerId) return ;

		playerCharComp->onGround = 3;
		
		//if (msg.fdata[0] == playerId)
		//{//
			
		//}
		//else playerCharComp->inAir = (msg.fdata[2] <= 0.0f) ? false : true;
		//std::cout << playerCharComp->inAir << '\n';
	} 

	
}


//======================================================================================================


glm::vec3 GameplayHandler::getCameraPos() const noexcept
{
	return world->currentScene->getTransformComponent(playerId)->getPosition() + glm::vec3(0.0f, 160.0f, -160);
}


//======================================================================================================



void GameplayHandler::createCharacterComponent(int id, int type)
{
	myAssert(world->currentScene->getEntity(id));
	world->currentScene->characterComponents.push_back(CharacterComponent(id, type));

}

//======================================================================================================


void GameplayHandler::createInteractableObjComp(int id, glm::vec3 size , EffectType eData, const Model* m)
{
	myAssert(world->currentScene->getEntity(id));
	world->currentScene->interactableObjectComponents.push_back(InteractableObjectComponent(id, size, eData, m));
}


//======================================================================================================




void GameplayHandler::drawHuds()
{

}

//======================================================================================================


void GameplayHandler::setObjectHolder(Entity id1, Entity id2)
{
	InteractableObjectComponent* intObjComp = world->currentScene->getInteractableObjectComponent(id1);
	CharacterComponent* charComp = world->currentScene->getCharacterComponent(id2);

	if (!intObjComp || !charComp)
		throw std::logic_error("ERROR::INVALID ARGUMENTS PASSED TO setObjectHolder();\n");


	intObjComp->holder = id2;
	for (int i = 0; i < MAX_NUM_OF_INTERACTABLE_OBJECTS; ++i)
		if (charComp->childrenObjects[i] < 0)
			charComp->childrenObjects[i] = id1;
}

//======================================================================================================


void GameplayHandler::handlePlayerInput(Message msg) //processes input messages
{
	myAssert(world->currentScene->getEntity(playerId));

	CharacterComponent* charComp = world->currentScene->getCharacterComponent(playerId);
	myAssert(charComp);

	if (msg.type == MessageType::KEY_PRESSED || msg.type == MessageType::KEY_RELEASED) 
	{
		bool pressed = msg.type == MessageType::KEY_PRESSED;

		switch (charComp->charState)
		{
		case CharacterState::Standing:
		{
			if (msg.type == MessageType::KEY_RELEASED) break;
			switch (msg.data)
			{
			case GLFW_KEY_UP:
			case GLFW_KEY_W:
				charComp->WalkUp(true);
				break;
			case GLFW_KEY_DOWN:
			case GLFW_KEY_S:
				charComp->WalkDown(true);
				break;
			case GLFW_KEY_LEFT:
			case GLFW_KEY_A:
				charComp->WalkLeft(true);
				break;
			case GLFW_KEY_RIGHT:
			case GLFW_KEY_D:
				charComp->WalkRight(true);
				break;
			case GLFW_KEY_SPACE:
				charComp->Jump();
				break;
			case GLFW_KEY_E:
			{
				if (!pressed) break;
				if (charComp->Attack(0))
					startEffect(playerId, EffectType::swordSwing00, charComp); //this will throw an exception if the player 
														//does not have a InteractableObjectComponent
				break;
			}
			case GLFW_KEY_P:

				loadEffects();

				break;
			default:
				break;
			}
			break;
		}
		case CharacterState::Walking:
		{
			switch (msg.data)
			{
			case GLFW_KEY_UP:
			case GLFW_KEY_W:
				charComp->WalkUp(pressed);
				break;
			case GLFW_KEY_DOWN:
			case GLFW_KEY_S:
				charComp->WalkDown(pressed);
				break;
			case GLFW_KEY_LEFT:
			case GLFW_KEY_A:
				charComp->WalkLeft(pressed);
				break;
			case GLFW_KEY_RIGHT:
			case GLFW_KEY_D:
				charComp->WalkRight(pressed);
				break;
			case GLFW_KEY_SPACE:
				if(pressed) charComp->Jump();
				break;
			case GLFW_KEY_LEFT_SHIFT:
				charComp->running = pressed;
				break;
			case GLFW_KEY_E:
			{
				if (!pressed) break;
				if (charComp->Attack(0))
					startEffect(playerId, EffectType::swordSwing00, charComp); //this will throw an exception if the player 
														//does not have a InteractableObjectComponent
				break;
			}
			default:
				break;
			}
			break;
		}
		
		default:
			break;
		}




		/*switch (msg.data)
		{
		case GLFW_KEY_UP:
			charComp->walkUpActive = msg.type == MessageType::KEY_PRESSED ? true : false;
			//if(msg.type == MessageType::KEY_PRESSED) charComp->charState = 
			//newMsg.fdata[0] = 3; //the up animation row
			//mustPlay = true;
			break;
		case GLFW_KEY_DOWN:
			charComp->walkDownActive = msg.type == MessageType::KEY_PRESSED ? true : false;
			newMsg.fdata[0] = 2;
			mustPlay = true;
			break;
		case GLFW_KEY_LEFT:
			charComp->walkLeftActive = msg.type == MessageType::KEY_PRESSED ? true : false;
			newMsg.fdata[0] = 1;
			mustPlay = true;
			break;
		case GLFW_KEY_RIGHT:
			charComp->walkRightActive = msg.type == MessageType::KEY_PRESSED ? true : false;
			newMsg.fdata[0] = 0;
			mustPlay = true;
			break;
		case GLFW_KEY_LEFT_SHIFT:
			charComp->running = msg.type == MessageType::KEY_PRESSED ? true : false;
			MessageType::DEFAULT_MESSAGE;
			break;
		case GLFW_KEY_SPACE:
			if (!(msg.type == MessageType::KEY_PRESSED)) break;
			mustJump = true;
			break;
		default:
			newMsg.type = MessageType::DEFAULT_MESSAGE;
			break;
		}*/
	}
}


//======================================================================================================


void GameplayHandler::handlerMouseInput(Message msg)
{

	if (msg.type == MessageType::MOUSE_BUTTOM_PRESSED && msg.data == GLFW_MOUSE_BUTTON_LEFT);
		
	else if (msg.type == MessageType::MOUSE_BUTTOM_RELEASED && msg.data == GLFW_MOUSE_BUTTON_LEFT);
		//glfwSetCursor(window, cursor);
}



//======================================================================================================

void GameplayHandler::handleInteractableObjects()
{

	for (int i = 0; i < world->currentScene->interactableObjectComponents.getSize(); ++i)
	{
		InteractableObjectComponent* intObjComp = &(world->currentScene->interactableObjectComponents[i]);

		//------------------------------------------------------
		//update each object
		intObjComp->update();

		//--------------------------------------------------------

		CharacterComponent* charComp = nullptr;
		glm::vec3 parentPos(0.0f, 0.0f, 0.0f);
		if (intObjComp->holder >= 0)
			charComp = world->currentScene->getCharacterComponent(intObjComp->holder);

		if (charComp)
			parentPos = world->currentScene->getTransformComponent(charComp->getEntityId())->getPosition();
		else
			parentPos = world->currentScene->getTransformComponent(intObjComp->getEntityId())->getPosition();


		for (int j = 0; j < MAX_NUM_OF_EFFECTS; ++j)
		{
			switch (intObjComp->currentEffect)
			{
			case EffectType::swordSwing00:
				swordUpdate(&swordSwing00, *intObjComp, charComp, parentPos);
				break;
			case EffectType::fireEffect01:
				fireEffectUpdate(&fireEffect01, *intObjComp, parentPos);
				break;
			case EffectType::fireEffect02:
				fireEffectUpdate(&fireEffect02, *intObjComp, parentPos);
				break;
			case EffectType::fireEffect03:
				fireEffectUpdate(&fireEffect03, *intObjComp, parentPos);
				break;
			default:


				break;
			}
		}

	}

}



//################################################################################
//#########################################################################################################


void GameplayHandler::startEffect(Entity id, EffectType effectIndex, CharacterComponent* cComp)
{
	InteractableObjectComponent* intObjComp = nullptr;

	
	if(cComp)
	{ 
		//walk through all character children objects

		for (int i = 0; i < MAX_NUM_OF_INTERACTABLE_OBJECTS; ++i)
		{
			if (cComp->childrenObjects[i] >= 0) //see if it is a valid entity id
			{
				intObjComp = world->currentScene->getInteractableObjectComponent(cComp->childrenObjects[i]);
			}
			else
				continue;

			myAssert(intObjComp);

			glm::vec3 pPos(0.0f, 0.0f, 0.0f);
			pPos = world->currentScene->getTransformComponent(cComp->getEntityId())->getPosition();


			//set the current effect according to the value of type
			intObjComp->currentEffect = effectIndex;

			switch (effectIndex)
			{
			case EffectType::swordSwing00:
				if (!cComp)
					throw std::logic_error("ERROR::NULL POINTER TO CHARACTER COMPONENT PASSED AS ARGUMENT TO startEffect();\n");

				swordStart(&swordSwing00, *intObjComp, cComp, pPos);
				intObjComp->currentEffect = EffectType::swordSwing00;
				break;

			case EffectType::swordSwing01:

				if (!cComp)
					throw std::logic_error("ERROR::NULL POINTER TO CHARACTER COMPONENT PASSED AS ARGUMENT TO startEffect();\n");

				swordStart(&swordSwing01, *intObjComp, cComp, pPos);
				intObjComp->currentEffect = EffectType::swordSwing01;
				break;

			case EffectType::swordSwing02:
				if (!cComp)
					throw std::logic_error("ERROR::NULL POINTER TO CHARACTER COMPONENT PASSED AS ARGUMENT TO startEffect();\n");

				swordStart(&swordSwing02, *intObjComp, cComp, pPos);
				intObjComp->currentEffect = EffectType::swordSwing02;
				break;
			default:
				throw std::logic_error("ERROR::INVALID PARAMETERS PASSED TO GameplayHandler::swordStart();\n");
				break;
			}
		}
	}

	else //if the object ins't holded by a character
	{
		intObjComp = world->currentScene->getInteractableObjectComponent(id);

		//set the current effect according to the value of type
		intObjComp->currentEffect = effectIndex;

		glm::vec3 pPos = world->currentScene->getTransformComponent(intObjComp->getEntityId())->getPosition();

		switch (effectIndex)
		{
		case EffectType::fireEffect01:
			fireEffectStart(&fireEffect01, *intObjComp, pPos);
			intObjComp->currentEffect = EffectType::fireEffect01;
			break;

		case EffectType::fireEffect02:
			fireEffectStart(&fireEffect02, *intObjComp, pPos);
			intObjComp->currentEffect = EffectType::fireEffect02;
			break;

		case EffectType::fireEffect03:
			fireEffectStart(&fireEffect03, *intObjComp, pPos);
			intObjComp->currentEffect = EffectType::fireEffect03;
			break;

		default:
			throw std::logic_error("ERROR::INVALID PARAMETERS PASSED TO GameplayHandler::swordStart();\n");
			break;
		}


	}


}


//#########################################################################################################

void GameplayHandler::loadEffects()
{
	std::cout << "->Loading Effects...\n";

	swordSwing00.loadFromFile("Assets/Effects/swordSwing01.effect");
	swordSwing01.loadFromFile("Assets/Effects/swordSwing01.effect");
	swordSwing02.loadFromFile("Assets/Effects/swordSwing01.effect");

	fireEffect01.loadFromFile("Assets/Effects/fireEffect01.effect");
	fireEffect02.loadFromFile("Assets/Effects/fireEffect02.effect");
	fireEffect03.loadFromFile("Assets/Effects/fireEffect03.effect");

}

//#########################################################################################################



glm::quat GameplayHandler::makeQuaternion(FLOAT_TYPE xRot, FLOAT_TYPE yRot, FLOAT_TYPE zRot)
{
	return
		glm::quat(glm::cos(glm::radians(xRot) / 2.0f),
			glm::sin(glm::radians(xRot) / 2.0f) * glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::quat(glm::cos(glm::radians(yRot) / 2.0f),
			glm::sin(glm::radians(yRot) / 2.0f) * glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::quat(glm::cos(glm::radians(zRot) / 2.0f),
			glm::sin(glm::radians(zRot) / 2.0f) * glm::vec3(0.0f, 0.0f, 1.0f));
}



//#########################################################################################################
//WeaponSwingEffect definitions:


/*
const GameplayHandler::KeyFrame GameplayHandler::WeaponSwingEffect::keyFrame1 =
{ glm::vec3(-10, 32, 12), GameplayHandler::makeQuaternion(15, 90, 0), glm::vec3(1.0f, 1.0f, 1.0f) };

const GameplayHandler::KeyFrame GameplayHandler::WeaponSwingEffect::keyFrame2 =
{ glm::vec3(-40, 16, -12), GameplayHandler::makeQuaternion(15, 120, -60), glm::vec3(1.0f, 1.0f, 1.0f) };

const GameplayHandler::KeyFrame GameplayHandler::WeaponSwingEffect::keyFrame21 =


const GameplayHandler::KeyFrame GameplayHandler::WeaponSwingEffect::keyFrame3 =
{ glm::vec3(28, -4, 12), GameplayHandler::makeQuaternion(100, -60, 0), glm::vec3(1.0f, 1.0f, 1.0f) };

*/




//####################################################################################################


void GameplayHandler::Effect::loadFromFile(std::string path)
{
	std::ifstream reader(path);
	std::string kfWord;
	std::string line;
	if (!reader.is_open()) //try to open the file
		throw std::logic_error("ERROR::FAILED TO LOAD EFFECT FROM FILE;\n");


	std::cout << "\n\n-------Reading Effect File: \n";

	//read the effect speed:
	reader >> effectSpeed;
	//std::cout << effectSpeed << "\n\t";
	for (int i = 0; i < NUMBER_OF_ATTRIBUTES; ++i)
	{
		reader >> effectSpeedScalings[i];
		std::cout << effectSpeedScalings[i] << ' ';
	}

	std::cout << '\n';

	//read the health effect:
	reader >> healthEffect;
	//std::cout << healthEffect << "\n\t";
	for (int i = 0; i < NUMBER_OF_ATTRIBUTES; ++i)
	{
		reader >> healthScalings[i];
		std::cout << healthScalings[i] << ' ';
	}

	std::cout << '\n';

	//read the mana effect:
	reader >> manaEffect;
	//std::cout << manaEffect << "\n\t";
	for (int i = 0; i < NUMBER_OF_ATTRIBUTES; ++i)
	{
		reader >> manaScalings[i];
		std::cout << manaScalings[i] << ' ';
	}

	std::cout << '\n';

	//read the move speed effect:
	reader >> moveSpeedEffect;
	//std::cout << moveSpeedEffect << "\n\t";
	for (int i = 0; i < NUMBER_OF_ATTRIBUTES; ++i)
	{
		reader >> moveSpeedScalings[i];
		std::cout << moveSpeedScalings[i] << ' ';
	}

	std::cout << '\n';

	//read the effect duration:
	reader >> totalDuration;
	//std::cout << totalDuration << '\n';


	//reset the keyFrames vector
	keyFrames.resize(0);

	//now read the keyframes
	while (std::getline(reader, line))
	{
		std::istringstream lineReader(line);

		if (line.size() < 2)
			continue;
		/*
		//test if this is a keyframe line(they start with "kf") 
		if (!line.find("kf"))
			continue;
			*/
		char ch, ch2;
		lineReader >> ch >> ch2; //read the "kf"
 
		std::cout << "\nKeyframe: \n";
		keyFrames.push_back(KeyFrame()); //create a new keyframe
		int i = keyFrames.size() - 1;

		//read translations:
		lineReader >> keyFrames[i].translation.x;
		lineReader >> keyFrames[i].translation.y;
		lineReader >> keyFrames[i].translation.z;

		std::cout << keyFrames[i].translation.x << " ";
		std::cout << keyFrames[i].translation.y << " ";
		std::cout << keyFrames[i].translation.z << "\t";

		//read rotatations:
		glm::vec3 eulerAngles;
		lineReader >> eulerAngles.x;
		lineReader >> eulerAngles.y;
		lineReader >> eulerAngles.z;
		keyFrames[i].rotation = makeQuaternion(eulerAngles.x, eulerAngles.y, eulerAngles.z);

		std::cout << eulerAngles.x << " ";
		std::cout << eulerAngles.y << " ";
		std::cout << eulerAngles.z << "\t";

		//read scalings:
		lineReader >> keyFrames[i].scaling.x;
		lineReader >> keyFrames[i].scaling.y;
		lineReader >> keyFrames[i].scaling.z;

		std::cout << keyFrames[i].scaling.x << " ";
		std::cout << keyFrames[i].scaling.y << " ";
		std::cout << keyFrames[i].scaling.z << "\t";

		//read the keyframe time:
		lineReader >> keyFrames[i].kfTime;
		std::cout << keyFrames[i].kfTime << '\n';
	}


	reader.close();
}



//=======================================================================================================




FLOAT_TYPE GameplayHandler::Effect::getTotalDuration() const noexcept
{
	return totalDuration;
}


//=======================================================================================================


FLOAT_TYPE GameplayHandler::Effect::getEffectSpeed() const noexcept
{

	return effectSpeed;
}


//=======================================================================================================

const FLOAT_TYPE* GameplayHandler::Effect::getSpeedScalings() const noexcept
{
	return &(effectSpeedScalings[0]);
}


//=======================================================================================================

FLOAT_TYPE GameplayHandler::Effect::getHealthEffect() const noexcept
{
	return healthEffect;
}


//=======================================================================================================


const FLOAT_TYPE* GameplayHandler::Effect::getHealthScalings() const noexcept
{
	return &(healthScalings[0]);
}


//=======================================================================================================


FLOAT_TYPE GameplayHandler::Effect::getManaEffect() const noexcept
{
	return manaEffect;
}


//=======================================================================================================


const FLOAT_TYPE* GameplayHandler::Effect::getManaScalings() const noexcept
{
	return &(manaScalings[0]);
}


//=======================================================================================================



FLOAT_TYPE GameplayHandler::Effect::getEnergyEffect() const noexcept
{
	return energyEffect;
}


//=======================================================================================================


const FLOAT_TYPE* GameplayHandler::Effect::getEnergyScalings() const noexcept
{
	return &(energyScalings[0]);
}


//=======================================================================================================


int GameplayHandler::Effect::getNumOfKeyFrames() const noexcept
{
	return keyFrames.size();
}


//=======================================================================================================


const GameplayHandler::KeyFrame& GameplayHandler::Effect::getKeyframe(int i) const
{
	if (i < 0 || i >= keyFrames.size())
		throw std::logic_error("ERROR::INVALID ARGUMENT PASSED TO GameplayHandler::EffectFunction::"
			"getKeyFrame();\n");

	return keyFrames[i];
}



//####################################################################################################



void GameplayHandler::swordUpdate(const GameplayHandler::Effect* effect,
									InteractableObjectComponent& intObjComp,
									CharacterComponent* cComp,
									glm::vec3 parentPos)
{

	if (!intObjComp.isEffectActive || !intObjComp.actived) //don't update if the effect or object is disabled
		return;

	//increment the current state by the effect speed
	if(intObjComp.playing == true)
		intObjComp.currentState += effect->getEffectSpeed();

	//see if the effect must end
	if (intObjComp.currentState >= effect->getTotalDuration())
	{
		intObjComp.currentState = 0.0f;
		intObjComp.playing = false;
		intObjComp.isEffectActive = false;
		return;
	}

	//----------------------------------------------------------------------
	const GameplayHandler::KeyFrame* kFrame1 = nullptr;
	const GameplayHandler::KeyFrame* kFrame2 = nullptr;

	myAssert(effect->getNumOfKeyFrames() > 1);

	//set the two keyframes according to the current state
	for (int i = 0; i < effect->getNumOfKeyFrames() - 1; ++i)
	{
		if (effect->getKeyframe(i + 1).kfTime > intObjComp.currentState) 
		{
			kFrame1 = &effect->getKeyframe(i); 
			kFrame2 = &effect->getKeyframe(i + 1); 

			break;
		}
	}
	myAssert(kFrame1 && kFrame2);

	FLOAT_TYPE delta = kFrame2->kfTime - kFrame1->kfTime;
	FLOAT_TYPE factor = (intObjComp.currentState - kFrame1->kfTime) / delta;

	myAssert(factor >= 0.0f && factor <= 1.0f);

	//set interpolated translation:
	glm::vec3 deltaTranslation = kFrame2->translation - kFrame1->translation;
	glm::vec3 translation = kFrame1->translation + (deltaTranslation * factor);

	//set interpolated rotation:
	glm::quat rotation = glm::slerp(kFrame1->rotation, kFrame2->rotation, factor);

	//set interpolated scaling:
	glm::vec3 deltaScaling = kFrame2->scaling - kFrame1->scaling;
	glm::vec3 scaling = kFrame1->scaling + (deltaScaling * factor);

	//---------------------------
	bool flip = false;
	FLOAT_TYPE directionRotation = 0.0f;
	if (cComp)
	{
		directionRotation = 180.0f + (cComp->modelDirection * 90.0f);
	}
	else
		myAssert(false);

	//set the transformation
	//intObjComp.transform = 
		//glm::mat3(glm::vec3(scaling.x, 0.0f, 0.0f), glm::vec3(0.0f, scaling.y, 0.0f), glm::vec3(0.0f, 0.0f, scaling.z)) *
		
	glm::mat4 rotMatrix = glm::mat3(rotation);
	rotMatrix[3].w = 1.0f;

	intObjComp.transform = glm::mat4(1.0f);
	intObjComp.transform = glm::translate(intObjComp.transform, parentPos);
	if (flip)
		intObjComp.transform = glm::scale(intObjComp.transform, glm::vec3(-1.0f, 1.0f, 1.0f));
	intObjComp.transform = glm::rotate(intObjComp.transform, glm::radians(directionRotation), glm::vec3(0.0f, 1.0f, 0.0f));
	intObjComp.transform = glm::translate(intObjComp.transform, translation);
	intObjComp.transform = intObjComp.transform * glm::toMat4(rotation);
	intObjComp.transform = glm::scale(intObjComp.transform, scaling);
	
	//intObjComp.transform[3] += glm::vec4(parentPos, 0.0f);
	intObjComp.hitBox.pos = intObjComp.transform[3];
	//intObjComp.transform[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}


//======================================================================================================


void GameplayHandler::swordProcess(const GameplayHandler::Effect* effect,
									InteractableObjectComponent& intObjComp,
									CharacterComponent* cComp,
									glm::vec3 parentPos)
{



}


//======================================================================================================

void GameplayHandler::swordStart(const GameplayHandler::Effect* effect,
									InteractableObjectComponent& intObjComp,
									CharacterComponent* cComp,
									glm::vec3 parentPos)
{
	intObjComp.isEffectActive = true;
	intObjComp.playing = true;
	intObjComp.currentState = 0.0f;

	

	intObjComp.hitBox.pos = parentPos + effect->getKeyframe(0).translation;
	intObjComp.transform = glm::mat3(glm::vec3(effect->getKeyframe(0).scaling.x, 0.0f, 0.0f),
		glm::vec3(0.0f, effect->getKeyframe(0).scaling.y, 0.0f),
		glm::vec3(0.0f, 0.0f, effect->getKeyframe(0).scaling.z)) *
		glm::mat3(effect->getKeyframe(0).rotation);
	//intObjComp.hitBox.pos = parentPos + translation;
	intObjComp.transform[3] = glm::vec4(intObjComp.hitBox.pos, 1.0f);

}


//#####################################################################################################



void GameplayHandler::fireEffectUpdate(const GameplayHandler::Effect* effect , 
										InteractableObjectComponent& intObjComp,
										glm::vec3 parentPos)

{

	if (!intObjComp.isEffectActive || !intObjComp.actived) //don't update if the effect or object is disabled
		return;

	//increment the current state by the effect speed
	if (intObjComp.playing == true)
		intObjComp.currentState += effect->getEffectSpeed();

	//see if the effect must end
	if (intObjComp.currentState >= effect->getTotalDuration())
	{
		intObjComp.currentState = 0.0f;
		intObjComp.playing = false;
		intObjComp.isEffectActive = false;
		return;
	}

	//----------------------------------------------------------
	intObjComp.hitBox.pos = parentPos;
	intObjComp.transform = glm::mat4(1.0);
	intObjComp.transform[3] = glm::vec4(intObjComp.hitBox.pos, 1.0f);

	//----------------------------------------------------------

	world->currentScene->particleSystem.generateParticles(2, FIRE_PARTICLE, 
		glm::vec3(6, 3, 6), glm::vec3(9, 2, 9), 0.4f);
}



//======================================================================================================



void GameplayHandler::fireEffectProcess(const GameplayHandler::Effect* effect, 
										InteractableObjectComponent& intObjComp,
										CharacterComponent& charComp)
{
	



}



//======================================================================================================




void GameplayHandler::fireEffectStart(const GameplayHandler::Effect* effect,
										InteractableObjectComponent& intObjComp,
										glm::vec3 parentPos)
{
	intObjComp.isEffectActive = true;
	intObjComp.playing = true;
	intObjComp.currentState = 0.0f;

	intObjComp.hitBox.pos = parentPos;
	intObjComp.transform = glm::mat4(1.0);
	intObjComp.transform[3] = glm::vec4(intObjComp.hitBox.pos, 1.0f);

}