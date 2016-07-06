
#include "DebugForm.h"
#include "ChannelsBuffer.h"
#include "WayPoint.h"

void DebugFormClass::init(Genie &genie){

}

void DebugFormClass::update(Genie &genie){
	//UPDATE GPS 
	genie.WriteObject(GENIE_OBJ_USER_LED, 2, channelsBuffer.getValueAs<byte>(CAN_GPSVALID));

	//UPDATE BMS STATUS
	if (channelsBuffer.isValueUpdated(CAN_BMS)){
		genie.WriteStr(1, (char*)channelsBuffer.getValueAsString(CAN_BMS).c_str());
	}

	//UPDATE ENGINE MAP
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 8, channelsBuffer.getValueAs<byte>(CAN_MAP));

	//UPDATE BATTERY VOLTAGE
	genie.WriteObject(GENIE_OBJ_LED_DIGITS, 7, channelsBuffer.getValueAs<float>(CAN_BAT) * 100);

	//UPDATE CELLS VOLTAGE
	unsigned char cell;
	for (int i = 0; i < 10; i++){
		cell = channelsBuffer.getValueAs<float>(CAN_CELL0 + i) / 4.2 * 100;
		genie.WriteObject(GENIE_OBJ_SPECTRUM, 0, ((i << 8) | cell));
	}
	
}

DebugFormClass debugForm;