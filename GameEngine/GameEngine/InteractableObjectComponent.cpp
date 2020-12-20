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



#include "InteractableObjectComponent.h"

InteractableObjectComponent::InteractableObjectComponent()
	:Component{ -1 }
{
}


//=========================================================================================================


InteractableObjectComponent::InteractableObjectComponent(Entity id, 
														glm::vec3 hitBoxSize, 
														EffectType effectType,
														const Model* m)
	:Component{ id }
{
	hitBox.setSize(hitBoxSize.x, hitBoxSize.y, hitBoxSize.z);
	currentEffect = effectType;

	if (m)
	{
		model = m;
		for (int i = 0; i < model->mBoneData.size(); ++i)
			mBoneTransforms.push_back(model->mBoneData[i].fullTransform);
	}
	else
	{
		model = nullptr;
		std::cout << 
			"WARNING::NULL PTR PASSED AS ARGUMENT TO InteractableObjectComponent() FUNCTION;\n";
	}

}


//=========================================================================================================


void InteractableObjectComponent::update() noexcept
{
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


//=========================================================================================================


void InteractableObjectComponent::setCurrentEffect(EffectType eType) noexcept
{
	currentEffect = eType;
}


//=========================================================================================================


void InteractableObjectComponent::setModel(const Model* m) noexcept
{
	if (m)
	{
		model = m;
		for (int i = 0; i < model->mBoneData.size(); ++i)
			mBoneTransforms.push_back(model->mBoneData[i].fullTransform);
	}
	else
	{
		model = nullptr;
		std::cout << "WARNING::NULL PTR PASSED AS ARGUMENT TO setModel() FUNCTION;\n";
	}

}


//=========================================================================================================


void InteractableObjectComponent::setPosition(glm::vec3 p) noexcept
{
	pos = p;
}


//=========================================================================================================


glm::vec3 InteractableObjectComponent::getPosition() const noexcept
{
	return pos;
}


//=========================================================================================================


void InteractableObjectComponent::play(int animId)
{
	myAssert(model);

	if (model->sceneData.animations.size() <= animId || model->sceneData.animations.size() == 0 || animId < 0)
		std::logic_error("ERROR::INVALID ARGUMENTS PASSED TO InteractableObjectComponent::play();\n");

	if (currentAnimation != animId) { animationTime = 0.0f; } //reset animation time if the animation was changed
	currentAnimation = animId;
}



//=========================================================================================================



void InteractableObjectComponent::stop(bool reset) noexcept
{
	playing = 0;
}



//=========================================================================================================



const std::vector<glm::mat4>& InteractableObjectComponent::boneTransform(int animId, FLOAT_TYPE timeInSecs)
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

	//*transforms = &mBoneData; //transforms now points to a pointer that points to a vector containing all bone transformations

	//set the transformation matrix for each bone

	/*transforms.resize(mBoneTransforms.size());
	for (int i = 0; i < mBoneTransforms.size(); ++i)
	{
		transforms[i] = mBoneTransforms[i];
	} */
		
	return mBoneTransforms;
}



//=========================================================================================================




void InteractableObjectComponent::readNodeHierachy(int animId, 
													FLOAT_TYPE animTime, 
													const NodeData& node, 
													const glm::mat4& parentTransform)
{
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
		;
   
	}

	glm::mat4 globalTransform = parentTransform * nodeTransform;

	//configure bones
	if (model->mBoneMapping.find(node.name) != model->mBoneMapping.end())
	{
		unsigned int boneIndex = model->mBoneMapping.at(node.name);
		//std::cout << boneIndex << '\n';
		//set the bone full transform:
		mBoneTransforms[boneIndex] =
			model->m_globalInverseTransform *
			globalTransform *
			model->mBoneData[boneIndex].offset;


	}

	for (int i = 0; i < node.children.size(); ++i)
		readNodeHierachy(animId, animTime, node.children[i], globalTransform);

}