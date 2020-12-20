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


The TransformComponent store the position, rotation and scale data about an object.
*/
//#############################################################################################


#ifndef TRANSFORM_COMPONENT
#define TRANSFORM_COMPONENT

#include <cassert>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Entity.h"

#include "GlobalDefines.h"


class TransformComponent : public Component
{
public:

	TransformComponent(Entity);
	TransformComponent();

	//Functions:
	void update();

	void setParent(Entity); //! put this function on world instead of here
	Entity getParent() const noexcept; 

	void setTransform(glm::mat4) noexcept;

	void setOrientation(FLOAT_TYPE, glm::vec3) noexcept; //set the orientation quaternion and update the transform matrix
	void setOrientation(glm::quat) noexcept;

	void rotate(FLOAT_TYPE, glm::vec3) noexcept; //rotate and update the transform matrix
	void rotate(glm::quat) noexcept;

	void move(glm::vec3) noexcept;
	void setPosition(glm::vec3) noexcept; //set the translation matrix and update the transform matrix

	void setScale(glm::vec3) noexcept; //set scale and update the transform matrix
	

	glm::quat getOrientation() const noexcept;
	glm::vec3 getScale() const noexcept; 
	glm::vec3 getPosition() const noexcept;  
	glm::mat4 getTransform() const noexcept;

	glm::mat4 getScaledTransform(glm::vec3) const noexcept;
	

private:
	Entity parent = -1; //negative parent means that this transform
										//is relative to world coordinates
	//data:
	glm::quat orientation = glm::quat(1, glm::vec3(0.0f));
	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::mat4 transform; //a matrix that contains all the other three transformations
};




#endif // !TRANSFORM_COMPONENT
