
#include "Channel.h"

Channel::Channel(){
	ttlTimer.setDuration(CH_BUFFER_TTL).start();
}

void Channel::setSize(byte size){
	this->size = size;
}

void Channel::setID(unsigned short ID){
	this->ID = ID;
}

void Channel::setDataType(DataTypes type){
	this->type = type;
}

void Channel::setName(String name){
	this->name = name;
}

byte Channel::getSize(){
	return this->size;
}

unsigned short Channel::getID(){
	return this->ID;
}

Channel::DataTypes Channel::getDataType(){
	return this->type;
}

String Channel::getName(){
	return this->name;
}

void Channel::resetTTLTimer(){
	ttlTimer.start();
}

bool Channel::hasTTLFinished(){
	return ttlTimer.hasFinished();
}