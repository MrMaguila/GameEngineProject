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



This header defines the world class, it is responsible for storing all the scenes in the game. 
The Scene class stores entities and components that belongs to a specific area in the game. 
*/
//#################################################################################################



#ifndef WORLD
#define WORLD


#include <cassert>
#include <string>
#include <iostream>
#include <list>
#include <set>
#include "ObjectPool.h"
#include "Entity.h"

#include "hudHandling.h"
#include "ImageComponent.h"
#include "TransformComponent.h"
#include "LightComponent.h"
#include "PhysicalComponents.h"
#include "CharacterComponent.h"
#include "ModelComponent.h"
#include "InteractableObjectComponent.h"
#include "ParticleSystem.h"

#include "GlobalDefines.h"

//############################################################################################################


class Scene
{
public:
	Scene(int);

//Functions:
	Entity createEntity() noexcept;
	//this function will create an Entity, add a TransformComponent to it and return it's id

	void deleteEntity(Entity);
	//remove a entity and all of its components from the scene

	bool getEntity(Entity) const noexcept; 
	//returns true if the passed entity is in the scene

	int getNumOfEntities() const noexcept;
	unsigned int getId() const noexcept;

	ImageComponent* getImageComponent(Entity);
	TransformComponent* getTransformComponent(Entity);
	DirLightComponent* getDirLightComponent(Entity);
	PointLightComponent* getPointLightComponent(Entity);
	RigidBodyComponent<Sphere>* getSphereRigidBodyComponent(Entity);
	RigidBodyComponent<Box>* getBoxRigidBodyComponent(Entity);
	CharacterComponent* getCharacterComponent(Entity);
	ModelComponent* getModelComponent(Entity);
	InteractableObjectComponent* getInteractableObjectComponent(Entity);

//Public Data:
	/*
	std::list<TransformComponent> transformComponents;
	std::list<ImageComponent> imageComponents;
	std::list<DirLightComponent> dirLightComponents;
	std::list<PointLightComponent> pointLightComponents;
	std::list<RigidBodyComponent<Sphere>> sphereRigidBodyComponents;
	std::list<RigidBodyComponent<Box>> boxRigidBodyComponents;
	std::list<CharacterComponent> characterComponents;*/
	ObjectPool<TransformComponent> transformComponents;
	ObjectPool<ImageComponent> imageComponents;
	ObjectPool<DirLightComponent> dirLightComponents;
	ObjectPool<PointLightComponent> pointLightComponents;
	ObjectPool<RigidBodyComponent<Sphere>> sphereRigidBodyComponents;
	ObjectPool<RigidBodyComponent<Box>> boxRigidBodyComponents;
	ObjectPool<CharacterComponent> characterComponents;
	ObjectPool<ModelComponent> modelComponents;
	ObjectPool<InteractableObjectComponent> interactableObjectComponents;

	ParticleSystem particleSystem;

private:
//Private Data:
	std::set<Entity> entities;
	int sceneId;
};


//############################################################################################################
//The World class:



class World
{
public:

	~World();

//Functions:

	void initalize();
	void loadScene(std::string, int sceneId); //load a Scene from file
	void saveSceneData(std::string, int sceneId); //save a Scene data in the file
	void unloadScene(int sceneId); //free the memory of a Scene. Note: the current Scene cannot be unloaded;
	void setCurrentScene(int);
	Scene* getScene(int);

//Public data:
	Scene* currentScene = nullptr;

	//Huds:
	Hud<0, 4> playerStatusBar;
	int playerId = 0;

private:

//Private data:
	int numberOfScenes = 1;
	std::list<Scene*> scenes;
};


#endif // !WORLD
