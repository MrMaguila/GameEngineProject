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


This code is part of a self made game engine. it declares the gameplayHandler class. It is responsible for managing all gameplay related
components in the game, for giving responses for input messages and also control game events
*/
//##################################################################################################


#ifndef GAMEPLAY_HANDLER
#define GAMEPLAY_HANDLER

#include <cassert>
#include <vector>
#include <string>
#include <sstream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"


#include "stb_image.h"
#include "World.h"
#include "Entity.h"
#include "Observer.h"
#include "Sprite.h"
#include "CharacterComponent.h"
#include "InteractableObjectComponent.h"
#include "TextureHandler.h"
#include "ModelComponent.h"

#include "GlobalDefines.h"

class GameplayHandler : public Observer, public Subject
{
public:

	/*
		initialize - initialize the whole class, also loading save data and effects from file.
		params: a pointer to the world that contain the scenes(this world will be filled with save data)
				a pointer to a window
		//note: will throw if the params are invalid or if the save data cannot be loaded from file
	*/
	void initialize(World*, GLFWwindow*);

	/* update - updates every gameplay related component in the current scene */
	void update();

	void setPlayer(int);

	/* onNotify - this is used for the Observer patter */
	void onNotify(Message) override;

	glm::vec3 getCameraPos() const noexcept;

	/* createCharacterComponent - create and push a character component to the current scene */
	void createCharacterComponent(int, int);

	/* createInteractableObjComp - create and push a interactable object component to the current scene */
	void createInteractableObjComp(int, glm::vec3, EffectType effectType, const Model*);


	void drawHuds();

	/*
		setObjectHolder - this function makes id2 hold id1. for now, it assumes that 
		id1 contain a interactable object component and that id2 contain a character component, it 
		will throw if this is not true
	*/
	void setObjectHolder(Entity id1, Entity id2); 
	void startEffect(Entity, EffectType, CharacterComponent*);
private:

	//Data:
	int playerId; //id of the game object that represents the player
	World* world;
	GLFWwindow* window;

	Sprite cursor;

	double timer = 0.0f;

	//Private functions:
	void handlePlayerInput(Message);
	void handlerMouseInput(Message);
	void handleInteractableObjects();

	void loadEffects();

	//================================================


	
	static glm::quat makeQuaternion(FLOAT_TYPE xRot, FLOAT_TYPE yRot, FLOAT_TYPE zRot);

	struct KeyFrame
	{
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scaling;

		FLOAT_TYPE kfTime = 0.0f;
	};

	/*
		EffectFunction class - used to store effects data loaded from file, note that no 
		member of this class can be changed during runtime without using the loadFromFile method
	*/
	class Effect
	{
	public:
		
		void loadFromFile(std::string);


		FLOAT_TYPE getTotalDuration() const noexcept;

		//access functions:
		FLOAT_TYPE getEffectSpeed() const noexcept;
		const FLOAT_TYPE* getSpeedScalings() const noexcept;

		FLOAT_TYPE getHealthEffect() const noexcept;
		const FLOAT_TYPE* getHealthScalings() const noexcept;

		FLOAT_TYPE getManaEffect() const noexcept;
		const FLOAT_TYPE* getManaScalings() const noexcept;

		FLOAT_TYPE getEnergyEffect() const noexcept;
		const FLOAT_TYPE* getEnergyScalings() const noexcept;

		int getNumOfKeyFrames() const noexcept;
		const KeyFrame& getKeyframe(int) const; //this may throw exceptions

	private:

		//private data:
		FLOAT_TYPE totalDuration = 2.0f; //the maximum value for currentState

		FLOAT_TYPE healthEffect = 0.0f; //effects that change the life will use this as its intensity(e.g healing skills)
		FLOAT_TYPE healthScalings[NUMBER_OF_ATTRIBUTES]; //each element of this array represents how the health effect
															//is increased or decreased by every character attribute

		FLOAT_TYPE effectSpeed = 0.01f; //the amount the currentState is increased each game update
		FLOAT_TYPE effectSpeedScalings[NUMBER_OF_ATTRIBUTES]; 
		
		FLOAT_TYPE manaEffect = 0.0f;
		FLOAT_TYPE manaScalings[NUMBER_OF_ATTRIBUTES];

		FLOAT_TYPE energyEffect = 0.0f;
		FLOAT_TYPE energyScalings[NUMBER_OF_ATTRIBUTES];

		FLOAT_TYPE moveSpeedEffect = 0.0f;
		FLOAT_TYPE moveSpeedScalings[NUMBER_OF_ATTRIBUTES];


		std::vector<KeyFrame> keyFrames;
	};

	Effect swordSwing00;
	Effect swordSwing01;
	Effect swordSwing02;
	Effect fireEffect01;
	Effect fireEffect02;
	Effect fireEffect03;

	//=========================================================
	//Effects declarations:

	/*effectName - Update - update a interactable object component's effect related data
							params: 
								the effect, 
								the object, 
								the caster of the object(see InteractableObjectComponent definition for 
									more info about caster and holder),
								the holder position
				- Process - this is called when a game system notifies an event that involves an interactable object,
							its params are the same of the update.
				- Start - this is called when a character or other component starts an effect
	*/
	void swordUpdate(const Effect*, InteractableObjectComponent&, CharacterComponent*, glm::vec3);
	void swordProcess(const Effect*, InteractableObjectComponent&, CharacterComponent*, glm::vec3);
	void swordStart(const Effect*, InteractableObjectComponent&, CharacterComponent*, glm::vec3);


	void fireEffectUpdate(const Effect*, InteractableObjectComponent&, glm::vec3);
	void fireEffectProcess(const Effect*, InteractableObjectComponent&, CharacterComponent&);
	void fireEffectStart(const Effect*, InteractableObjectComponent&, glm::vec3);
};

#endif // !GAMEPLAY_HANDLER
