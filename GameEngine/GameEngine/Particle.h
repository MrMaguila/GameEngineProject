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

#ifndef PARTICLE
#define PARTICLE



#include "GlobalDefines.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
//===============================================================================================================




class Particle
{
public:

	enum class ParticleType
	{	
		Undefined = 0,
		Fire = 1
	};

	/*
		Contructors:
	*/
	Particle() noexcept;
	Particle(glm::vec3, glm::vec3, glm::vec4, FLOAT_TYPE, ParticleType) noexcept;
	Particle(const Particle&) noexcept;


	/*
		update - update the particle according to its type
			params: deltaTime, the time in seconds to be passed in the particle life
	*/
	void update(FLOAT_TYPE deltaTime) noexcept; 


private:

	friend class ParticleSystem;
	friend class GraphicalSystem;
	friend class GameplayHandler;
	friend class ParticlePool;
	
	glm::vec3 velocity;
	glm::vec3 position;
	glm::vec4 color;
	FLOAT_TYPE life = 0.0f; //in seconds
	ParticleType type = ParticleType::Undefined;
};

const Particle FIRE_PARTICLE =
	Particle(glm::vec3(0.0, 18.0 , 0.0), glm::vec3(0.0, -8.0, 0.0),
	8.0f * glm::vec4(1.0, 0.5, 0.0, 0.5), 1.2, Particle::ParticleType::Fire);


#endif // !PARTICLE