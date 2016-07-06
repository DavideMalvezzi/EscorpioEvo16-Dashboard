
#include "DisplayInterface.h"


void onEvent(){
	genieFrame evt;
	//Get the next pending event
	displayInterface.genie.DequeueEvent(&evt);

	//If current form is valid
	if (displayInterface.currentForm != NULL){
		//Redirect the screen event on the current form invoking onEvent method
		displayInterface.currentForm->onEvent(displayInterface.genie, evt);
	}
}

void DisplayInterfaceClass::init(){
	INIT_SERIAL(LCDSerial, LCD_SERIAL_BAUD);
	genie.Begin(LCDSerial);

	//Reset the LCD
	pinMode(RESET_PIN, OUTPUT); 
	digitalWrite(RESET_PIN, LOW);  
	delay(100);
	digitalWrite(RESET_PIN, HIGH); 
	delay(3500);

	//Attach event handler method
	genie.AttachEventHandler(&onEvent);

	//Init forms
	currentForm = &consoleForm;
	consoleForm.init(genie);
	mainForm.init(genie);
	debugForm.init(genie);
	mapsForm.init(genie);

	//Refresh rateo for the update method
	refreshTimer.setDuration(1000 / REFRESH_RATEO).start();
}


void DisplayInterfaceClass::update(){
	//Update events
	genie.DoEvents();
	//If it's refresh time then update current form
	if (refreshTimer.hasFinished()){
		if (currentForm != NULL){
			currentForm->update(genie);
		}
		refreshTimer.start();
	}
}

void DisplayInterfaceClass::setCurrentForm(LCDForm* currentForm){
	//If current form is not null invoke onExit method
	if (currentForm != NULL){
		this->currentForm->onExit(genie);
	}

	this->currentForm = currentForm;

	if (currentForm != NULL){
		//Activate next form
		genie.WriteObject(GENIE_OBJ_FORM, currentForm->getFormIndex(), 1);
		//Invoke new current form onEnter method
		currentForm->onEnter(genie);
	}
}


DisplayInterfaceClass displayInterface;

