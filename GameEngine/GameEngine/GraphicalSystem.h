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
GraphicalSystem and the ShaderProgram classes.
*/
//#############################################################################################


#ifndef GRAPHICAL_SYSTEM
#define GRAPHICAL_SYSTEM

#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "World.h"
#include "Entity.h"
#include "ImageComponent.h"
#include "Observer.h"
#include "TextureHandler.h"
#include "ModelComponent.h"
#include "InteractableObjectComponent.h"

#include "GlobalDefines.h"

//#######################################################################################################
//utility functions:

void compileShader(unsigned int);



//#######################################################################################################
//ShaderProgram class:



class ShaderProgram
{
public:
	ShaderProgram(std::string, std::string, std::string, bool);

	//Functions:
	unsigned int getId() const noexcept;

	void setBool(std::string, bool);
	void setInt(std::string, int);
	void setFLOAT_TYPE(std::string, FLOAT_TYPE);
	void setVec2(std::string, FLOAT_TYPE, FLOAT_TYPE);
	void setVec3(std::string, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE);
	void setVec3(std::string, glm::vec3);
	void setMat3(std::string, glm::mat3);
	void setVec4(std::string, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE);
	void setMat4(std::string, const glm::mat4&);

private:

	//Data:
	unsigned int id;
};

//#######################################################################################################
//The GraphicalSystem class:


class GraphicalSystem : public Observer
{
public:
	//Functions:
	void initialize(World*, GLFWwindow*);

	void onNotify(Message) override;
	void addNormalMap(Entity, const Texture*);
	void addEmissionMap(Entity, const Texture*);
	void addImageComponent(Entity, const Texture*, int, int);
	void addDirLightComponent(Entity, glm::vec3, glm::vec3, bool);
	void addPointLightComponent(Entity, glm::vec3, FLOAT_TYPE, FLOAT_TYPE);
	void addModelComponent(Entity, const Model*);
	void render(int, int, int); //this must be called once per frame
	void update();
	
	//----------------------------------------------------
	//public Data:

	//Video Settings:
	int shadowResolution = 512;
	glm::vec2 bufferDefaultSize = glm::vec2(1280, 720);
	int framesPerSecond = 60;
	bool vsync = false;
	bool gammaCorrection = false;
	bool bloom = true;
	bool blur = true;
	bool lowQualityRendering = true;

	//other data:
	glm::vec3 camPosition;
	glm::vec3 camDirection;
	const glm::vec3 defaultCamPosition = glm::vec3(0.0f, 200.0f, 0.0f);


private:


	//-------------------------------------------------
	//Private data:
	World* world; //keep a pointer to the World, so that it's components can be accessed from here
	GLFWwindow* window;
	std::vector<ShaderProgram> programs;
	int screenOffset, screenWidth, screenHeight;

	//Camera:

	glm::mat4 cameraView;
	glm::mat4 projection;

	//------------------------------------
	//Drawing data:
	Sprite cursor;
	glm::vec2 cursorPos;

	//sprite data:
	unsigned int spriteVAO;
	unsigned int spriteVBO;

	unsigned int cubeVAO;
	unsigned int cubeVBO;

	//additional drawing data:
	unsigned int sphereVAO;
	unsigned int sphereVBO;
	unsigned int sphereEBO;
	int sphereIndicesSize;
	
	//G-Buffer:
	unsigned int gBuffer; //used to store scene data(e.g. positions, normals, albedo)
	unsigned int gPositions;
	unsigned int gNormals;
	unsigned int gAlbedoSpecular; //albedo + specular color buffer
	unsigned int gEmission;
	unsigned int gDepthAndStencil;
	

	//offscreen drawing data:
	unsigned int offScreenFrameBuffer;
	unsigned int colorAttachment;
	unsigned int bloomColorAttachment; //color buffer used to apply bloom
	unsigned int depthStencilAttachment;
	unsigned int offscreenVAO; //the data of the squad used to draw the post-processed texture
	unsigned int offscreenVBO;


	//lighning data:
	unsigned int shadowFrameBuffer; //used to set the depth map for each light component
	unsigned int shadowColorBuffer; 

	unsigned int lightningFrameBuffer;
	unsigned int lightMapTexture;
	unsigned int lightMapDepthTexture;

	//blurring data:
	unsigned int pingPongFramebuffers[2]; //framebuffers for bloom
	unsigned int pingPongColorAttachments[2]; //textures for bloom

	unsigned int blurPingPongFrameBuffers[2]; //framebuffers for blur
	unsigned int blurPingPongColorAttachments[2]; //textures for blur

	//optimization related data:
	std::vector<std::pair<glm::mat4, int>> scaledFullTransforms;
	glm::mat4 identityMatrix = glm::mat4(1.0f);

	//Particles data:
	unsigned int particleVBO;
	unsigned int particleVAO;

	//------------------------------------------------
	//Private functions:


	void reloadTransforms(); //clear and refill scaledFullTransforms
	glm::mat4 getScaledFullTransfom(Entity) const;
	//the reloadTransforms and getScaledFullTransforms functions ensures that the full transform of each ImageComponent
	//or modelComponent is computed only one time per frame

	std::vector<unsigned int> genSphereVertices(FLOAT_TYPE*, int); //generate vertices, normals, textures coordinates for a sphere.
					//They are stored in the first argument. The second argumment is the size of the first and the return value
					//is a vector containing the indices

	glm::mat4 normalizeRows(int, glm::mat4) const noexcept;

	glm::mat4 getFullTransform(Entity, FLOAT_TYPE, FLOAT_TYPE, FLOAT_TYPE = 1.0f) const; //returns the objToWorld transformation matrix(taking into
	//account game objects hierachy. The matrix will be scaled by the tree last arguments)

	glm::mat4 getFullTransform2(Entity) const; //returns the objToWorld transformation matrix(taking into
	//account game objects hierachy)

	glm::quat getFullRotationQuaternion(Entity) const;


	void renderDepthMaps(); //generate shadow maps for each light component in the scene
	void renderLightMap();
	void applyBloom();
	void applyBlur();
	void drawHuds(int, int, int);
	void drawCursor(int, int, int);
	void drawPhysicsBoxes(int, int, int);

	//----------------------
	/*
		renderScene - render the entire scene(except the lightning and post-processing effects,
	//those are handled in the render function) with a specific shader program and point of view(the 2nd param);
	*/
	void renderScene(ShaderProgram&, const glm::mat4&, bool useNormalMap = false, 
		bool useEmissionMap = false, int billboard = false); 
	

	void renderSceneImages(ShaderProgram&, const glm::mat4&, bool useNormalMap = false, 
							bool useEmissionMap = false, int billboard = false);
	void renderSceneModels(ShaderProgram&, const glm::mat4&, bool useNormalMap = false, 
							bool useEmissionMap = false, int billboard = false);
	void renderSceneIntObjects(ShaderProgram&, const glm::mat4&, bool useNormalMap = false, 
							bool useEmissionMap = false, int billboard = false);
	void renderSceneCharacterModels(ShaderProgram&, const glm::mat4&, bool useNormalMap = false, 
							bool useEmissionMap = false, int billboard = false);

	//------------------------
	void renderSceneGeometry(ShaderProgram&, const glm::mat4&); //render just to the depth attachment of the current bound framebuffer

	void renderSceneImagesGeometry(ShaderProgram&, const glm::mat4&);
	void renderSceneModelsGeometry(ShaderProgram&, const glm::mat4&);
	void renderSceneIntObjectsGeometry(ShaderProgram&, const glm::mat4&);
	void renderSceneCharacterModelsGeometry(ShaderProgram&, const glm::mat4&);

	void renderDirLights(int, int, int);
	void renderPointLights(int, int, int);
	void renderEmissionMaps(int, int, int); //this function renders the emmission maps from the GBuffer to the
											//buffer used for bloom
	void renderParticles(int, int, int);
};


#endif // !GRAPHICAL_SYSTEM
