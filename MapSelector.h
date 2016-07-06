// MapSelector.h

#ifndef _MAPSELECTOR_h
#define _MAPSELECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <CanInterface.h>

#define SEL1_PIN	40
#define SEL2_PIN	39

#define CURR_MAP_TTL	500

#define MAP1	((LOW << 1) | HIGH)
#define MAP2	((HIGH << 1) | HIGH)
#define MAP3	((HIGH << 1) | LOW)
#define MAP4	((LOW << 1) | LOW)

#define MAPSEL_TAG F("MAPSEL")

class MapSelectorClass{

public:
	void init();
	void update();

 private:
	 byte currentMap;
	 Timer sendTimer;


};

extern MapSelectorClass MapSelector;

#endif

