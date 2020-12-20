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


//This header is part of a self made game engine. It declares the
//ModelHandler class, that is used to load and store all textures in the game.
*/
//#################################################################################

#ifndef MODEL_HANDLER
#define MODEL_HANDLER

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <exception>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "ModelComponent.h"

#include "GlobalDefines.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

/*
//###################################################################################

Face readFaceData(const std::string& line);

//================================================

glm::vec3 read3FVector(const std::string& line);

//================================================

glm::vec2 read2FVector(const std::string& line);


//===============================================

Material loadMaterial(const std::string& path, const std::string& name); //note: this function allocates GPU memory
																//and may throw exceptions
*/
//###################################################################################

class ModelHandler //note: this class is a singleton
{
public:

	//public functions:
	static ModelHandler& instance()
	{
		static ModelHandler modelHandlerInstance = ModelHandler();
		return *(&modelHandlerInstance);
	}
	
	~ModelHandler()
	{
		for (int i = 0; i < models.size(); ++i)
			models[i].clearMemory();
	}

	/*
		loadModel - this function loads a model from file
					params:
							the path to the file that contain the model,
							the model file name,
							load textures?,
							number of rows of the textures(they will be used like a spritesheet to make animations)
							number of collums of the textures,
							if the model type is glfb(it requires some tweaks)
	*/
	void loadModel(std::string path, std::string name, bool useMaterial = true, int nRows = 1, 
							int nCollums = 1, bool glbFileType = false);

	

	const Model* getModel(int id) const; //the model id is the same as it's index in the models vector, so 
									//the first loaded model will have id 0, the second id 1, and so forth.

private:

	//private functions:
	ModelHandler() {};

	//private data:
	std::vector<Model> models;
};


#endif // !MODEL_HANDLER
