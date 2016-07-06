
#include "MapsForm.h"

//Form
void MapsFormClass::init(Genie& genie){
	//Init string-list objs
	propList.init(PROP_TEXT_LIST, LIST_BUFFER_SIZE, &genie);
	detailList.init(DETAIL_TEXT_LIST, LIST_BUFFER_SIZE, &genie);
	valueList.init(VALUE_TEXT_LIST, LIST_BUFFER_SIZE, &genie); 
	statusMsg.init(STATUS_STRING, &genie);

	//Load motor cfgs
	if (!motorCfg.loadFromFile(MOTOR_CFG_FILE)){
		consoleForm.println(F("Error loading motor config file!"));
		ASSERT(false, F("Error loading motor config file!"));
	}

	//Load mapsets cfgs
	if (!mapCfg.loadFromFile(MAPS_CFG_FILE)){
		consoleForm.println(F("Error loading map config file!"));
		ASSERT(false, F("Error loading map config file!"));
	}
}
//OK
void MapsFormClass::update(Genie& genie){
	//Nothing to do here
}
//OK
void MapsFormClass::onEnter(Genie& genie){
	clearAll();
}
//OK
void MapsFormClass::onEvent(Genie& genie, genieFrame& evt){
	//Button press handler
	if (evt.reportObject.cmd == GENIE_REPORT_EVENT){
		if (evt.reportObject.object == GENIE_OBJ_WINBUTTON){
			switch (evt.reportObject.index){
				case GET_MOTOR_BUTTON:
					getMotorData();
					break;
				case GET_MAPSET_BUTTON:
					getMapSetData();
					break;
				case LOAD_MOT_BUTTON:
					onLoadMotorButtonPressed(genie);
					break;
				case LOAD_MAP_BUTTON:
					onLoadMapButtonPressed(genie);
					break;
				case OK_BUTTON:
					onOkButtonPressed(genie);
					break;
				case UP_BUTTON:
					onUpButtonPressed(genie);
					break;
				case DOWN_BUTTON:
					onDownButtonPressed(genie);
					break;
				case ENTER_BUTTON:
					onEnterButtonPressed(genie);
					break;
				case EXIT_BUTTON:
					onExitButtonPressed(genie);
					break;
			}
		}
	}

}
//OK
void MapsFormClass::clearAll(){
	currentState = NOTHING_LOADED;

	//Give focus to the string-list
	propList.clear();
	detailList.clear();
	valueList.clear();
	statusMsg.clear();

	propList.repaint();
	detailList.repaint();
	valueList.repaint();
	statusMsg.repaint();

	workingList = &propList;

}



//OK
void MapsFormClass::getMotorData(){
	Motor m;
	String value;
	StreamResult sResult;

	//Start the stream
	sResult = waitForStreamOverCan(CAN_DRIVER_CMD, GET_MOTOR_DATA_CMD, (byte*)&m, sizeof(Motor));
	//Clear display
	clearAll();

	//Print result
	switch (sResult){
		case SUCCES:
			currentState = GETTING_MOTOR_STATE;
			statusMsg.setMessage("Successful transfer!");
			workingList = &detailList;

			for (int i = 0; i < Motor::ATTR_COUNT; i++){
				value.remove(0, value.length());
				value += motorCfg.getProperty(i).getName();
				value += " = ";
			
				switch (i){
					case Motor::Name:
						value += m.name;
						break;

					case Motor::DefaultMap:
						value += (int)m.defaultMap;
						break;

					case Motor::Friction:
						value += m.friction;
						break;

					case Motor::FrictionGrad:
						value += String(m.frictionGrad, 6);
						break;

					case Motor::SpeedConst:
						value += m.speedConst;
						break;

					case Motor::SpeedTorqueGrad:
						value += String(m.speedTorqueGrad, 3);
						break;

					case Motor::TorqueConst:
						value += m.torqueConst;
						break;
				}

				detailList.addElement(value);
			}

			detailList.repaint();
			break;

		case ERROR:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage("Error on transfer!");
			break;

		case TIMEOUT:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage("Transfer timed out!");
			break;

		case WRONG_ACK:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage("Wrong ack!");
			break;
	}

}
//OK
void MapsFormClass::getMapSetData(){
	MotorMap mapSet[MAPS_PER_SET];
	String value;
	StreamResult sResult;

	//Start the stream
	sResult = waitForStreamOverCan(CAN_DRIVER_CMD, GET_MAPSET_DATA_CMD, (byte*)&mapSet, sizeof(MotorMap) * MAPS_PER_SET);
	//Clear the display
	clearAll();

	//Print result
	switch (sResult){
		case SUCCES:
			currentState = GETTING_MAPSET_STATE;
			statusMsg.setMessage("Successful transfer!");
			workingList = &detailList;
			//TODO: print
			detailList.repaint();
			break;

		case ERROR:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage("Error on transfer!");
			break;

		case TIMEOUT:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage("Transfer timed out!");
			break;
		case WRONG_ACK:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage("Wrong ack!");
			break;

	}

}



//Set
//OK
StreamResult MapsFormClass::setMotorData(){
	Motor motor;
	//Load struct from cfg
	motorCfg.toStruct((byte*)&motor, MOTOR_TYPES, propList.getCurrentElement() * Motor::ATTR_COUNT);
	//Stream
	return streamOverCan(CAN_DRIVER_CMD, SET_MOT_DATA_CMD, (byte*)&motor, sizeof(Motor));

}
//OK
StreamResult MapsFormClass::setMapData(){
	MotorMap mapset[MAPS_PER_SET];

	//Load selected mapset
	Configuration maps;
	maps.loadFromProperty(mapCfg.getProperty(propList.getCurrentElement()));

	if (maps.getPropertyCount() / MotorMap::ATTR_COUNT == MAPS_PER_SET){
		//Load struct from cfg
		for (int i = 0; i < MAPS_PER_SET; i++){
			maps.toStruct((byte*)&mapset[i], MAPS_TYPES, i * MotorMap::ATTR_COUNT);
		}

		//Stream
		return streamOverCan(CAN_DRIVER_CMD, SET_MAP_DATA_CMD, (byte*)mapset, sizeof(MotorMap) * MAPS_PER_SET);
	}

	return ABORT;
}



//OK
void MapsFormClass::onLoadMotorButtonPressed(Genie& genie){
	clearAll();
	loadMotorProperties();
}
//OK
void MapsFormClass::onLoadMapButtonPressed(Genie& genie){
	clearAll();
	loadMapSetProperties();
}
//OK
void MapsFormClass::onUpButtonPressed(Genie& genie){
	workingList->up();
}
//OK
void MapsFormClass::onDownButtonPressed(Genie& genie){
	workingList->down();
}
//OK
void MapsFormClass::onEnterButtonPressed(Genie& genie){
	switch (currentState){
		case LOADING_MOT_STATE_PROP:
			loadMotorDetails();
			workingList->repaint();
			break;

		case LOADING_MAP_STATE_PROP:
			loadMapSetDetails();
			workingList->repaint();
			break;	

		case LOADING_MAP_STATE_DETAIL:
			loadMapSetValues();
			workingList->repaint();
			break;
	}	
}
//OK
void MapsFormClass::onExitButtonPressed(Genie& genie){
	switch (currentState){
		case LOADING_MOT_STATE_DETAIL:
			//Clear current list
			workingList->clear();
			//Change working list
			workingList = &propList;
			//Change state
			currentState = LOADING_MOT_STATE_PROP;
			break;

		case LOADING_MAP_STATE_DETAIL:
			//Clear current list
			workingList->clear();
			//Change working list
			workingList = &propList;
			//Change state
			currentState = LOADING_MAP_STATE_PROP;
			break;

		case LOADING_MAP_STATE_VALUE:
			//Clear current list
			workingList->clear();
			//Change working list
			workingList = &detailList;
			//Change state
			currentState = LOADING_MAP_STATE_DETAIL;
			break;
	}
	
}
//OK
void MapsFormClass::onOkButtonPressed(Genie& genie){
	StreamResult sResult;

	statusMsg.clear();
	
	switch (currentState){
		case LOADING_MOT_STATE_PROP:
		case LOADING_MOT_STATE_DETAIL:
			sResult = setMotorData();
			break;

		case LOADING_MAP_STATE_PROP:
		case LOADING_MAP_STATE_DETAIL:
		case LOADING_MAP_STATE_VALUE:
			sResult = setMapData();
			break;

		default:
			return;
	}

	switch (sResult){
		case SUCCES:
			statusMsg.setMessage("Successful transfer!");
			break;

		case ERROR:
			statusMsg.setMessage("Error on transfer!");
			break;

		case TIMEOUT:
			statusMsg.setMessage("Transfer timed out!");
			break;

		case ABORT:
			statusMsg.setMessage("Map number not match!");
			break;
	}
}


//Show	
//OK
void MapsFormClass::loadMotorProperties(){
	//Set new state
	currentState = LOADING_MOT_STATE_PROP;

	//If there are props
	if (motorCfg.getPropertyCount() > 0){
		//Clear list
		propList.clear();
		//Load motors names
		for (int i = Motor::Name; i < motorCfg.getPropertyCount(); i += Motor::ATTR_COUNT){
			propList.addElement(motorCfg.getProperty(i).asString());
		}
		//Repaint
		propList.repaint();
	}

}
//OK
void MapsFormClass::loadMotorDetails(){
	//Clear detail list
	detailList.clear();
	//Load current motor prop's detail
	for (int i = Motor::Name + 1; i < Motor::ATTR_COUNT; i++){
		//Add element
		detailList.addElement(
			motorCfg.getProperty(
				propList.getCurrentElement() * Motor::ATTR_COUNT + i
			).getName() +
			" = " +
			motorCfg.getProperty(
				propList.getCurrentElement() * Motor::ATTR_COUNT + i
			).asString()
		);
	}

	//Change working list
	workingList = &detailList;
	//Change state
	currentState = LOADING_MOT_STATE_DETAIL;
}
//OK
void MapsFormClass::loadMapSetProperties(){
	//Set new state
	currentState = LOADING_MAP_STATE_PROP;

	//If there are props
	if (mapCfg.getPropertyCount() > 0){
		//Clear list
		propList.clear();
		//Load mapset's names
		for (int i = 0; i < mapCfg.getPropertyCount(); i++){
			propList.addElement(mapCfg.getProperty(i).getName());
		}
		//Repaint
		propList.repaint();
	}
}
//OK
void MapsFormClass::loadMapSetDetails(){
	//Clear detail list
	detailList.clear();

	//Load selected mapset
	Configuration mapset;
	mapset.loadFromProperty(mapCfg.getProperty(propList.getCurrentElement()));
	
	//Load current map prop's names
	for (int i = MotorMap::Name; i < mapset.getPropertyCount(); i += MotorMap::ATTR_COUNT){
		detailList.addElement(mapset.getProperty(i).asString());
	}

	//Change working list
	workingList = &detailList;
	//Change state
	currentState = LOADING_MAP_STATE_DETAIL;
}
//OK
void MapsFormClass::loadMapSetValues(){
	//Clear detail list
	valueList.clear();

	//Load selected mapset
	Configuration mapset;
	mapset.loadFromProperty(mapCfg.getProperty(propList.getCurrentElement()));

	//Load current map prop's values
	for (int i = MotorMap::Name + 1; i < MotorMap::ATTR_COUNT; i++){
		valueList.addElement(
			mapset.getProperty(
				detailList.getCurrentElement() * MotorMap::ATTR_COUNT + i
			).getName() +
			" = " +
			mapset.getProperty(
				detailList.getCurrentElement() * MotorMap::ATTR_COUNT + i
			).asString()
		);
	}

	//Change working list
	workingList = &valueList;
	//Change state
	currentState = LOADING_MAP_STATE_VALUE;
}


//Rx/Tx
StreamResult MapsFormClass::streamOverCan(unsigned short canID, const char* openStreamCmd, byte* buffer, int size){
	byte ack;
	int r, q;
	Timer timeOut;
	CAN_FRAME frame;

	//Open the stream over can
	canInterface.send(canID, (byte*)openStreamCmd, strlen(openStreamCmd));

	//Needed packets to send
	q = size / 8;
	r = size % 8;
	ack = getAck(buffer, size);

	LOGLN(size);
	LOG_ARR(buffer, size, HEX);

	//Send packets
	for (int i = 0; i < q; i++){
		canInterface.send(canID, buffer + 8 * i, 8);
		delay(PACKET_DELAY);
	}

	//Send remaining bytes
	if (r != 0){
		canInterface.send(canID, buffer + 8 * q, r);
		delay(PACKET_DELAY);
	}

	//Send ack
	canInterface.send(canID, &ack, 1);

	//Init timeout timer
	timeOut.setDuration(CMD_TIMEOUT).start();

	while (!timeOut.hasFinished()){
		//Read all response frame
		frame = canInterface.read();
		//If the packet has the stream id
		if (frame.id == canID){
			//if response is OK
			if (strcmp((const char*)frame.data.bytes, OK_CMD) == 0){
				return SUCCES;
			}
			//if response is ERROR
			else if (strcmp((const char*)frame.data.bytes, ERROR_CMD) == 0){
				return ERROR;
			}
		}
	}

	return TIMEOUT;
}
//OK
StreamResult MapsFormClass::waitForStreamOverCan(unsigned short canID, const char* openStreamCmd, byte* buffer, int expectedBytes){
	byte ack;
	int memIndex;
	Timer timeOut;
	CAN_FRAME frame;

	//Open the stream over can
	canInterface.send(canID, (byte*)openStreamCmd, strlen(openStreamCmd));

	//Set timeout timer
	timeOut.setDuration(CMD_TIMEOUT).start();

	memIndex = 0;
	//Wait for can packet
	while (!timeOut.hasFinished() && memIndex < expectedBytes){
		frame = canInterface.read();
		//Read responses
		if (frame.id == CAN_DRIVER_CMD){
			//On error
			if (strcmp((const char*)frame.data.bytes, ERROR_CMD) == 0){
				return ERROR;
			}
			//Save in buffer the data
			memcpy(
				buffer + memIndex,
				frame.data.bytes,
				memIndex + frame.length > expectedBytes ? expectedBytes - memIndex : frame.length	//Buffer over-run check
			);
			memIndex += frame.length;
		}
	}

	//Timeout
	if (memIndex < expectedBytes){
		return TIMEOUT;
	} 
	//Wait for ack
	while (!timeOut.hasFinished()){
		frame = canInterface.read();
		if (frame.id == CAN_DRIVER_CMD && frame.length == 1){
			ack = frame.data.bytes[0];
			if (ack != getAck(buffer, expectedBytes)){
				//LOGLN((int)ack);
				//LOGLN((int)getAck(buffer, expectedBytes))
				return WRONG_ACK;
			}
			return SUCCES;
		}
	}

	return TIMEOUT;
}
//OK
byte MapsFormClass::getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}


MapsFormClass mapsForm;

