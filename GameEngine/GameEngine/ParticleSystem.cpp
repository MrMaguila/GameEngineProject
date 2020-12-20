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

#include "ParticleSystem.h"



ParticlePool::ParticlePool() noexcept
{

}

//============================================================================================


int ParticlePool::size() const noexcept
{
	return poolSize;
}



//============================================================================================


void ParticlePool::addParticle(const Particle& p) noexcept
{
	for (int i = 0; i < PARTICLE_POOL_SIZE; ++i)
		if (particles[i].life <= 0)
		{
			particles[i] = p;
			break;
		}
}





//============================================================================================



ParticleSystem::ParticleSystem() noexcept
{
	generator = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	distribuition = std::uniform_real_distribution<double>(-1.0, 1.0);

}


//=============================================================================================


void ParticleSystem::update(FLOAT_TYPE dt) noexcept
{

	for(int i = 0; i < PARTICLE_POOL_SIZE; ++i)
	{
		if(particlePool.particles[i].life > 0.0)
			particlePool.particles[i].update(dt);
	}


}




//=============================================================================================


void ParticleSystem::generateParticles(int amount, Particle mp, glm::vec3 pv, glm::vec3 vv, FLOAT_TYPE lv) noexcept
{

	for (int i = 0; i < amount; ++i)
	{
		Particle p(mp);

		//change the starting position by a random amount:
		p.position.x += float(distribuition(generator)) * pv.x;
		p.position.y += float(distribuition(generator)) * pv.y;
		p.position.z += float(distribuition(generator)) * pv.z;

		//the starting velocity
		p.velocity.x += float(distribuition(generator)) * vv.x;
		p.velocity.y += float(distribuition(generator)) * vv.y;
		p.velocity.z += float(distribuition(generator)) * vv.z;

		//and also the life
		p.life += float(distribuition(generator)) * lv;


		
		//push the particle on the pool(note that this may realocate the pool memory,
		//make sure it is initialized to a suficient amount)
		particlePool.addParticle(p);
	}
		
}