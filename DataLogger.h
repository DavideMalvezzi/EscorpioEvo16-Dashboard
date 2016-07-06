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

#define LOG_FILE_FOLDER		"DataLog/"
#define DL_CFG_FILE			"DataLog.cfg"
#define SAMPLE_PER_SECOND	0
#define LOG_FILE_NAME		1
#define LOG_FILE_FORMAT		2
#define LOG_FILE_NUM		3


class DataLoggerClass {

public:
	void init();
	void update();

private:
	String logFileName;
	Timer logTimer;

	void createNewLogFile(Configuration& cfg);
};

extern DataLoggerClass dataLogger;
#endif

