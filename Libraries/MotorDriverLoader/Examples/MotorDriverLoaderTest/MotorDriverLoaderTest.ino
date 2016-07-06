
#include <avr\eeprom.h>
#include <CanInterfaceUno.h>
#include <EEPROM.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <SPI.h>
#include <Timer.h>
#include <Utils.h>
#include <MotorDriverLoader.h>

#define SERIAL_BAUD		9600
#define CAN_SPEED		CAN_250KBPS

void setup() {
	INIT_SERIAL(Serial, SERIAL_BAUD);
	canInterface.init(CAN_SPEED);

	if (!driverLoader.initFromEEPROM()){
		LOGLN(F("Invalid config saved in the EEPROM!"));
	}
	
}

void loop(){
	CanFrame frame;
	if (canInterface.available()){
		canInterface.readFrame(&frame);

		if (frame.ID == CAN_DRIVER_CMD){
			driverLoader.onCanRequest(&frame);
		}
	}

}
