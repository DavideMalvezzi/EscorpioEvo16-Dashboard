#include "DataLogger.h"
#include "CANInterface.h"
#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "ConsoleForm.h"


void DataLoggerClass::init(){
	Configuration cfg;

	if (cfg.initWithFile(DL_CFG_FILE)){
		cfg.debug();
		createNewLogFile(cfg);

		logTimer.setDuration(1000.0f / cfg[SAMPLE_PER_SECOND].toInt()).start();

		LOGLN(F("DATALOGGER_INIT"));
		consoleForm.println(F("DATALOGGER_INIT"));

	}
	else{
		consoleForm.println(F("DataLogger configuration file not found!"));
		ASSERT(false, F("DataLogger configuration file not found!"));
	}

	
}

void DataLoggerClass::update(){
	if (logTimer.hasFinished()){

		File logFile = SD.open(logFileName, O_WRITE);
		for (int i = 0; i < channelsConfig.getChannelCount(); i++){
			logFile.print(channelsBuffer.getValueAsString(channelsConfig.getChannelByIndex(i)->ID));
			logFile.print(",");
		}
		logFile.println();
		logFile.close();


		logTimer.start();

		LOGLN(F("DATALOGGER_LOGGED"));
	}
}


void DataLoggerClass::createNewLogFile(Configuration& cfg){
	logFileName = LOG_FILE_FOLDER;
	logFileName += cfg[LOG_FILE_NAME] + cfg[LOG_FILE_NUM] + cfg[LOG_FILE_FORMAT];
	cfg.setValue(LOG_FILE_NUM, cfg[LOG_FILE_NUM].toInt() + 1);
	cfg.updateCfgFile();

	Channel* c;
	File logFile = SD.open(logFileName, O_WRITE | O_CREAT);
	for (int i = 0; i < channelsConfig.getChannelCount(); i++){
		c = channelsConfig.getChannelByIndex(i);
		logFile.print(c->name);
		logFile.print(" (0x");
		logFile.print(c->ID, HEX);
		logFile.print("),");
	}
	logFile.println();
	logFile.close();
}


DataLoggerClass dataLogger;

