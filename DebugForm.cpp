
#include "DebugForm.h"


void DebugFormClass::init(Genie &genie){
	//Nothing to do here, for now
}

void DebugFormClass::update(Genie &genie){

	//BMS
	if (channelsBuffer.isValueUpdated(CanID::BMS_STATUS)){
		updateString(genie, BMS_STATUS_STRING, channelsBuffer.getValueAsString(CanID::BMS_STATUS));
	}
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
		
		case CAN_ON_SERIAL_BUTTON:
			INIT_SERIAL(CAN_DEBUG_SERIAL, CAN_SERIAL_BAUD);
			canInterface.setCanDebugSerialPort(&CAN_DEBUG_SERIAL);
		break;

		case CAN_ON_BLUETOOTH_BUTTON:
			INIT_SERIAL(BL_SERIAL, CAN_SERIAL_BAUD);
			canInterface.setCanDebugSerialPort(&BL_SERIAL);
			break;

		case CAN_STOP_BUTTON:
			canInterface.setCanDebugSerialPort(NULL);
			break;

		case DRIVER_SETTING_BUTTON:
			displayInterface.setCurrentForm(&mapsForm);
			break;

		case BMS_DEBUG_BUTTON:
			BMS.setDebugMode();
			break;
	}

}

DebugFormClass debugForm;