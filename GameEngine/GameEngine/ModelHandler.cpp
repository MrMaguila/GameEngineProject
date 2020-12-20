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


#include "ModelHandler.h"



//##############################################################################



void ModelHandler::loadModel(std::string path, std::string name, bool useMaterial, 
	int nRows, int nCollums, bool glbFileType)
{
	//path must contain the address of the folder that contains the .fbx defining the model. The
	//textures also must be on a .fbm folder within this same folder. The name string contain the name of the .obj

	std::cout << "\n\nTrying to load: " << path + name << "...\n";

	//----------------------------------------------------------------
	//create a model using the Model::init() function

	models.push_back(Model()); //create a model
	
	//and initialize it
	models[models.size() - 1].loadFromFile(path + name, nRows, nCollums, glbFileType);

	//---------------------------------------------------------------
	std::cout << "Successfully loaded " << name << ";\n\n";

}



const Model* ModelHandler::getModel(int id) const
{
	if(!(id >= 0 && id < models.size()))
		throw std::logic_error("ERROR::INVALID ARGUMENT PASSED TO ModelHandler::getModel();\n");

	return &(models[id]);
}
