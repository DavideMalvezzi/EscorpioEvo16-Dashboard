
#include "MapSelector.h"

void MapSelectorClass::init(){
	pinMode(SEL1_PIN, INPUT);
	pinMode(SEL2_PIN, INPUT);

	currentMap = INVALID_MAP;

	sel1CurrState = LOW;
	sel1PrevState = LOW;
	sel2CurrState = LOW;
	sel2PrevState = LOW;

	sendTimer.setDuration(CURR_MAP_TTL).start();
	sel1Timer.setDuration(READ_TIMEOUT);
	sel2Timer.setDuration(READ_TIMEOUT);

}

void MapSelectorClass::update(){
	checkBounce(SEL1_PIN, sel1CurrState, sel1PrevState, sel1Timer);
	checkBounce(SEL2_PIN, sel2CurrState, sel2PrevState, sel2Timer);

	updateMap();

	if (sendTimer.hasFinished()){
		canInterface.send(CanID::DRIVER_SET_MAP_CMD, &currentMap, sizeof(byte));
		Log.i(MAPSEL_TAG) << F("Send map: ") << (int)currentMap << Endl;
		sendTimer.start();
	}
}

void MapSelectorClass::updateMap(){
	byte newMap;
	byte selMap = (sel1CurrState << 1) | sel2CurrState;

	switch (selMap){
		case MAP1:
			newMap = 1;
			break;

		case MAP2:
			newMap = 2;
			break;

		case MAP3:
			newMap = 3;
			break;

		case MAP4:
			newMap = 4;
			break;

		default:
			newMap = INVALID_MAP;
			Log.e(MAPSEL_TAG) << F("Invalid map: ") << Bin << selMap << Endl;
			break;
	}

	if (newMap != currentMap && newMap != INVALID_MAP){
		currentMap = newMap;
		canInterface.send(CanID::DRIVER_SET_MAP_CMD, &currentMap, sizeof(byte));
		Log.i(MAPSEL_TAG) << F("New map: ") << (int)currentMap << Endl;
	}

}

void MapSelectorClass::checkBounce(byte pin, byte& currState, byte& prevState, Timer& t){
	byte reading = digitalRead(pin);

	if (reading != prevState){
		prevState = reading;
		t.start();
	}

	if (t.hasFinished()){
		if (prevState != currState){
			currState = prevState;
		}
	}

}



MapSelectorClass mapSelector;

