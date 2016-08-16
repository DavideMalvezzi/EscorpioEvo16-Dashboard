// TelemetryInterface.h

#ifndef _TELEMETRYINTERFACE_h
#define _TELEMETRYINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "Logger.h"
#include "Timer.h"

/**
* Class representing the telemetry interface with the android app
*/

//Logger tag
#define TLM_TAG				F("TLM")

//Telemetry packets header
#define TLM_CMD				F("TLM")

//Telemetry log time period
#define TLM_LOG_PERIOD		500

class TelemetryInterfaceClass {

 public:
	//Function to call in the setup
	void init(UARTClass* logSerial = NULL);

	//Function to call in the loop
	void update();

	//Set the serial port
	void setLogSerial(UARTClass* logSerial);


 private:
	 //Log timer
	 Timer logTimer;

	 //Serial port pointer
	 UARTClass* logSerial;
};

//Telemetry instance
extern TelemetryInterfaceClass telemetryInterface;

#endif

