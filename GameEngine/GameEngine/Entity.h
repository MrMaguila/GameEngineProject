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



this header defines the Component class, which is a base class for all types of
components in the game, like phisicalComponent, graphicalComponent and so on. Components 
and entities are meant to be stored in the World and Scene classes, so all a Component need
to hold is its specific data and an integer that represents the entity in which it belongs.
*/
//###########################################################################################

#ifndef ENTITY_HANDLER
#define ENTITY_HANDLER


#include "GlobalDefines.h"

using Entity = int; //an entity is just an indentifier


//##################################################################################
//the Component base class:


class Component
{
public:
	Component(Entity);
	
	virtual void update()=0;

	Entity getEntityId() const noexcept;
	bool isActived() const noexcept;
	void enable() noexcept;
	void disable() noexcept;

protected:
	~Component() {}; //don't allow a pointer to a Component be deleted

	Entity entityId = 0;
	bool actived = true;
};


#endif // !ENTITY_HANDLER
