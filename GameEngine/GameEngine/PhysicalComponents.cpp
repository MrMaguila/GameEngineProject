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

#include "PhysicalComponents.h"



//============================================================================================
//Sphere definitions:


Sphere::Sphere()
	: radius{ 0.0f }, pos{ glm::vec3(0.0f, 0.0f, 0.0f) } 
{
}

//-------------------------------------------------------------------------------------------

Sphere::Sphere(FLOAT_TYPE r, glm::vec3 p)
	: radius{ r }, pos{ p } 
{
}

//-------------------------------------------------------------------------------------------

ShapeType Sphere::getType() const noexcept
{
	return ShapeType::sphere; 
}

//-------------------------------------------------------------------------------------------

glm::vec3 Sphere::getSize() const noexcept
{
	return glm::vec3(radius);
}

//-------------------------------------------------------------------------------------------

void Sphere::setSize(int nr, int, int) noexcept
{
	radius = nr;
}


//============================================================================================
//Box definitions:


void Box::setSize(int xSize, int ySize, int zSize) noexcept
{
	//set the size vector
	size = glm::vec3(xSize, ySize, zSize);
	FLOAT_TYPE xs = xSize / 2.0f;
	FLOAT_TYPE ys = ySize / 2.0f;
	FLOAT_TYPE zs = zSize / 2.0f;

	//and configure the vertices(the vertices are in local space, they are converted to world space by
	//using the TransformComponent's transform matrix)
	vertices[0] = glm::vec3(xs, ys, -zs); //top left of front face(beeing seen from the direction of +z)
	vertices[1] = glm::vec3(-xs, ys, -zs); //top right of front face
	vertices[2] = glm::vec3(xs, -ys, -zs); //botton left of front face
	vertices[3] = glm::vec3(-xs, -ys, -zs);//botton right of front face

	vertices[4] = glm::vec3(xs, ys, zs); //top left of back face
	vertices[5] = glm::vec3(-xs, ys, zs); //top right of back face
	vertices[6] = glm::vec3(xs, -ys, zs); //botton left of back face
	vertices[7] = glm::vec3(-xs, -ys, zs);//botton right of back face
}

//-------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------

glm::vec3 Box::getVertex(int i) const
{
	myAssert(i >= 0 && i < 8);
	return vertices[i];
}

//-------------------------------------------------------------------------------------------

glm::vec3 Box::getSize() const noexcept
{
	return size;
}





//============================================================================================
//RigidBodyComponent definitions:

template<typename T>
void RigidBodyComponent<T>::update() noexcept
{

}


//--------------------------------------------------------------------------------------------

template<typename T>
void RigidBodyComponent<T>::addForce(glm::vec3 force) noexcept
{
	forces += force;
}

//--------------------------------------------------------------------------------------------

template<typename T>
void RigidBodyComponent<T>::addLinearVelocity(glm::vec3 vel) noexcept
{
	linearVelocity += vel;
}


//--------------------------------------------------------------------------------------------

template<typename T>
void RigidBodyComponent<T>::move(glm::vec3 m) noexcept
{
	if(mass >= 0.0)
		shape.pos += m;
}

//--------------------------------------------------------------------------------------------

template<typename T>
void RigidBodyComponent<T>::setMass(FLOAT_TYPE m) noexcept
{
	mass = m; 
	//infinite mass
}

//--------------------------------------------------------------------------------------------

template<typename T>
glm::vec3 RigidBodyComponent<T>::getSize() const  noexcept
{
	return shape.getSize();
}



//--------------------------------------------------------------------------------------------

template<typename T>
glm::vec3 RigidBodyComponent<T>::getPosition() const  noexcept
{
	return shape.pos;
}



//--------------------------------------------------------------------------------------------

template<typename T>
void RigidBodyComponent<T>::setPosition(glm::vec3 newPos) noexcept
{
	shape.pos = newPos;
}


//--------------------------------------------------------------------------------------------

template<typename T>
void RigidBodyComponent<T>::setSize(glm::vec3 newSize) noexcept
{
	shape.setSize(newSize.x, newSize.y, newSize.z);
}



//--------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------

template<typename T>
RigidBodyComponent<T>::RigidBodyComponent()
	:Component{ -1 }
{

}


//--------------------------------------------------------------------------------------------

template<typename T>
RigidBodyComponent<T>::RigidBodyComponent(Entity id, T s)
	:Component{ id }, shape{ s }
{

}


//###############################################################################################


//Explicit template initialization:
template class RigidBodyComponent<Sphere>;
template class RigidBodyComponent<Box>;
