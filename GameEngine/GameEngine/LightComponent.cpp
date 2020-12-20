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

#include "LightComponent.h"


//dirLightComponent definitions:

DirLightComponent::DirLightComponent()
	:Component{ -1 }
{

}


//####################################################################################################

DirLightComponent::DirLightComponent(Entity id, int shadowRes, bool sc)
	:Component{ id }
{
	widht = shadowRes;
	height = shadowRes;
	shadowCaster = sc;

	if (sc)
	{
		//intialize the depth buffer:
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowRes, shadowRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		FLOAT_TYPE borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

}

//####################################################################################################

void DirLightComponent::updateMatrix(glm::vec3 newPos) noexcept
{
	//don't update the matrix if the new position is almost the same
	
	if (std::fabs(glm::length(newPos - position)) < 4.0f) return;

	position = newPos;
	
	lightMatrix = glm::ortho(-600.0f, 600.0f, -600.0f, 600.0f, 0.1f, 600.0f) *
		glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

//####################################################################################################

void DirLightComponent::clearMemory() //clear the depthTexture memory
{
	if (shadowCaster && depthTexture);
		glDeleteTextures(1, &depthTexture);
}

//####################################################################################################

void DirLightComponent::setColor(glm::vec3 color) noexcept
{
	lightColor = color;
}

//####################################################################################################

void DirLightComponent::update() noexcept
{

}


//######################################################################
//PointLightComponent definitions:


void PointLightComponent::clearMemory()
{
	if (depthCubeMap)
		glDeleteTextures(1, &depthCubeMap);
}

//####################################################################################################

void PointLightComponent::update() noexcept
{

}

//####################################################################################################

PointLightComponent::PointLightComponent()
	:Component{ -1 }
{

}


//####################################################################################################


PointLightComponent::PointLightComponent(Entity id, int shadowRes, FLOAT_TYPE lin, FLOAT_TYPE quadratic)
	:Component{ id }, linearAttenuation{ lin }, quadraticAttenuation{ quadratic }
{
	width = shadowRes;
	height = shadowRes;
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (int i = 0; i < 6; ++i) //initialize each side of the cube map:
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowRes, shadowRes, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//####################################################################################################

void PointLightComponent::updateMatrices(glm::vec3 newPos) noexcept
{
	//see if the position was not changed and if the light position was not configured before the call to this function
	if (newPos == position && position != glm::vec3(0.0f)) return;

	position = newPos;
	glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 400.0f);

	//update all matrices:
	posXDepthMapMatrix = lightProj * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f));

	negXDepthMapMatrix = lightProj * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f));

	posYDepthMapMatrix = lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));

	negYDepthMapMatrix = lightProj * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));

	posZDepthMapMatrix = lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f));

	negZDepthMapMatrix = lightProj * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f));
}