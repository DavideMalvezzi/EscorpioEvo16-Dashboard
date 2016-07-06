/*
 Name:		DashBoard.ino
 Created:	8/25/2015 7:25:30 PM
 Author:	Davide Malvezzi
*/

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
#include "MainForm.h"
#include "MapsForm.h"
#include "MapSelector.h"
#include "PhoneInterface.h"
#include "Shell.h"
#include "StrategySettings.h"	
#include "WdtUtils.h"		//Hacked: activated WDT in variant.cpp
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
//Updates
void updateStrategy();
//Events
void onCanPacketReceived(CAN_FRAME& frame);
void onGpsDataReceived(GpsData& gps);

//Code flag
#define WDT_ON		
#define LCD_ON
#define DL_ON
#define STRAT_ON
#define PHONE_ON
//#define SHELL_ON

//Log tag
#define INIT_TAG	F("INIT")
#define LOOP_TAG	F("LOOP")
#define BTN_TAG		F("BTN")

//SW info
#define SW_REV		F("12")
#define SW_INFO		String(F("Dashboard SW Rev ")) + SW_REV + String(F(" built ")) + F(__DATE__) + String(" ") + F(__TIME__)

//Strategy stuff
#define STRATEGY_STEP_PERIOD	500
Timer strategyTimer;
///////////////////////////

//Test stuff
boolean ledStatus;
int loops;
unsigned long avgExecutionTime;
Timer t, sec;
///////////////////////////


void setup() {
	//Start timer to count init time
	t.start();

	//Serial, SD, digital pin, BMS, mapSelector, CAN
	initPorts();

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
			Log.i(DL_TAG) << F("Datalogger FAIL") << Endl;
		}
	#endif

	//WheelSensor, Strategy
	#ifdef STRAT_ON
		if (initStategy()){
			consoleForm.println(F("Strategy OK"));
			Log.i(STRAT_TAG) << F("Strategy OK ") << t.elapsedTime() << Endl;
		}
		else{
			consoleForm.println(F("Strategy FAIL"));
			Log.i(STRAT_TAG) << F("Strategy FAIL") << t.elapsedTime() << Endl;
		}
	#endif

	//Phone
	#ifdef PHONE_ON
		phoneInterface.init();
		phoneInterface.setGpsDataHandler(&onGpsDataReceived);
		consoleForm.println(F("Phone OK"));
		Log.i(PHONE_TAG) << F("Phone OK ") << t.elapsedTime() << Endl;
	#endif

	//Physical buttons
	initButtons();
	consoleForm.println(F("Buttons OK"));
	Log.i(BTN_TAG) << F("Buttons OK ") << t.elapsedTime() << Endl;
	
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
		//WARNING: To enable the WDT it's needed to remove WDT_Disable in file variant.cpp
		enableWDT(WDT_TIMEOUT);
	#else
		disableWDT();
	#endif

}

void loop() {
	//Reset Watchdog
	#ifdef WDT_ON
		resetWDT();
	#endif

	//Test execution time
	t.start();
	
	//Read all packet and update debug if necessary
	canInterface.update();

	//Update BMS led button
	BMS.update();

	//Check map selector config
	mapSelector.update();

	#ifdef PHONE_ON
		//Check for in-coming packet from the phone
		phoneInterface.update();
	#endif

	#ifdef STRAT_ON
		//Strategy update
		updateStrategy();
	#endif

	#ifdef DL_ON
		//Log data on SD
		dataLogger.update();
	#endif

	#ifdef LCD_ON
		//Display update
		displayInterface.update();
	#endif

	//Buttons update
	Button::update();

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
	INIT_SD(SD, SD_SS_PIN);

	pinMode(RED_LED, OUTPUT);
	pinMode(YELLOW_LED, OUTPUT);
	pinMode(GREEN_LED, INPUT);

	digitalWrite(RED_LED, LOW);
	digitalWrite(YELLOW_LED, LOW);

	//Controls
	mapSelector.init();
	BMS.init();

	//Can
	canInterface.init(CAN_SPEED);
	canInterface.setCanEventCallBack(&onCanPacketReceived);

	//Utils
	shell.init(&LOG_SERIAL);
	Log.init(&LOG_SERIAL);
}

boolean initDataLogger(){
	//Datalogger
	if (channelsConfig.init()){
		channelsBuffer.init();
		dataLogger.init();
		return true;
	}

	return false;
}

boolean initStategy(){
	//WheelSensor
	wheelSensor.init(); //Reset is called in the init

	//Strategy
	if (strategySettings.init()){
		//strategySettings.debugGPSSettings();
		//strategySettings.debugTrackSettings();
		strategy.init();
		strategyTimer.setDuration(STRATEGY_STEP_PERIOD).start();
		return true;
	}

	return false;
}

void initButtons(){
	//Reverse logic		rising edge = release, falling edge = pressed
	Button::setMaxNumber(BUTTON_NUM);
	Button::add(BMS_BUTTON_PIN,				NULL, &onStartButtonPressed		);
	Button::add(BL_CALL_BUTTON_PIN,			NULL, &onCallButtonPress		);
	Button::add(WHEEL_RESET_BUTTON_PIN,		NULL, &onResetButtonPress		);
	Button::add(LCD_CHANGE_FORM_BUTTON_PIN, NULL, &onChangeFormButtonPress	);	
}

//Update functions
void updateStrategy(){
	//Strategy update
	if (strategyTimer.hasFinished() && strategySettings.isValid()){

		if (!channelsBuffer.isValueUpdated(CanID::MOTOR_POWER)){
			wheelSensor.setPower(0);
		}

		strategy.step(
			wheelSensor.getLap(),
			wheelSensor.getRelativeSpace() * 100,
			wheelSensor.getTimeMillis() / 10,
			wheelSensor.getSpeed() * 360
		);

		//TODO: save strategy values into channelbuffer

		strategyTimer.start();
	}
}

//Events functions
void onCanPacketReceived(CAN_FRAME& frame){
	//Log.i(CAN_TAG) << F("Received ") << frame.id << " " << frame.length << " " << Hex << Log.array<byte>(frame.data.bytes, frame.length) << Endl;

	channelsBuffer.setValue(frame.id, frame.data.bytes, frame.length);

	switch (frame.id){
		case CanID::MOTOR_POWER:
			wheelSensor.setPower(channelsBuffer.getValueAs<float>(CanID::MOTOR_POWER));
			break;

		//Don't take the status from channelBuffer because in the case the DashBoard is in safe mode channelBuffer might be not available
		//and so no state is passed to the BMS class, instead pass the raw packet that is still available as the CAN is
		case CanID::BMS_STATUS:
			BMS.onStateChanged((const char*)frame.data.bytes);
			break;

		case CanID::MOTOR_MAP:
			mainForm.setNewCurrentMap(channelsBuffer.getValueAs<byte>(CanID::MOTOR_MAP));
			break;
	}

}

void onGpsDataReceived(GpsData& gps){
	byte i = 0;

	//Look if inside a waypoint 
	while (i < strategySettings.getWayPointsNum() && 
		!strategySettings.getWayPoint(i).processNewPoint(gps.latitude, gps.longitude))i++;

	//If found -> process
	if (i < strategySettings.getWayPointsNum()){
		wheelSensor.processWayPoint(i);
		channelsBuffer.setValue(CanID::GPS_WAYPOINT, &i, sizeof(byte));
	}

	Log.i("GPS") << gps.latitude << " " << gps.longitude << " " << gps.altitude << " " << gps.speed << " " << gps.accuracy << Endl;
}

//Buttons events
void onResetButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Wheel sensor reset button pressed") << Endl;
#endif
	wheelSensor.reset();
}

void onCallButtonPress(void* data){
#ifdef LOOP_DEBUG
	Log.i(BTN_TAG) << F("Call button pressed") << Endl;
#endif

	phoneInterface.call();
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
