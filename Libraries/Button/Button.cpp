#include "Button.h"

Vector<Button*>* Button::activeButtons = new Vector<Button*>;
unsigned long int Button::pressTime = 0;

void Button::setMaxNumber(int max){
	activeButtons->resize(max);
}

void Button::add(byte pin, void(*pressFunction)(void*), void(*releaseFunction)(void*), void* data){
	activeButtons->append(new Button(pin, pressFunction, releaseFunction, data));
}

void Button::update(){
	int reading;
	Button* b;
	for (int i = 0; i < activeButtons->getSize(); i++){
		b = activeButtons->at(i);
		reading = digitalRead(b->buttonPin);

		if (reading != b->newState) {
			b->newState = reading;
			b->pushTimer.start();
		}

		if (b->pushTimer.hasFinished()) {
			if (b->newState != b->buttonState) {
				b->buttonState = b->newState;
				if (b->buttonState == HIGH){
					//b->pressTimer.start();
					if (b->pressFunction != NULL){
						b->pressFunction(b->data);
					}
				}
				else{
					//pressTime = b->pressTimer.elapsedTime();
					if (b->releaseFunction != NULL){
						b->releaseFunction(b->data);
					}
				}
			}
		}
	}
}

unsigned long int Button::getPressTime(){
	return pressTime;
}

Button::Button(byte pin, void(*pressFunction)(void*), void(*releaseFunction)(void*), void* data){
		this->buttonPin = pin;
		this->data = data;
		this->pressFunction = pressFunction;
		this->releaseFunction = releaseFunction;
		this->newState = LOW;
		this->buttonState = LOW;
		this->pushTimer.setDuration(BOUNCE_TIME);

		pinMode(buttonPin, INPUT);		
	}