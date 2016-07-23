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

#define TLM_TAG				F("TLM")
#define TLM_CMD				F("TLM")
#define TLM_LOG_PERIOD		500

class TelemetryInterfaceClass {

 public:
	 void init(UARTClass* logSerial = NULL);
	void setLogSerial(UARTClass* logSerial);
	void update();

 private:
	 Timer logTimer;
	 UARTClass* logSerial;
};

extern TelemetryInterfaceClass telemetryInterface;

#endif

