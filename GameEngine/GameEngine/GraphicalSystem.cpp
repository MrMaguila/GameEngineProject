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

#include "GraphicalSystem.h"




void compileShader(unsigned int shaderId)
{
	glCompileShader(shaderId);
	int success;
	char log[256];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) //error in compilation
	{
		glGetShaderInfoLog(shaderId, 256, nullptr, log);
		std::cerr << "->ERROR::SHADER COMPILATION FAILURE; INFOLOG: " << log << '\n';
	}
}



//#########################################################################################################
//ShaderProgram definitions:


ShaderProgram::ShaderProgram(std::string vs, std::string gs, std::string fs, bool geometry)
//create a opengl shader program
{
	std::string vertexCode;
	std::string geometryCode;
	std::string fragmentCode;
	std::ifstream vsFileReader;
	std::ifstream gsFileReader;
	std::ifstream fsFileReader;
	std::stringstream vsStringStream;
	std::stringstream gsStringStream;
	std::stringstream fsStringStream;

	vsFileReader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gsFileReader.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fsFileReader.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	//read the vertex shader code from file:
	vsFileReader.open(vs);
	vsStringStream << vsFileReader.rdbuf();
	vsFileReader.close();
	vertexCode = vsStringStream.str();

	//now the geometry shader code:
	if (geometry) //the geometry shader is optional
	{
		gsFileReader.open(gs);
		gsStringStream << gsFileReader.rdbuf();
		gsFileReader.close();
		geometryCode = gsStringStream.str();
		//gsStringStream.clear();
	}

	//and finally the fragment shader code:
	fsFileReader.open(fs);
	fsStringStream << fsFileReader.rdbuf();
	fsFileReader.close();
	fragmentCode = fsStringStream.str();

	//convert to c-string:
	const char* vsCode = vertexCode.c_str();
	const char* gsCode = (geometry) ? geometryCode.c_str() : "\0";
	const char* fsCode = fragmentCode.c_str();


	//now create and compile the shaders:
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsCode, nullptr);
	compileShader(vertexShader);

	unsigned int geometryShader = 0;
	if (geometry) 
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &gsCode, nullptr);
		compileShader(geometryShader);
	}

	unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsCode, nullptr);
	compileShader(fragmentShader);


	//and them create the program:
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	if (geometry) glAttachShader(id, geometryShader);
	glAttachShader(id, fragmentShader);


	static int shaderNumber = 0;

	//link it:
	glLinkProgram(id);
	int success;
	char log[256];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 256, nullptr, log);
		std::cout << "->ERROR::SHADER PROGRAM WAS NOT SUCCESSFULLY LINKED;" << shaderNumber <<" ;INFO LOG: " << log << '\n';
	}
	++shaderNumber;


	//delete the shaders:
	glDeleteShader(vertexShader);
	if (geometry) glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

}

//-----------------------------------------------------------------------------------------------------------
//Functions:

unsigned int ShaderProgram::getId() const noexcept
{
	return id;
}

//-----------------------------------------------------------------------------------------------------------
//Uniform configuration functions

void ShaderProgram::setBool(std::string uniformName, bool val)
{
	glUniform1i(glGetUniformLocation(id, uniformName.c_str()), val);
}

void ShaderProgram::setInt(std::string uniformName, int val)
{
	glUniform1i(glGetUniformLocation(id, uniformName.c_str()), val);
}

void ShaderProgram::setFLOAT_TYPE(std::string uniformName, FLOAT_TYPE val)
{
	glUniform1f(glGetUniformLocation(id, uniformName.c_str()), val);
}

void ShaderProgram::setVec2(std::string uniformName, FLOAT_TYPE val1, FLOAT_TYPE val2)
{
	glUniform2f(glGetUniformLocation(id, uniformName.c_str()), val1, val2);
}

void ShaderProgram::setVec3(std::string uniformName, FLOAT_TYPE val1, FLOAT_TYPE val2, FLOAT_TYPE val3)
{
	glUniform3f(glGetUniformLocation(id, uniformName.c_str()), val1, val2, val3);
}

void ShaderProgram::setVec3(std::string uniformName, glm::vec3 val)
{
	glUniform3f(glGetUniformLocation(id, uniformName.c_str()), val.x, val.y, val.z);
}

void ShaderProgram::setMat3(std::string uniformName, glm::mat3 val)
{
	glUniformMatrix3fv(glGetUniformLocation(id, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setVec4(std::string uniformName, FLOAT_TYPE val1, FLOAT_TYPE val2, FLOAT_TYPE val3, FLOAT_TYPE val4)
{
	glUniform4f(glGetUniformLocation(id, uniformName.c_str()), val1, val2, val3, val4);
}

void ShaderProgram::setMat4(std::string uniformName, const glm::mat4& val)
{
	glUniformMatrix4fv(glGetUniformLocation(id, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}







//#############################################################################################################
//GraphicalSystem definitions:




void GraphicalSystem::initialize(World* w, GLFWwindow* win)
{
	//set the world:
	myAssert(w);
	world = w;
	myAssert(win);
	window = win;

	//=====================================================
	//load shaders:
	programs.push_back(ShaderProgram("Assets/Shaders/SpriteVertexShader.vs", "",   //0
		"Assets/Shaders/SpriteFragmentShader.fs", false)); //false means it does not have a geometry shader

	programs.push_back(ShaderProgram("Assets/Shaders/shadowMapRendering/DirLightDepthVertexShader.vs", "", //1
		"Assets/Shaders/shadowMapRendering/DirLightDepthFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/PostProcessedSquadVertexShader.vs", "", //2
		"Assets/Shaders/PostProcessedSquadFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/LightningVertexShader.vs", "", //3
		"Assets/Shaders/LightningFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/lightMapRendering/RenderDirLightVertexShader.vs", "", //4
		"Assets/Shaders/lightMapRendering/RenderDirLightFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/RenderSceneVertexShader.vs", "", //5
		"Assets/Shaders/RenderSceneFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/shadowMapRendering/PointLightDepthVertexShader.vs", //6
		"Assets/Shaders/shadowMapRendering/PointLightDepthGeometryShader.gs", 
		"Assets/Shaders/shadowMapRendering/PointLightDepthFragmentShader.fs", true));

	programs.push_back(ShaderProgram("Assets/Shaders/lightMapRendering/RenderPointLightVertexShader.vs", "", //7
		"Assets/Shaders/lightMapRendering/RenderPointLightFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/blurring/blurringVertexShader.vs", "", //8
		"Assets/Shaders/blurring/blurringFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/blurring/blurringVertexShader2.vs", "", //9
		"Assets/Shaders/blurring/blurringFragmentShader2.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/blurring/blurringVertexShader3.vs", "", //10
		"Assets/Shaders/blurring/blurringFragmentShader3.fs", false));
	
	programs.push_back(ShaderProgram("Assets/Shaders/interfaceRendering/DefaultHudVertexShader.vs", "", //11
		"Assets/Shaders/interfaceRendering/DefaultHudFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/interfaceRendering/MouseCursorVertexShader.vs", "", //12
		"Assets/Shaders/interfaceRendering/MouseCursorFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/interfaceRendering/PhysicsVertexShader.vs", "", //13
		"Assets/Shaders/interfaceRendering/PhysicsFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/deferredShading/RenderToGBufferVertexShader.vs", "", //14
		"Assets/Shaders/deferredShading/RenderToGBufferFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/deferredShading/RenderSceneVertexShader.vs", "", //15
		"Assets/Shaders/deferredShading/RenderSceneFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/deferredShading/ApplyAmbientVertexShader.vs", "", //16
		"Assets/Shaders/deferredShading/ApplyAmbientFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/deferredShading/StencilPassVertexShader.vs", "", //17
		"Assets/Shaders/deferredShading/StencilPassFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/deferredShading/RenderDirLightVertexShader.vs", "", //18
		"Assets/Shaders/deferredShading/RenderDirLightFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/deferredShading/RenderEmissionMapVertexShader.vs", "", //19
		"Assets/Shaders/deferredShading/RenderEmissionMapFragmentShader.fs", false));

	programs.push_back(ShaderProgram("Assets/Shaders/particleRendering/pointParticleVertexShader.vs", "", //20
		"Assets/Shaders/particleRendering/pointParticleFragmentShader.fs", false));


	//====================================================
	//intialize camera:
	camPosition = glm::vec3(0.0f, 160.0f, 0.0f);
	camDirection = glm::vec3(0.00f, -2.0f, 2.0f);
	cameraView = glm::lookAt(camPosition, camDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	projection = glm::perspective(FLOAT_TYPE(glm::radians(45.0f)), 16.0f/ 9.0f, 0.1f, 1080.0f);
	//projection = glm::ortho(-192.0f, 192.0f, -108.0f, 108.0f, 0.1f, 1080.0f);


	//#####################################################
	//Initialize drawing data:

	//--------------------------------------------------------
	//get a texture for the cursor sprite:
	cursor.initialize(TextureHandler::instance().get(13), 1, 1);

	//------------------
	//Create the G-Buffer

	
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//---------------
	//generate the position color buffer
	glGenTextures(1, &gPositions);
	glBindTexture(GL_TEXTURE_2D, gPositions);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositions, 0);

	//---------------
	//generate a color buffer for normals and metallic(rgb for normals and a for metallic)
	glGenTextures(1, &gNormals);
	glBindTexture(GL_TEXTURE_2D, gNormals);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormals, 0);

	//---------------
	//generate the color + specular color buffer(the specular component is used for roughness)
	glGenTextures(1, &gAlbedoSpecular);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpecular, 0);

	//---------------
	//generate the emission map color buffer
	glGenTextures(1, &gEmission);
	glBindTexture(GL_TEXTURE_2D, gEmission);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmission, 0);

	//---------------
	//set the four color buffers as the attachments for the g buffer
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);
	
	//---------------
	//create and attach a texture for the depth and stencil attachment
	glGenTextures(1, &gDepthAndStencil);
	glBindTexture(GL_TEXTURE_2D, gDepthAndStencil);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_DEPTH_STENCIL, //!
		GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gDepthAndStencil, 0);

	//check the gBuffer status
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	//unbing everything:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//=================================================
	//---------------
	//Sprites data:



	FLOAT_TYPE sphereVertices[936];

	std::vector<unsigned int> sphereIndicesVec = genSphereVertices(sphereVertices, 936);
	std::cout << "Sphere indices size: "<< sphereIndicesVec.size() << '\n';
	myAssert(sphereIndicesVec.size() == 504);

	unsigned int sphereIndices[504];
	sphereIndicesSize = 504;
	for (int i = 0; i < 504; ++i)
		sphereIndices[i] = sphereIndicesVec[i];

	
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVBO);
	glGenBuffers(1, &sphereEBO);

	glBindVertexArray(sphereVAO);
	//configure the vbo
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//configure the ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereIndices), &sphereIndices, GL_STATIC_DRAW);


	//unbind the buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//------------------------------------------------------------------
	//compute the tangent of the first triangle for the sprite vertices:

	glm::vec3 tangent11;

	glm::vec3 pos1(-0.5f, 0.5f, 0.0f);
	glm::vec3 pos2(-0.5f, -0.5f, 0.0f);
	glm::vec3 pos3(0.5f, -0.5f, 0.0f);
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);

	glm::vec3 edge1 = pos2 - pos1; //2 - 1
	glm::vec3 edge2 = pos3 - pos1; //3 - 1
	glm::vec2 deltaUV1 = uv2 - uv1; //2 - 1
	glm::vec2 deltaUV2 = uv3 - uv1; //3 - 1

	FLOAT_TYPE f = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV2.x * deltaUV1.y));

	tangent11.x = f * ((deltaUV2.y * edge1.x) + (-deltaUV1.y * edge2.x));
	tangent11.y = f * ((deltaUV2.y * edge1.y) + (-deltaUV1.y * edge2.y));
	tangent11.z = f * ((deltaUV2.y * edge1.z) + (-deltaUV1.y * edge2.z));
	tangent11 = glm::normalize(tangent11);

	//now for the second triangle:

	glm::vec3 tangent12;

	pos1 = glm::vec3(-0.5f, 0.5f, 0.0f);
	pos2 = glm::vec3(0.5f, -0.5f, 0.0f);
	pos3 = glm::vec3(0.5f, 0.5f, 0.0f);
	uv1 = glm::vec2(0.0f, 1.0f);
	uv2 = glm::vec2(1.0f, 0.0f);
	uv3 = glm::vec2(1.0f, 1.0f);

	edge1 = pos2 - pos1;
	edge2 = pos3 - pos1;
	deltaUV1 = uv2 - uv1;
	deltaUV2 = uv3 - uv1;

	f = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV2.x * deltaUV1.y));

	tangent12.x = f * ((deltaUV2.y * edge1.x) + (-deltaUV1.y * edge2.x));
	tangent12.y = f * ((deltaUV2.y * edge1.y) + (-deltaUV1.y * edge2.y));
	tangent12.z = f * ((deltaUV2.y * edge1.z) + (-deltaUV1.y * edge2.z));
	tangent12 = glm::normalize(tangent12);

	//---------------------------------------------------------------------
	//now for the quad back face
	glm::vec3 tangent21;

	pos1 = glm::vec3(0.5f, -0.5f, 0.0f);
	pos2 = glm::vec3(-0.5f, -0.5f, 0.0f);
	pos3 = glm::vec3(-0.5f, 0.5f, 0.0f);
	uv1 = glm::vec2(1.0f, 0.0f);
	uv2 = glm::vec2(0.0f, 0.0f);
	uv3 = glm::vec2(0.0f, 1.0f);

	edge1 = pos2 - pos1; //2 - 1
	edge2 = pos3 - pos1; //3 - 1
	deltaUV1 = uv2 - uv1; //2 - 1
	deltaUV2 = uv3 - uv1; //3 - 1

	f = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV2.x * deltaUV1.y));

	tangent21.x = f * ((deltaUV2.y * edge1.x) + (-deltaUV1.y * edge2.x));
	tangent21.y = f * ((deltaUV2.y * edge1.y) + (-deltaUV1.y * edge2.y));
	tangent21.z = f * ((deltaUV2.y * edge1.z) + (-deltaUV1.y * edge2.z));
	tangent21 = glm::normalize(tangent21);

	//now for the second triangle:

	glm::vec3 tangent22;

	pos1 = glm::vec3(0.5f, -0.5f, 0.0f);
	pos2 = glm::vec3(-0.5f, 0.5f, 0.0f);
	pos3 = glm::vec3(0.5f, 0.5f, 0.0f);
	uv1 = glm::vec2(1.0f, 0.0f);
	uv2 = glm::vec2(0.0f, 1.0f);
	uv3 = glm::vec2(1.0f, 1.0f);

	edge1 = pos2 - pos1;
	edge2 = pos3 - pos1;
	deltaUV1 = uv2 - uv1;
	deltaUV2 = uv3 - uv1;

	f = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV2.x * deltaUV1.y));

	tangent22.x = f * ((deltaUV2.y * edge1.x) + (-deltaUV1.y * edge2.x));
	tangent22.y = f * ((deltaUV2.y * edge1.y) + (-deltaUV1.y * edge2.y));
	tangent22.z = f * ((deltaUV2.y * edge1.z) + (-deltaUV1.y * edge2.z));
	tangent22 = glm::normalize(tangent22);


	float quadData[] =
	{
		//first quad(front face):
		//Positions				//Normals			//texCoord	 //tangent							
		-0.5f, 0.5f, 0.28f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,	 tangent11.x, tangent11.y, tangent11.z,
		-0.5f, -0.5f, 0.28f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	 tangent11.x, tangent11.y, tangent11.z,
		0.5f, -0.5f, 0.28f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,	 tangent11.x, tangent11.y, tangent11.z,

		-0.5f, 0.5f, 0.28f,		0.0f, 0.0f, 1.0f,	0.0f, 1.0f,  tangent12.x, tangent12.y, tangent12.z,
		0.5f, -0.5f, 0.28f,		0.0f, 0.0f, 1.0f,	1.0f, 0.0f,  tangent12.x, tangent12.y, tangent12.z,
		0.5f, 0.5f, 0.28f,		0.0f, 0.0f, 1.0f,	1.0f, 1.0f,  tangent12.x, tangent12.y, tangent12.z,

		//second quad(back face):
		 0.5f, -0.5f, -0.28f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,	 tangent21.x, tangent21.y, tangent21.z,
		-0.5f, -0.5f, -0.28f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,	 tangent21.x, tangent21.y, tangent21.z,
		-0.5f, 0.5f, -0.28f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,	 tangent21.x, tangent21.y, tangent21.z,
				
		0.5f, -0.5f, -0.28f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,  tangent22.x, tangent22.y, tangent22.z,
		-0.5f, 0.5f, -0.28f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,  tangent22.x, tangent22.y, tangent22.z,
		0.5f, 0.5f, -0.28f,		0.0f, 0.0f, -1.0f,	1.0f, 1.0f,  tangent22.x, tangent22.y, tangent22.z
	};


	glGenVertexArrays(1, &spriteVAO);
	glGenBuffers(1, &spriteVBO);

	glBindVertexArray(spriteVAO);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(3 * sizeof(FLOAT_TYPE)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(6 * sizeof(FLOAT_TYPE)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11, (void*)(8 * sizeof(FLOAT_TYPE)));
	glEnableVertexAttribArray(3);

	//unbind everything:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//================================================


	float cubeVertices[] =
	{
		//positions				//normals				//Texture Coord	
	//back face
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 0.0f, // bottom-right    
		 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f, // top-right              
		 0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 1.0f, // top-right
		-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f, // bottom-left                
	// front face
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, // bottom-left
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		1.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		1.0f, 0.0f, // bottom-right        
		 0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		1.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,		0.0f, 1.0f, // top-left        
	// left face
		-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f, // top-right
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 1.0f, // top-left       
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, // bottom-left
		-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,		1.0f, 0.0f, // top-right
		-0.5f, -0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f, // bottom-right
	// right face
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f, 0.0f,		1.0f, 0.0f, // top-left
		 0.5f,  0.5f, -0.5f,	1.0f, 0.0f, 0.0f,		1.0f, 1.0f, // top-right      
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f, // bottom-right          
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f, 0.0f,		0.0f, 1.0f, // bottom-right
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, // bottom-left
		0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, // top-left
	// bottom face          
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f, // top-right
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,		1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		1.0f, 1.0f, // top-left        
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,		1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f, // top-right
		-0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f, // bottom-right
	// top face
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f, 0.0f,		0.0f, 1.0f, // top-left
		 0.5f,  0.5f, -0.5f,	0.0f,  1.0f, 0.0f,		1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f, 0.0f,		1.0f, 0.0f, // bottom-right                 
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f, 0.0f,		1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,	0.0f,  1.0f, 0.0f,		0.0f, 0.0f, // bottom-left  
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f, 0.0f,		0.0f, 1.0f  // top-left  
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//unbind everything:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//=================================================
	//Initialize the offScreen data:
	
	FLOAT_TYPE offScreenQuadData[]
	{
		//positions:			//texture Coordinates
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,

		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f
	};

	glGenVertexArrays(1, &offscreenVAO);
	glGenBuffers(1, &offscreenVBO);

	glBindVertexArray(offscreenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, offscreenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(offScreenQuadData), &offScreenQuadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float) ));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//==================================================================

	//create a framebuffer:
	glGenFramebuffers(1, &offScreenFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, offScreenFrameBuffer);

	//create and attach a texture for the color attachment
	glGenTextures(1, &colorAttachment);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGB, GL_FLOAT, nullptr); //!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);


	//create and attach another color attachment for the bloom effect:
	glGenTextures(1, &bloomColorAttachment);
	glBindTexture(GL_TEXTURE_2D, bloomColorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGB, GL_FLOAT, nullptr);  //!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomColorAttachment, 0);


	//tell opengl to use the two color buffers for drawing:
	unsigned int drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

	//create and attach a texture for the depth and stencil attachment
	glGenTextures(1, &depthStencilAttachment);
	glBindTexture(GL_TEXTURE_2D, depthStencilAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_DEPTH_STENCIL, //!
		GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilAttachment, 0);
	
	//check for errors:
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "->ERROR::FRAMEBUFFER NOT COMPLETE;\n";
		myAssert(false);
	}
	//unbind everything:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//=====================================================
	//create a frambuffer to handle shadows:
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind the framebuffer

	//=====================================================
	//create another frambuffer to handle lightning:
	glGenFramebuffers(1, &lightningFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lightningFrameBuffer);

	//create a color and depth texture for it:
	glGenTextures(1, &lightMapTexture);
	glBindTexture(GL_TEXTURE_2D, lightMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);  //!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightMapTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &lightMapDepthTexture);
	glBindTexture(GL_TEXTURE_2D, lightMapDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr); //!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, lightMapDepthTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind it

	//====================================================================
	//Create two pingpong framebuffers, they are used for the bloom effect:

	glGenFramebuffers(2, pingPongFramebuffers);
	glGenTextures(2, pingPongColorAttachments);

	for (int i = 0; i < 2; ++i) //initialize two textures and set them as the color attachments of the framebuffers
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFramebuffers[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongColorAttachments[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bufferDefaultSize.x / 4, bufferDefaultSize.y / 4, 0, GL_RGB, GL_FLOAT, NULL); //!

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongColorAttachments[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	//unbind everything:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//============================================================================
	//create another two pingpong framebuffers to handle blur:

	glGenFramebuffers(2, blurPingPongFrameBuffers);
	glGenTextures(2, blurPingPongColorAttachments);

	for (int i = 0; i < 2; ++i) //initialize two textures and set them as the color attachments of the framebuffers
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurPingPongFrameBuffers[i]);
		glBindTexture(GL_TEXTURE_2D, blurPingPongColorAttachments[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, bufferDefaultSize.x, bufferDefaultSize.y, 0, GL_RGB, GL_FLOAT, NULL); //!

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurPingPongColorAttachments[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	//----------------------------------------------------------

	//unbind everything:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//----------------------------------------------------------

	//initialize particle data:

	float particleData[3];
	particleData[0] = 0.0f;
	particleData[1] = 0.0f;
	particleData[2] = 0.0f;


	glGenVertexArrays(1, &particleVAO);
	glGenBuffers(1, &particleVBO);


	glBindVertexArray(particleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, particleData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	//unbind everything:
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}


//------------------------------------------------------------------------------------------------------



void GraphicalSystem::onNotify(Message msg)
{
	
	if (msg.type == MessageType::PLAY || msg.type == MessageType::STOP)
	{
		//std::cout << "NOTIFIED " << msg.fdata[0] << "\n";
		myAssert(world->currentScene->getEntity(msg.data));
		ImageComponent* imagComp = world->currentScene->getImageComponent(msg.data);
		myAssert(imagComp);
		msg.type == MessageType::PLAY 
			? imagComp->play(msg.fdata[1], msg.fdata[0]) 
			: imagComp->stop(msg.data);
		//imagComp->play(msg.fdata[1], msg.fdata[0]);
	}
	
	if (msg.type == MessageType::MOUSE_POSITION_NOTIFICATION)
	{
		cursorPos.x = msg.fdata[0];
		cursorPos.y = msg.fdata[1];
	}
}




//-------------------------------------------------------------------------------------------------------




void GraphicalSystem::addNormalMap(Entity id, const Texture* nMap)
{
	myAssert(world->currentScene->getEntity(id));

	for (int i = 0; i < world->currentScene->imageComponents.getSize(); ++i)
	{
		if (world->currentScene->imageComponents[i].getEntityId() == id) 
		{
			world->currentScene->imageComponents[i].normalMap = nMap; return; 
		}
	}
	myAssert(false);
}


//---------------------------------------------------------------------------------------------------------


void GraphicalSystem::addEmissionMap(Entity id, const Texture* eMap)
{
	myAssert(world->currentScene->getEntity(id));

	for (int i = 0; i < world->currentScene->imageComponents.getSize(); ++i)
	{
		if (world->currentScene->imageComponents[i].getEntityId() == id) 
		{
			world->currentScene->imageComponents[i].emissionMap = eMap; return;
		}
	}
	myAssert(false);
}




//-------------------------------------------------------------------------------------------------------

void GraphicalSystem::addImageComponent(Entity id, const Texture* tex, int nr, int nc)
{
	ImageComponent imagComp(id);
	//set the component sprite texture:
	if (!tex) myAssert(false);

	imagComp.spt.setTexture(tex);

	
	imagComp.spt.rows = (nr == 0) ? 1 : nr;
	imagComp.spt.columns = (nc == 0) ? 1 : nc;

	imagComp.spt.width = tex->getWidth() / imagComp.spt.columns;
	imagComp.spt.height = tex->getHeight() / imagComp.spt.rows;

	world->currentScene->imageComponents.push_back(imagComp);
}




//-----------------------------------------------------------------------------------------------------





void GraphicalSystem::addDirLightComponent(Entity id, glm::vec3 color, glm::vec3 direction, bool sc)
{
	myAssert(world->currentScene->getEntity(id));
	DirLightComponent comp(id, 2048, sc);
	comp.setColor(color);
	comp.lightAmbient = glm::vec3(color * 0.4f);
	comp.direction = direction;
	world->currentScene->dirLightComponents.push_back(comp);
}

//-----------------------------------------------------------------------------------------------------



void GraphicalSystem::addPointLightComponent(Entity id, glm::vec3 color, FLOAT_TYPE lin, FLOAT_TYPE quadratic)
{
	myAssert(world->currentScene->getEntity(id));

	PointLightComponent comp(id, shadowResolution, lin, quadratic);
	comp.lightAmbient = glm::vec3(0.1f * color);
	comp.lightColor = glm::vec3(color);

	FLOAT_TYPE maxComp = glm::max(glm::max(color.x, color.y), color.z);

	comp.radius = (-lin + glm::sqrt(pow(lin, 2) - 4 * quadratic * (-(256 / 15) * maxComp + 1.0f))) / (2.0f * quadratic);

	world->currentScene->pointLightComponents.push_back(comp);
}


//======================================================================================================


void GraphicalSystem::addModelComponent(Entity id, const Model* model)
{
	world->currentScene->modelComponents.push_back(ModelComponent(id));

	myAssert(model);
	world->currentScene->getModelComponent(id)->setModel(model);
}



//------------------------------------------------------------------------------------------------------


std::vector<unsigned int> GraphicalSystem::genSphereVertices(FLOAT_TYPE* data, int size) //data size is assumed to be equal to size
{
	//declare the variables
	FLOAT_TYPE fPi = glm::pi<FLOAT_TYPE>();

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoordinates;

	glm::vec3 vertex; FLOAT_TYPE xy;
	glm::vec3 normal;
	glm::vec2 texCoord;

	const int sectorCount = 12;
	const int stackCount = 8;
	const FLOAT_TYPE radius = 1.0f;
	FLOAT_TYPE sectorStep = (2.0f * fPi) / sectorCount;
	FLOAT_TYPE stackStep = (fPi) / stackCount;

	FLOAT_TYPE currSectorAngle;
	FLOAT_TYPE currStackAngle;
	
	

	for (int i = 0; i <= stackCount; ++i)
	{
		currStackAngle = fPi / 2.0f - (i * stackStep); //start at +90 and go to -90
		vertex.z = glm::sin(currStackAngle) * radius;
		xy = glm::cos(currStackAngle) * radius;

		for (int j = 0; j <= sectorCount; ++j)
		{
			currSectorAngle = j * sectorStep;

			vertex.x = xy * glm::sin(currSectorAngle);
			vertex.y = xy * glm::cos(currSectorAngle);
			//add a new vertex to the vertex list:
			vertices.push_back(vertex);

			//compute the normal:
			normal = vertex * (1.0f / radius);
			normals.push_back(normal);

			//and the texture coordinates
			texCoord.x = FLOAT_TYPE(j) / sectorCount;
			texCoord.y = FLOAT_TYPE(i) / stackCount;
			texCoordinates.push_back(texCoord);
		}
	}

	std::cout << "->Size of vertices: " << vertices.size() << '\n';

	//put all the data(position, normal and texture coordintes) in the data array:

	myAssert(size == (vertices.size() * 3) + (normals.size() * 3) + (texCoordinates.size() * 2));
	int stride = 8; //the stride is 8 FLOAT_TYPEs
	for (int i = 0; i < vertices.size(); ++i)
	{
		int offset = stride * i;
		//positions
		data[offset + 0] = vertices[i].x;
		data[offset + 1] = vertices[i].y;
		data[offset + 2] = vertices[i].z;
		//normals
		data[offset + 3] = normals[i].x;
		data[offset + 4] = normals[i].y;
		data[offset + 5] = normals[i].z;
		//and texture coordinates
		data[offset + 6] = texCoordinates[i].x;
		data[offset + 7] = texCoordinates[i].y;
	}

	//--------------------------------
	//also generate and return a vector holding the indices of the triangles that make up the sphere

	std::vector<unsigned int> indices;
	int k1, k2;

	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if (i != stackCount - 1)
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	return indices;
}



//------------------------------------------------------------------------------------------------------

glm::mat4 GraphicalSystem::normalizeRows(int nRows, glm::mat4 mat) const noexcept
//normalize the first nRows three-dimensional rows of a matrix
{
	for (int i = 0; i < nRows; ++i)
	{
		FLOAT_TYPE lenght = std::sqrt((mat[i][0] * mat[i][0]) + (mat[i][1] * mat[i][1]) + (mat[i][2] * mat[i][2]));
		if (lenght <= 0.001f) lenght = 0.001f;
		mat[i][0] /= lenght;
		mat[i][1] /= lenght;
		mat[i][2] /= lenght;
	}

	return mat;
}


inline glm::mat4 GraphicalSystem::getFullTransform(Entity id, FLOAT_TYPE w, FLOAT_TYPE h, FLOAT_TYPE z) const
{
	myAssert(world->currentScene->getEntity(id));
	//glm::mat4 fullTransform = glm::mat4(1.0f);

	glm::vec3 scale(w, h, z);

	const TransformComponent* current = world->currentScene->getTransformComponent(id);
	//auto tComp = world->currentScene->getTransformComponent(id);
	//fullTransform = tComp->getTransform();

	//scale the matrix by the sprite scale
	scale.x *= current->getScale().x;
	scale.y *= current->getScale().y;
	scale.z *= current->getScale().z;
	glm::mat4 fullTransform = current->getScaledTransform(glm::vec3(scale));

	

	while (current->getParent() >= 0)
	{
		//set current to the TransformComponent of it's parent 
		current = world->currentScene->getTransformComponent(current->getParent());
		//combine the parent and child transform: (note that the parent's scale will not affect
		//it childs, this is done by normalizing the columns of the 3x3 uper matrix)
		fullTransform = normalizeRows(3, current->getTransform()) * fullTransform;
	}
	
	return fullTransform;
}


//-------------------------------------------------------------------------------------------------------


glm::mat4 GraphicalSystem::getFullTransform2(Entity id) const
{
	myAssert(world->currentScene->getEntity(id));
	// = glm::mat4(1.0f);

	const TransformComponent* current = world->currentScene->getTransformComponent(id);
	glm::mat4 fullTransform =
		current->getTransform();

	//const TransformComponent* current = world->currentScene->getTransformComponent(id);
	while (current->getParent() >= 0)
	{
		//set current to the TransformComponent of it's parent
		current = world->currentScene->getTransformComponent(current->getParent());
		//combine the parent and child transform: (note that the parent's scale will not affect
		//it childs, this is done normalizing the columns of the 3x3 uper matrix)
		fullTransform = normalizeRows(3, current->getTransform()) * fullTransform;
	}

	return fullTransform;
}


//-------------------------------------------------------------------------------------------------------


glm::quat GraphicalSystem::getFullRotationQuaternion(Entity id) const
{
	myAssert(world->currentScene->getEntity(id));
	glm::quat fullQuaternion;


	//iterate over all TransformComponents 
	for (int i = 0; i < world->currentScene->transformComponents.getSize(); ++i)
	{
		if (world->currentScene->transformComponents[i].getEntityId() == id)
		{
			fullQuaternion = world->currentScene->transformComponents[i].getOrientation();

			const TransformComponent* current = &(world->currentScene->transformComponents[i]);
			while (current->getParent() >= 0)
			{
				//set current to the TransformComponent of it's parent
				current = world->currentScene->getTransformComponent(current->getParent());
				//combine the parent and child transform: (note that the parent's scale will not affect
				//it childs, this is done normalizing the columns of the 3x3 uper matrix)
				fullQuaternion = current->getOrientation() * fullQuaternion;
			}
			break;
		}
	}
	return fullQuaternion;
}


//----------------------------------------------------------------------------------------------


void GraphicalSystem::renderDepthMaps()
{
	//set the framebuffer:
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowResolution * 4.0f, shadowResolution * 4.0f);

	glUseProgram(programs[1].getId());
	glm::mat4 viewAndProj;
	glm::vec3 lightPos;
	//render the scene for each directional light:
	for (int i = 0; i < world->currentScene->dirLightComponents.getSize(); ++i)
	{
		DirLightComponent* dirLightComp = &(world->currentScene->dirLightComponents[i]);

		if (!dirLightComp->actived || !dirLightComp->shadowCaster)
			continue;

		myAssert(dirLightComp);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirLightComp->depthTexture, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		lightPos = glm::vec3(getFullTransform2(dirLightComp->getEntityId()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		dirLightComp->updateMatrix(lightPos);
		
		//set the matrix to the light point of view:
		//viewAndProj = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 400.0f) *
		//	glm::lookAt(lightPos, lightPos + iter->direction, glm::vec3(0.0f, 1.0f, 0.0f));
		//render the full scene to the shadowFrameBuffer's depth buffer
		renderSceneGeometry(programs[1], dirLightComp->lightMatrix);
	}


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);

	//---------------------------------------------------------
	//now for the point lights:
	
	glViewport(0, 0, shadowResolution, shadowResolution);
	//glDisable(GL_CULL_FACE);

	glUseProgram(programs[6].getId());

	glm::mat4 lightProj;
	for (int i = 0; i < world->currentScene->pointLightComponents.getSize(); ++i)
	{
		PointLightComponent* pointLightComp = &(world->currentScene->pointLightComponents[i]);

		if (!pointLightComp->actived)
			continue;

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointLightComp->depthCubeMap, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			myAssert(false);

		glClear(GL_DEPTH_BUFFER_BIT);

		//std::cout << iter->getEntityId() << ' ';
		//set the light specific variables:
		FLOAT_TYPE farPlane = 800.0f;

		lightPos = glm::vec3(getFullTransform2(pointLightComp->getEntityId()) * glm::vec4(glm::vec3(0.0f), 1.0f));

		pointLightComp->updateMatrices(lightPos);

		//set uniforms:
		programs[6].setVec3("lightPos", lightPos);
		programs[6].setInt("farPlane", farPlane);

		//programs[6].setMat4("faceMatrices[0]", pointLightComp->posXDepthMapMatrix);
		glUniformMatrix4fv(glGetUniformLocation(programs[6].getId(), "faceMatrices[0]"),
			1, GL_FALSE, glm::value_ptr(pointLightComp->posXDepthMapMatrix));
		//programs[6].setMat4("faceMatrices[1]", pointLightComp->negXDepthMapMatrix);
		glUniformMatrix4fv(glGetUniformLocation(programs[6].getId(), "faceMatrices[1]"),
			1, GL_FALSE, glm::value_ptr(pointLightComp->negXDepthMapMatrix));
		//programs[6].setMat4("faceMatrices[2]", pointLightComp->posYDepthMapMatrix);
		glUniformMatrix4fv(glGetUniformLocation(programs[6].getId(), "faceMatrices[2]"),
			1, GL_FALSE, glm::value_ptr(pointLightComp->posYDepthMapMatrix));
		//programs[6].setMat4("faceMatrices[3]", pointLightComp->negYDepthMapMatrix);
		glUniformMatrix4fv(glGetUniformLocation(programs[6].getId(), "faceMatrices[3]"),
			1, GL_FALSE, glm::value_ptr(pointLightComp->negYDepthMapMatrix));
		//programs[6].setMat4("faceMatrices[4]", pointLightComp->posZDepthMapMatrix);
		glUniformMatrix4fv(glGetUniformLocation(programs[6].getId(), "faceMatrices[4]"),
			1, GL_FALSE, glm::value_ptr(pointLightComp->posZDepthMapMatrix));
		//programs[6].setMat4("faceMatrices[5]", pointLightComp->negZDepthMapMatrix);
		glUniformMatrix4fv(glGetUniformLocation(programs[6].getId(), "faceMatrices[5]"),
			1, GL_FALSE, glm::value_ptr(pointLightComp->negZDepthMapMatrix));

		//and them draw to the point light depth buffer:

		renderSceneGeometry(programs[6], glm::mat4(1.0f));
	}
	glDisable(GL_CULL_FACE);
	
	
	glCullFace(GL_BACK);
}





//-------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderLightMap()
{

	glBindFramebuffer(GL_FRAMEBUFFER, lightningFrameBuffer);
	glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(programs[4].getId());
	//set uniforms:
	programs[4].setVec3("viewPos", camPosition);
	programs[4].setInt("numOfLights", world->currentScene->dirLightComponents.getSize());


	bool firstLight = true; //blending will be disabled when drawing the first light

	//render the scene for each light, the resulting scene lightning will be a single texture
	for (int i = 0; i < world->currentScene->dirLightComponents.getSize(); ++i)
	{
		if (!firstLight)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthFunc(GL_EQUAL);
		}
		else { glDisable(GL_BLEND); glDepthFunc(GL_LEQUAL); };

		DirLightComponent* dirLightComp = &(world->currentScene->dirLightComponents[i]);

		glm::vec3 lightPos = glm::vec3(getFullTransform2(dirLightComp->getEntityId()) * glm::vec4(glm::vec3(0.0f), 1.0f));
		//glm::mat4 viewAndProj = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 400.0f) *
			//glm::lookAt(lightPos, lightPos + iter->direction, glm::vec3(0.0f, 1.0f, 0.0f));
		dirLightComp->updateMatrix(lightPos);
		programs[4].setMat4("lightMatrix", dirLightComp->lightMatrix);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, dirLightComp->depthTexture);
		programs[4].setInt("directionalLight.depthMap", 1);
		programs[4].setVec3("directionalLight.direction", dirLightComp->direction);
		programs[4].setVec3("directionalLight.ambient", dirLightComp->lightAmbient);
		programs[4].setVec3("directionalLight.color", dirLightComp->lightColor);

		renderScene(programs[4], projection * cameraView, true, false); 
		firstLight = false;
	}

	//-----------------------------------------------------------------------
	//now for point lights

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
		

	glUseProgram(programs[7].getId());
	programs[7].setInt("numOfLights", world->currentScene->pointLightComponents.getSize());
	programs[7].setVec3("viewPos", camPosition);

	for (int i = 0; i < world->currentScene->pointLightComponents.getSize(); ++i)
	{
		if (!firstLight)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthFunc(GL_EQUAL);
		}
		else { glDisable(GL_BLEND); glDepthFunc(GL_LEQUAL); }

		PointLightComponent* pointLightComp = &(world->currentScene->pointLightComponents[i]);

		glm::vec3 lightPos = glm::vec3(getFullTransform2(pointLightComp->getEntityId()) * glm::vec4(glm::vec3(0.0f), 1.0f));
		//std::cout << "\n(" << lightPos.x << ',' << lightPos.y << ',' << lightPos.z << ")";
		FLOAT_TYPE farPlane = 800.0f; // int(abs(log(iter->linearAttenuation)) * 100);
		programs[7].setVec3("pointLight.position", lightPos);
		programs[7].setVec3("pointLight.ambient", pointLightComp->lightAmbient);
		programs[7].setVec3("pointLight.color", pointLightComp->lightColor);
		programs[7].setFLOAT_TYPE("pointLight.constant", pointLightComp->constantAttenuation);
		programs[7].setFLOAT_TYPE("pointLight.linear", pointLightComp->linearAttenuation);
		programs[7].setFLOAT_TYPE("pointLight.quadratic", pointLightComp->quadraticAttenuation);

		programs[7].setInt("farPlane", farPlane);

		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightComp->depthCubeMap);
		programs[7].setInt("pointLight.depthMap", 10);

		//set camera matrix and them draw the scene using the point light shader
		glm::mat4 viewAndProj = projection * cameraView;

		renderScene(programs[7], viewAndProj, true); //!
		firstLight = false;
	}


}


//---------------------------------------------------------------------------------------------------------



void GraphicalSystem::applyBloom()
{
	glViewport(0, 0, bufferDefaultSize.x / 4, bufferDefaultSize.y / 4);

	bool horizontal = true, firstIteration = true;
	int blurringIterations = 3;

	glUseProgram(programs[8].getId());

	programs[8].setFLOAT_TYPE("intensity", 1.0f);
	programs[8].setVec2("screenSize", bufferDefaultSize.x / 4 , bufferDefaultSize.y / 4);

	for (int i = 0; i < blurringIterations; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFramebuffers[horizontal]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (firstIteration) ? bloomColorAttachment : pingPongColorAttachments[!horizontal]);
		programs[8].setInt("image", 0);

		//draw the texture to the other framebuffer(one of the two pingpong buffers) using a blurring shader
		glBindVertexArray(offscreenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		horizontal = !horizontal;

		if(firstIteration) firstIteration = false;
	}

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//---------------------------------------------------------------------------------------------------------

//this function blurries the upper part of the screen, to boost the notion of perspective
void GraphicalSystem::applyBlur()
{
	glBindFramebuffer(GL_FRAMEBUFFER, blurPingPongFrameBuffers[0]);
	glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);

	glUseProgram(programs[10].getId());
	
	glClear(GL_COLOR_BUFFER_BIT);
	programs[10].setVec2("minAndMaxIntensity", 0.0f, 0.6f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	programs[10].setInt("image", 0);

	//draw the scene offscreen color buffer to the pingPong buffer using a blurring shader
	glBindVertexArray(offscreenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	//note: the blurPingPongFramebuffer[0]'s color attachment can now be used as the final result for drawing the scene

	//----------------------------
	//unbind everything:
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


//---------------------------------------------------------------------------------------------------------


void GraphicalSystem::drawHuds(int offset, int width ,int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(offset, 0, width - (2 * offset), height);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//draw the player status hud:
	glUseProgram(programs[11].getId());
	glm::vec2 position;
	glm::vec2 size;

	for (int i = 0; i < world->playerStatusBar.numberOfImages; ++i)
	{
		position = world->playerStatusBar.getSpritePos(i);
		size = glm::vec2(world->playerStatusBar.getImage(i)->width, world->playerStatusBar.getImage(i)->height);
		programs[11].setVec2("hudPos", position.x, position.y);
		programs[11].setVec2("hudSize", size.x, size.y);
		programs[11].setVec2("screenSize", width, height);
		programs[11].setFLOAT_TYPE("horizontalPercent", world->playerStatusBar.getSpriteHorizontalPercent(i));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, world->playerStatusBar.getImage(i)->getTexture()->getGlId());

		programs[11].setInt("tex", 0);

		glBindVertexArray(offscreenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}


//---------------------------------------------------------------------------------------------------------


void GraphicalSystem::drawCursor(int offset, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(offset, 0, width - (2 * offset) , height);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	
	glUseProgram(programs[12].getId()); //use the mouse shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cursor.getTexture()->getGlId()); //bind the mouse texture

	programs[12].setInt("tex", 0);
	programs[12].setVec2("mousePos", cursorPos.x, cursorPos.y);
	programs[12].setVec2("screenSize", width - (2 * offset), height);

	glBindVertexArray(offscreenVAO); 
	glDrawArrays(GL_TRIANGLES, 0, 6); //and draw a quad to the default framebuffer
}


//---------------------------------------------------------------------------------------------------------




void GraphicalSystem::drawPhysicsBoxes(int offset, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(offset, 0, width - (2 * offset), height);
	//glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(programs[13].getId());
	glBindVertexArray(cubeVAO);

	glm::mat4 viewAndProj = projection * cameraView;

	
	programs[13].setMat4("viewAndProj", viewAndProj);
	 
	//walk through each physical component and draw their boxes:
	for (int i = 0; i < world->currentScene->boxRigidBodyComponents.getSize(); ++i)
	{
		RigidBodyComponent<Box>* boxComp = &(world->currentScene->boxRigidBodyComponents[i]);
		//if (iter->mass < 0.0f) continue;
		programs[13].setMat4("model", normalizeRows(3, getFullTransform2(boxComp->getEntityId())));
		programs[13].setVec3("size", boxComp->getSize());
		//std::cout << iter->getSize().x << ',' << iter->getSize().y << ',' << iter->getSize().z << '\n';
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	for (int i = 0; i < world->currentScene->interactableObjectComponents.getSize(); ++i)
	{
		InteractableObjectComponent* intObjComp = &(world->currentScene->interactableObjectComponents[i]);

		if (!(intObjComp->isEffectActive))
			continue;

		glm::mat4 model = intObjComp->transform;
		//if (intObjComp->holder >= 0)
		//	model[3] = glm::vec4(world->currentScene->getTransformComponent(intObjComp->holder)->getPosition(), 1.0f);;
		//model[3] = glm::vec4(intObjComp->hitBox.pos, 1.0f);

		programs[13].setMat4("model", model);
		programs[13].setVec3("size", intObjComp->hitBox.getSize());
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



//---------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderDirLights(int, int, int)
{
	glm::mat4 viewAndProj = projection * cameraView;

	glBindFramebuffer(GL_FRAMEBUFFER, offScreenFrameBuffer);
	glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);
	

	glUseProgram(programs[18].getId()); //set the program

	//configure uniforms:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPositions);
	programs[18].setInt("gPos", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormals);
	programs[18].setInt("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);
	programs[18].setInt("gAlbedoSpec", 2);

	programs[18].setVec3("viewPos", camPosition);
	
	
	//glDisable(GL_CULL_FACE);

	for (int i = 0; i < world->currentScene->dirLightComponents.getSize(); ++i)
	{
		DirLightComponent* dirLightComp = &(world->currentScene->dirLightComponents[i]);
		//glm::mat4 model = getFullTransform2(dirLightComp->getEntityId());

		if (!dirLightComp->actived)
			continue;

		//--------------------------------
		//lightning pass:

		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		programs[18].setVec3("light.color", dirLightComp->lightColor);
		programs[18].setVec3("light.direction", -glm::normalize(dirLightComp->direction));
		programs[18].setMat4("light.pvm", dirLightComp->lightMatrix);
		programs[18].setVec2("screenSize", bufferDefaultSize.x, bufferDefaultSize.y);


		if (dirLightComp->shadowCaster)
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, dirLightComp->depthTexture);
			programs[18].setInt("shadowMap", 3);
			programs[18].setBool("useShadowMap", true);
		}
		else
			programs[18].setBool("useShadowMap", false);

		glBindVertexArray(offscreenVAO);
		//glBindVertexArray(cubeVAO);
		//glDrawElements(GL_TRIANGLES, sphereIndicesSize, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//std::cout << "-> " << pointLightComp->radius << '\n';
	}

	glDisable(GL_BLEND);
}



//---------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderPointLights(int, int, int)
{
	glm::mat4 viewAndProj = projection * cameraView;

	glBindFramebuffer(GL_FRAMEBUFFER, offScreenFrameBuffer);
	glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);

	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programs[15].getId()); //set the program

	//configure uniforms:
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPositions);
	programs[15].setInt("gPos", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormals);
	programs[15].setInt("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);
	programs[15].setInt("gAlbedoSpec", 2);

	programs[15].setVec3("viewPos", camPosition);

	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);

	for (int i = 0; i < world->currentScene->pointLightComponents.getSize(); ++i)
	{
		PointLightComponent* pointLightComp = &(world->currentScene->pointLightComponents[i]);
		glm::mat4 model = getFullTransform2(pointLightComp->getEntityId());
		
		//--------------------------------
		//lightning pass:

		glEnable(GL_CULL_FACE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		//glEnable(GL_STENCIL_TEST);
		//glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		glCullFace(GL_BACK);
		//glCullFace(GL_FRONT);
		//glEnable(GL_BLEND);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, pointLightComp->depthCubeMap);
		programs[15].setInt("light.depthCubeMap", 3);
		programs[15].setInt("light.farPlane", 800);
		programs[15].setVec3("light.pos",
			glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		programs[15].setVec3("light.color", pointLightComp->lightColor);
		programs[15].setFLOAT_TYPE("light.kConstant", pointLightComp->constantAttenuation);
		programs[15].setFLOAT_TYPE("light.kLinear", pointLightComp->linearAttenuation);
		programs[15].setFLOAT_TYPE("light.kQuadratic", pointLightComp->quadraticAttenuation);
		programs[15].setFLOAT_TYPE("light.radius", pointLightComp->radius / 2);

		programs[15].setVec2("screenSize", bufferDefaultSize.x, bufferDefaultSize.y);
		programs[15].setFLOAT_TYPE("sphereRadius", pointLightComp->radius / 2);

		programs[15].setMat4("model", model);
		programs[15].setMat4("viewAndProj", viewAndProj);

		glBindVertexArray(cubeVAO);
		//glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, sphereIndicesSize, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//std::cout << "-> " << pointLightComp->radius << '\n';
	}
	glFrontFace(GL_CCW);
}


//---------------------------------------------------------------------------------------------------------




void GraphicalSystem::renderEmissionMaps(int offset, int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, offScreenFrameBuffer);
	glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(programs[19].getId());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);
	programs[19].setInt("gAlbedoSpec", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gEmission);
	programs[19].setInt("gEmission", 1);

	programs[19].setVec2("screenSize", bufferDefaultSize.x, bufferDefaultSize.y);

	//render a quad to the offscreen framebuffer
	glBindVertexArray(offscreenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


//---------------------------------------------------------------------------------------------------------


void GraphicalSystem::renderParticles(int, int, int)
{
	glm::mat4 viewAndProj = projection * cameraView;
	int i = 0;
	int j = 0;
	static float positions[3 * 200];
	static float colors[4 * 200];
	int pCount = world->currentScene->particleSystem.particlePool.size();
	int count = 0;
	


	//glBindFramebuffer(GL_FRAMEBUFFER, offScreenFrameBuffer);
	//glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);

	glEnable(GL_BLEND);
	glDepthFunc(GL_ONE);
	glEnable(GL_PROGRAM_POINT_SIZE);


	glUseProgram(programs[20].getId());

	glBindVertexArray(particleVAO);


	glUniformMatrix4fv(glGetUniformLocation(programs[20].getId(), "projAndView"), 1, GL_FALSE, glm::value_ptr(viewAndProj));
	

	for(i = 0; i < pCount; ++i) //draw the particles as instanced points, 500 each time
	{
		if (world->currentScene->particleSystem.particlePool.particles[i].life <= 0)
			continue;

		//fill the positions and colors arrays
		glm::vec3 pos = world->currentScene->particleSystem.particlePool.particles[i].position;
		glm::vec4 color = world->currentScene->particleSystem.particlePool.particles[i].color;

		positions[(j * 3) + 0] = pos.x;
		positions[(j * 3) + 1] = pos.y;
		positions[(j * 3) + 2] = pos.z;

		colors[(j * 4) + 0] = color.x;
		colors[(j * 4) + 1] = color.y;
		colors[(j * 4) + 2] = color.z;
		colors[(j * 4) + 3] = color.w;
		
		++j;

		if (j == 200 || (i == pCount && j != 0))
		{
			//configure uniforms and draw: 
			glUniform3fv(glGetUniformLocation(programs[20].getId(), "positions"), j, &(positions[0]));
			glUniform4fv(glGetUniformLocation(programs[20].getId(), "colors"), j, &(colors[0]));
			//std::cout << "->>" << j << '\n';
			glDrawArraysInstanced(GL_POINTS, 0, 1, j);

			j = 0;
		}
	
	}


	glDisable(GL_BLEND);
}


//---------------------------------------------------------------------------------------------------------


void GraphicalSystem::reloadTransforms()
{
	if (scaledFullTransforms.capacity() < 
		world->currentScene->imageComponents.getSize() + 
		world->currentScene->modelComponents.getSize() +
		world->currentScene->interactableObjectComponents.getSize())
	{
		//reserve more memory to it(also makes sure it does not reserve less than 20)
		scaledFullTransforms.reserve((scaledFullTransforms.capacity() > 10 ? scaledFullTransforms.capacity() : 10) * 2);
	}
	scaledFullTransforms.clear(); //reset objects and size, but not the capacity

	//load all transform components from all Entities that have a ImageComponent, ModelComponent or intObjComp
	for (int i = 0; i < world->currentScene->imageComponents.getSize(); ++i)
	{
		const ImageComponent* imagComp = &(world->currentScene->imageComponents[i]);
		scaledFullTransforms.push_back(
			std::pair<glm::mat4, Entity>(
				getFullTransform(imagComp->getEntityId(), imagComp->spt.width, imagComp->spt.height), 
				imagComp->getEntityId()));
	}
	for (int i = 0; i < world->currentScene->modelComponents.getSize(); ++i)
	{
		Entity eId = world->currentScene->modelComponents[i].getEntityId();
		scaledFullTransforms.push_back(
			std::pair<glm::mat4, Entity>(getFullTransform(eId, 1.0f, 1.0f), eId));
	}
	for (int i = 0; i < world->currentScene->interactableObjectComponents.getSize(); ++i)
	{
		const InteractableObjectComponent* intObjComp = &(world->currentScene->interactableObjectComponents[i]);
		scaledFullTransforms.push_back(
			std::pair<glm::mat4, Entity>(getFullTransform(intObjComp->getEntityId(), 1.0f, 1.0f), intObjComp->getEntityId()));
	}
	for (int i = 0; i < world->currentScene->characterComponents.getSize(); ++i)
	{
		Entity eId = world->currentScene->characterComponents[i].getEntityId();
		scaledFullTransforms.push_back(
			std::pair<glm::mat4, Entity>(getFullTransform(eId, 1.0f, 1.0f), eId));
	}

}


//---------------------------------------------------------------------------------------------------------


glm::mat4 GraphicalSystem::getScaledFullTransfom(Entity id) const
{
	for (int i = 0; i < scaledFullTransforms.size(); ++i)
		if (scaledFullTransforms[i].second == id) return scaledFullTransforms[i].first;
	myAssert(false); //entity not found
	return identityMatrix;
}


//---------------------------------------------------------------------------------------------------------


void GraphicalSystem::render(int offset, int width, int height)
{
	
	glfwSwapBuffers(window);
	
	

	//==================================================
	//reload transforms:
	reloadTransforms();

	//==================================================
	//update camera:
	cameraView = glm::lookAt(camPosition, camPosition + camDirection, glm::vec3(0.0f, 1.0f, 0.0f));

	//==================================================================================================
	//render to the gBuffer


	

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, bufferDefaultSize.x, bufferDefaultSize.y);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glUseProgram(programs[14].getId());

	

	//configure uniforms:
	//programs[14].setVec3("viewPos", camPosition);

	glUniform3f(glGetUniformLocation(programs[14].getId(), "viewPos"), camPosition.x, camPosition.y, camPosition.z);

	
	glm::mat4 viewAndProj = projection * cameraView;

	

	
	
	renderScene(programs[14], viewAndProj, true, true , 50);
	
	//render the particles
	renderParticles(width, height, offset);

	
	//------------------------------------------
	//render the depth map for each light:
	renderDepthMaps();

	//-------------------------------------------
	//do the second pass of the deferred shading

	//render the point lights
	renderPointLights(width, height, offset);



	//and the directional lights
	renderDirLights(width, height, offset);

	renderEmissionMaps(width, height, offset);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);
	/*
	//render a quad:
	glUseProgram(programs[16].getId());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpecular);
	programs[16].setInt("gAlbedoSpec", 0);

	glBindVertexArray(offscreenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	*/
	glDisable(GL_BLEND);
	glBindVertexArray(0);

	//==================================================
	//now, the forward part of the rendering algorithm
	
	//first, copy the depth buffer of the gBuffer to the offScreen depth buffer
	/*
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, offScreenFrameBuffer);
	glBlitFramebuffer(0, 0, bufferDefaultSize.x, bufferDefaultSize.y, 0, 0, bufferDefaultSize.x, bufferDefaultSize.y,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		*/
		

	//===================================================
	//Post-processing:

	

	auto t1 = glfwGetTime();
	
	if (blur)
		applyBlur();

	if (bloom)
		applyBloom();
		
	

	//====================================================
	//draw to the default framebuffer


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(offset, 0, width - (2*offset) , height);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//draw a quad in front of the window with the color texture attachment of the offscreen frambuffer
	glUseProgram(programs[2].getId());
	glBindVertexArray(offscreenVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorAttachment);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, world->currentScene->pointLightComponents[1].depthCubeMap);
	programs[2].setInt("tex", 0);
	
	if (blur)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, blurPingPongColorAttachments[0]);
		programs[2].setInt("blurTex", 1);
		programs[2].setBool("useBlur", true);
		//programs[2].setBool("useBlur", false);
	}
	else 
		programs[2].setBool("useBlur", false);

	if (bloom)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, pingPongColorAttachments[0]);
		programs[2].setInt("bloomTex", 2);
		programs[2].setBool("useBloom", true);
		//programs[2].setBool("useBloom", false);
	}
	else 
		programs[2].setBool("useBloom", false);

	

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);



	//drawPhysicsBoxes(offset, width, height); //used for debugging the physics engine
	

	drawHuds(offset, width, height);
	drawCursor(offset, width, height);
}





//----------------------------------------------------------------------------------------------------------------------




void GraphicalSystem::update()
{
	//update graphical components:
	for (int i = 0; i < world->currentScene->imageComponents.getSize(); ++i)
		world->currentScene->imageComponents[i].update();

	//update shadow maps position of each direction lights
	for (int i = 0; i < world->currentScene->dirLightComponents.getSize(); ++i)
		world->currentScene->getTransformComponent(world->currentScene->dirLightComponents[i].getEntityId())->
		setPosition(glm::vec3(camPosition.x, camPosition.y + 100, camPosition.z + 50.0f));

}




//----------------------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderSceneImages(ShaderProgram& shader, 
										const glm::mat4& viewAndProj, 
										bool useNormalMaps,
										bool useEmissionMaps, 
										int billboard)
{


	glm::mat4 model(1.0f);
	glBindVertexArray(spriteVAO);
	for (int i = 0; i < world->currentScene->imageComponents.getSize(); ++i)
	{
		ImageComponent* imagComp = &(world->currentScene->imageComponents[i]);
		if (!imagComp->actived)
			continue;
		//glUseProgram(programs[0].getId());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (*imagComp).spt.getTexture()->getGlId());
		//shader.setInt("tex", 0);
		glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);



		if (useNormalMaps && imagComp->normalMap)
		{
			shader.setBool("useNormalMap", true);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, imagComp->normalMap->getGlId());
			shader.setInt("normalsTex", 1);
		}
		else
			shader.setBool("useNormalMap", false);

		if (useEmissionMaps && imagComp->emissionMap)
		{
			shader.setBool("useEmissionMap", true);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, imagComp->emissionMap->getGlId());
			shader.setInt("emissionTex", 2);
		}
		else shader.setBool("useEmissionMap", false);

		shader.setBool("useRoughnessMap", false);
		shader.setBool("useMetallicMap", false);
		shader.setFLOAT_TYPE("materialSpecular", 32.0f);

		//get the transformation matrix:
		model = getScaledFullTransfom(imagComp->getEntityId());//getFullTransform(imagComp->getEntityId(), imagComp->spt.width, imagComp->spt.height);

		
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(glGetUniformLocation(shader.getId(), "modelInverse"), 1, GL_FALSE,
			glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(model)))));

		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), imagComp->spt.rows);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), imagComp->spt.columns);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), imagComp->spt.currentRow);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), imagComp->spt.currentColumn);


		glDrawArrays(GL_TRIANGLES, 0, 12);

	}


}


//----------------------------------------------------------------------------------------------------------------------


void GraphicalSystem::renderSceneModels(ShaderProgram& shader, 
										const glm::mat4& viewAndProj, 
										bool useNormalMaps,
										bool useEmissionMaps, 
										int billboard)
{
	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 model(1.0f);
	for (int i = 0; i < world->currentScene->modelComponents.getSize(); ++i)
	{
		ModelComponent* modelComp = &(world->currentScene->modelComponents[i]);
		myAssert(modelComp && modelComp->model);
		//std::cout << "-> Size: " << modelComp->model->mEntries[0].materialIndex << '\n';

		//configure animation data:
		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), modelComp->model->mMaterial.animations);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), modelComp->model->mMaterial.framesPerAnimation);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), modelComp->currentRow);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), modelComp->currentColumn);

		//-------------------------------------------------------
		//configure the bones data

		if (modelComp->model->mBoneData.size() > 0 && modelComp->model->sceneData.animations.size() > 0)
		{
			//std::cout << "Duration: " << modelComp->model->sceneData.animations[0].duration << '\t'
			//	<< modelComp->model->sceneData.animations[0].channels.size() << '\n';

			shader.setBool("useBones", true);
			myAssert(modelComp->model->mBoneData.size() <= 50);

			FLOAT_TYPE duration = FLOAT_TYPE(modelComp->model->sceneData.animations[0].duration) / modelComp->model->sceneData.animations[0].ticksPerSecond;

			std::vector<glm::mat4> transforms;

			//update the mBoneData vector with the bones transforms
			modelComp->boneTransform(0, std::fmod(glfwGetTime(), duration), transforms);


			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "boneTransforms"),
				modelComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(modelComp->mBoneTransforms[0]));
		}
		else
			shader.setBool("useBones", false);



		for (int j = 0; j < modelComp->model->mEntries.size(); ++j) //walk through all meshes of the model
		{
			//myAssert(modelComp->model->mMaterials.size() != 0);
			const Material* material = &(modelComp->model->mMaterial);

			myAssert(material);
			shader.setFLOAT_TYPE("materialSpecular", material->Ns);

			model = getScaledFullTransfom(modelComp->getEntityId());
			model[3] += glm::vec4(modelComp->pos, 0.0f);

			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix3fv(glGetUniformLocation(shader.getId(), "modelInverse"), 1, GL_FALSE,
				glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(model)))));


			//std::cout << "Bones:" << modelComp->model->mBoneData.size() << "\tAnimations:"
			//	<< modelComp->model->sceneData.animations.size() << "\n";


			//----------------------------------
			//bind the albedo texture:
			if (material->hasTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->albedoTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
			}
			else
				continue;

			//----------------------------------
			//bind the normal map(if avaible and asked for):
			if (material->hasNormalMap && useNormalMaps)
			{
				shader.setBool("useNormalMap", true);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->normalMapTexture.getGlId());
				shader.setInt("normalsTex", 1);
			}
			else
				shader.setBool("useNormalMap", false);

			//----------------------------------
			//bind the emission map:
			if (useEmissionMaps && material->hasEmissionMap)
			{
				shader.setBool("useEmissionMap", true);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, material->emissionMapTexture.getGlId());
				shader.setInt("emissionTex", 2);
			}
			else
				shader.setBool("useEmissionMap", false);

			//----------------------------------
			//bind the roughness map

			if (material->hasRoughnessMap)
			{
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, material->roughnessMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "roughnessTex"), 3);
				shader.setBool("useRoughnessMap", true);
			}
			else shader.setBool("useRoughnessMap", false);

			//----------------------------------
			//bind the metallic map

			if (material->hasMetallicMap)
			{
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, material->metallicMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "metallicTex"), 4);
				shader.setBool("useMetallicMap", true);
			}
			else shader.setBool("useMetallicMap", false);

			//Draw the mesh:
			glBindVertexArray(modelComp->model->VAO);

			glDrawElementsBaseVertex(GL_TRIANGLES,
				modelComp->model->mEntries[j].numOfIndices,
				GL_UNSIGNED_INT,
				(void*)(modelComp->model->mEntries[j].baseIndex * sizeof(unsigned int)),
				modelComp->model->mEntries[j].baseVertex);

			glBindVertexArray(0);
		}

	}

	glDisable(GL_CULL_FACE);



}


//----------------------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderSceneIntObjects(ShaderProgram& shader, 
											const glm::mat4& viewAndProj, 
											bool useNormalMaps,
											bool useEmissionMaps, 
											int billboard)
{
	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 model(1.0f);
	for (int i = 0; i < world->currentScene->interactableObjectComponents.getSize(); ++i)
	{
		InteractableObjectComponent* intObjComp = &(world->currentScene->interactableObjectComponents[i]);
		myAssert(intObjComp);

		if (intObjComp->model == nullptr)
			continue;

		const CharacterComponent* charComp = nullptr;
		//see if the object is holded by some character:
		if (intObjComp->holder >= 0)
		{
			charComp = world->currentScene->getCharacterComponent(intObjComp->holder);

			//use the character transform instead of the object
			model = getScaledFullTransfom(intObjComp->holder);
			model[3] += glm::vec4(charComp->getModelPos(), 0.0f);

			//rotate according to the direction the character is facing
			glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), charComp->getDirection() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = model * rotMat;
		}
		else //else, use the object transform
		{
			model = intObjComp->transform * getScaledFullTransfom(intObjComp->getEntityId());
			model[3] += glm::vec4(intObjComp->pos, 0.0f);
		}
		//std::cout << "-> Size: " << modelComp->model->mEntries[0].materialIndex << '\n';


		if (!intObjComp->isActived())
			continue;

		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), intObjComp->model->mMaterial.animations);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), intObjComp->model->mMaterial.framesPerAnimation);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), intObjComp->currentRow);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), intObjComp->currentColumn);

		//-------------------------------------------------------
		//bind the bones data

		if (intObjComp->model->mBoneData.size() > 0 && intObjComp->model->sceneData.animations.size() > 0)
		{
			//std::cout << "Duration: " << modelComp->model->sceneData.animations[0].duration << '\t'
			//	<< modelComp->model->sceneData.animations[0].channels.size() << '\n';

			shader.setBool("useBones", true);
			myAssert(intObjComp->model->mBoneData.size() <= 50);

			//FLOAT_TYPE duration = FLOAT_TYPE(intObjComp->model->sceneData.animations[0].duration) / intObjComp->model->sceneData.animations[0].ticksPerSecond;

			const std::vector<glm::mat4>* transforms = nullptr;

			//if the object is holded by a character, the animation id and time used will be the character ones
			int animationIndex = (intObjComp->holder >= 0)
				? charComp->getCurrentAnimation()
				: intObjComp->currentAnimation;
			FLOAT_TYPE animTime = (intObjComp->holder >= 0)
				? charComp->getAnimationTime()
				: intObjComp->animationTime;

			//-------------------
			//update and get the bone transforms:
			transforms = &(intObjComp->boneTransform(animationIndex, animTime));
			
			//and send them to the gpu:
			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "boneTransforms"),
				transforms->size(), GL_FALSE, glm::value_ptr((*transforms)[0]));
		}
		else
			shader.setBool("useBones", false);

		//model = intObjComp->transform * getScaledFullTransfom(intObjComp->getEntityId());
		//model[3] += glm::vec4(intObjComp->pos, 0.0f);

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(glGetUniformLocation(shader.getId(), "modelInverse"), 1, GL_FALSE,
			glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(model)))));


		for (int j = 0; j < intObjComp->model->mEntries.size(); ++j) //walk through all meshes of the model
		{
			const Material* material = &(intObjComp->model->mMaterial);

			myAssert(material);
			shader.setFLOAT_TYPE("materialSpecular", material->Ns);

			//----------------------------------
			//bind the albedo texture:
			if (material->hasTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->albedoTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
			}
			else
				continue;

			//----------------------------------
			//bind the normal map(if avaible and asked for):
			if (material->hasNormalMap && useNormalMaps)
			{
				shader.setBool("useNormalMap", true);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->normalMapTexture.getGlId());
				shader.setInt("normalsTex", 1);
			}
			else
				shader.setBool("useNormalMap", false);

			//----------------------------------
			//bind the emission map:
			if (useEmissionMaps && material->hasEmissionMap)
			{
				shader.setBool("useEmissionMap", true);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, material->emissionMapTexture.getGlId());
				shader.setInt("emissionTex", 2);
			}
			else
				shader.setBool("useEmissionMap", false);

			//----------------------------------
			//bind the roughness map

			if (material->hasRoughnessMap)
			{
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, material->roughnessMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "roughnessTex"), 3);
				shader.setBool("useRoughnessMap", true);
			}
			else shader.setBool("useRoughnessMap", false);

			//----------------------------------
			//bind the metallic map

			if (material->hasMetallicMap)
			{
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, material->metallicMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "metallicTex"), 4);
				shader.setBool("useMetallicMap", true);
			}
			else shader.setBool("useMetallicMap", false);

			//Draw the mesh:
			glBindVertexArray(intObjComp->model->VAO);

			glDrawElementsBaseVertex(GL_TRIANGLES,
				intObjComp->model->mEntries[j].numOfIndices,
				GL_UNSIGNED_INT,
				(void*)(intObjComp->model->mEntries[j].baseIndex * sizeof(unsigned int)),
				intObjComp->model->mEntries[j].baseVertex);

			glBindVertexArray(0);
		}

	}

	glDisable(GL_CULL_FACE);



}


//----------------------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderSceneCharacterModels(ShaderProgram& shader,
								const glm::mat4& viewAndProj,
								bool useNormalMaps,
								bool useEmissionMaps,
								int billboard)
{

	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);




	glm::mat4 model(1.0f);
	for (int i = 0; i < world->currentScene->characterComponents.getSize(); ++i)
	{

		

		CharacterComponent* charComp = &(world->currentScene->characterComponents[i]);
		myAssert(charComp);
		//std::cout << "-> Size: " << modelComp->model->mEntries[0].materialIndex << '\n';

		if (!charComp->isActived())
			continue;

		const Model* charModel = charComp->getModel();

		//std::cout << charModel->mMaterial.animations << ',' << charModel->mMaterial.framesPerAnimation << ',' 
		//	<< charComp->getCurrentRow() << ',' << charComp->getCurrentColumn() <<'\n';
		
		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), true);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), charModel->mMaterial.animations);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), charModel->mMaterial.framesPerAnimation);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), charComp->getCurrentRow());
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), charComp->getCurrentColumn());

		//-------------------------------------------------------
		//bind the bones data
		
		if (charModel->mBoneData.size() > 0 && charModel->sceneData.animations.size() > 0)
		{
			//std::cout << "Duration: " << modelComp->model->sceneData.animations[0].duration << '\t'
			//	<< modelComp->model->sceneData.animations[0].channels.size() << '\n';

			
			shader.setBool("useBones", true);
			myAssert(charModel->mBoneData.size() <= 50);

			FLOAT_TYPE duration = FLOAT_TYPE(charModel->sceneData.animations[charComp->getCurrentAnimation()].duration) 
				/ charModel->sceneData.animations[charComp->getCurrentAnimation()].ticksPerSecond;
		
			std::vector<glm::mat4> transforms;
			
			
			//update the mBoneData vector with the bones transforms
			//charComp->boneTransform(charComp->getCurrentAnimation(), charComp->getAnimationTime(), transforms);
			charComp->boneTransform(charComp->getCurrentAnimation(), charComp->getAnimationTime() , transforms);

			
			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "boneTransforms"),
				charComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(charComp->mBoneTransforms[0]));
		}
		else
			shader.setBool("useBones", false);

		

		model = getScaledFullTransfom(charComp->getEntityId());
		model[3] += glm::vec4(charComp->getModelPos(), 0.0f);

		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), charComp->getDirection() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = model * rotMat;

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix3fv(glGetUniformLocation(shader.getId(), "modelInverse"), 1, GL_FALSE,
			glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(model)))));

		
		for (int j = 0; j < charModel->mEntries.size(); ++j) //walk through all meshes of the model
		{
			const Material* material = &(charModel->mMaterial);

			myAssert(material);
			shader.setFLOAT_TYPE("materialSpecular", material->Ns);

			//----------------------------------
			//bind the albedo texture:
			if (material->hasTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, material->albedoTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
			}
			else
				continue;

			//----------------------------------
			//bind the normal map(if avaible and asked for):
			if (material->hasNormalMap && useNormalMaps)
			{
				shader.setBool("useNormalMap", true);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, material->normalMapTexture.getGlId());
				shader.setInt("normalsTex", 1);
			}
			else
				shader.setBool("useNormalMap", false);

			//----------------------------------
			//bind the emission map:
			if (useEmissionMaps && material->hasEmissionMap)
			{
				shader.setBool("useEmissionMap", true);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, material->emissionMapTexture.getGlId());
				shader.setInt("emissionTex", 2);
			}
			else
				shader.setBool("useEmissionMap", false);

			//----------------------------------
			//bind the roughness map

			if (material->hasRoughnessMap)
			{
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, material->roughnessMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "roughnessTex"), 3);
				shader.setBool("useRoughnessMap", true);
			}
			else shader.setBool("useRoughnessMap", false);
			//----------------------------------
			//bind the metallic map

			if (material->hasMetallicMap)
			{
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, material->metallicMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "metallicTex"), 4);
				shader.setBool("useMetallicMap", true);
			}
			else shader.setBool("useMetallicMap", false);

			//Draw the mesh:
			glBindVertexArray(charModel->VAO);

			

			glDrawElementsBaseVertex(GL_TRIANGLES,
				charModel->mEntries[j].numOfIndices,
				GL_UNSIGNED_INT,
				(void*)(charModel->mEntries[j].baseIndex * sizeof(unsigned int)),
				charModel->mEntries[j].baseVertex);

			glBindVertexArray(0);
		}

	}


	glDisable(GL_CULL_FACE);






}


//----------------------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderScene(ShaderProgram& shader, const glm::mat4& viewAndProj, bool useNormalMaps, 
	bool useEmissionMaps, int billboard)
//viewPort, framebuffer and framebuffer's attachments setup are meant to be
//handled by the caller of this funcion
{
	//First, draw the sprites from all imageComponents:
	
	
	glUseProgram(shader.getId());
	shader.setMat4("viewAndProj", viewAndProj);

	//useNormalMaps = false;
	//useEmissionMaps = false;
	
	shader.setBool("useBones", false);
	renderSceneImages(shader, viewAndProj, useNormalMaps, useEmissionMaps, billboard);
	
	//------------------------------
	//render models
	
	renderSceneModels(shader, viewAndProj, useNormalMaps, useEmissionMaps, billboard);

	
	//render intObjComp's models:
	renderSceneIntObjects(shader, viewAndProj, useNormalMaps, useEmissionMaps, billboard);

	
	//render character component's models:
	renderSceneCharacterModels(shader, viewAndProj, useNormalMaps, useEmissionMaps, billboard);
	


	


}




//=================================================================================================================



void GraphicalSystem::renderSceneImagesGeometry(ShaderProgram& shader, const glm::mat4& viewAndProj)
{
	shader.setBool("useBones", false);




	glm::mat4 pvm(1.0f);
	glBindVertexArray(spriteVAO);
	for (int i = 0; i < world->currentScene->imageComponents.getSize(); ++i)
	{
		ImageComponent* imagComp = &(world->currentScene->imageComponents[i]);
		//glUseProgram(programs[0].getId());

		if (!imagComp->actived)
			continue;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (*imagComp).spt.getTexture()->getGlId());
		//shader.setInt("tex", 0);
		glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);

		//get the transformation matrix:

		glm::mat4 model = getScaledFullTransfom(imagComp->getEntityId());
		pvm = viewAndProj * model;

		shader.setBool("useAlphaMap", false);
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "viewAndProj"), 1, GL_FALSE, glm::value_ptr(viewAndProj));
		//glUniformMatrix3fv(glGetUniformLocation(shader.getId(), "modelInverse"), 1, GL_FALSE,
			//glm::value_ptr(glm::transpose(glm::inverse(glm::mat3(model)))));

		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), imagComp->spt.rows);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), imagComp->spt.columns);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), imagComp->spt.currentRow);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), imagComp->spt.currentColumn);


		glDrawArrays(GL_TRIANGLES, 0, 12);
	}
}



//----------------------------------------------------------------------------------------------------------------




void GraphicalSystem::renderSceneModelsGeometry(ShaderProgram& shader, const glm::mat4& viewAndProj)
{
	glm::mat4 model(1.0f);
	glm::mat4 pvm(1.0f);

	glDisable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);



	for (int i = 0; i < world->currentScene->modelComponents.getSize(); ++i)
	{
		ModelComponent* modelComp = &(world->currentScene->modelComponents[i]);
		myAssert(modelComp);
		
		
		if (modelComp->model->mBoneData.size() > 0 && modelComp->model->sceneData.animations.size() > 0)
		{
		
			
			//std::cout << modelComp->getEntityId() << '\n';
			shader.setBool("useBones", true);
			myAssert(modelComp->model->mBoneData.size() <= 50);


			FLOAT_TYPE duration = FLOAT_TYPE(modelComp->model->sceneData.animations[0].duration) / modelComp->model->sceneData.animations[0].ticksPerSecond;

			std::vector<glm::mat4> transforms;

			//update the mBoneData vector with the bones transforms
			
			//modelComp->boneTransform(0, std::fmod(glfwGetTime(), duration), transforms);
			
			
			/*for (int i = 0; i < transforms.size(); ++i)
			{

				glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "boneTransforms"),
					1, GL_FALSE, &(transforms[i])[0][0]);
			}
			*/
			

			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "boneTransforms"),
				modelComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(modelComp->mBoneTransforms[0]));
			

			
			//for (int j = 0; j < modelComp->mBoneTransforms.size(); ++j)
			//{
				//glUniformMatrix4fv(glGetUniformLocation(shader.getId(), std::string("boneTransforms").c_str()),
					//modelComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(modelComp->mBoneTransforms[0]));
			//}
		}
		else
			shader.setBool("useBones", false);

		

		//modelComp->bindForDraw();
		
		model = getScaledFullTransfom(modelComp->getEntityId());
		model[3] += glm::vec4(modelComp->pos, 0.0f);
		
		
		pvm = viewAndProj * model;
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "viewAndProj"), 1, GL_FALSE, glm::value_ptr(viewAndProj));


		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), modelComp->model->mMaterial.animations);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), modelComp->model->mMaterial.framesPerAnimation);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), modelComp->currentRow);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), modelComp->currentColumn);

		
		
		for (int j = 0; j < modelComp->model->mEntries.size(); ++j) //walk through all meshes of the model
		{
			
			int materialId = modelComp->model->mEntries[j].materialIndex;
			if (modelComp->model->mMaterial.hasTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, modelComp->model->mMaterial.albedoTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
			}
			else
				continue;
			
			if (modelComp->model->mMaterial.hasAlphaMap)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, modelComp->model->mMaterial.alphaMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "alphaMap"), 1);
				shader.setBool("useAlphaMap", true);
			}
			else
				shader.setBool("useAlphaMap", false);

			//Draw the mesh:
			
			glBindVertexArray(modelComp->model->VAO);
			glDrawElementsBaseVertex(GL_TRIANGLES,
				modelComp->model->mEntries[j].numOfIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * modelComp->model->mEntries[j].baseIndex),
				modelComp->model->mEntries[j].baseVertex);

			
			glBindVertexArray(0);
			
			
		}
		
	

		

		glDisable(GL_CULL_FACE);

	}
	
	
}


//----------------------------------------------------------------------------------------------------------------



void GraphicalSystem::renderSceneIntObjectsGeometry(ShaderProgram& shader, const glm::mat4& viewAndProj)
{
	glm::mat4 model(1.0f);
	glm::mat4 pvm(1.0f);

	//Render InteractableObjectComponent's models
	for (int i = 0; i < world->currentScene->interactableObjectComponents.getSize(); ++i)
	{
		InteractableObjectComponent* intObjComp = &(world->currentScene->interactableObjectComponents[i]);
		myAssert(intObjComp);

		if (intObjComp->model == nullptr)
			continue;

		const CharacterComponent* charComp = nullptr; 

		//see if the object is holded by some character:
		if (intObjComp->holder >= 0)
		{
			charComp = world->currentScene->getCharacterComponent(intObjComp->holder);

			//use the character transform instead of the object
			model = getScaledFullTransfom(intObjComp->holder);
			model[3] += glm::vec4(charComp->getModelPos(), 0.0f);

			//rotate according to the direction the character is facing
			glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), charComp->getDirection() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = model * rotMat;
		}
		else //else, use the object transform
		{
			model = intObjComp->transform * getScaledFullTransfom(intObjComp->getEntityId());
			model[3] += glm::vec4(intObjComp->pos, 0.0f);
		}

		//modelComp->bindForDraw();

		//model = intObjComp->transform * getScaledFullTransfom(intObjComp->getEntityId());
		//model[3] += glm::vec4(intObjComp->pos, 0.0f);
			

		pvm = viewAndProj * model;
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "viewAndProj"), 1, GL_FALSE, glm::value_ptr(viewAndProj));


		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), 1);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), 1);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), 0);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), 0);



		for (int j = 0; j < intObjComp->model->mEntries.size(); ++j) //walk through all meshes of the model
		{
			int materialId = intObjComp->model->mEntries[j].materialIndex;
			if (intObjComp->model->mMaterial.hasTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, intObjComp->model->mMaterial.albedoTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
			}
			else
				continue;

			if (intObjComp->model->mMaterial.hasAlphaMap)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, intObjComp->model->mMaterial.alphaMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "alphaMap"), 1);
				shader.setBool("useAlphaMap", true);
			}
			else
				shader.setBool("useAlphaMap", false);

			
			if (intObjComp->model->mBoneData.size() > 0 && intObjComp->model->sceneData.animations.size() > 0)
			{
				
				shader.setBool("useBones", true);
				myAssert(intObjComp->model->mBoneData.size() <= 50);
							
				glUniformMatrix4fv(glGetUniformLocation(shader.getId(), std::string("boneTransforms").c_str()),
						intObjComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(intObjComp->mBoneTransforms[0]));
			}
			else
				shader.setBool("useBones", false);


			//Draw the mesh:
			glBindVertexArray(intObjComp->model->VAO);

			glDrawElementsBaseVertex(GL_TRIANGLES,
				intObjComp->model->mEntries[j].numOfIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * intObjComp->model->mEntries[j].baseIndex),
				intObjComp->model->mEntries[j].baseVertex);
		}

		glDisable(GL_CULL_FACE);

	}
}



//----------------------------------------------------------------------------------------------------------------




void GraphicalSystem::renderSceneCharacterModelsGeometry(ShaderProgram& shader, const glm::mat4& viewAndProj)
{
	glm::mat4 model(1.0f);
	glm::mat4 pvm(1.0f);

	for (int i = 0; i < world->currentScene->characterComponents.getSize(); ++i)
	{
		CharacterComponent* charComp = &(world->currentScene->characterComponents[i]);
		myAssert(charComp);

		const Model* charModel = charComp->getModel();
			 
		model = getScaledFullTransfom(charComp->getEntityId());
		model[3] += glm::vec4(charComp->getModelPos(), 0.0f);

		glm::mat4 rotMat = glm::rotate(glm::mat4(1.0), charComp->getDirection() * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = model * rotMat;

		pvm = viewAndProj * model;

		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "pvm"), 1, GL_FALSE, glm::value_ptr(pvm));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "viewAndProj"), 1, GL_FALSE, glm::value_ptr(viewAndProj));
		
		glUniform1i(glGetUniformLocation(shader.getId(), "modelSpriteSheet"), false); //!
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfRows"), charModel->mMaterial.animations);
		glUniform1i(glGetUniformLocation(shader.getId(), "numOfColumns"), charModel->mMaterial.framesPerAnimation);
		glUniform1i(glGetUniformLocation(shader.getId(), "textureRow"), charComp->getCurrentRow());
		glUniform1i(glGetUniformLocation(shader.getId(), "textureColumn"), charComp->getCurrentColumn());


		if (charModel->mBoneData.size() > 0 && charModel->sceneData.animations.size() > 0)
		{
			//std::cout << "Duration: " << modelComp->model->sceneData.animations[0].duration << '\t'
			//	<< modelComp->model->sceneData.animations[0].channels.size() << '\n';


			shader.setBool("useBones", true);
			myAssert(charModel->mBoneData.size() <= 50);

			//get the animation duration:
			FLOAT_TYPE duration = FLOAT_TYPE(charModel->sceneData.animations[charComp->getCurrentAnimation()].duration) / 
				charModel->sceneData.animations[charComp->getCurrentAnimation()].ticksPerSecond;

			std::vector<glm::mat4> transforms;

			//update the mBoneData vector with the bones transforms
			//charComp->boneTransform(charComp->getCurrentAnimation(), charComp->getAnimationTime(), transforms);


			glUniformMatrix4fv(glGetUniformLocation(shader.getId(), "boneTransforms"),
				charComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(charComp->mBoneTransforms[0]));
		}
		else
			shader.setBool("useBones", false);


		for (int j = 0; j < charModel->mEntries.size(); ++j) //walk through all meshes of the model
		{
			int materialId = charModel->mEntries[j].materialIndex;
			if (charModel->mMaterial.hasTexture)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, charModel->mMaterial.albedoTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "tex"), 0);
			}
			else
				continue;

			if (charModel->mMaterial.hasAlphaMap)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, charModel->mMaterial.alphaMapTexture.getGlId());
				glUniform1i(glGetUniformLocation(shader.getId(), "alphaMap"), 1);
				shader.setBool("useAlphaMap", true);
			}
			else
				shader.setBool("useAlphaMap", false);

			/*
			if (charModel->mBoneData.size() > 0 && charModel->sceneData.animations.size() > 0)
			{

				shader.setBool("useBones", true);
				myAssert(charModel->mBoneData.size() <= 50);

				FLOAT_TYPE duration = FLOAT_TYPE(charModel->sceneData.animations[0].duration) / charModel->sceneData.animations[0].ticksPerSecond;
				//const std::vector<Model::BoneData>* transforms = nullptr;

				for (int j = 0; j < charComp->mBoneTransforms.size(); ++j)
				{
					glUniformMatrix4fv(glGetUniformLocation(shader.getId(), std::string("boneTransforms").c_str()),
						charComp->mBoneTransforms.size(), GL_FALSE, glm::value_ptr(charComp->mBoneTransforms[0]));
				}
			}
			else
				shader.setBool("useBones", false);
			*/

			//Draw the mesh:
			glBindVertexArray(charModel->VAO);

			glDrawElementsBaseVertex(GL_TRIANGLES,
				charModel->mEntries[j].numOfIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * charModel->mEntries[j].baseIndex),
				charModel->mEntries[j].baseVertex);
		}

		glDisable(GL_CULL_FACE);

	}
}



//----------------------------------------------------------------------------------------------------------------




//=================================================================================================================

//render just to the depth attachment of the current bound framebuffer
void GraphicalSystem::renderSceneGeometry(ShaderProgram& shader, const glm::mat4& viewAndProj) 
{

	glUseProgram(shader.getId());
	
	//render images geometry:
	renderSceneImagesGeometry(shader, viewAndProj);

	//render models geometry:
	renderSceneModelsGeometry(shader, viewAndProj);

	//render interactables objects model's geometry:
	renderSceneIntObjectsGeometry(shader, viewAndProj);

	//render character components model's geometry:
	renderSceneCharacterModelsGeometry(shader, viewAndProj);

}