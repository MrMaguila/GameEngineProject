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

#include "Particle.h"


Particle::Particle() noexcept
{ 
	life = 0.0f;
}



Particle::Particle(glm::vec3 vel, glm::vec3 pos, glm::vec4 c, FLOAT_TYPE lf, Particle::ParticleType pt) noexcept
	:velocity {std::move(vel)}, position{ std::move(pos)}, color{ std::move(c)}, life{ std::move(lf)}, type{std::move(pt)}
{
}

//=============================================================================================

Particle::Particle(const Particle& p) noexcept
	:velocity{ p.velocity }, position{ p.position }, color{ p.color }, life{ p.life }, type{ p.type }
{


}


//=============================================================================================


void Particle::update(FLOAT_TYPE deltaTime) noexcept
{
	life -= deltaTime; 

	switch (type)
	{
	case Particle::ParticleType::Undefined:

		break;
	case Particle::ParticleType::Fire:
		position += velocity * deltaTime;
		color *= 0.96;

		break;
	default:
		break;
	}
}
