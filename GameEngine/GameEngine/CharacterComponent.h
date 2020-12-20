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


#ifndef CHARACTER_COMPONENT
#define CHARACTER_COMPONENT


#include <cassert>
#include <iostream>
#include <utility>
#include <glm/glm.hpp>
#include "Entity.h"
#include "ModelComponent.h"

#include "GlobalDefines.h"

//########################################################################
//Helper functions, structs and Enums:



enum class CharacterType
{
	Unknown,
	Ally,
	Player,
	Enemy
};



enum class CharacterState
{
	Standing,
	Walking,
	attacking0,
	Running,
};

//########################################################################


#define MAX_NUM_OF_INTERACTABLE_OBJECTS 3
#define NUMBER_OF_ATTRIBUTES 8

class CharacterComponent : public Component
{
public:
	CharacterComponent();

	FLOAT_TYPE movingSpeed = 1.5f;
	FLOAT_TYPE runSpeedMultiplier = 1.35f;
	FLOAT_TYPE jumpForce = 70.0f;
	int jumpCooldown = 0;
	FLOAT_TYPE maxHealth = 100;
	FLOAT_TYPE health = 100;
	int maxMana = 0;
	int mana = 0;
	FLOAT_TYPE maxEnergy = 100;
	FLOAT_TYPE energy = 100;
	FLOAT_TYPE enegyCostMultiplier = 0.05f;
	FLOAT_TYPE energyRegenBoost = 0.05f;
	int xp = 0;
	int lvl = 0;
	CharacterType charType = CharacterType::Unknown;
	CharacterState charState = CharacterState::Standing;
	CharacterState charPreviouseState = CharacterState::Standing;

	bool walkUpActive = false;
	bool walkDownActive = false;
	bool walkLeftActive = false;
	bool walkRightActive = false;
	bool running = false;
	bool mustJump = false;
	int casting = 0;
	int onGround = 0;

	//Ai data:
	glm::vec2 direction = glm::vec2(0.0f, 0.0f); //x == sen(angle) and y == cos(angle)
	FLOAT_TYPE speed = 1.0f;

	//Functions:

	void setModel(const Model*);
	const Model* getModel() const;
	void updateModel();

	int getCurrentAnimation() const noexcept;
	FLOAT_TYPE getAnimationTime() const noexcept;


	int getCurrentRow() const noexcept; 
	int getCurrentColumn() const noexcept;
	void setModelPos(glm::vec3) noexcept;
	glm::vec3 getModelPos() const noexcept;
	int getDirection() const noexcept;
	void boneTransform(int animId, FLOAT_TYPE timeInSecs, std::vector<glm::mat4>& transforms);
	void readNodeHierachy(int animId, FLOAT_TYPE animTime, const NodeData& node, const glm::mat4& parentTransform);
	std::vector<glm::mat4> mBoneTransforms;


private:

	friend class GameplayHandler;
	friend class AIEngine;

	
	//model data:
	const Model* model = nullptr;
	int modelDirection = 0; //0 = south, 1 = East, 2 = North, 3 = West. This is updated each game update
	
	glm::vec3 pos = glm::vec3(0.0f);



	//model animation data:
	int currentAnimation = 0;
	FLOAT_TYPE animationTime = 0.0f;

	//texture animation data:
	int currentRow = 0;
	int currentColumn = 0;
	int playing = 0;
	int frameCounter = 0;


	
	//functions:
	void AddHealth(int) noexcept; 
	void AddMana(int) noexcept;
	void AddEnergy(int) noexcept;

	void play(int animId);
	void playTextureAnimation(int dur, int row); 
	void stop(bool reset) noexcept;

	//state setters:
	void WalkUp(bool) noexcept;
	void WalkDown(bool) noexcept;
	void WalkLeft(bool) noexcept;
	void WalkRight(bool) noexcept;
	void Jump() noexcept;
	bool Attack(int) noexcept;

	//std::pair<int, int> getAnimation() const; //return the charater spritesheet's animation based on the charater state 
	glm::vec3 getMovingVector() const noexcept;
	FLOAT_TYPE getAnimationDuration() const;

	bool IsIdle() const noexcept;

	CharacterComponent(int, int); //only the GameplayHandler class is allowed to initialize a CharacterComponent
	void update();
	
	int childrenObjects[MAX_NUM_OF_INTERACTABLE_OBJECTS];
};


#endif // !CHARACTER_COMPONENT
