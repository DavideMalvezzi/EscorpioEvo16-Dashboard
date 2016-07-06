
#include "BMSInterface.h"

void BMSInterface::init(){
	pinMode(BMS_BUTTON_LED, OUTPUT);
	digitalWrite(BMS_BUTTON_LED, LOW);

	state = Unknown;
	ledTimer.setDuration(BMS_LED_DUR).start();
}

void BMSInterface::update(){

	if (ledTimer.hasFinished()){

		switch (state){

			case Enabled:
				ledStatus = (ledStatus + 1) % 10;
				digitalWrite(BMS_BUTTON_LED, ledStatus < 5 ? HIGH : LOW);
				
				break;

			case Run:
				digitalWrite(BMS_BUTTON_LED, HIGH);
				break;

			case Debug:
				ledStatus = (ledStatus + 1) % 10;
				digitalWrite(BMS_BUTTON_LED, ledStatus < 1 ? HIGH : LOW);
				break;

			case Error:
				ledStatus = !ledStatus;
				digitalWrite(BMS_BUTTON_LED, ledStatus);
				break;

			default:
				digitalWrite(BMS_BUTTON_LED, LOW);
				break;
		}

		ledTimer.start();
	}
}

void BMSInterface::onStateChanged(const char* newState){
	if (strcmp(newState, ENABLED_STATE) == 0){
		state = Enabled;
		ledStatus = 0;
	}
	else if (strcmp(newState, RUN_STATE) == 0){
		state = Run;
	}
	else if (strcmp(newState, ERROR_STATE) == 0){
		state = Error;
		ledStatus = 0;
	}
	else if (strcmp(newState, DEBUG_STATE) == 0){
		state = Debug;
		ledStatus = 0;
	}
	else{
		state = Unknown;
	}

}

void BMSInterface::start(){
	byte cmd = BMS_START_CMD;
	canInterface.send(CanID::BMS_CMD, &cmd, sizeof(byte));
	Log.i(BMS_TAG) << F("start") << Endl;
}

void BMSInterface::setDebugMode(){
	byte cmd = BMS_DEBUG_CMD;
	canInterface.send(CanID::BMS_CMD, &cmd, sizeof(byte));
	Log.i(BMS_TAG) << F("set debug mode") << Endl;
}


BMSInterface BMS;
