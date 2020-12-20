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



This header implents the classes needed to use the Observer pattern,
used to allow communication between the game's systems
*/
//#################################################################################

#ifndef OBSERVER_HEADER
#define OBSERVER_HEADER

#include <cstdint>
#include <stdexcept>
#include <cassert>
#include <iostream>

#include "GlobalDefines.h"

enum class MessageType
{
	//input messages:
	DEFAULT_MESSAGE,
	KEY_PRESSED,
	KEY_RELEASED,
	KEY_HOLD_ON,

	UP_KEY_PRESSED,
	UP_KEY_RELEASED,
	DOWN_KEY_PRESSED,
	DOWN_KEY_RELEASED,
	LEFT_KEY_PRESSED,
	LEFT_KEY_RELEASED,
	RIGHT_KEY_PRESSED,
	RIGHT_KEY_RELEASED,

	//physics related messages:
	APPLY_FORCE,
	APPLY_VERTICAL_FORCE,
	CHANGE_SPEED,
	COLLISION_OCCURRED,

	//animation related messages:
	PLAY,
	STOP,

	//mouse input:
	MOUSE_BUTTOM_PRESSED,
	MOUSE_BUTTOM_RELEASED,
	MOUSE_POSITION_NOTIFICATION
};


struct Message
{
	Message(MessageType, uint16_t);
	Message();

	MessageType type = MessageType::DEFAULT_MESSAGE;
	uint16_t data = 0;
	int idata[3] = { 0, 0, 0 };
	FLOAT_TYPE fdata[3] = {0.0f, 0.0f, 0.0f};
};


//#################################################################################
//The Observer and Subject classes:


#define MAX_MESSAGES 60
#define MAX_OBSERVERS 6

class Observer
{
public:

	virtual void onNotify(Message) = 0; //proccess a received message

protected:

	~Observer(); //destructor
};

inline Observer::~Observer() {};


class Subject
{
public:

	//Functions:
	int getNumOfObservers() const noexcept;
	void addObserver(Observer&); //add a observer to the list
	void notify(); //send all messages to all observers. This function is meant to be called only in the frame end.
			//Messages need to be stored (using the storeMessage method) before each frame's end to be all sent by
			//the notify method

protected:

	Subject(); //make the class abstract
	~Subject(); //don't allow deletion through a ptr to Subject 

	//protected Functions:
	void storeMessage(Message);

	//Data:
	Observer* observers[MAX_OBSERVERS];
	int numOfObservers = 0;

	Message messages[MAX_MESSAGES];
	int numOfMessages;
};

inline Subject::Subject() {};
inline Subject::~Subject() {};


#endif // !OBSERVER_HEADER
