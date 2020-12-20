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

#include "Entity.h"



//Component definitions:

Component::Component(Entity id)
	:entityId{ id }
{}

Entity Component::getEntityId() const noexcept
{
	return entityId;
}

bool Component::isActived() const noexcept
{
	return actived;
}

void Component::enable() noexcept
{
	actived = true;
}

void Component::disable() noexcept
{
	actived = false;
}