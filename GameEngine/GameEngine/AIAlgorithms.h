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


#ifndef AI_ALGORITHMS
#define AI_ALGORITHMS


#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "CharacterComponent.h"
#include <random>
#include <ctime>
#include <chrono>

#include "GlobalDefines.h"

class Algorithms
{
public:

	Algorithms()
	{
		randomEngine.seed(std::time(0));
	}

	glm::vec3 seek(const CharacterComponent&, glm::vec3, glm::vec3, glm::vec3, FLOAT_TYPE, FLOAT_TYPE);
	glm::vec3 flee(const CharacterComponent&, glm::vec3, glm::vec3, glm::vec3, FLOAT_TYPE);

	glm::vec3 matchVelocity(glm::vec3, glm::vec3, FLOAT_TYPE);

	glm::vec3 binomialWandering(const CharacterComponent& char1);

private:

	std::default_random_engine randomEngine;
	std::uniform_real_distribution<FLOAT_TYPE> uniformDist = std::uniform_real_distribution<FLOAT_TYPE>(0.0f, 1.0f);
	std::normal_distribution<FLOAT_TYPE> normalDist = std::normal_distribution<FLOAT_TYPE>(-1.0f, 1.0f);
	//std::binomial_distribution<FLOAT_TYPE> binomialDist = std::binomial_distribution<FLOAT_TYPE>(-1.0f, 1.0f);
};


//====================================================================================================================


inline glm::vec3 Algorithms::seek(const CharacterComponent& char1, glm::vec3 velocity, 
	glm::vec3 pos, glm::vec3 targetPos, FLOAT_TYPE radius, FLOAT_TYPE slowDown)
{
	glm::vec3 distanceVec = targetPos - pos;
	FLOAT_TYPE len = glm::length(distanceVec);
	if (len < radius) //the character is at an acceptable distance from the target
		return glm::vec3(0.0f, 0.0f, 0.0f);
	if (len < slowDown)
	{
		FLOAT_TYPE a = slowDown - radius;
		return glm::normalize(distanceVec) * (((1.0f - 0.8f) * (len - radius) / (slowDown - radius)) + 0.8f);
	}
	return glm::normalize(distanceVec);
}

//----------------------------------------------------------------------------------------------------------------

inline glm::vec3 Algorithms::flee(const CharacterComponent& char1, glm::vec3 velocity, glm::vec3 pos, glm::vec3 targetPos, FLOAT_TYPE radius)
{
	glm::vec3 distanceVec = pos - targetPos;
	if (glm::length(distanceVec) > radius) //the character is at an acceptable distance from the target
		return glm::vec3(0.0f, 0.0f, 0.0f);

	return glm::normalize(distanceVec);
}

//----------------------------------------------------------------------------------------------------------------

inline glm::vec3 Algorithms::matchVelocity(glm::vec3 vel, glm::vec3 targetVel, FLOAT_TYPE tolerance)
{
	glm::vec3 result = vel - targetVel;
	if (glm::length(result) < tolerance)
		return glm::vec3(0.0f, 0.0f, 0.0f);


}


//----------------------------------------------------------------------------------------------------------------

inline glm::vec3 Algorithms::binomialWandering(const CharacterComponent& char1)
{
	glm::vec3 result;
	if (glm::length(char1.direction) < 0.001f) //if the character doesn't have a target direction
	{
		FLOAT_TYPE angle = (uniformDist(randomEngine) - uniformDist(randomEngine)) * glm::pi<FLOAT_TYPE>(); //generate a random orientation
		result.x = glm::sin(angle);
		result.y = 0.0f;
		result.z = glm::cos(angle);
	}
	else
	{
		FLOAT_TYPE rotation = (uniformDist(randomEngine) - uniformDist(randomEngine)) * (glm::pi<FLOAT_TYPE>() / 4.0f);
		//to sum the angles, with the formula sin(a + b) = sin(a) * cos(b) + cos(a) * sin(b)
								        //and cos(a + b) = cos(a) * cos(b) - sin(a) * sin(b)
		result.x = char1.direction.x * glm::cos(rotation) + char1.direction.y * glm::sin(rotation);
		result.y = 0.0f;
		result.z = char1.direction.y * glm::cos(rotation) - char1.direction.x * glm::sin(rotation);
	};

	return glm::normalize(result);
}

#endif // !AI_ALGORITHMS
