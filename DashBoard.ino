/*
 Name:		DashBoard.ino
 Created:	8/25/2015 7:25:30 PM
 Author:	Davide Malvezzi
*/

#include "GpsInterface.h"
#include "SteerSensor.h"
#include "BMSInterface.h"
#include "Channel.h"
#include "ConsoleForm.h"
#include "DebugForm.h"
#include "CanInterface.h"
#include "ChannelsBuffer.h"
#include "ChannelsConfig.h"
#include "DataLogger.h"
#include "DisplayInterface.h"
#include "HWConfig.h"
#include "Interprete.h"
#include "LCDForm.h"
#include "LCDStringList.h"
#include "LCDStringMsg.h"
#include "Logger.h"
#include "GpsInterface.h"
#include "MainForm.h"
#include "MapsForm.h"
#include "MapSelector.h"
#include "PhoneInterface.h"
#include "Shell.h"
#include "StrategySettings.h"	
#include "TelemetryInterface.h"
#include "WheelSensor.h"	

#include <BitArray.h>
#include <Button.h>
#include <ByteBuffer.h>
#include <Configuration.h>
#include <due_can.h>
#include <DueTimer.h>
#include <genieArduino.h>	//Hacked: to reduce communication time
#include <Map.h>
#include <SPI.h>
#include <SD.h> 
#include <Timer.h>
#include <Utils.h>
#include <Vector.h>


//Inits
void initPorts();
boolean initDataLogger();
boolean initStategy();
void initButtons();

//Events
void onCanPacketReceived(CAN_FRAME& frame);
void onGpsDataReceived(GpsData& gps);

//Code flag
#define WDT_ON		
#define LCD_ON
#define DL_ON
#define STRAT_ON
#define PHONE_ON
#define TLM_ON
#define GPS_ON
#define LOOP_DEBUG
//#define SHELL_ON

//Log tag
#define INIT_TAG	F("INIT")
#define LOOP_TAG	F("LOOP")
#define BTN_TAG		F("BTN")

//SW info
#define SW_REV		F("19")
#define SW_INFO		String(F("Dashboard SW Rev ")) + SW_REV + String(F(" built ")) + F(__DATE__) + String(" ") + F(__TIME__)
///////////////////////////

//Test stuff
boolean ledStatus;
int loops;
unsigned long avgExecutionTime;
Timer t, sec;
///////////////////////////

#ifdef WDT_ON
	void watchdogSetup(){}
#endif

void setup() {
	//Serial, SD, digital pin, BMS, mapSelector, CAN
	initPorts();

	//Start timer to count init time
	t.start();

	//Display
	#ifdef LCD_ON
		displayInterface.init();
		consoleForm.println(SW_INFO);
		Log.i(INIT_TAG) << SW_INFO << Endl;
	#endif

	//Channels, Datalogger
	#ifdef DL_ON
		if (initDataLogger()){
			consoleForm.println(F("Datalogger OK"));
			Log.i(DL_TAG) << F("Datalogger OK ") << t.elapsedTime() << Endl;
		}
		else{
			consoleForm.println(F("Datalogger FAIL"));
			Log.e(DL_TAG) << F("Datalogger FAIL") << Endl;
		}
	#endif

	//Strategy
	#ifdef STRAT_ON
		if (initStategy()){
			consoleForm.println(F("Strategy OK"));
			Log.i(STRAT_TAG) << F("Strategy OK ") << t.elapsedTime() << Endl;
		}
		else{
			consoleForm.println(F("Strategy FAIL"));
			Log.e(STRAT_TAG) << F("Strategy FAIL") << Endl;
		}
	#endif

	//GPS
	#ifdef GPS_ON
		//GPS
		if (gps.init()){
			gps.debugSettings();
			consoleForm.println(F("Gps OK"));
			Log.i(GPS_TAG) << F("Gps OK ") << t.elapsedTime() << Endl;
		}
		else{
			consoleForm.println(F("Gps FAIL"));
			Log.e(GPS_TAG) << F("Gps FAIL ") << Endl;
		}
	#endif


	//Phone
	#ifdef PHONE_ON
		phoneInterface.init();
		phoneInterface.setGpsDataHandler(&onGpsDataReceived);
		consoleForm.println(F("Phone OK"));
		Log.i(PHONE_TAG) << F("Phone OK ") << t.elapsedTime() << Endl;
	#endif

	#ifdef TLM_ON
		telemetryInterface.init(&BL_SERIAL);
		consoleForm.println(F("Telemetry OK"));
		Log.i(TLM_TAG) << F("Telemetry OK ") << t.elapsedTime() << Endl;
	#endif
	
	//Notify init completed
	consoleForm.println(F("Dashboard init OK"));
	Log.i(INIT_TAG) << F("Dashboard init OK ") << t.elapsedTime() << Endl;

	//Go to the main form
	displayInterface.setCurrentForm(&mainForm);

	//Init time
	Log.i(INIT_TAG) << F("Time needed to init: ") << t.elapsedTime() << Endl;

	//Init test stuff
	ledStatus = LOW;
	sec.setDuration(1000).start();
	avgExecutionTime = 0;
	loops = 0;
	
	//Enable WDT
	#ifdef WDT_ON
		watchdogEnable(WDT_TIMEOUT);
	#else
		watchdogDisable();
	#endif
}

//bool print = false;

void loop() {
	//Reset Watchdog
	#ifdef WDT_ON
		watchdogReset();
	#endif

	//Test execution time
	t.start();
	
	//Read all packets and update debug if necessary
	canInterface.update();

	//Update BMS led button
	BMS.update();

	//Check map selector config
	mapSelector.update();

	//Read from sensors
	steerSensor.update();

	//Update the values on ChannelBuffer
	wheelSensor.update();

	//Buttons update
	Button::update();

	#ifdef PHONE_ON
		//Check for in-coming packet from the phone
		phoneInterface.update();
	#endif

	#ifdef STRAT_ON
		//Strategy update
		strategy.update();
	#endif
	
	/* Test sensore di velocità
	if (print){
		Log.e("MOT") << "Gap is " << wheelSensor.getLastGap() << Endl;
	}
	else if (wheelSensor.isGapValid()){
		Log.e("MOT") << "Speed is " << wheelSensor.getSpeed() * 3.6 << Endl;
		Log.e("MOT") << "Gap is " << wheelSensor.getLastGap() << Endl;
		if (wheelSensor.getSpeed() * 3.6 >= 28){
			print = true;
		}
	}
	*/

	#ifdef TLM_ON
		//Log telemetry's data
		telemetryInterface.update();
	#endif

	#ifdef DL_ON
		//Log data on SD
		dataLogger.update();
	#endif

	#ifdef LCD_ON
		//Display update
		displayInterface.update();
	#endif

	#ifdef SHELL_ON
		//Shell update
		shell.update();
	#endif

	//Test execution time
	avgExecutionTime += t.elapsedTime();
	loops++;

	if (sec.hasFinished()){
		ledStatus = !ledStatus;
		digitalWrite(YELLOW_LED, ledStatus);

		Log.i(LOOP_TAG) << F("Loop calls: ") << loops
						<< F("\t avgExecTime: ") << (float)avgExecutionTime / loops * 1000 << F(" us ")
						<< F("\t freeMem: ") << freeMemory() << Endl;
		avgExecutionTime = 0;
		loops = 0;
		sec.start();
	}
	//////////////////////
}

//Inits functions
void initPorts(){
	INIT_SERIAL(LOG_SERIAL, LOG_SERIAL_BAUD);
	INIT_SERIAL(BL_SERIAL, BL_SERIAL_BAUD);
	INIT_SD(SD, SD_SS_PIN);

	pinMode(RED_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(GREEN_LED, INPUT);
	pinMode(BL_RST_PIN, OUTPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	analogReadResolution(12);

	//Controls
	mapSelector.init();
	BMS.init();
	initButtons();

	//Can
	canInterface.init(CAN_SPEED);
	canInterface.setCanEventCallBack(&onCanPacketReceived);

	//SteerSensor
	steerSensor.init();

	//WheelSensor
	wheelSensor.init(); //Reset is called in the init

	//Utils
	Log.init(&LOG_SERIAL);

	#ifdef SHELL_ON
		shell.init(&LOG_SERIAL);
	#endif
}

boolean initDataLogger(){
	//Datalogger
	if (channelsConfig.init()){
		//channelsConfig.debug();
		channelsBuffer.init();
		dataLogger.init();
		return true;
	}

	return false;
}

boolean initStategy(){
	//Strategy
	if (strategySettings.init()){
		strategy.init();
		return true;
	}

	return false;
}

void initButtons(){
	//Reverse logic		rising edge = release, falling edge = pressed
	Button::setMaxNumber(BUTTON_NUM);
	Button::add(BMS_BUTTON_PIN,				NULL, &onStartButtonPressed		);
	Button::add(BL_CALL_BUTTON_PIN,			NULL, &onCallButtonPress		);
	Button::add(LCD_CHANGE_FORM_BUTTON_PIN, NULL, &onChangeFormButtonPress	);	

	//Need to check the press time, so connect to the rising edge
	Button::add(WHEEL_RESET_BUTTON_PIN, &onResetButtonPress, NULL);

}

//Events functions
void onCanPacketReceived(CAN_FRAME& frame){
	//Log.i(CAN_TAG) << F("Received ") << frame.id << " " << frame.length << " " << Hex << Log.array<byte>(frame.data.bytes, frame.length) << Endl;

	channelsBuffer.setValue(frame.id, frame.data.bytes, frame.length);

	switch (frame.id){
		case CanID::MOTOR_POWER:
			wheelSensor.setPower(channelsBuffer.getValueAs<float>(CanID::MOTOR_POWER));
			break;

		case CanID::MOTOR_DUTY_CICLE:
			mainForm.updateCurrentMotorPower(channelsBuffer.getValueAs<byte>(CanID::MOTOR_DUTY_CICLE));
			break;

		//Don't take the status from channelBuffer because in the case the DashBoard is in safe mode channelBuffer might be not available
		//and so no state is passed to the BMS class, instead pass the raw packet that is as available as the CAN is
		case CanID::BMS_STATUS:
			BMS.onStateChanged((const char*)frame.data.bytes);
			break;

		case CanID::MOTOR_MAP:
			mainForm.updateCurrentMap(channelsBuffer.getValueAs<byte>(CanID::MOTOR_MAP));
			break;

	}

}

void onGpsDataReceived(const GpsData& gpsData){
	byte i = 0;

	if (gps.isValid()){
		//Look if inside a waypoint 
		while (i < gps.getWayPointsCount() && 
			!gps.getWayPoint(i).processNewPoint(gpsData.latitude, gpsData.longitude))i++;

		//If found -> process
		if (i < gps.getWayPointsCount()){
			wheelSensor.processWayPoint(i);
			channelsBuffer.setValue<byte>(CanID::GPS_WAYPOINT, i+1);
		}
	}
	else{
		Log.w(GPS_TAG) << F("Gps waypoint not loaded") << Endl;
	}

}

//Buttons events
void onResetButtonPress(void* data){
	if (Button::getPressTime() > WHEEL_SENSOR_RESET_TIME){

		#ifdef LOOP_DEBUG
			Log.i(BTN_TAG) << F("Wheel sensor reset button pressed") << Endl;
		#endif

		wheelSensor.reset();
	}
}

void onCallButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Call button pressed") << Endl;
#endif

	phoneInterface.startCall();
}

void onChangeFormButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Change form button pressed") << Endl;
#endif

	if (displayInterface.getCurrentForm() == &mainForm){
		displayInterface.setCurrentForm(&debugForm);
	}
	else if (displayInterface.getCurrentForm() == &debugForm){
		displayInterface.setCurrentForm(&mainForm);
	}
}

void onStartButtonPressed(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("BMS start button pressed") << Endl;
#endif

	BMS.start();
}
