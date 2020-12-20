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

#include "World.h"


//Scene class definitions:


Scene::Scene(int id)
	:sceneId{ id }, 
	transformComponents{ 50, TransformComponent() },
	imageComponents{ 50, ImageComponent() },
	dirLightComponents{ 50, DirLightComponent() },
	pointLightComponents{ 50, PointLightComponent() },
	sphereRigidBodyComponents{ 50, RigidBodyComponent<Sphere>() },
	boxRigidBodyComponents{ 50, RigidBodyComponent<Box>() },
	characterComponents{ 50, CharacterComponent() },
	modelComponents{ 50, ModelComponent() },
	interactableObjectComponents{ 50, InteractableObjectComponent() }
{


}

//==================================================================================================


Entity Scene::createEntity() noexcept
{
	Entity id = 0; 
	auto next = entities.begin();
	
	for (auto iter = entities.begin(); iter != entities.end(); ++iter)
	{
		next = iter;
		++next;

		if (next == entities.end()) //no gap found, so put it at the end
		{
			++id;
			break;
		}

		if (*next - *iter > 1) //if there's a gap between two elements
		{
			id = (*iter) + 1;
			break;
		}
		++id;
	}

	entities.insert(id); //no elements in the entities set

	//Attach a Transform Component to it:
	TransformComponent tComp(id);
	transformComponents.push_back(tComp);

	return id;
}


//==================================================================================================


void Scene::deleteEntity(Entity id)
{
	myAssert(getEntity(id));

	//remove all of it's components:
	
	//transform component:
	for(int i = 0; i < transformComponents.getSize(); ++i)
		if (transformComponents[i].getEntityId() == id) { transformComponents.erase(i); break; }

	//image component:
	for(int i = 0; i < imageComponents.getSize(); ++i)
		if (imageComponents[i].getEntityId() == id) { imageComponents.erase(i); break; }

	//dirLight component:
	for (int i = 0; i < dirLightComponents.getSize(); ++i)
		if (dirLightComponents[i].getEntityId() == id) { dirLightComponents.erase(i); break; }

	//pointLight component:
	for (int i = 0; i < pointLightComponents.getSize(); ++i)
		if (pointLightComponents[i].getEntityId() == id) { pointLightComponents.erase(i); break; }

	//sphereRigidBody component:
	for(int i = 0; i < sphereRigidBodyComponents.getSize(); ++i)
		if (sphereRigidBodyComponents[i].getEntityId() == id) { sphereRigidBodyComponents.erase(i); break; }

	//boxRigidBody component:
	for (int i = 0; i < boxRigidBodyComponents.getSize(); ++i)
		if (boxRigidBodyComponents[i].getEntityId() == id) { boxRigidBodyComponents.erase(i); break; }

	//character component:
	for (int i = 0; i < characterComponents.getSize(); ++i)
		if (characterComponents[i].getEntityId() == id) { characterComponents.erase(i); break; }

	//model component:
	for(int i = 0; i < modelComponents.getSize(); ++i)
		if (modelComponents[i].getEntityId() == id) { modelComponents.erase(i); break; }

	//interactableObject component:
	for (int i = 0; i < interactableObjectComponents.getSize(); ++i)
		if (interactableObjectComponents[i].getEntityId() == id) { interactableObjectComponents.erase(i); break; }


	entities.erase(id); //remove it from the Entities container
}


//==================================================================================================


bool Scene::getEntity(Entity id) const noexcept
{
	for (auto iter = entities.begin(); iter != entities.end(); ++iter)
		if (*iter == id) return true;

	return false; //there's no Entity with such id in the Scene
}


//==================================================================================================


int Scene::getNumOfEntities() const noexcept
{
	return entities.size();
}


//==================================================================================================


unsigned int Scene::getId() const noexcept
{
	return sceneId;
}


//==================================================================================================


ImageComponent* Scene::getImageComponent(Entity id)
{
	for (int i = 0; i < imageComponents.getSize(); ++i)
		if (imageComponents[i].getEntityId() == id) return &(imageComponents[i]);
	myAssert(false);

	return nullptr;
}


//==================================================================================================


TransformComponent* Scene::getTransformComponent(Entity id)
{
	for (int i = 0; i < transformComponents.getSize(); ++i)
		if (transformComponents[i].getEntityId() == id) return &(transformComponents[i]);
	myAssert(false);

	return nullptr;
}


//==================================================================================================


DirLightComponent* Scene::getDirLightComponent(Entity id)
{
	for (int i = 0; i < dirLightComponents.getSize(); ++i)
		if (dirLightComponents[i].getEntityId() == id) return &(dirLightComponents[i]);
	myAssert(false);

	return nullptr;
}


//==================================================================================================


PointLightComponent* Scene::getPointLightComponent(Entity id)
{
	for (int i = 0; i < pointLightComponents.getSize(); ++i)
		if (pointLightComponents[i].getEntityId() == id) return &(pointLightComponents[i]);
	myAssert(false);
	return nullptr;

}


//==================================================================================================


RigidBodyComponent<Sphere>* Scene::getSphereRigidBodyComponent(Entity id)
{
	for (int i = 0; i < sphereRigidBodyComponents.getSize(); ++i)
		if (sphereRigidBodyComponents[i].getEntityId() == id) return &(sphereRigidBodyComponents[i]);
	myAssert(false);

	return nullptr;

}


//==================================================================================================


RigidBodyComponent<Box>* Scene::getBoxRigidBodyComponent(Entity id)
{
	for (int i = 0; i < boxRigidBodyComponents.getSize(); ++i)
		if (boxRigidBodyComponents[i].getEntityId() == id) return &(boxRigidBodyComponents[i]);
	myAssert(false);

	return nullptr;
}


//==================================================================================================


CharacterComponent* Scene::getCharacterComponent(Entity id)
{
	for (int i = 0; i < characterComponents.getSize(); ++i)
		if (characterComponents[i].getEntityId() == id) return &(characterComponents[i]);
	myAssert(false);
	
	return nullptr;
}


//==================================================================================================


ModelComponent* Scene::getModelComponent(Entity id)
{
	for (int i = 0; i < modelComponents.getSize(); ++i)
		if (modelComponents[i].getEntityId() == id) return &(modelComponents[i]);
	myAssert(false);

	return nullptr;
}


//==================================================================================================



InteractableObjectComponent* Scene::getInteractableObjectComponent(Entity id)
{
	for (int i = 0; i < interactableObjectComponents.getSize(); ++i)
		if (interactableObjectComponents[i].getEntityId() == id) return &(interactableObjectComponents[i]);
	myAssert(false);

	return nullptr;
}


//###############################################################################################


World::~World()
{
	for (auto iter = scenes.begin(); iter != scenes.end(); ++iter)
	{
		delete (*iter); //delete the Scene that (*iter) points to
	}
}


//==================================================================================================


void World::initalize()
{
	//create numberOfScenes empty Scenes and stores them in the scenes list:
	for (int i = 0; i < numberOfScenes; ++i)
		scenes.push_back(new Scene(i));
}


//==================================================================================================


void World::loadScene(std::string path, int id)
//load the data of a Scene from file
{
	
}

//==================================================================================================

void World::saveSceneData(std::string, int)
//save a Scene data in a file
{


}

//==================================================================================================

void World::unloadScene(int id)
{
	if (!currentScene) return;
	if (currentScene->getId() == id) myAssert(false);
}

//==================================================================================================

void World::setCurrentScene(int id)
{
	myAssert(id >= 0 && id < numberOfScenes);
	for (auto iter = scenes.begin(); iter != scenes.end(); ++iter)
		if ((*iter)->getId() == id)
		{
			currentScene = *iter;
			return;
		}
	myAssert(false);
}

//==================================================================================================

Scene* World::getScene(int id)
{
	myAssert(id >= 0 && id < numberOfScenes);
	for (auto iter = scenes.begin(); iter != scenes.end(); ++iter)
		if ((*iter)->getId() == id) return *iter;
}