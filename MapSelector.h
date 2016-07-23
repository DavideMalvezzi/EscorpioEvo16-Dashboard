// MapSelector.h

#ifndef _MAPSELECTOR_h
#define _MAPSELECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <CanInterface.h>

#include "HWConfig.h"
#include "MainForm.h"

#define CURR_MAP_TTL	500
#define READ_TIMEOUT	75

#define INVALID_MAP		-1
#define DEFAULT_MAP		0
#define MAP1			((LOW << 1) | HIGH)
#define MAP2			((HIGH << 1) | HIGH)
#define MAP3			((HIGH << 1) | LOW)
#define MAP4			((LOW << 1) | LOW)

#define MAPSEL_TAG		F("MAPSEL")

class MapSelectorClass{

public:
	void init();
	void update();

	byte getCurrentMap(){ return currentMap; }

 private:
	 byte currentMap;
	 byte sel1CurrState, sel1PrevState;
	 byte sel2CurrState, sel2PrevState;
	 Timer sel1Timer, sel2Timer, sendTimer;

	 void checkBounce(byte pin, byte& currState, byte& prevState, Timer& t);
	 void updateMap();
};

extern MapSelectorClass mapSelector;

#endif

