
#include "TelemetryInterface.h"

void TelemetryInterfaceClass::init(UARTClass* logSerial){
	setLogSerial(logSerial);
	logTimer.setDuration(TLM_LOG_PERIOD).start();
}

void TelemetryInterfaceClass::setLogSerial(UARTClass* logSerial){
	this->logSerial = logSerial;
}

void TelemetryInterfaceClass::update(){
	if (channelsConfig.isValid() && logTimer.hasFinished()){
		if (logSerial != NULL){
			logSerial->print("\n");	
			logSerial->print(TLM_TAG);
			logSerial->print(channelsBuffer.getBufferSize() + channelsConfig.getChannelCount() * sizeof(unsigned short));
			logSerial->print("\n");
			channelsBuffer.sendOnStream(logSerial);

			Log.i(TLM_TAG) << F("Logged") << Endl;
		}
		logTimer.start();
	}
}


TelemetryInterfaceClass telemetryInterface;

