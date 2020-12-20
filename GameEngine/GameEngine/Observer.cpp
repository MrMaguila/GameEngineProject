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

#include "Observer.h"


//Message definitions:


Message::Message(MessageType t, uint16_t d)
	:type{ t }, data{ d } {}


Message::Message()
	:type{ MessageType::DEFAULT_MESSAGE }, data{ 0 } {}



//Subject class definitions:


int Subject::getNumOfObservers() const noexcept
{
	return numOfObservers;
}

void Subject::addObserver(Observer& ob) //add a observer to the list
{
	if (numOfObservers >= MAX_OBSERVERS) myAssert(false);
	observers[numOfObservers] = &ob;
	numOfObservers += 1;
}

/*
void Subject::storeMessage(Message msg)
{
	if (numOfMessages >= MAX_MESSAGES) { myAssert(false);  return; }
	messages[numOfMessages] = msg;
	numOfMessages += 1;
}
*/

void Subject::storeMessage(Message msg)
{
	//if (numOfMessages >= MAX_MESSAGES) { myAssert(false);  return; }
	if (numOfMessages >= MAX_MESSAGES) myAssert(false); //don't store if the message storage is full
	messages[numOfMessages] = msg;
	numOfMessages += 1;
}


void Subject::notify() //send all messages to all observers
{
	for (int i = 0; i < numOfMessages; ++i)
		for (int j = 0; j < numOfObservers; ++j)
			observers[j]->onNotify(messages[i]);
	numOfMessages = 0;
	
}
