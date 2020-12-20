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

#include "TransformComponent.h"



//Definitions:
TransformComponent::TransformComponent(Entity id)
	:Component{ id }
{
	transform = glm::mat4(1.0f);
	scale = glm::vec3(1.0f);
}

//--------------------------------------------------------------------------------------------------

TransformComponent::TransformComponent()
	:Component{ -1 }
{
	transform = glm::mat4(1.0f);
	scale = glm::vec3(1.0f);
}


//--------------------------------------------------------------------------------------------------



void TransformComponent::setTransform(glm::mat4 t) noexcept
{
	transform = t;
}



//--------------------------------------------------------------------------------------------------

void TransformComponent::update()
{ 

}

//--------------------------------------------------------------------------------------------------

void TransformComponent::setParent(Entity id)
{
	parent = id;
}

//--------------------------------------------------------------------------------------------------

Entity TransformComponent::getParent() const noexcept
{
	return parent;
}

//--------------------------------------------------------------------------------------------------

void TransformComponent::setOrientation(FLOAT_TYPE angle, glm::vec3 n) noexcept
//angle are assumed to be in degrees
{
	//set the orientation quaternion:
	orientation = glm::quat(glm::cos(glm::radians(angle) / 2.0f), glm::sin(glm::radians(angle) / 2.0f) * n);

	//and update the transform matrix
	transform = glm::mat3(
		glm::vec3(scale.x, 0.0f, 0.0f),
		glm::vec3(0.0, scale.y, 0.0f),
		glm::vec3(0.0f, 0.0f, scale.z)) * glm::toMat3(orientation);
	transform[3] = glm::vec4(translation, 1.0f);
}



void TransformComponent::setOrientation(glm::quat newOrientation) noexcept
//angle are assumed to be in degrees
{
	//set the orientation quaternion:
	orientation = newOrientation;

	//and update the transform matrix
	transform = glm::mat3(
		glm::vec3(scale.x, 0.0f, 0.0f),
		glm::vec3(0.0, scale.y, 0.0f),
		glm::vec3(0.0f, 0.0f, scale.z)) * glm::toMat3(orientation);
	transform[3] = glm::vec4(translation, 1.0f);
}


//-------------------------------------------------------------------------------------------------

void TransformComponent::rotate(FLOAT_TYPE angle, glm::vec3 n) noexcept
//angle are assumed to be in degrees
{
	n = glm::normalize(n);
	//define a new quaternion representing the rotation about n by angle radians:
	glm::quat rotQuat = glm::quat(glm::cos(glm::radians(angle) / 2.0f), glm::sin(glm::radians(angle) / 2.0f) * n);
	//and then combine the rotation present in the orientation quaternion with rotQuat by
	orientation = rotQuat * orientation; //multiplying the two

	//and update the transform matrix
	transform = glm::mat4(glm::toMat3(orientation) * glm::mat3(
		glm::vec3(scale.x, 0.0f, 0.0f),
		glm::vec3(0.0, scale.y, 0.0f),
		glm::vec3(0.0f, 0.0f, scale.z)));
	transform[3] = glm::vec4(translation, 1.0f);
	
}

//-------------------------------------------------------------------------------------------------


void TransformComponent::rotate(glm::quat rot) noexcept
{
	//combine the rotation present in the orientation quaternion with rot by
	orientation = rot * orientation; //multiplying the two

	//and update the transform matrix
	transform = glm::mat4(glm::toMat3(orientation) * glm::mat3(
		glm::vec3(scale.x, 0.0f, 0.0f),
		glm::vec3(0.0, scale.y, 0.0f),
		glm::vec3(0.0f, 0.0f, scale.z)));
	transform[3] = glm::vec4(translation, 1.0f);
}

//-------------------------------------------------------------------------------------------------


void TransformComponent::move(glm::vec3 mv) noexcept
{
	translation += mv;
	transform[3] = glm::vec4(translation, 1.0f); 
}


//-------------------------------------------------------------------------------------------------

void TransformComponent::setScale(glm::vec3 s) noexcept
{
	scale = s;

	//update the transform matrix:
	transform = glm::toMat3(orientation) * glm::mat3(
		glm::vec3(scale.x, 0.0f, 0.0f),
		glm::vec3(0.0, scale.y, 0.0f),
		glm::vec3(0.0f, 0.0f, scale.z));
	transform[3] = glm::vec4(translation, 1.0f);
}


//--------------------------------------------------------------------------------------------------


void TransformComponent::setPosition(glm::vec3 m) noexcept
{
	translation = m;
	transform[3] = glm::vec4(translation, 1.0f); //update just the fourth collum of the transform matrix
}

//--------------------------------------------------------------------------------------------------

glm::quat TransformComponent::getOrientation() const noexcept
{
	return orientation;
}

glm::vec3 TransformComponent::getScale() const noexcept
{
	return scale;
}

glm::vec3 TransformComponent::getPosition() const noexcept
{
	return translation;
}


glm::mat4 TransformComponent::getTransform() const noexcept
{
	return transform;
}


glm::mat4 TransformComponent::getScaledTransform(glm::vec3 s) const noexcept
{
	glm::mat4 t(1.0f);
	t = glm::toMat3(orientation) * glm::mat3(
		glm::vec3(s.x, 0.0f, 0.0f),
		glm::vec3(0.0, s.y, 0.0f),
		glm::vec3(0.0f, 0.0f, s.z));
	t[3] = glm::vec4(translation, 1.0f);


	return t;
}