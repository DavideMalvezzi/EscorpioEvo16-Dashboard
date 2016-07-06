
#include "DebugForm.h"


void DebugFormClass::init(Genie &genie){
	//Nothing to do here, for now
}

void DebugFormClass::update(Genie &genie){

	//BMS
	updateString(genie, BMS_STATUS_STRING, channelsBuffer.getValueAsString(CanID::BMS_STATUS));
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, BATTERY_V_DIGITS, channelsBuffer.getValueAs<float>(CanID::PACK_VOLTAGE) * 100.0);

	byte cell;
	for (int i = 0; i < CELLS_BARS_NUM; i++){
		cell = channelsBuffer.getValueAs<float>(CanID::BATTERY_CELL_0 + i) / CELLS_MAX_VOLT * 100.0;
		updateWidget(genie, GENIE_OBJ_SPECTRUM, CELLS_BARS, ((i << 8) | cell));
	}
	
	//Driver
	updateWidget(genie, GENIE_OBJ_LED_DIGITS, DRIVER_MAP_DIGIT, channelsBuffer.getValueAs<byte>(CanID::MOTOR_MAP));

}

void DebugFormClass::onEvent(Genie& genie, genieFrame& evt){
	switch (evt.reportObject.index){
		
		case DEBUG_ON_SERIAL_BUTTON:
			INIT_SERIAL(DebugSerial, CAN_SERIAL_BAUD);
			canInterface.setCanDebugSerialPort(&DebugSerial);
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

		case DRIVER_SETTING_BUTTON:
			displayInterface.setCurrentForm(&mapsForm);
			break;
	}

}

DebugFormClass debugForm;