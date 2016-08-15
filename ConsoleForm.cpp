
#include "ConsoleForm.h"


void ConsoleFormClass::init(Genie &genie){
	this->genie = &genie;
	text.reserve(TEXT_BUFFER_SIZE);
	clear();
}

void ConsoleFormClass::update(Genie &genie){
	//Nothing to do here
}

void ConsoleFormClass::print(String str){
	text.concat(str);
}

void ConsoleFormClass::println(String str){
	addText(str + "\n");
}

void ConsoleFormClass::clear(){
	text = "";	
}

void ConsoleFormClass::addText(String str){
	int index = text.indexOf("\n");
	//Remove first lines until there is enough space for the new text
	//Create a sort of scroll-up effect, like real shell
	if (str.length() < TEXT_BUFFER_SIZE){
		while (text.length() + str.length() >= TEXT_BUFFER_SIZE){
			text.remove(0, index == -1 ? text.length() : index);
			index = text.indexOf("\n");
		}
		text.concat(str);
	}
	
	genie->WriteStr(CONSOLE_STR, (char*)text.c_str());
}


ConsoleFormClass consoleForm;