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

This code is part of a self made game engine. It defines all functions used for collision 
detection and response.
*/
//#############################################################################################

#ifndef COLLISION_HANDLING
#define COLLISION_HANDLING



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

#include "PhysicalComponents.h"

#include "GlobalDefines.h"


namespace glm
{
	inline FLOAT_TYPE maxLengthDot(const glm::vec3& vec1, const glm::vec3& vec2) noexcept
	{
		return std::fabs((vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z));
	}
};

//############################################################################################
//Collision detection algorithms

//take two spheres and returns true if they are penetrating or colliding,
//also set separation vector as the value of the third parameter(it's possible to get the collsion
//normal by normalizing the separation vector)
inline bool detectSphereToSphere(const Sphere& sp1, const Sphere& sp2, glm::vec3& separVec) noexcept
{
	FLOAT_TYPE overlap = glm::length(sp1.pos - sp2.pos) - (sp1.radius + sp2.radius);
	if (overlap >= 0.0f) return false; //no collision(the distance bettween centers is higher than the two radius)

	//else, they are overlaping, so set the separation vector and return true
	separVec = glm::normalize(sp1.pos - sp2.pos) * overlap; //since the overlap is negative, separVec will point
												//from sp1 to sp2
	return true;
}


//========================================================================================================


//take two possitions and two radius as arguments, and return true only if the
//the two spheres defined by these four data are overlaping
inline bool boundSphereTest(glm::vec3 p1, glm::vec3 p2, FLOAT_TYPE r1, FLOAT_TYPE r2) noexcept
{	
	return (glm::length(p1 - p2) <= r1 + r2) ? true : false;
}


//========================================================================================================

//returns the index of vertices with the max and min projections, also returns their respective projection values 
//in the projections argument. Assumes the size of vertices is 8.
inline std::pair<int, int> getMinAndMaxProj(const glm::vec3* vertices, const glm::vec3& vec, 
										std::pair<FLOAT_TYPE, FLOAT_TYPE>& projections) noexcept
{
	std::pair<int, int> minAndMax = {0, 0};
	std::pair<FLOAT_TYPE, FLOAT_TYPE> minAndMaxProjections = { glm::dot(vertices[0], vec), glm::dot(vertices[0], vec) };
	FLOAT_TYPE proj;
	for (int i = 1; i < 8; ++i)
	{
		proj = glm::dot(vertices[i], vec);
		if (proj < minAndMaxProjections.first) { minAndMaxProjections.first = proj; minAndMax.first = i; }
		if (proj > minAndMaxProjections.second) { minAndMaxProjections.second = proj; minAndMax.second = i; }
	};

	projections = minAndMaxProjections;
	return minAndMax;
}


//========================================================================================================

//assumes the lenght of vertices is 8:
inline int getClosestVertex(const glm::vec3* vertices, const glm::vec3& pos) noexcept
{
	int closest = 0;
	FLOAT_TYPE closestLength = glm::length(vertices[0] - pos);
	for (int i = 1; i < 8; ++i)
	{
		FLOAT_TYPE currentLength = glm::length(vertices[i] - pos);
		if (currentLength < closestLength) { closest = i; closestLength = currentLength; }
	}

	return closest;
}


//========================================================================================================


//assumes the lenght of vertices is 8
inline int getMaxProjectionVertex(const glm::vec3* vertices, const glm::vec3& vec) noexcept
{
	int max = 0;
	FLOAT_TYPE maxProj = glm::maxLengthDot(vertices[0], vec);
	for (int i = 1; i < 8; ++i)
	{
		FLOAT_TYPE currentProj = glm::maxLengthDot(vertices[i], vec);
		if (currentProj >= maxProj) { max = i; maxProj = currentProj; }
	}

	return max;
}


//========================================================================================================

//take two boxes and their respective model matrices as input, and also returns a separation vector
//in the fifth parameter. Returns true if penetration or collision happened
inline bool detectBoxToBox2(const Box& b1, const Box& b2, const glm::mat4& model1, 
							const glm::mat4& model2, glm::vec3& separ, bool a2) noexcept
{
	glm::vec3 b1Vertices[8]; //b1 vertices in world space
	glm::vec3 b2Vertices[8]; //b2 vertices in world space
	glm::vec3 separVecs[3]; //the vectors used to test overlap, they will be the three positive normals from b1
	glm::vec3 separVecs2[3]; //the three normals from b2

	//set the values for the vertices
	for (int i = 0; i < 8; ++i)
	{
		b1Vertices[i] = glm::vec3(model1 * glm::vec4(b1.getVertex(i), 1.0f)) - b1.pos;
		b2Vertices[i] = glm::vec3(model2 * glm::vec4(b2.getVertex(i), 1.0f)) - b2.pos;
	}

	//set the values for the separation vectors:
	separVecs[0] = glm::normalize(model1 * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)); //the w value is 0.0f because it is a direction vector
	separVecs[1] = glm::normalize(model1 * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	separVecs[2] = glm::normalize(model1 * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

	std::vector<FLOAT_TYPE> overlaps;

	glm::vec3 centerToCenter = b2.pos - b1.pos; //a vector from b1 center to b2 center
	FLOAT_TYPE mtvLength = 0.0f;
	//--------------------------------------
	//Test overlap on first axis:
	FLOAT_TYPE overlap = glm::maxLengthDot(centerToCenter, separVecs[0]) 
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVecs[0])], separVecs[0])
		+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVecs[0])], separVecs[0]));

	int a = 0;
	if (overlap > 0.0) return false; //no intersection in this axis
	mtvLength = overlap;
	separ = separVecs[0] * mtvLength;

	//--------------------------------------
	//Test overlap on second axis:
	overlap = glm::maxLengthDot(centerToCenter, separVecs[1])
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVecs[1])], separVecs[1])
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVecs[1])], separVecs[1]));

	if (overlap > 0.0f) return false; //no intersection in this axis
	if (std::fabs(overlap) <= std::fabs(mtvLength))
	{
		mtvLength = overlap;
		separ = separVecs[1] * mtvLength;
	}

	//--------------------------------------
	//Test overlap on third axis:
	overlap = glm::maxLengthDot(centerToCenter, separVecs[2])
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVecs[2])], separVecs[2])
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVecs[2])], separVecs[2]));

	if (overlap > 0.0f) return false; //no intersection in this axis
	if (std::fabs(overlap) <= std::fabs(mtvLength))
	{
		mtvLength = overlap;
		separ = separVecs[2] * mtvLength;
	}


	//=======================================================================
	//--------------------------------------
	//set the values for b2 local axes:
	centerToCenter = b1.pos - b2.pos;
	
	separVecs2[0] = glm::normalize(model2 * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)); //the w value is 0.0f because it is a direction vector
	separVecs2[1] = glm::normalize(model2 * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	separVecs2[2] = glm::normalize(model2 * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

	//Test overlap on first axis:
	overlap = glm::maxLengthDot(centerToCenter, separVecs2[0])
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVecs2[0])], separVecs2[0])
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVecs2[0])], separVecs2[0]));

	if (overlap > 0.0f) return false; //no intersection in this axis
	if (std::fabs(overlap) <= std::fabs(mtvLength))
	{
		mtvLength = overlap;
		separ = separVecs2[0] * mtvLength;
	}


	//--------------------------------------
	//Test overlap on second axis:
	overlap = glm::maxLengthDot(centerToCenter, separVecs2[1])
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVecs2[1])], separVecs2[1])
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVecs2[1])], separVecs2[1]));

	if (overlap > 0.0f) return false; //no intersection in this axis
	
	if (std::fabs(overlap) <= std::fabs(mtvLength))
	{
		mtvLength = overlap;
		separ = separVecs2[1] * mtvLength;
	}


	//--------------------------------------
	//Test overlap on third axis:
	overlap = glm::maxLengthDot(centerToCenter, separVecs2[2])
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVecs2[2])], separVecs2[2])
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVecs2[2])], separVecs2[2]));

	if (overlap > 0.0f) return false; //no intersection in this axis
	if (std::fabs(overlap) <= std::fabs(mtvLength))
	{
		mtvLength = overlap;
		separ = separVecs2[2] * mtvLength;
	}

	//return true;
	
	//==============================================================================
	//test if the boxes axes are parallel(if they are, there's no need to test for the remaining axes):
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			//std::cout << "{" << fabs(glm::dot(separVecs[i], separVecs2[j])) << "}\n";
			if(fabs(glm::dot(separVecs[i], separVecs2[j])) > 0.99f)
			{
				return true; //don't test for the remaining axes
			}
		}

	}

	//std::cout << "Haa\n";
	

	//---------------------------------------
	//test overlap on b.x ^ a.x:
	glm::vec3 separVec = glm::cross(separVecs[0], separVecs2[0]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.y ^ a.x:
	separVec = glm::cross(separVecs[1], separVecs2[0]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.z ^ a.x:
	separVec = glm::cross(separVecs[2], separVecs2[0]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.x ^ a.y:
	separVec = glm::cross(separVecs[0], separVecs2[1]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.y ^ a.y:
	separVec = glm::cross(separVecs[1], separVecs2[1]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.z ^ a.y:
	separVec = glm::cross(separVecs[2], separVecs2[1]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.x ^ a.z:
	separVec = glm::cross(separVecs[0], separVecs2[2]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.y ^ a.z:
	separVec = glm::cross(separVecs[1], separVecs2[2]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}

	//---------------------------------------
	//test overlap on b.z ^ a.z:
	separVec = glm::cross(separVecs[2], separVecs2[2]);

	overlap = glm::maxLengthDot(centerToCenter, separVec)
		- (glm::maxLengthDot(b1Vertices[getMaxProjectionVertex(b1Vertices, separVec)], separVec)
			+ glm::maxLengthDot(b2Vertices[getMaxProjectionVertex(b2Vertices, separVec)], separVec));
	if (overlap > 0.0f) return false;
	if (overlap > mtvLength)
	{
		mtvLength = overlap;
		separ = separVec * mtvLength;
	}


	return true;
}







//========================================================================================================






//take two boxes and their respective model matrices as input, and also returns a separation vector
//in the fifth parameter. Returns true if penetration or collision happened
inline bool detectBoxToBox(const Box& b1, const Box& b2, const glm::mat4& model1, 
							const glm::mat4& model2, glm::vec3& separ) noexcept
{
	glm::vec3 b1Vertices[8]; //b1 vertices in world space
	glm::vec3 b2Vertices[8]; //b2 vertices in world space
	glm::vec3 separVecs[3]; //the vectors used to test overlap, they will be the three positive normals from b1

	FLOAT_TYPE separVecSize = 0.0f;

	//set the values for the vertices
	for (int i = 0; i < 8; ++i)
	{
		b1Vertices[i] = model1 * glm::vec4(b1.getVertex(i), 1.0f);
		b2Vertices[i] = model2 * glm::vec4(b2.getVertex(i), 1.0f);
	}

	//set the values for the separation vectors:
	glm::mat4 normalMatrix = model1;

	separVecs[0] = glm::normalize(normalMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)); //the w value is 0.0f because it is a direction vector
	separVecs[1] = glm::normalize(normalMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	separVecs[2] = glm::normalize(normalMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

	

	//-------------------------------------------
	//Test overlap in the first separation vector(the b1's local x axis):
	std::pair<FLOAT_TYPE, FLOAT_TYPE> b1Projections;
	std::pair<FLOAT_TYPE, FLOAT_TYPE> b2Projections;

	//set the min and max projections for b2:
	std::pair<int, int> b2Indexes = getMinAndMaxProj(b2Vertices, separVecs[0], b2Projections);
	
	//set the min and max projections for b1(since the separVecs are normals of b1 sides, there's no need to test each vertex):
	std::pair<int, int> b1Indexes = { 1, 0 };
	b1Projections.first = glm::dot(b1Vertices[1], separVecs[0]); //the min projection
	b1Projections.second = glm::dot(b1Vertices[0], separVecs[0]); //the max projection


	//test overlap:
	if (b2Projections.first > b1Projections.second || b1Projections.first > b2Projections.second)
		return false;
	FLOAT_TYPE overlap = glm::min(b1Projections.second, b2Projections.second) - glm::max(b1Projections.first, b2Projections.first);
	/*(b2Projections.first < b1Projections.second)
		? b2Projections.first - b1Projections.second
		: b1Projections.first - b2Projections.second;*/
	//glm::min(b2Projections.first - b1Projections.second, b1Projections.first - b2Projections.second);
	separVecSize = overlap;
	separ = separVecSize * separVecs[0];

	//----------------------------------
	//Test overlap in the second separating vector(the b1's local y axis):

	
	//set the min and max projections for b2:
	b2Indexes = getMinAndMaxProj(b2Vertices, separVecs[1], b2Projections);

	//set the min and max projections for b1(since the separVecs are normals of b1 sides, there's no need to test each vertex):
	b1Indexes = { 2, 0 };
	b1Projections.first = glm::dot(b1Vertices[2], separVecs[1]); //the min projection
	b1Projections.second = glm::dot(b1Vertices[0], separVecs[1]); //the max projection

	//test overlap:
	if (b2Projections.first > b1Projections.second || b1Projections.first > b2Projections.second)
		return false;
	overlap = glm::min(b1Projections.second, b2Projections.second) - glm::max(b1Projections.first, b2Projections.first);
	if (fabs(overlap) < fabs(separVecSize)) { separVecSize = overlap; separ = separVecSize * separVecs[1]; };

	//---------------------------------
	//Test overlap in the third separating vector(the b1's local z axis):
	
	//set the min and max projections for b2:
	b2Indexes = getMinAndMaxProj(b2Vertices, separVecs[2], b2Projections);

	//set the min and max projections for b1(since the separVecs are normals of b1 sides, there's no need to test each vertex):
	b1Indexes = { 0, 4 };
	b1Projections.first = glm::dot(b1Vertices[0], separVecs[2]); //the min projection
	b1Projections.second = glm::dot(b1Vertices[4], separVecs[2]); //the max projection

	//test overlap:
	if (b2Projections.first > b1Projections.second || b1Projections.first > b2Projections.second)
		return false;
	overlap = glm::min(b1Projections.second, b2Projections.second) - glm::max(b1Projections.first, b2Projections.first);
	if (fabs(overlap) < fabs(separVecSize)) { separVecSize = overlap; separ = separVecSize * separVecs[2]; }







	/*

	//=========================================================================================================
	
	normalMatrix = model2;

	separVecs[0] = glm::normalize(normalMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)); //the w value is 0.0f because it is a direction vector
	separVecs[1] = glm::normalize(normalMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	separVecs[2] = glm::normalize(normalMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

	//-------------------------------------------
	//Test overlap in the first separation vector(the b1's local x axis):
	//std::pair<FLOAT_TYPE, FLOAT_TYPE> b1Projections;
	//std::pair<FLOAT_TYPE, FLOAT_TYPE> b2Projections;

	//set the min and max projections for b1:
	b1Indexes = getMinAndMaxProj(b1Vertices, separVecs[0], b1Projections);

	//set the min and max projections for b1(since the separVecs are normals of b1 sides, there's no need to test each vertex):
	b2Indexes = { 1, 0 };
	b2Projections.first = glm::dot(b2Vertices[1], separVecs[0]); //the min projection
	b2Projections.second = glm::dot(b2Vertices[0], separVecs[0]); //the max projection

	
	//test overlap:
	if (b1Projections.first > b2Projections.second || b2Projections.first > b1Projections.second)
		return false;
	overlap = glm::min(b2Projections.second, b1Projections.second) - glm::max(b2Projections.first, b1Projections.first);
	if (fabs(overlap) < fabs(separVecSize)) { separVecSize = overlap; separ = separVecSize * separVecs[0]; }
	

	//----------------------------------
	//Test overlap in the second separating vector(the b1's local y axis):


	//set the min and max projections for b2:
	b1Indexes = getMinAndMaxProj(b1Vertices, separVecs[1], b1Projections);

	//set the min and max projections for b1(since the separVecs are normals of b1 sides, there's no need to test each vertex):
	b2Indexes = { 2, 0 };
	b2Projections.first = glm::dot(b2Vertices[2], separVecs[1]); //the min projection
	b2Projections.second = glm::dot(b2Vertices[0], separVecs[1]); //the max projection

	//test overlap:
	if (b1Projections.first > b2Projections.second || b2Projections.first > b1Projections.second)
		return false;
	overlap = glm::min(b2Projections.second, b1Projections.second) - glm::max(b2Projections.first, b1Projections.first);
	if (fabs(overlap) < fabs(separVecSize)) { separVecSize = overlap; separ = separVecSize * separVecs[1]; };

	//---------------------------------
	//Test overlap in the third separating vector(the b1's local z axis):

	//set the min and max projections for b2:
	b1Indexes = getMinAndMaxProj(b1Vertices, separVecs[2], b1Projections);

	//set the min and max projections for b1(since the separVecs are normals of b1 sides, there's no need to test each vertex):
	b2Indexes = { 0, 4 };
	b2Projections.first = glm::dot(b2Vertices[0], separVecs[2]); //the min projection
	b2Projections.second = glm::dot(b2Vertices[4], separVecs[2]); //the max projection

	//test overlap:
	if (b1Projections.first > b2Projections.second || b2Projections.first > b1Projections.second)
		return false;
	overlap = glm::min(b2Projections.second, b1Projections.second) - glm::max(b2Projections.first, b1Projections.first);
	if (fabs(overlap) < fabs(separVecSize)) { separVecSize = overlap; separ = separVecSize * separVecs[2]; }

	*/
	//overlap happened:

	//std::cout << "(" << glm::length(separVecs[0]) << ", " << glm::length(separVecs[1]) << ", " << glm::length(separVecs[2]) << ")\n";
	return true;
}


//#############################################################################################
//Collision response


//take two spheres, and and separation vector as parameter. Also assumes the separation
//vector goes from sphere 1 to sphere 2
inline void resolveSphereToSphere(RigidBodyComponent<Sphere>& sp1, RigidBodyComponent<Sphere>& sp2, 
									glm::vec3 separVec) noexcept
{
	//unless separVec is the zero vector, the two spheres are overlaping, so correct this first:
	FLOAT_TYPE totalMass = sp1.mass + sp2.mass;
	sp1.move(-separVec * (sp2.mass / totalMass)); //the more mass the other sphere have, the more this will be moved
	sp2.move(separVec * (sp1.mass / totalMass)); //the same here

	//now add velocity due to collision
	glm::vec3 normal = glm::length(separVec) >= 0.001f 
		? glm::normalize(separVec) 
		: glm::normalize(sp2.getPosition() - sp1.getPosition());

	//get the relative velocity
	glm::vec3 relVel = sp2.linearVelocity - sp1.linearVelocity; 

	FLOAT_TYPE impulse = ((1.0f - 3.0f) * glm::dot(relVel, normal)) / (1.0f / sp1.mass + 1.0f / sp2.mass);
	//std::cout << sp1.linearVelocity.z << '\n';
	sp1.linearVelocity -= impulse * normal / sp1.mass;
	sp2.linearVelocity += impulse * normal / sp2.mass;
}



//========================================================================================================



inline void resolveBoxToBox(RigidBodyComponent<Box>& box1, RigidBodyComponent<Box>& box2, 
							glm::vec3 separVec) noexcept
{
	if (glm::dot(separVec, box1.getPosition() - box2.getPosition()) >= 0) separVec *= -1.0f;
	if (glm::length(separVec) <= 0.001f) return;

	//correct penetration:
	FLOAT_TYPE totalMass = (box1.mass > 0.0 ? box1.mass : 0.0f) + (box2.mass > 0.0 ? box2.mass : 0.0f);
	if (totalMass > 0.0f)
	{
		if (box1.mass > 0.0f && box2.mass > 0.0f)
		{
			box1.move(-separVec * (box2.mass / totalMass)); //the more mass the other box have, the more this will be moved
			box2.move(separVec * (box1.mass / totalMass)); //the same here
		}
		else
		{
			box1.move(-separVec * (box1.mass > 0.0f ? 1.0f : 0.0f));
			box2.move(separVec * (box2.mass > 0.0f ? 1.0f : 0.0f));
		}
	}
	
	//now add velocity due to collision
	glm::vec3 normal = glm::length(separVec) >= 0.0001f
		? glm::normalize(separVec)
		: glm::normalize(box2.getPosition() - box1.getPosition());

	//-----------------------------
	//apply the impulse:

	//get the relative velocity
	glm::vec3 relVel = box2.linearVelocity - box1.linearVelocity;

	
	if (glm::dot(relVel, normal) >= 0.0) return; //they aren't moving toward each other

	FLOAT_TYPE impulse = ((1.0f - 0.2f) * glm::dot(relVel, normal)) / 
		(1.0f / (box1.mass > 0.0f ? box1.mass : 1.0f) + (1.0f / (box2.mass > 0 ? box2.mass : 1.0f)));
	
	if(box1.mass > 0.0f)
		box1.linearVelocity += impulse * normal / box1.mass;
	if (box2.mass > 0.0f)
		box2.linearVelocity -= impulse * normal / box2.mass;

	//-----------------------------
	//compute and apply friction:
	
	if (fabs(glm::dot(relVel, normal)) < 0.001f) return; //don't compute friction

	
	glm::vec3 tangent = relVel - (glm::dot(relVel, normal) * normal);
	if(glm::length(tangent) < 0.0001) return; 
	tangent = glm::normalize(tangent);
	
	if (box1.mass > 0.0f)
		box1.linearVelocity -= (0.8f * impulse) * tangent / box1.mass;
	if (box2.mass > 0.0f)
		box2.linearVelocity += (0.8f * impulse) * tangent / box2.mass;
}



//========================================================================================================

inline void resolveBoxToObjBox(const Box& box1, FLOAT_TYPE box1Mass, FLOAT_TYPE box1Vel, 
								RigidBodyComponent<Box>& box2, glm::vec3 separVec, 
								glm::vec3& holderResponseImpulse) noexcept
{
	if (glm::dot(separVec, box1.pos - box2.getPosition()) >= 0) separVec *= -1.0f;
	if (glm::length(separVec) <= 0.001f) return;

	//correct penetration:
	FLOAT_TYPE totalMass = (box1Mass > 0.0 ? box1Mass : 0.0f) + (box2.mass > 0.0 ? box2.mass : 0.0f);
	if (totalMass > 0.0f)
	{
		if (box1Mass > 0.0f && box2.mass > 0.0f)
		{
			//holderResponsePos += (-separVec * (box2.mass / totalMass)); //the more mass the other box have, the more this will be moved
			box2.move(separVec * (box1Mass / totalMass)); //the same here
		}
		else
		{
			//holderResponsePos += (-separVec * (box1Mass > 0.0f ? 1.0f : 0.0f));
			box2.move(separVec * (box2.mass > 0.0f ? 1.0f : 0.0f));
		}
	}

	//now add velocity due to collision
	glm::vec3 normal = glm::length(separVec) >= 0.0001f
		? glm::normalize(separVec)
		: glm::normalize(box2.getPosition() - box1.pos);

	//-----------------------------
	//apply the impulse:

	//get the relative velocity
	glm::vec3 relVel = box2.linearVelocity - (glm::normalize(box2.getPosition() - box1.pos) * box1Vel);


	if (glm::dot(relVel, normal) >= 0.0) return; //they aren't moving toward each other

	FLOAT_TYPE impulse = ((1.0f - 0.2f) * glm::dot(relVel, normal)) /
		(1.0f / (box1Mass > 0.0f ? box1Mass : 1.0f) + (1.0f / (box2.mass > 0 ? box2.mass : 1.0f)));

	//if (box1Mass > 0.0f)
		holderResponseImpulse = impulse * normal;
	if (box2.mass > 0.0f)
		box2.linearVelocity -= impulse * normal / box2.mass;

	//-----------------------------
	//compute and apply friction:

	if (fabs(glm::dot(relVel, normal)) < 0.001f) return; //don't compute friction


	glm::vec3 tangent = relVel - (glm::dot(relVel, normal) * normal);
	if (glm::length(tangent) < 0.0001) return;
	tangent = glm::normalize(tangent);

	//if (box1.mass > 0.0f)
	//	box1.linearVelocity -= (0.8f * impulse) * tangent / box1.mass;
	if (box2.mass > 0.0f)
		box2.linearVelocity += (0.8f * impulse) * tangent / box2.mass;
}





#endif // !COLLISION_HANDLING
