
#include "MapSelector.h"

void MapSelectorClass::init(){
	pinMode(SEL1_PIN, INPUT);
	pinMode(SEL2_PIN, INPUT);

	sendTimer.setDuration(CURR_MAP_TTL).start();
}

void MapSelectorClass::update(){
	byte newMap;
	byte selMap = (digitalRead(SEL1_PIN) << 1) | digitalRead(SEL2_PIN);
		
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
			newMap = 0;
			Log.e(MAPSEL_TAG) << F("Invalid map: ") << Bin << selMap << Endl;
			break;
	}

	if (newMap != currentMap && newMap != 0){
		currentMap = newMap;
		canInterface.send(CanID::DRIVER_SET_MAP_CMD, &currentMap, sizeof(byte));
		Log.i(MAPSEL_TAG) << F("New map: ") << (int)currentMap << Endl;
	}
	else if (sendTimer.hasFinished()){
		canInterface.send(CanID::DRIVER_SET_MAP_CMD, &currentMap, sizeof(byte));
		Log.i(MAPSEL_TAG) << F("Send map: ") << (int)currentMap << Endl;
		sendTimer.start();
	}

}


MapSelectorClass MapSelector;

