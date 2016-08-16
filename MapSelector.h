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

/**
* Class containing the map selector external interface
*/

//Map TTL
#define CURR_MAP_TTL	500

//Debounce time
#define READ_TIMEOUT	75

//Invalid map
#define INVALID_MAP		-1

//Default map is the value received via can bus from the driver when the default map is loaded
#define DEFAULT_MAP		0

//Map selector combinations
#define MAP1			((LOW << 1) | HIGH)
#define MAP2			((HIGH << 1) | HIGH)
#define MAP3			((HIGH << 1) | LOW)
#define MAP4			((LOW << 1) | LOW)

//Logger tag
#define MAPSEL_TAG		F("MAPSEL")

class MapSelectorClass{

public:
	//Function to call in the setup
	void init();

	//Function to call in the loop
	void update();

	//Get the current selected map
	byte getCurrentMap(){ return currentMap; }

 private:
	 //Current map
	 byte currentMap;

	 //Variables to handle the map changes.
	 //Look for the button debounces example for more
	 byte sel1CurrState, sel1PrevState;
	 byte sel2CurrState, sel2PrevState;
	 Timer sel1Timer, sel2Timer;
	 
	 //Send map through can timer
	 Timer sendTimer;

	 //Check the map selector changes
	 void checkBounce(byte pin, byte& currState, byte& prevState, Timer& t);

	 //Update the current map
	 void updateMap();
};

//Map selector instance
extern MapSelectorClass mapSelector;

#endif

