
#include "DebugForm.h"


void DebugFormClass::init(Genie &genie){
	//Nothing to do here, for now
}

void DebugFormClass::update(Genie &genie){
	//UPDATE GPS 
	/*
	genie.WriteObject(GENIE_OBJ_USER_LED, 2, channelsBuffer.getValueAs<byte>(CanID::GPS_VALID));

	//UPDATE BMS STATUS
	if (channelsBuffer.isValueUpdated(CanID::BMS_STATUS)){
		genie.WriteStr(1, (char*)channelsBuffer.getValueAsString(CanID::BMS_STATUS).c_str());
	}

	//UPDATE ENGINE MAP
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 8, channelsBuffer.getValueAs<byte>(CanID::MOTOR_MAP));

	//UPDATE BATTERY VOLTAGE
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 7, channelsBuffer.getValueAs<float>(CanID::BATTERY_VOLTAGE) * 100);

	//UPDATE CELLS VOLTAGE
	unsigned char cell;
	for (int i = 0; i < 10; i++){
		cell = channelsBuffer.getValueAs<float>(CanID::BATTERY_CELL_0 + i) / 4.2 * 100;
		genie.WriteObject(GENIE_OBJ_SPECTRUM, 0, ((i << 8) | cell));
	}
	*/
	
}

void DebugFormClass::onEvent(Genie& genie, genieFrame& evt){
	switch (evt.reportObject.index){

		case DEBUG_ON_SERIAL_BUTTON:
			INIT_SERIAL(CANSerial, CAN_SERIAL_BAUD);
			canInterface.setCanDebugSerialPort(&CANSerial);
			updateWidget(genie, GENIE_OBJ_USER_LED, DEBUG_ACTIVE_LED, HIGH);
		break;

		case DEBUG_ON_BLUETOOTH_BUTTON:
			INIT_SERIAL(BLSerial, CAN_SERIAL_BAUD);
			canInterface.setCanDebugSerialPort(&BLSerial);
			updateWidget(genie, GENIE_OBJ_USER_LED, DEBUG_ACTIVE_LED, HIGH);
			break;

		case DEBUG_STOP_BUTTON:
			canInterface.setCanDebugSerialPort(NULL);
			updateWidget(genie, GENIE_OBJ_USER_LED, DEBUG_ACTIVE_LED, LOW);
			break;
	}

}

DebugFormClass debugForm;