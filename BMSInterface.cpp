
#include "BMSInterface.h"

void BMSInterface::init(){
	pinMode(BMS_BUTTON_LED, OUTPUT);
	digitalWrite(BMS_BUTTON_LED, LOW);

	state = Unknown;
	ledTimer.setDuration(BMS_LED_DUR).start();
	lastStateUpdate.setDuration(BMS_STATE_TTL).start();
}

void BMSInterface::update(){
	if (lastStateUpdate.hasFinished()){
		Log.w(BMS_TAG) << F("State TTL finished") << Endl;

		state = Unknown;
		lastStateUpdate.start();
	}

	if (ledTimer.hasFinished()){
		Log.i(BMS_TAG) << F("State is ") << state << Endl;

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
				Log.w(BMS_TAG) << F("Unknown state") << Endl;
				break;
		}

		ledTimer.start();
	}
}

void BMSInterface::onStateChanged(const char* newStateString){
	BMSState newState;

	if (strcmp(newStateString, ENABLED_STATE) == 0){
		newState = Enabled;
	}
	else if (strcmp(newStateString, RUN_STATE) == 0){
		newState = Run;
	}
	else if (strcmp(newStateString, ERROR_STATE) == 0){
		newState = Error;
	}
	else if (strcmp(newStateString, DEBUG_STATE) == 0){
		newState = Debug;
	}
	else{
		newState = Unknown;
	}

	if (newState != state){
		ledStatus = 0;
		state = newState;
	}

	lastStateUpdate.start();
}

void BMSInterface::start(){
	byte cmd = BMS_START_CMD;
	canInterface.send(CanID::BMS_CMD, &cmd, sizeof(byte));
	Log.i(BMS_TAG) << F("Start") << Endl;
}

void BMSInterface::setDebugMode(){
	byte cmd = BMS_DEBUG_CMD;
	canInterface.send(CanID::BMS_CMD, &cmd, sizeof(byte));
	Log.i(BMS_TAG) << F("Set debug mode") << Endl;
}


BMSInterface BMS;
