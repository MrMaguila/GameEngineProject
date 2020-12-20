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


This code is part of a self made game engine. it declares the InteractableObjectComponent class. It is 
responsible of representing all kinds of stuff that can be interacted by any characterComponent in the 
game.
20/07/2020
*/
//##################################################################################################




#ifndef INTERACTABLE_OBJECT_COMPONENT
#define INTERACTABLE_OBJECT_COMPONENT

#include <cassert>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Entity.h"
#include "PhysicalComponents.h"
#include "Sprite.h"
#include "Texture.h"
#include "ModelComponent.h"

#include "GlobalDefines.h"

#define MAX_NUM_OF_EFFECTS 3

//####################################################################################################

enum class EffectType
{
	nothing = 0,
	swordSwing00 = 1,
	swordSwing01 = 2,
	swordSwing02 = 3,
	fireEffect01 = 4,
	fireEffect02 = 5,
	fireEffect03 = 6

};


//###############################################################################################

class InteractableObjectComponent : public Component
{
public:

	InteractableObjectComponent();

	/*
		constructor - params: entity id, 
						size of the hitbox, 
						effect type and
						a ptr to a model
	*/
	InteractableObjectComponent(Entity id, glm::vec3 hitBoxSize, EffectType type, const Model*);

	/* update - update animations */
	void update() noexcept; 

	
	void setCurrentEffect(EffectType) noexcept;

	
	void setModel(const Model*) noexcept; //note: a nullptr model can be passed
	void setPosition(glm::vec3) noexcept;
	glm::vec3 getPosition() const noexcept;


private:

	friend class GameplayHandler;
	friend class PhysicsEngine;
	friend class GraphicalSystem;

	glm::mat4 transform = glm::mat4(1.0);
	Box hitBox;


	Entity holder = -1; //id of the character component that this effect belongs to. It must be negative
					//if it does not belongs to anyone. This makes the hitBox position be relative to the 
					//character position

	Entity caster = -1; //id of the character who started the effect(it is not necessary the holder)
	
	//model data:
	glm::vec3 pos = glm::vec3(0.0f); //this is used to keep the model from some distance of its Entity center
	const Model* model = nullptr;

	//model animation data:
	int currentAnimation = 0;
	FLOAT_TYPE animationTime = 0.0f;

	//texture animation data:
	int playing = 0;
	int frameCounter = 0;
	int currentRow = 0;
	int currentColumn = 0;

	void play(int animId); 
	void stop(bool reset) noexcept;

	//model skinning data:
	std::vector<glm::mat4> mBoneTransforms;

	//private functions:

	/*
		boneTransform - returns the bones interpolated transformations from the model.
		params: id of the animation that contains the bones,
				time used in the interpolation
	*/
	const std::vector<glm::mat4>& boneTransform(int animId, FLOAT_TYPE timeInSecs);

	/*
		readNodeHierachy - is used by the boneTransform() method, see it's implementation for more details
	*/
	void readNodeHierachy(int animId, FLOAT_TYPE animTime, 
		const NodeData& node, const glm::mat4& parentTransform);



	//other data:
	EffectType currentEffect;
	//bool playing = false;
	bool isEffectActive = false; //is the current effect active
	FLOAT_TYPE currentState = 0.0f; 
};



#endif //!INTERACTABLE_OBJECT_COMPONENT