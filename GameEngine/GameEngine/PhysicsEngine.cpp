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

#include "PhysicsEngine.h"


//PhysicsEngine definitions:


void PhysicsEngine::initialize(World* w)
{
	world = w;
}





//--------------------------------------------------------------------------------------------------------------





void PhysicsEngine::update() //integrate by timeStep seconds
{
	solveForSpheres();
	solveForBoxes();

	world->currentScene->particleSystem.update(timeStep);

}




//--------------------------------------------------------------------------------------------------------------




FLOAT_TYPE PhysicsEngine::getTimeStep() const noexcept
{
	return timeStep;
}

//--------------------------------------------------------------------------------------------------------------

void PhysicsEngine::setTimeStep(FLOAT_TYPE ts) noexcept
{
	timeStep = ts;
}

//-------------------------------------------------------------------------------------------------------------

void PhysicsEngine::addSpherePhysicalComponent(Entity id, FLOAT_TYPE radius, glm::vec3 pos)
{
	if (!world->currentScene->getEntity(id))
		std::logic_error("ERROR::INVALID ARGUMENT PASSED TO PhysicsEngine::addSpherePhysicalComponent(): INVALID ID;\n");
	//create a component
	RigidBodyComponent<Sphere> comp(id, Sphere(radius, pos)); 
	//and add a copy of it to the current scene
	world->currentScene->sphereRigidBodyComponents.push_back(comp);
}


//------------------------------------------------------------------------------------------------------------

void PhysicsEngine::addBoxPhysicalComponent(Entity id, int x, int y, int z, glm::vec3 position)
{
	if (!world->currentScene->getEntity(id))
		std::logic_error("ERROR::INVALID ARGUMENT PASSED TO PhysicsEngine::addBoxPhysicalComponent(): INVALID ID;\n");

	Box b;
	b.setSize(x, y, z);
	b.pos = position;
	//create a component
	RigidBodyComponent<Box> comp(id, b);
	//and add a copy of it to the current scene
	world->currentScene->boxRigidBodyComponents.push_back(comp);
}

//------------------------------------------------------------------------------------------------------------



void PhysicsEngine::onNotify(Message msg)
{
	//std::cout << "HEEE" << int(msg.type) << '\n';
	//std::cout << "Physics: " << numOfMessages << '\n';
	switch (msg.type)
	{
	case MessageType::APPLY_FORCE:
	{
		
		RigidBodyComponent<Box>* boxComp = world->currentScene->getBoxRigidBodyComponent(msg.data);
		myAssert(boxComp);
		boxComp->addLinearVelocity(glm::vec3(msg.fdata[0], msg.fdata[1], msg.fdata[2]) / boxComp->mass);
		break;
	}
	case MessageType::APPLY_VERTICAL_FORCE:
	{
		RigidBodyComponent<Box>* boxComp = world->currentScene->getBoxRigidBodyComponent(msg.data);
		myAssert(boxComp);
		//std::cout << fabs(boxComp->linearVelocity.y) << '\n';
		//if(fabs(boxComp->deltaSpace.z) < 0.1f)
		boxComp->addLinearVelocity(glm::vec3(0.0f, msg.fdata[0] / boxComp->mass, 0.0f));

	}
	case MessageType::DEFAULT_MESSAGE:
		return;
	default:
		return;
	}
}




//-------------------------------------------------------------------------------------------------------------



glm::mat4 PhysicsEngine::normalizeRows(int nRows, glm::mat4 mat) const noexcept
//normalize the first nRows three-dimensional rows of a matrix
{
	for (int i = 0; i < nRows; ++i)
	{
		FLOAT_TYPE lenght = std::sqrt((mat[i][0] * mat[i][0]) + (mat[i][1] * mat[i][1]) + (mat[i][2] * mat[i][2]));
		if (lenght <= 0.0001f) lenght = 0.001f;
		mat[i][0] /= lenght;
		mat[i][1] /= lenght;
		mat[i][2] /= lenght;
	}

	return mat;
}

//-----------------------------------------------------------------------------------------------------------

glm::mat4 PhysicsEngine::getFullTransform(Entity id) const
{
	myAssert(world->currentScene->getEntity(id));
	glm::mat4 fullTransform = glm::mat4(1.0f);


	//iterate over all TransformComponents 
	for (int i = 0; i < world->currentScene->transformComponents.getSize(); ++i)
	{
		if (world->currentScene->transformComponents[i].getEntityId() == id)
		{
			fullTransform = 
				normalizeRows(3, world->currentScene->transformComponents[i].getTransform());

			const TransformComponent* current = &(world->currentScene->transformComponents[i]);
			while (current->getParent() >= 0)
			{
				//set current to the TransformComponent of it's parent
				current = world->currentScene->getTransformComponent(current->getParent());
				//combine the parent and child transform: (note that the parent's scale will not affect
				//it childs, this is done normalizing the collums of the 3x3 uper matrix)
				fullTransform = normalizeRows(3, current->getTransform()) * fullTransform;
			}
			break;
		}
	}
	return fullTransform;
}


//----------------------------------------------------------------------------------------------------------



glm::quat PhysicsEngine::getFullRotationQuaternion(Entity id) const
{
	myAssert(world->currentScene->getEntity(id));
	glm::quat fullQuaternion;


	//iterate over all TransformComponents 
	for (int i = 0; i < world->currentScene->transformComponents.getSize(); ++i)
	{
		if (world->currentScene->transformComponents[i].getEntityId() == id)
		{
			fullQuaternion = 
				world->currentScene->transformComponents[i].getOrientation();

			const TransformComponent* current = &(world->currentScene->transformComponents[i]);
			while (current->getParent() >= 0)
			{
				//set current to the TransformComponent of it's parent
				current = world->currentScene->getTransformComponent(current->getParent());
				//combine the parent and child transform: (note that the parent's scale will not affect
				//it childs, this is done normalizing the collums of the 3x3 uper matrix)
				fullQuaternion = current->getOrientation() * fullQuaternion;
			}
			break;
		}
	}
	return fullQuaternion;
}


//-------------------------------------------------------------------------------------------------



void PhysicsEngine::solveForSpheres() 
//update and solve collision for all sphere rigid bodies in the scene
{

	//for each sphere rigid body in the scene
	for (int i = 0; i < world->currentScene->sphereRigidBodyComponents.getSize(); ++i)
	{
		RigidBodyComponent<Sphere>* sphereComp = &(world->currentScene->sphereRigidBodyComponents[i]);
		//update it's data:
		sphereComp->linearVelocity += (sphereComp->forces * timeStep) / sphereComp->mass;
		glm::vec3 deltaS = sphereComp->linearVelocity;

		//reset forces:
		sphereComp->forces *= 0.0f;

		//update position:
		sphereComp->shape.pos += deltaS;
		world->currentScene->getTransformComponent(sphereComp->getEntityId())->setPosition(sphereComp->shape.pos);

		//apply air resistance
		sphereComp->linearVelocity *= 0.8f;
		if (glm::length(sphereComp->linearVelocity) <= 0.1) sphereComp->linearVelocity = glm::vec3(0.0f);

		//handle collisions:
		//auto iter2 = iter;
		int i2 = i + 1;
		glm::vec3 separVec;
		//test collision bettween this sphere and the other spheres in the scene
		for (; i2 < world->currentScene->sphereRigidBodyComponents.getSize(); ++i2)
		{
			RigidBodyComponent<Sphere>* sphereComp2 = &(world->currentScene->sphereRigidBodyComponents[i2]);
			if (detectSphereToSphere(sphereComp->shape, sphereComp2->shape, separVec))
				resolveSphereToSphere(*sphereComp, *sphereComp2, separVec);
		}
	}

}



//-----------------------------------------------------------------------------------------------------------


void PhysicsEngine::solveForBoxes()
{
	//std::cout << "-------------------------\n";
	//for each box rigid body in the scene
	for (int i = 0; i < world->currentScene->boxRigidBodyComponents.getSize(); ++i)
	{
		RigidBodyComponent<Box>* boxComp = &(world->currentScene->boxRigidBodyComponents[i]);
		//update it's data:
		boxComp->linearVelocity += (boxComp->forces * timeStep) / (boxComp->mass > 0.0f ? boxComp->mass : 1.0f);
		if (boxComp->mass > 0.0f) boxComp->linearVelocity.y += -10.0f * timeStep;
		glm::vec3 deltaS = boxComp->linearVelocity;
		
		//reset forces:
		boxComp->forces *= 0.0f;

		//update position:
		//iter->deltaSpace = -iter->shape.pos;
		boxComp->shape.pos += deltaS;
		//iter->deltaSpace += iter->shape.pos;
		world->currentScene->getTransformComponent(boxComp->getEntityId())->setPosition(boxComp->shape.pos);

		//apply(fake) air resistance forces:
		boxComp->linearVelocity *= 0.90f;

		if (std::fabs(boxComp->linearVelocity.x) <= 0.025) boxComp->linearVelocity.x = 0.0f;
		if (std::fabs(boxComp->linearVelocity.y) <= 0.04) boxComp->linearVelocity.y = 0.0f;
		if (std::fabs(boxComp->linearVelocity.z) <= 0.025) boxComp->linearVelocity.z = 0.0f;
		

		//-------------------------------
		//handle collisions:
		//auto iter2 = iter;
		int i2 = i + 1;
		glm::vec3 separVec; //used to correct penetration

		glm::mat4 model = getFullTransform(boxComp->getEntityId()); //used to convert the box vertices to world space

		//test collision bettween this box and the other boxes in the scene
		for (; i2 < world->currentScene->boxRigidBodyComponents.getSize(); ++i2)
		{
			RigidBodyComponent<Box>* boxComp2 = &(world->currentScene->boxRigidBodyComponents[i2]);

			if (boxComp->mass <= 0 && boxComp2->mass <= 0) continue; //don't solve for objects with infinite mass

			//first do a simple bounding sphere test:
			if (!boundSphereTest(boxComp->shape.pos, boxComp2->shape.pos,
				glm::length(boxComp->shape.getVertex(0)),
				glm::length(boxComp2->shape.getVertex(0))))
				//glm::max(boxComp->shape.getSize().x, glm::max(boxComp->shape.getSize().y, boxComp->shape.getSize().z)), //!
				//glm::max(boxComp2->shape.getSize().x, glm::max(boxComp2->shape.getSize().y, boxComp2->shape.getSize().z)))) 
				continue;
	
			//----------------------------
			//collision might have happened, do a more complex test:
			glm::vec3 mtv;
			glm::mat4 model2 = getFullTransform(boxComp2->getEntityId());
	
			bool a2 = (boxComp->getEntityId() == 0 && boxComp2->getEntityId() == 2) ? true : false;
			if (!detectBoxToBox2(boxComp2->shape, boxComp->shape, model2, model, mtv, a2)) continue; //collision detection failed

			resolveBoxToBox(*boxComp2, *boxComp, mtv);

			//----------------------------
			Message msg; //a message notifiyng the collision
			msg.type = MessageType::COLLISION_OCCURRED;
			msg.fdata[0] = boxComp->getEntityId();  //the id of the first body
			msg.fdata[1] = boxComp2->getEntityId(); //and the id of the second
			msg.fdata[2] = boxComp->shape.pos.y - boxComp2->shape.pos.y; //if this is positve, then box1 is above box2

			storeMessage(msg); //store the message(it will be sent in the frame's end)
		}

		//test collision between this box and each interactable object component's boxes in the scene
		for (int j = 0; j < world->currentScene->interactableObjectComponents.getSize(); ++j)
		{
			const InteractableObjectComponent* intObjComp = &(world->currentScene->interactableObjectComponents[j]);
			
			if (!intObjComp->actived || 
				!intObjComp->isEffectActive ||
				 intObjComp->getEntityId() == boxComp->getEntityId() ||
				 intObjComp->holder == boxComp->getEntityId())
				continue;

			//do a bounding sphere test
			if (!boundSphereTest(boxComp->shape.pos, intObjComp->hitBox.pos,
				glm::length(boxComp->shape.getVertex(0)),
				glm::length(intObjComp->hitBox.getVertex(0))))
				//glm::max(boxComp->shape.getSize().x, glm::max(boxComp->shape.getSize().y, boxComp->shape.getSize().z)), //!
				//glm::max(intObjComp->hitBox.getSize().x, glm::max(intObjComp->hitBox.getSize().y, intObjComp->hitBox.getSize().z))))
				continue;

			//----------------------------
			//collision might have happened, do a more complex test:
			glm::vec3 mtv;
			
			glm::mat4 model2 = intObjComp->transform;
			 
			//bool a2 = (boxComp->getEntityId() == 0 && boxComp2->getEntityId() == 2) ? true : false;
			if (!detectBoxToBox2(intObjComp->hitBox, boxComp->shape, model2, model, mtv, false)) continue; //collision detection failed

			glm::vec3 holderInpulse;
			resolveBoxToObjBox(intObjComp->hitBox, 10.0f, 8.0f, *boxComp, mtv, holderInpulse);

			holderInpulse = glm::normalize(holderInpulse) * glm::min(glm::length(holderInpulse), 1.0f);

			if (intObjComp->holder >= 0)
				world->currentScene->getBoxRigidBodyComponent(intObjComp->holder)->addLinearVelocity(
					(holderInpulse));

			//std::cout << boxComp->getEntityId() << " Collided with " << intObjComp->getEntityId() << '\n';

			//----------------------------
			Message msg; //a message notifiyng the collision
			msg.type = MessageType::COLLISION_OCCURRED;
			msg.idata[0] = boxComp->getEntityId();  //the id of the first body
			msg.idata[1] = intObjComp->getEntityId(); //and the id of the second
			msg.fdata[0] = mtv.x;
			msg.fdata[1] = mtv.y;
			msg.fdata[2] = mtv.z;

			storeMessage(msg); //store the message(it will be sent in the frame's end)

		}


	}

}