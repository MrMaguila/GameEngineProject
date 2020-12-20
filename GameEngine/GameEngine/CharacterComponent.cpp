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

#include "CharacterComponent.h"



void CharacterComponent::setModel(const Model* m)
{
	if (!m)
		throw std::logic_error("ERROR::INVALID ARGUMENTS PASSED TO CharacterComponent::setModels();\n");

	model = m;

	mBoneTransforms.clear();
	for (int i = 0; i < model->mBoneData.size(); ++i)
		mBoneTransforms.push_back(model->mBoneData[i].fullTransform);

}



//-------------------------------------------------------------------------------------------



const Model* CharacterComponent::getModel() const
{
	myAssert(model);
	return model;
}


//-------------------------------------------------------------------------------------------


void CharacterComponent::updateModel()
{

	//mBoneTransforms.clear();
	//for (int i = 0; i < model->mBoneData.size(); ++i)
	//	mBoneTransforms.push_back(model->mBoneData[i].fullTransform);
}


//-------------------------------------------------------------------------------------------

int CharacterComponent::getCurrentAnimation() const noexcept
{
	return currentAnimation;
}


FLOAT_TYPE CharacterComponent::getAnimationTime() const noexcept
{
	return animationTime;
}


//-------------------------------------------------------------------------------------------

int CharacterComponent::getCurrentRow() const noexcept
{
	return currentRow;
}

//-------------------------------------------------------------------------------------------

int CharacterComponent::getCurrentColumn() const noexcept
{
	return currentColumn;
}


//-------------------------------------------------------------------------------------------


void CharacterComponent::setModelPos(glm::vec3 newPos) noexcept
{
	pos = newPos;
}

//-------------------------------------------------------------------------------------------

glm::vec3 CharacterComponent::getModelPos() const noexcept
{

	return pos;
}

//-------------------------------------------------------------------------------------------

int CharacterComponent::getDirection() const noexcept
{ 
	return modelDirection;
}

//-------------------------------------------------------------------------------------------


void CharacterComponent::boneTransform(int animId, FLOAT_TYPE timeInSecs, std::vector<glm::mat4>& transforms)
{
	myAssert(model);

	if (animId < 0 || animId >= model->sceneData.animations.size())
		throw std::logic_error("ERRROR::INVALID ANIMATION ID PASSED TO boneTransform();\n");
	
	FLOAT_TYPE ticksPerSec = model->sceneData.animations[animId].ticksPerSecond;
	if (ticksPerSec == 0.0f)
		ticksPerSec = 25.0f;
	
	FLOAT_TYPE timeInTicks = timeInSecs * ticksPerSec;
	FLOAT_TYPE animTime = std::fmod(timeInTicks, model->sceneData.animations[animId].duration);
	
	readNodeHierachy(animId, animTime, model->sceneData.rootNode, glm::mat4(1.0f));
	
	//set the transformation matrix for each bone
	//transforms.resize(mBoneTransforms.size());
	//for (int i = 0; i < mBoneTransforms.size(); ++i)
	//{
	//	transforms[i] = mBoneTransforms[i];
	//}
}





//-------------------------------------------------------------------------------------------


void CharacterComponent::readNodeHierachy(int animId, FLOAT_TYPE animTime, const NodeData& node, const glm::mat4& parentTransform)
{
	myAssert(model);

	//std::cout << "->> ";
	auto animNode = model->sceneData.animations[animId].channels.find(node.name);

	
	


	glm::mat4 nodeTransform = node.transformation;
	if (animNode != model->sceneData.animations[animId].channels.end())
	{
		//get the interpolated transformation matrix
		glm::mat4 scaling = glm::mat3(1.0f);
		glm::vec3 scalingVec = animNode->second.getInterpolatedScaling(animTime);
		scaling[0][0] = scalingVec.x;
		scaling[1][1] = scalingVec.y;
		scaling[2][2] = scalingVec.z;

		glm::vec3 translationVec = animNode->second.getInterpolatedTranslation(animTime);
	
		nodeTransform =
			glm::translate(glm::mat4(1.0f), translationVec) *
			glm::scale(glm::mat4(1.0f), scalingVec) *
			glm::toMat4(glm::normalize(animNode->second.getInterpolatedRotation(animTime)));

  
	}
	

	glm::mat4 globalTransform = parentTransform * nodeTransform;

	//configure bones
	if (model->mBoneMapping.find(node.name) != model->mBoneMapping.end())
	{
	
		unsigned int boneIndex = model->mBoneMapping.at(node.name);
	
		//set the bone full transform:
		mBoneTransforms[boneIndex] =
			model->m_globalInverseTransform *
			globalTransform *
			model->mBoneData[boneIndex].offset;
		//std::cout << "<<-\n";

	}

	
	for (int i = 0; i < node.children.size(); ++i)
		readNodeHierachy(animId, animTime, node.children[i], globalTransform);

}






//-------------------------------------------------------------------------------------------

void CharacterComponent::AddHealth(int n) noexcept
{
	health += n;
	if (health > maxHealth) health = maxHealth;
	else if (health < 0) health = 0;
}

//-------------------------------------------------------------------------------------------

void CharacterComponent::AddMana(int n) noexcept
{
	mana += n;
	if (mana > maxMana) mana = maxMana;
	else if (mana < 0) mana = 0;
}

//-------------------------------------------------------------------------------------------

void CharacterComponent::AddEnergy(int n) noexcept
{
	energy += (n > 0 ? (n * energyRegenBoost * (IsIdle() ? 8.0 : 4.0)) : n * enegyCostMultiplier * 2.0);
	if (energy > maxEnergy) energy = maxEnergy;
	else if (energy < 0) energy = 0;
}


//-------------------------------------------------------------------------------------------

void CharacterComponent::play(int animId)
{
	myAssert(model);

	if (model->sceneData.animations.size() <= animId || model->sceneData.animations.size() == 0 || animId < 0)
		std::logic_error("ERROR::INVALID ARGUMENTS PASSED TO CharacterComponent::play();\n");

	if (currentAnimation != animId) { animationTime = 0.0f; } //reset animation time if the animation was changed
	currentAnimation = animId;
}



//-------------------------------------------------------------------------------------------

void CharacterComponent::playTextureAnimation(int dur, int row)
{
	myAssert(model);
	
	if (//(model->mMaterial.animations < 2 || model->mMaterial.framesPerAnimation < 2) ||
		(model->mMaterial.animations <= row) || row < 0) myAssert(false); //the sprite isn't a spritesheet
	playing = dur;
	if (currentRow != row) { playing = dur; frameCounter = 0; currentColumn = 0; } //reset current frame if the animation was changed
	currentRow = row;
}


//-------------------------------------------------------------------------------------------


void CharacterComponent::stop(bool reset) noexcept
{
	playing = 0;
}

//-------------------------------------------------------------------------------------------


void CharacterComponent::WalkUp(bool a) noexcept
{
	if (casting) 
		return;

	if (a)
	{
		modelDirection = 2; //turn model up(pointing to north)
		charPreviouseState = charState;
		charState = CharacterState::Walking;
		walkUpActive = true;
	}
	else //stop moving up if a is false, and set charState to standing if the character was not moving
			//to any other direction
	{
		bool b = walkUpActive;
		walkUpActive = false;//doing this makes IsIdle() return true if the character was moving just to north
		if (b && IsIdle())
		{
			modelDirection = 2;
			charPreviouseState = charState;
			charState = CharacterState::Standing;
		}
	}

}

void CharacterComponent::WalkDown(bool a) noexcept
{
	if (casting) 
		return;

	if (a)
	{
		modelDirection = 0; //turn model down(pointing to south)
		charPreviouseState = charState;
		charState = CharacterState::Walking;
		walkDownActive = true;
	}
	else //stop moving down if a is false, and set charState to standing if the character was not moving
			//to any other direction
	{
		bool b = walkDownActive;
		walkDownActive = false; //doing this makes IsIdle() return true if the character was moving just to south
		if (b && IsIdle())
		{
			modelDirection = 0;
			charPreviouseState = charState;
			charState = CharacterState::Standing;
		}
	}

}

void CharacterComponent::WalkLeft(bool a) noexcept
{
	
	if (casting) 
		return;

	if (a)
	{
		modelDirection = 3; //turn model left(pointing to west)
		charPreviouseState = charState;
		charState = CharacterState::Walking;
		walkLeftActive = true;
	}
	else //stop moving left if a is false, and set charState to standing if the character was not moving
			//to any other direction
	{
		bool b = walkLeftActive;
		walkLeftActive = false; //doing this makes IsIdle() return true if the character was moving just to west
		if (b && IsIdle())
		{
			modelDirection = 3;
			charPreviouseState = charState;
			charState = CharacterState::Standing;
		}
	}
}

void CharacterComponent::WalkRight(bool a) noexcept
{
	if (casting) 
		return;

	if (a)
	{
		modelDirection = 1; //turn model right(pointing to east)
		charPreviouseState = charState;
		charState = CharacterState::Walking;
		walkRightActive = true;
	}
	else //stop moving right if a is false, and set charState to standing if the character was not moving
			//to any other direction
	{
		bool b = walkRightActive;
		walkRightActive = false; //doing this makes IsIdle() return true if the character was moving just to east
		if (b && IsIdle())
		{
			modelDirection = 1;
			charPreviouseState = charState;
			charState = CharacterState::Standing;
		}
	}
}

void CharacterComponent::Jump() noexcept
{
	if (onGround > 0 && energy > 40 && jumpCooldown == 0 && casting == 0)
		mustJump = true;
}

//------------------------------------------------------------------------------------------


bool CharacterComponent::Attack(int attackNum) noexcept
{
	if (onGround && casting == 0)
	{
		switch (charState)
		{
		case CharacterState::Standing:
		case CharacterState::Walking:
		case CharacterState::Running:
			casting = 40;
			charPreviouseState = CharacterState::Standing;
			charState = CharacterState::attacking0;
			walkUpActive = false;
			walkDownActive = false;
			walkLeftActive = false;
			walkRightActive = false;
			return true;
			break;
		
		default:
			break;
		}
	}

	return false;
}



//------------------------------------------------------------------------------------------

FLOAT_TYPE CharacterComponent::getAnimationDuration() const 
{
	myAssert(model);
	return model->sceneData.animations[currentAnimation].duration;
}


//------------------------------------------------------------------------------------------



glm::vec3 CharacterComponent::getMovingVector() const noexcept
{
	glm::vec3 result(
		movingSpeed * (walkLeftActive - walkRightActive), //movement on x
		0.0f, //on y
		movingSpeed * (walkUpActive - walkDownActive)); //and on z
	if (glm::length(result) > movingSpeed * 1.1f) result *= 0.75f;
	return result * speed;
}

//-------------------------------------------------------------------------------------------

bool CharacterComponent::IsIdle() const noexcept
{
	return (!walkDownActive && !walkLeftActive && !walkRightActive && !walkUpActive && casting == 0);
}



//-------------------------------------------------------------------------------------------

CharacterComponent::CharacterComponent()
	:Component{ -1 }, charType{ CharacterType::Unknown }
{
	for (int i = 0; i < MAX_NUM_OF_INTERACTABLE_OBJECTS; ++i)
		childrenObjects[i] = -1;
}


//-------------------------------------------------------------------------------------------

CharacterComponent::CharacterComponent(int id, int type)
	:Component{ id }, charType{ type }
{
	for (int i = 0; i < MAX_NUM_OF_INTERACTABLE_OBJECTS; ++i)
		childrenObjects[i] = -1;

}

//-------------------------------------------------------------------------------------------

void CharacterComponent::update()
{
	AddEnergy(1); //regenerate energy

	onGround -= (onGround > 0) ? 1 : 0; //update onGround state

	jumpCooldown -= (jumpCooldown > 0) ? 1 : 0; //and jump cooldown

	if (casting - 1 == 0) 
		charState = charPreviouseState;

	casting -= (casting > 0) ? 1 : 0;
	
	//-------------------------------------------
	//update animations
	switch (charState)
	{
	case CharacterState::attacking0:
	{
		playTextureAnimation(4, 0);
		play(0); //play a model animation
		break;
	}
	case CharacterState::Standing:
	{
		playTextureAnimation(30, 0);
		play(2); //play a model animation
		break;
	}
	case CharacterState::Walking:
	{
		playTextureAnimation(8.0f / speed, 0);
		play(1); //play a model animation
		break;
	}

	default:
		myAssert(false);
		break;
	}


	//----------------------------
	//update texture animation data:
	if (playing > 0 && model)
	{
		//std::cout << "HAAAA " << playing << "\n";
		++frameCounter;
		if (frameCounter >= playing)
		{
			currentColumn = (currentColumn + 1) % model->mMaterial.framesPerAnimation;
			frameCounter = 0;
		}
	}

	



}