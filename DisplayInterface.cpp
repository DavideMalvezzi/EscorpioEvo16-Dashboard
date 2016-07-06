
#include "DisplayInterface.h"
#include "ConsoleForm.h"
#include "MainForm.h"
#include "DebugForm.h"
#include "MapsForm.h"
#include <Button.h>

void onEvent(){
	genieFrame evt;
	displayInterface.genie.DequeueEvent(&evt);
	if (displayInterface.currentForm != -1 && displayInterface.forms[displayInterface.currentForm] != NULL){
		displayInterface.forms[displayInterface.currentForm]->onEvent(displayInterface.genie, evt);
	}

}

void DisplayInterfaceClass::init(){
	INIT_SERIAL(LCDSerial, LCD_SERIAL_BAUD);
	genie.Begin(LCDSerial);

	pinMode(RESET_PIN, OUTPUT);  // Set D4 on Arduino to Output (4D Arduino Adaptor V2 - Display Reset)
	digitalWrite(RESET_PIN, LOW);  // Reset the Display via D4
	delay(100);
	digitalWrite(RESET_PIN, HIGH);  // unReset the Display via D4
	delay(3500);

	genie.WriteContrast(15);
	genie.AttachEventHandler(&onEvent);

	currentForm = -1;
	consoleForm.init(genie);
	mainForm.init(genie);
	debugForm.init(genie);
	mapsForm.init(genie);

	forms.resize(MAX_FORM_NUMBER);
	forms.append(&mainForm);
	forms.append(&debugForm);
	forms.append(&mapsForm);

	refreshTimer.setDuration(1000 / REFRESH_RATEO).start();

	LOGLN("DISPLAY_INIT");
	consoleForm.println("DISPLAY_INIT");
}


void DisplayInterfaceClass::update(){
	genie.DoEvents();
	if (refreshTimer.hasFinished()){
		if (currentForm != -1 && forms[currentForm] != NULL){
			forms[currentForm]->update(genie);
			refreshTimer.start();
		}
	}
}

void DisplayInterfaceClass::nextForm(){
	if (currentForm != -1){
		forms[currentForm]->onExit(genie);
	}

	currentForm = (currentForm + 1) % MAX_FORM_NUMBER;
	genie.WriteObject(GENIE_OBJ_FORM, forms[currentForm]->getFormIndex(), 1);

	forms[currentForm]->onEnter(genie);
}


DisplayInterfaceClass displayInterface;

