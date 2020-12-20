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

#ifndef LIGHT_COMPONENT
#define LIGTH_COMPONENT


#include "Entity.h"
#include "Texture.h"

#include "GlobalDefines.h"

#include <cassert>
#include <string>
#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class DirLightComponent : public Component
{
public:
	DirLightComponent();

	void clearMemory(); //clear the depthTexture memory from the GPU
	void setColor(glm::vec3) noexcept;
	void update() noexcept;
	
private:

	friend class GraphicalSystem;

	//constructor:
	DirLightComponent(Entity, int, bool = true);

	//functions:
	void updateMatrix(glm::vec3) noexcept;

	//Light data:
	glm::vec3 lightAmbient = glm::vec3(0.1f);
	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 direction;
	glm::vec3 position;
	glm::mat4 lightMatrix = glm::mat4(1.0f);

	//depth buffer data:
	unsigned int depthTexture;
	int widht;
	int height;
	bool shadowCaster = true; //this light will only produce shadows if this is set to true
};



//###########################################################################



class PointLightComponent : public Component
{
public:
	
	PointLightComponent();

	void clearMemory(); //clear the depthTexture memory from the GPU
	void update() noexcept;

private:

	friend class GraphicalSystem;

	//Constructor:
	PointLightComponent(Entity, int, FLOAT_TYPE, FLOAT_TYPE);


	//functions:
	void updateMatrices(glm::vec3) noexcept;


	//Light data:
	glm::vec3 lightAmbient = glm::vec3(0.1f);
	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 position;
	FLOAT_TYPE constantAttenuation = 1.0f;
	FLOAT_TYPE linearAttenuation = 0.027f;
	FLOAT_TYPE quadraticAttenuation = 0.0028f;
	FLOAT_TYPE radius = 0.0f;

	glm::mat4 posXDepthMapMatrix;
	glm::mat4 negXDepthMapMatrix;
	glm::mat4 posYDepthMapMatrix;
	glm::mat4 negYDepthMapMatrix;
	glm::mat4 posZDepthMapMatrix;
	glm::mat4 negZDepthMapMatrix;


	//depth buffer data
	unsigned int depthCubeMap;
	int width;
	int height;
};

#endif // !LIGHT_COMPONENT
