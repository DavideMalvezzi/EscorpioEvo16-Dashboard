
#include "DisplayInterface.h"


void onEvent(){
	genieFrame evt;
	//Get the next pending event
	displayInterface.genie.DequeueEvent(&evt);
	//If current form is valid
	if (displayInterface.currentForm != -1 && displayInterface.forms[displayInterface.currentForm] != NULL){
		//Redirect the screen event on the current form invoking onEvent method
		displayInterface.forms[displayInterface.currentForm]->onEvent(displayInterface.genie, evt);
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

	genie.WriteContrast(15);
	//Attach event handler method
	genie.AttachEventHandler(&onEvent);

	//Init forms
	currentForm = -1;
	consoleForm.init(genie);
	mainForm.init(genie);
	debugForm.init(genie);
	mapsForm.init(genie);

	//Add forms to formList
	forms.resize(MAX_FORM_NUMBER);
	forms.append(&mainForm);
	forms.append(&debugForm);
	forms.append(&mapsForm);

	//Refresh rateo for the update method
	refreshTimer.setDuration(1000 / REFRESH_RATEO).start();
}


void DisplayInterfaceClass::update(){
	//Update events
	genie.DoEvents();
	//If it's refresh time then update current form
	if (refreshTimer.hasFinished()){
		if (currentForm != -1 && forms[currentForm] != NULL){
			forms[currentForm]->update(genie);
			refreshTimer.start();
		}
	}
}

void DisplayInterfaceClass::nextForm(){
	//If current form is not null invoke onExit method
	if (currentForm != -1){
		forms[currentForm]->onExit(genie);
	}

	//Activate next form
	currentForm = (currentForm + 1) % MAX_FORM_NUMBER;
	genie.WriteObject(GENIE_OBJ_FORM, forms[currentForm]->getFormIndex(), 1);

	//Invoke new current form onEnter method
	forms[currentForm]->onEnter(genie);
}


DisplayInterfaceClass displayInterface;

