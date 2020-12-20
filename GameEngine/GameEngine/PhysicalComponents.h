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

#ifndef PHYSICAL_COMPONENTS
#define PHYSICAL_COMPONENTS



#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Entity.h"

#include "GlobalDefines.h"


//######################################################################################################
//helper classes and functions:


enum class ShapeType
{
	sphere = 0,
	box = 1
};


struct Sphere
{
	Sphere();
	Sphere(FLOAT_TYPE, glm::vec3);

	//functions:
	ShapeType getType() const noexcept;
	glm::vec3 getSize() const noexcept;
	void setSize(int, int, int) noexcept;

	//Data:
	FLOAT_TYPE radius = 1.0;
	glm::vec3 pos = glm::vec3(0.0f);
};



struct Box 
{
	void setSize(int, int, int) noexcept; 
	glm::vec3 getVertex(int) const;
	glm::vec3 getSize() const noexcept;

	//Data:
	glm::vec3 pos = glm::vec3(0.0f); //the same as the center of the box

private:
	//Private data:
	glm::vec3 vertices[8];
	glm::vec3 size;
};


//######################################################################################################
//RigidBodyComponent:

template<typename T>
class RigidBodyComponent : public Component
{
public:

	RigidBodyComponent();

	//functions:
	void update() noexcept;
	void addForce(glm::vec3) noexcept; //apply a force at the center of mass
	void addLinearVelocity(glm::vec3) noexcept; //directly increases the velocity(this is helpfull for the collision response)
	void move(glm::vec3) noexcept;

	void setMass(FLOAT_TYPE) noexcept;
	
	glm::vec3 getSize() const noexcept;
	glm::vec3 getPosition() const noexcept;
	void setPosition(glm::vec3) noexcept;
	void setSize(glm::vec3) noexcept;

	FLOAT_TYPE mass = 1.0;
	glm::vec3 linearVelocity = glm::vec3(0.0f);
private:

	friend class PhysicsEngine; //allow the physics engine to access the private data

	//constructor:
	RigidBodyComponent(Entity, T);


	//Data:
	T shape;
	glm::mat3 inertiaTersors = glm::mat3(0); 
	glm::vec3 forces = glm::vec3(0.0f);
	glm::vec3 deltaSpace = glm::vec3(0.0f);

	//angular constraints:
	bool xRot = true, yRot = true, zRot = true; //used to enable or disable rotation on each axis

};




#endif // !PHYSICAL_COMPONENTS


