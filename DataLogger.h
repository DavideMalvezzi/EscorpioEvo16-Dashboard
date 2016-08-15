// DataLogger.h

#ifndef _DATALOGGER_h
#define _DATALOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/**
* Class containing all the function to manage the log files on the sd card
*/

#include <Configuration.h>
#include <Timer.h>

#include "CANInterface.h"
#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "ConsoleForm.h"
#include "Logger.h"

//Logger tag
#define DL_TAG	F("DL")

//Log files folder
#define LOG_FILE_FOLDER		F("DataLog/")

//Datalogger cfg file
#define DL_CFG_FILE			"DATALOG.CFG"

//Default log file name if the cfg parsing fails
#define DEFAULT_LOG_NAME		F("LOG999.CSV")

//Default sampling rateo if the cfg parsing fails
#define DEFAULT_SAMPLE_PER_S	2

class DataLoggerClass {

public:
	//Init function to call in the setup
	void init();

	//Update function to call in the loop
	void update();

private:
	//Enum rapresenting the configuration inside the datalogger cfg file
	enum Attr : byte {
		SAMPLE_PER_SECOND,
		LOG_FILE_NAME,
		LOG_FILE_FORMAT,		
		LOG_FILE_NUM	
	};

	//Current log file name
	String logFileName;

	//When the timer has finished it time to log
	Timer logTimer;

	//Function to update the cfg file with the next log file to create
	void updateCfg(Configuration& cfg);

	//Function to create the new log file to use
	void createNewLogFile();
};

//Datalogger instance
extern DataLoggerClass dataLogger;
#endif

