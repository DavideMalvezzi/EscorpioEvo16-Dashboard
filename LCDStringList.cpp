
#include "LCDStringList.h"

void LCDStringList::init(byte widgetIndex, int maxSize, Genie* parent){
	this->widgetIndex = widgetIndex;	//LCD widget index
	this->currentElement = -1;	//No elements
	this->elementCount = 0;		//No elements
	this->buffer.reserve(maxSize);	//Reserve buffer
	this->parent = parent;
}

void LCDStringList::addElement(String elem){
	//If no elements set the first selected
	if (elementCount == 0){
		currentElement = 0;
		buffer.concat(SELECT_LINE_CHAR);
	}
	else{
		buffer.concat(' ');
	}

	//Add element
	buffer.concat(elem);
	buffer.concat('\n');
	elementCount++;
}

void LCDStringList::clear(){
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

	//Reset
	currentElement = -1;
	elementCount = 0;
}

void LCDStringList::down(){
	int currentLine;
	//If there are almost two elements and if the last element is not the current one
	if (elementCount > 1 && currentElement < elementCount - 1){

		//Search for the current line and remove '>'
		currentLine = buffer.indexOf(SELECT_LINE_CHAR);
		buffer.setCharAt(currentLine, ' ');

		//Skip to next line
		currentLine = buffer.indexOf('\n', currentLine);
		//Go down
		buffer.setCharAt(currentLine + 1, SELECT_LINE_CHAR);
		//Update
		currentElement++;
		repaint();
		
	}

}

void LCDStringList::removeElement(int index){
	//TODO: Implement if necessary
}

void LCDStringList::repaint(){
	parent->WriteStr((int)widgetIndex, (char*)buffer.c_str());
}

void LCDStringList::up(){
	int current, index = 0;
	//If there are almost two elements and if the first element is not the current one
	if (elementCount > 1 && currentElement != 0){
		current = buffer.indexOf(SELECT_LINE_CHAR);
		buffer.setCharAt(current, ' ');

		for (int i = 0; i < currentElement - 1; i++){
			index = buffer.indexOf('\n', index + 1);
		}

		if (index > 0){
			buffer.setCharAt(index + 1, SELECT_LINE_CHAR);
			currentElement--;
		}
		else{
			currentElement = 0;
			buffer.setCharAt(0, SELECT_LINE_CHAR);
		}

		//Update
		repaint();	
	}
}

int LCDStringList::getCurrentElement(){
	return currentElement;
}

int LCDStringList::getWidgetIndex(){
	return widgetIndex;
}

int LCDStringList::getElementCount(){
	return elementCount;
}


