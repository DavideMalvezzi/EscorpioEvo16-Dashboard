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

#define LOG_FILE_FOLDER		F("DataLog/")
#define DL_CFG_FILE			"DATALOG.CFG"

#define DEFAULT_LOG_NAME		F("LOG999.CSV")
#define DEFAULT_SAMPLE_PER_S	2

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

	void updateCfg(Configuration& cfg);
	void createNewLogFile();
};

extern DataLoggerClass dataLogger;
#endif

