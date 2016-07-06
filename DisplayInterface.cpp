
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
	INIT_SERIAL(LCD_SERIAL, LCD_SERIAL_BAUD);
	genie.Begin(LCD_SERIAL);

	//Reset the LCD
	pinMode(LCD_RESET_PIN, OUTPUT); 
	digitalWrite(LCD_RESET_PIN, LOW);
	delay(100);
	digitalWrite(LCD_RESET_PIN, HIGH);
	delay(3500);

	//Attach event handler method
	genie.AttachEventHandler(&onEvent);

	//Init forms
	consoleForm.init(genie);
	mainForm.init(genie);
	debugForm.init(genie);
	mapsForm.init(genie);
	currentForm = &consoleForm;

	//Refresh rateo for the update method
	refreshTimer.setDuration(1000 / REFRESH_RATEO).start();

	enabled = true;
}


void DisplayInterfaceClass::update(){
	//Update events
	genie.DoEvents();

	//If it's refresh time then update current form
	if (enabled && refreshTimer.hasFinished()){
		if (currentForm != NULL){
			currentForm->update(genie);
		}

		//If the refresh time is high disable the future updates. Probably the screen froze or shut down
		if (refreshTimer.elapsedTime() > MAX_UPDATE_TIME){
			enabled = false;
			Log.w(LCD_TAG) << F("Disabled LCD updates") << Endl;
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
		//Restart refresh timer
		refreshTimer.start();
	}
}


DisplayInterfaceClass displayInterface;

