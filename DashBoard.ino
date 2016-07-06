/*
 Name:		DashBoard.ino
 Created:	8/25/2015 7:25:30 PM
 Author:	Davide Malvezzi
*/

#include "ConsoleForm.h"
#include "DebugForm.h"
#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "DataLogger.h"
#include "DisplayInterface.h"
#include "Interprete.h"
#include "MainForm.h"
#include "MapsForm.h"
#include "PhoneInterface.h"
#include "StrategySettings.h"
#include "variant.h"
#include "WheelSensor.h"

#include <ArduinoJson.h>
#include <BitArray.h>
#include <Button.h>
#include <ByteBuffer.h>
#include <Configuration.h>
#include <due_can.h>
#include <DueTimer.h>
#include <genieArduino.h>
#include <Map.h>
#include <SPI.h>
#include <SD.h> 
#include <Timer.h>
#include <Utils.h>
#include <Vector.h>

#define SERIAL_BAUD		9600
#define SD_SS_PIN		28
#define BUTTON_NUM		3

#define RED_LED		34
#define YELLOW_LED  32

#define STRATEGY_STEP_PERIOD	500
Timer strategyTimer;

//Test stuff
int loops;
unsigned long avgExecutionTime;
Timer t, sec;


void setup() {
	INIT_SERIAL(Serial, SERIAL_BAUD);
	INIT_SD(SD, SD_SS_PIN);

	pinMode(RED_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	//Display
	displayInterface.init();

	//Can
	canInterface.init(CAN_BPS_125K);

	//Datalogger
	LOGLN("DATALOGGER SETTINGS");
	channelsConfig.init();
	//channelsConfig.debug();	//Print channels list
	channelsBuffer.init();
	dataLogger.init();

	
	//WheelSensor
	wheelSensor.init();
	onResetButtonPress();
	/*
	//Strategy
	LOGLN("STRATEGY SETTINGS")
	strategySettings.init();
	//strategySettings.debugGPSSettings();

	strategy.setTFirstProfile(strategySettings.FirstProfile[0]);
	strategy.setSFirstProfile(strategySettings.FirstProfile[1]);
	strategy.setTProfile(strategySettings.Profile[0]);
	strategy.setSProfile(strategySettings.Profile[1]);
	strategy.setTLastProfile(strategySettings.LastProfile[0]);
	strategy.setSLastProfile(strategySettings.LastProfile[1]);
	strategy.setTrackData(strategySettings.TrackData);
	strategy.init();
	strategyTimer.setDuration(STRATEGY_STEP_PERIOD).start();
	*/
	//Phone
	phoneInterface.init();

	/*
	SdFile root = SD.getRoot();
	String path;
	
	root.ls();

	
	root.rewind();
	path = root.getNextFilePath();
	while (path.length()){
		Serial.println(path);
		path = root.getNextFilePath();
	}
	*/


	Button::setMaxNumber(BUTTON_NUM);
	//Reverse logic rising edge = release falling edge = pressed
	Button::add(RESET_BUTTON_PIN, NULL, &onResetButtonPress);
	Button::add(CALL_BUTTON_PIN, NULL, &onCallButtonPress);
	Button::add(CHANGE_FORM_BUTTON_PIN, NULL, &onChangeFormButtonPress);

	LOGLN("INIT_COMPLETED");
	consoleForm.println("INIT_COMPLETED");

	onChangeFormButtonPress();	//Advance to main form

	sec.setDuration(1000).start();
	avgExecutionTime = 0;
	loops = 0;

	WDT_Disable(WDT);
}

void loop() {
	WDT_Restart(WDT);

	t.start();

	//Can update
	//canInterface.readAll();

	//Wheelsensor update
	
	if (channelsBuffer.isValueUpdated(CAN_PWR)){
		wheelSensor.setPower(channelsBuffer.getValueAs<float>(CAN_PWR));
		LOGLN("WHEEL_SENSOR_UPDATED_POWER");
	}
	/*

	//Waypoint check
	if (channelsBuffer.isValueUpdated(CAN_GPSVALID)){
		if (channelsBuffer.getValueAs<bool>(CAN_GPSVALID)){
			int i = 0;
			double lat = channelsBuffer.getValueAs<float>(CAN_LAT);
			double lon = channelsBuffer.getValueAs<float>(CAN_LON);
			while (i < strategySettings.getWayPointsNum() && !strategySettings.getWayPoint(i).ProcessNewPoint(lat, lon))i++;

			if (i < strategySettings.getWayPointsNum()){
				wheelSensor.processWayPoint(i);
				LOGLN("WAY_POINT_FOUND");
				channelsBuffer.setValue(CAN_WAYPOINT, (byte*)&i, 1);
			}
		}
	}
	

	//Strategy update
	//TODO: step each 500ms and save values in channel buffer
	
	if (strategyTimer.hasFinished()){
		LOGLN("Step");
		strategy.step(
			wheelSensor.getLap(),
			wheelSensor.getRelativeSpace() * 100,
			wheelSensor.getRelativeMillis() / 10,
			wheelSensor.getSpeed() * 360
		);
		strategyTimer.start();
	}
	


	//TODO: on update save values inside channel buffer
	//Phone update
	phoneInterface.update();
	*/
	//Display update
	displayInterface.update();

	//Datalogger update
	//dataLogger.update();

	Button::update();

	//Execution time
	avgExecutionTime += t.elapsedTime();
	loops++;

	/*
	if (sec.hasFinished()){
		LOG("Loop call: "); LOG(loops); 
		LOG("       avgTime: "); LOG((float)avgExecutionTime / loops);
		LOG("       freeMem: "); LOGLN(freeMemory());
		avgExecutionTime = 0;
		loops = 0;
		sec.start();
	}
	*/
	
}

//Buttons events
void onResetButtonPress(void* data){
	wheelSensor.Speed = 0;
	wheelSensor.RelativeSpace = 0;
	wheelSensor.FullLaps = 0;
	wheelSensor.AvgSpeed = 0;
	wheelSensor.Space = 0;
	wheelSensor.TimeMillis = 0;
	wheelSensor.LastFinishTime = 0;
	wheelSensor.LastRelativeMillis = 0;
	wheelSensor.Energy = 0;
	LOGLN("WHEEL_SENSOR_RESET_BUTTON_PRESSED");
}

void onCallButtonPress(void* data){
	if (!phoneInterface.isCallActive()){
		BLSerial.println(START_CALL_CMD);
	}
	else{
		BLSerial.println(END_CALL_CMD);
	}
	phoneInterface.callActive = !phoneInterface.callActive;
	LOGLN("CALL_BUTTON_PRESSED");
}

void onChangeFormButtonPress(void* data){
	displayInterface.nextForm();
	LOGLN("FORM CHANGE_BUTTON_PRESSED");
}





#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
	char top;
#ifdef __arm__
	return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	return &top - __brkval;

#else  // __arm__
	return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}