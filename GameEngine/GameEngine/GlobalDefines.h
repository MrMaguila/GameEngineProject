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

#ifndef GLOBAL_DEFINES
#define GLOBAL_DEFINES


#include <cassert>
#include <iostream>

/*
	FLOAT_TYPE - defines the floating point type to be used
				according to the system arquiteture. float for 32bit
				and double for 64bit
*/

#ifndef FLOAT_TYPE

	#if _WIN64
		#define FLOAT_TYPE double
	#else 
		#define FLOAT_TYPE float
	#endif

#endif //!FLOAT_TYPE


inline void myAssert(bool a)
{
#ifdef NDEBUG
	if (!a)
		while (true)
		{
			char ch;
			std::cin >> ch;
		}


#else 
	assert(a);
#endif //!NDEBUG

}





#endif //GLOBAL_DEFINES