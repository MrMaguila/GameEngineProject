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


This code is part of a self made game engine. It contains the declarations of the 
of the ParticleSystem class, which is used to generate, store and 
update all particles in the game. 
	The other game systems can request the generation of particles by accessing the ParticleSystem that is 
contained in the Scene class.
The particles being stored in just one big container instead of in indiviual components makes it possible to
update(this is the job of the PhysicsEngine) them in a multithreaded way(not implemented yet) and allows a better use 
of the cpu cache. These particles does not interact with other game objects, they are just graphical elements.
*/
//#############################################################################################


#ifndef PARTICLE_SYSTEM
#define PARTICLE_SYSTEM








#include <cassert>
#include <iostream>
#include <random>
#include <chrono>

#include "Entity.h"
#include "Particle.h"
#include "ObjectPool.h"
#include "Observer.h"
#include "GlobalDefines.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

//===============================================================================================================


#define PARTICLE_POOL_SIZE 300

class ParticlePool
{
public:

	ParticlePool() noexcept;



	int size() const noexcept;
	void addParticle(const Particle&) noexcept;

private:

	friend class ParticleSystem;
	friend class GraphicalSystem;

	Particle errorParticle; //used to avoid exceptions

	Particle particles[PARTICLE_POOL_SIZE];
	const int poolSize = PARTICLE_POOL_SIZE;
};


//===============================================================================================================

class ParticleSystem
{
public:

	

	ParticleSystem() noexcept;

	//don't allow copying:
	ParticleSystem(const ParticleSystem&) = delete;
	ParticleSystem(ParticleSystem&) = delete;
	ParticleSystem(ParticleSystem&&) = delete;



	void update(FLOAT_TYPE dt) noexcept; //update all particles, by the specified amount, in seconds. this 
										//is espected to be called by the physics engine, so that it can use diferent 
										//threads to proccess particles and other objects

	
	/*
		generateParticles- params:
								amount of particles
								model particle
								position variation - how much the position can vary from the model particle position.
								velocity variation - the same of position, but for velocity
								life variation

								
	*/
	void generateParticles(int amount, Particle mp, glm::vec3 pv, glm::vec3 vv, FLOAT_TYPE lv) noexcept;


	//-------------------------------------------
	//particle definitions

	const Particle FIRE_PARTICLE =
		Particle(glm::vec3(0.0), glm::vec3(0.0, 0.1, 0.0),
			8.0f * glm::vec4(1.0, 0.5, 0.0, 1.0), 1.0, Particle::ParticleType::Fire);

private:


	friend class PhysicsEngine;
	friend class GraphicalSystem;


	ParticlePool particlePool;
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribuition;
};




#endif // !PARTICLE_SYSTEM
