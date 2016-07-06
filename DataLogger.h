// DataLogger.h

#ifndef _DATALOGGER_h
#define _DATALOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Configuration.h>
#include <Timer.h>

#include "CANInterface.h"
#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "ConsoleForm.h"

#define LOG_FILE_FOLDER		"DataLog/"
#define DL_CFG_FILE			"DataLog.cfg"

class DataLoggerClass {

public:
	void init();
	void update();

private:
	enum Attr : byte {
		SAMPLE_PER_SECOND,
		LOG_FILE_NAME,
		LOG_FILE_FORMAT,		
		LOG_FILE_NUM	
	};

	String logFileName;
	Timer logTimer;

	void createNewLogFile(Configuration& cfg);
};

extern DataLoggerClass dataLogger;
#endif

