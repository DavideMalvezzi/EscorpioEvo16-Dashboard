// HWConfig.h

#ifndef _HWCONFIG_h
#define _HWCONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define NEW_DASHBOARD

#ifdef NEW_DASHBOARD

//Bluetooth
#define BL_SERIAL					Serial2
#define BL_SERIAL_BAUD				115200
#define BL_CALL_BUTTON_PIN			4
#define BL_RST_PIN					12
#define BL_STATE_PIN				13

//BMS
#define BMS_BUTTON_LED				A1
#define BMS_BUTTON_PIN				42

//Buttons
#define BUTTON_NUM					4

//CAN
#define CAN_DEBUG_SERIAL			SerialUSB
#define CAN_SERIAL_BAUD				115200
#define CAN_SPEED					CAN_BPS_125K

//LCD
#define LCD_SERIAL					Serial1
#define LCD_SERIAL_BAUD				115200
#define LCD_RESET_PIN				3
#define LCD_CHANGE_FORM_BUTTON_PIN	6

//Gas
#define GAS_BUTTON					46

//Leds
#define RED_LED						10
#define YELLOW_LED					11
#define GREEN_LED					13

//Log
#define LOG_SERIAL					Serial
#define LOG_SERIAL_BAUD				115200

//MapSelector
#define SEL1_PIN					8
#define SEL2_PIN					9

//SD
#define SD_SS_PIN					23

//Sensors
#define STEER_SENSOR_PIN			A2
#define AN_1						A3
#define AN_2						A4

//WDT
#define WDT_TIMEOUT					10000

//Wheel
#define WHEEL_RESET_BUTTON_PIN		44

#else

//Bluetooth
#define BL_SERIAL					Serial2
#define BL_SERIAL_BAUD				9600
#define BL_CALL_BUTTON_PIN			2

//BMS
#define BMS_BUTTON_LED				5
#define BMS_BUTTON_PIN				5

//Buttons
#define BUTTON_NUM					4

//CAN
#define CAN_DEBUG_SERIAL			SerialUSB
#define CAN_SERIAL_BAUD				115200
#define CAN_SPEED					CAN_BPS_125K

//LCD
#define LCD_SERIAL					Serial1
#define LCD_SERIAL_BAUD				115200
#define LCD_RESET_PIN				6
#define LCD_CHANGE_FORM_BUTTON_PIN	3

//Leds
#define RED_LED						34
#define YELLOW_LED					32
#define GREEN_LED					44

//Log
#define LOG_SERIAL					Serial
#define LOG_SERIAL_BAUD				115200

//MapSelector
#define SEL1_PIN					8
#define SEL2_PIN					9

//SD
#define SD_SS_PIN					28

//WDT
#define WDT_TIMEOUT					10000

//Wheel
#define WHEEL_RESET_BUTTON_PIN		4

#endif



#endif

