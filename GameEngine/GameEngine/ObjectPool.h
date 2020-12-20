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


This header is part of a self made game engine. It declares the
ObjectPool class, used to allocate large chunks of memory
*/
//#################################################################################

#ifndef OBJECT_POOL
#define OBJECT_POOL


#include <cassert>
#include <utility>
#include <iostream>
#include <vector>

#include "GlobalDefines.h"

//##################################################
//ObjectPool class declaration:


template<typename T>
class ObjectPool
{
public:

	/* 
		constructor
				params: initial size
						default value for all pool elements
	*/
	ObjectPool(int, const T&);


	T& operator[](int) noexcept;

	void push_back(const T&) noexcept; 
	void erase(int) noexcept;  
	int getReseved() const noexcept;
	int getSize() const noexcept;
	std::pair<T, bool>& front() noexcept;
	std::pair<T, bool>& back() noexcept;

private:

	int size; //number of active Ts in the pool
	T errorElem;

	std::vector<std::pair<T, bool>> pool;
};

//=================================================


//=================================================


template<typename T>
ObjectPool<T>::ObjectPool(int n, const T& defaultVal)
	:size{ 0 }, errorElem{ defaultVal }
{
	myAssert(n != 0);
	pool = std::vector<std::pair<T, bool>>(n, std::pair<T, bool>(defaultVal, false)); //initialize the pool
}


//=================================================


template<typename T>
T& ObjectPool<T>::operator[](int i) noexcept
{
	if (!(i >= 0 && i < pool.size() && i < size))
	{
		std::cout << "WARNING::ObjectPool::operator[] WAS CALLED WITH INVALID ARGUMENTS. "
			"FUNCTION HAS EXITED TO AVOID EXCEPTIONS;\n";
		return errorElem; 
	}
	
	int k = 0;
	for(int j = 0; j <= pool.size(); ++j) //returns the i'th active T in the pool
	{
		if (pool[j].second == true)
		{
			if (k == i)
				return pool[j].first;

			++k;
		}
	}
	myAssert(false);
}


//=================================================

/*
template<typename T>
const T& ObjectPool<T>::operator[](int i) const
{
	myAssert(i >= 0 && i < pool.size() && i < size);

	int k = 0;
	for (int j = 0; j <= pool.size(); ++j) //returns the i'th active T in the pool
	{
		if (pool[j].second == true)
		{
			if (k == i)
				return pool[j].first;
			++k;
		}
	}
	myAssert(false);
}
*/


//=================================================

template<typename T>
void ObjectPool<T>::push_back(const T& t) noexcept
{
	//std::cout << "Pool Size: "<< size << '\n';
	if (size == pool.size()) //all Ts are actived
	{
		std::cout << "!WARNING: POOL OBJECTS BEING REALOCATED;\n";
		pool.resize(pool.size() + 100, std::pair<T, bool>(T(), false));
	}
	
	for (int i = 0; i < pool.size(); ++i) //try to find a inactive T in the pool
	{
		if(pool[i].second == false) 
		{
			pool[i].first = t;
			pool[i].second = true; //now it is active
			++size;
			return;
		}
	}

	myAssert(false);
}

template<typename T>
void ObjectPool<T>::erase(int i) noexcept //erase the i'th T in the pool
{
	if (!(i >= 0 && i < pool.size() && i < size))
	{
		std::cout << "!WARNING: ObjectPool::erase() WAS CALLED WITH INVALID ARGUMENTS."
			"FUNCTION HAS BEEN RETURNED TO AVOID EXCEPTIONS;\n";
		return;
	}

	int k = 0;
	for (int j = 0; j < pool.size(); ++j) //find the i'th active T in the pool
	{
		if (pool[j].second == true)
		{
			if (k == i) 
			{
				pool[j].second = false; //just disable the object
				--size; //decrease the number of activated Ts
				return;
			}

			++k;
		}
	}

	std::cout << "!WARNING: ObjectPool::erase() DID NOT FOUND ANY ELEMENT TO DELETE;\n";

}

//=================================================


template<typename T>
int ObjectPool<T>::getReseved() const noexcept
{
	return pool.size();
}


//=================================================


template<typename T>
int ObjectPool<T>::getSize() const noexcept
{
	return size;
}



//=================================================

template<typename T>
std::pair<T, bool>& ObjectPool<T>::front() noexcept
{
	if (pool.size() == 0)
	{
		std::cout << "WARNING: ObjectPool::front() called on a pool with 0 elements. errorElem returned "
			"to avoid exceptions;\n";
		return errorElem;
	}
	return pool[0];
}


//=================================================

template<typename T>
std::pair<T, bool>& ObjectPool<T>::back() noexcept
{
	if (pool.size() == 0)
	{
		std::cout << "WARNING: ObjectPool::front() called on a pool with 0 elements. errorElem returned "
			"to avoid exceptions;\n";
		return errorElem;
	}

	return pool[pool.size() - 1];
}


#endif // !OBJECT_POOL
