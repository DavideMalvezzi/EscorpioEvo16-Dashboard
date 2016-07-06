
#include "LCDStringMsg.h"

void LCDStringMsg::init(byte widgetIndex, Genie* parent){
	this->widgetIndex = widgetIndex;
	this->parent = parent;
}

void LCDStringMsg::clear(){
	//Replace all char except \n with with space
	for (int i = 0; i < buffer.length(); i++){
		if (buffer.charAt(i) != '\n'){
			buffer.setCharAt(i, ' ');
		}
	}

	//Update screen
	repaint();

	//Clear the buffer
	buffer.remove(0, buffer.length());
}

const String& LCDStringMsg::getMessage(){
	return buffer;
}

void LCDStringMsg::repaint(){
	parent->WriteStr(widgetIndex, (char*)buffer.c_str());
}

void LCDStringMsg::setMessage(String str){
	clear();
	buffer += str;
	repaint();
}

void LCDStringMsg::setMessage(const char* str){
	clear();
	buffer += str;
	repaint();
}