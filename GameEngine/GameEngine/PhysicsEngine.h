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

#ifndef PHYSICS_ENGINE
#define PHYSICS_ENGINE


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

#include "World.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "PhysicalComponents.h"
#include "CollisionHandling.h"
#include "Observer.h"

#include "GlobalDefines.h"


class PhysicsEngine : public Observer, public Subject
{
public:

	void initialize(World*);
	void update(); //integrate by timeStep seconds
	FLOAT_TYPE getTimeStep() const noexcept;
	void setTimeStep(FLOAT_TYPE) noexcept; 

	void addSpherePhysicalComponent(Entity, FLOAT_TYPE, glm::vec3);
	void addBoxPhysicalComponent(Entity, int, int, int, glm::vec3);


	void onNotify(Message) override;

private:

	//private data
	World* world; //hold a ptr to the world to get access to all scenes
	FLOAT_TYPE timeStep = 0.016f; //the delta time of each integration, measured in seconds

	//private functions:
	glm::mat4 normalizeRows(int, glm::mat4) const noexcept;
	glm::mat4 getFullTransform(Entity) const;
	glm::quat getFullRotationQuaternion(Entity) const;

	void solveForSpheres();
	void solveForBoxes();
};


#endif // !PHYSICS_ENGINE
