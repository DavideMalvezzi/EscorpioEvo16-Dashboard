
#include "MapsForm.h"

void MapsFormClass::init(Genie& genie){
	leftList.reserve(LIST_BUFFER_SIZE);
	rightList.reserve(LIST_BUFFER_SIZE);
	statusString.reserve(16);
	motorCfg.initWithFile(MOTOR_CFG_FILE);
	mapCfg.initWithFile(MAPS_CFG_FILE);

	currentState = GETTING_DATA_STATE;
}

void MapsFormClass::update(Genie& genie){

}

void MapsFormClass::onEnter(Genie& genie){
	genie.WriteObject(GENIE_OBJ_STRINGS, LEFT_TEXT_LIST, 0);
	genie.WriteObject(GENIE_OBJ_STRINGS, RIGHT_TEXT_LIST, 0);
	genie.WriteObject(GENIE_OBJ_STRINGS, STATUS_STRING, 0);

}

void MapsFormClass::onEvent(Genie& genie, genieFrame& evt){
	if (evt.reportObject.cmd == GENIE_REPORT_EVENT){
		if (evt.reportObject.object == GENIE_OBJ_WINBUTTON){
			switch (evt.reportObject.index){
			case GET_DATA_BUTTON:
				onGetDataButtonPressed(genie);
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
			}
		}
	}

}

void MapsFormClass::onGetDataButtonPressed(Genie &genie){
	currentState = GETTING_DATA_STATE;
	clearString(genie, leftList, LEFT_TEXT_LIST);
	clearString(genie, rightList, RIGHT_TEXT_LIST);
	clearString(genie, statusString, STATUS_STRING);

	canInterface.send(CAN_DRIVER_CMD, (byte*)GET_DATA_CMD, sizeof(GET_DATA_CMD));

	if (getMotorData()){
		if (getMapData()){
			genie.WriteStr(LEFT_TEXT_LIST, (char*)leftList.c_str());
			genie.WriteStr(RIGHT_TEXT_LIST, (char*)rightList.c_str());
		}
	}
	genie.WriteStr(STATUS_STRING, (char*)statusString.c_str());

}

bool MapsFormClass::getMotorData(){
	CAN_FRAME frame;
	Motor motor;
	Timer timeOut;
	int memIndex;

	timeOut.setDuration(CMD_TIMEOUT).start();

	memIndex = 0;
	while (!timeOut.hasFinished() && memIndex < sizeof(Motor)){
		frame = canInterface.read();
		if (frame.id == CAN_DRIVER_CMD){
			if (strcmp((char*)frame.data.bytes, INVALID_RX_CMD) == 0){
				statusString.concat(F("Invalid\nconfig"));
				return false;
			}
			memcpy(
				(byte*)(&motor) + memIndex,
				frame.data.bytes,
				memIndex + frame.length > sizeof(Motor) ? sizeof(Motor)-memIndex : frame.length
				);
			memIndex += frame.length;
		}
	}

	if (memIndex < sizeof(Motor)){
		statusString.concat(F("Receive\ntimed\nout"));
		return false;
	}
	else if (motor.ack != getAck((byte*)&motor, sizeof(Motor)-1)){
		statusString.concat(F("Wrong\nmotor\nack"));
		LOGLN(motor.ack);
		LOGLN(getAck((byte*)&motor, sizeof(Motor)-1));
		return false;
	}
	else{
		leftList.concat(motor.name);

		rightList.concat(motorCfg.getPropertyName(1));
		rightList.concat('=');
		rightList.concat(String(motor.torqueConst, 6));
		rightList.concat("\n");

		rightList.concat(motorCfg.getPropertyName(2));
		rightList.concat('=');
		rightList.concat(String(motor.speedConst, 6));
		rightList.concat("\n");

		rightList.concat(motorCfg.getPropertyName(3));
		rightList.concat('=');
		rightList.concat(String(motor.speedTorqueGrad, 6));
		rightList.concat("\n");

		rightList.concat(motorCfg.getPropertyName(4));
		rightList.concat('=');
		rightList.concat(String(motor.friction, 6));
		rightList.concat("\n");

		rightList.concat(motorCfg.getPropertyName(5));
		rightList.concat('=');
		rightList.concat(String(motor.frictionGrad, 6));
		rightList.concat("\n");

		//statusString.concat(F("Params\nRetrieve\nSuccess"));

	}

	return true;
}

bool MapsFormClass::getMapData(){
	CAN_FRAME frame;
	MotorMap map;
	Timer timeOut;
	int memIndex;

	timeOut.setDuration(CMD_TIMEOUT).start();

	memIndex = 0;
	while (!timeOut.hasFinished() && memIndex < sizeof(MotorMap)){
		frame = canInterface.read();
		if (frame.id == CAN_DRIVER_CMD){
			if (strcmp((char*)frame.data.bytes, INVALID_RX_CMD) == 0){
				statusString.concat(F("Invalid\nconfig"));
				return false;
			}
			memcpy(
				(byte*)(&map) + memIndex,
				frame.data.bytes,
				memIndex + frame.length > sizeof(MotorMap) ? sizeof(MotorMap)-memIndex : frame.length
				);
			memIndex += frame.length;
		}
	}

	if (memIndex < sizeof(MotorMap)){
		statusString.concat(F("Receive\ntimed\nout"));
		return false;
	}
	else if (map.ack != getAck((byte*)&map, sizeof(MotorMap)-1)){
		statusString.concat(F("Wrong\nmap\nack"));
		return false;
	}
	else{

		for (int i = 0; i < motorCfg.getPropertiesCount() / 4; i++){
			leftList.concat('\n');
		}
		leftList.concat(map.name);

		rightList.concat('\n');

		rightList.concat(mapCfg.getPropertyName(1));
		rightList.concat('=');
		rightList.concat(String(map.a0, 6));
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(2));
		rightList.concat('=');
		rightList.concat(String(map.a1, 6));
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(3));
		rightList.concat('=');
		rightList.concat(String(map.a2, 6));
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(4));
		rightList.concat('=');
		rightList.concat(map.useSyncRect);
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(5));
		rightList.concat('=');
		rightList.concat(map.useSyncSafe);
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(6));
		rightList.concat('=');
		rightList.concat(String(map.syncTrh, 6));
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(7));
		rightList.concat('=');
		rightList.concat(map.useEnergyRecovery);
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(8));
		rightList.concat('=');
		rightList.concat(map.flatOut);
		rightList.concat("\n");

		rightList.concat(mapCfg.getPropertyName(9));
		rightList.concat('=');
		rightList.concat(String(map.flatLev, 6));
		rightList.concat("\n");

		statusString.concat(F("Params\nretrieve\nsuccess"));
	}

	return true;
}

void MapsFormClass::onLoadMotorButtonPressed(Genie& genie){
	currentState = LOADING_MOT_STATE;

	if (motorCfg.getPropertiesCount() > 0){
		currentListIndex = 0;
		clearString(genie, leftList, LEFT_TEXT_LIST);

		for (int i = 0; i < motorCfg.getPropertiesCount() / MOTOR; i++){
			if (i == currentListIndex){
				leftList.concat('>');
			}
			else{
				leftList.concat(" ");
			}

			leftList.concat(motorCfg.getProperty(i * MOTOR));
			leftList.concat('\n');
		}
		leftList.remove(leftList.length() - 1); //remove last \n

		genie.WriteStr(LEFT_TEXT_LIST, (char*)leftList.c_str());
		showConfig(genie);
	}

}

void MapsFormClass::onLoadMapButtonPressed(Genie& genie){
	currentState = LOADING_MAP_STATE;

	if (mapCfg.getPropertiesCount() > 0){
		currentListIndex = 0;
		clearString(genie, leftList, LEFT_TEXT_LIST);

		for (int i = 0; i < mapCfg.getPropertiesCount() / MAP; i++){
			if (i == currentListIndex){
				leftList.concat('>');
			}
			else{
				leftList.concat(" ");
			}

			leftList.concat(mapCfg.getProperty(i * MAP));
			leftList.concat('\n');
		}
		leftList.remove(leftList.length() - 1); //remove last \n

		genie.WriteStr(LEFT_TEXT_LIST, (char*)leftList.c_str());
		showConfig(genie);
	}

}

void MapsFormClass::onDownButtonPressed(Genie& genie){
	int current, index;

	if (currentState == LOADING_MAP_STATE || currentState == LOADING_MOT_STATE){

		current = leftList.indexOf('>');
		leftList.setCharAt(current, 0x20);

		index = leftList.indexOf('\n', current);

		if (index > 0){
			leftList.setCharAt(index + 1, '>');
			currentListIndex++;
		}
		else{
			leftList.setCharAt(current, '>');
		}

		genie.WriteStr(LEFT_TEXT_LIST, (char*)leftList.c_str());
		showConfig(genie);
	}
}

void MapsFormClass::onUpButtonPressed(Genie& genie){
	int current, index = 0;

	if (currentState == LOADING_MAP_STATE || currentState == LOADING_MOT_STATE){

		current = leftList.indexOf('>');
		leftList.setCharAt(current, 0x20);

		for (int i = 0; i < currentListIndex - 1; i++){
			index = leftList.indexOf('\n', index);
		}

		if (index > 0){
			leftList.setCharAt(index + 1, '>');
			currentListIndex--;
		}
		else{
			currentListIndex = 0;
			leftList.setCharAt(0, '>');
		}

		genie.WriteStr(LEFT_TEXT_LIST, (char*)leftList.c_str());
		showConfig(genie);
	}
}

void MapsFormClass::onOkButtonPressed(Genie& genie){
	clearString(genie, statusString, STATUS_STRING);

	if (currentState == LOADING_MAP_STATE){
		setMapData();
	}
	else if (currentState == LOADING_MOT_STATE){
		setMotorData();
	}

	genie.WriteStr(STATUS_STRING, (char*)statusString.c_str());
}

void MapsFormClass::setMotorData(){
	int r, q;
	Timer timeOut;
	CAN_FRAME frame;
	Motor motor;

	memcpy(motor.name, motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_NAME).c_str(), sizeof(motor.name));
	motor.defaulMap = motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_DEF_MAP).toInt();
	motor.friction = motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_FRICTION).toFloat();
	motor.frictionGrad = motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_FRICTION_GRAD).toFloat();
	motor.speedConst = motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_SPEED_CONST).toFloat();
	motor.speedTorqueGrad = motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_SPD_TORQUE_GRAD).toFloat();
	motor.torqueConst = motorCfg.getProperty(MOTOR * currentListIndex + MOTOR_TORQUE_CONST).toFloat();
	motor.ack = getAck((byte*)&motor, sizeof(Motor)-1);


	canInterface.send(CAN_DRIVER_CMD, (byte*)SET_MOT_DATA_CMD, sizeof(SET_MOT_DATA_CMD));

	q = sizeof(Motor) / 8;
	r = sizeof(Motor) % 8;

	for (int i = 0; i < q; i++){
		canInterface.send(CAN_DRIVER_CMD, (byte*)(&motor) + 8 * i, 8);
	}

	if (r != 0){
		canInterface.send(CAN_DRIVER_CMD, (byte*)(&motor) + 8 * q, r);
	}

	timeOut.setDuration(CMD_TIMEOUT).start();

	while (!timeOut.hasFinished()){
		frame = canInterface.read();
		if (frame.id == CAN_DRIVER_CMD){
			if (strcmp((char*)frame.data.bytes, OK_RX_CMD) == 0){
				statusString.concat(F("Data\nloading\ncompleted"));
				return;
			}
			else if (strcmp((char*)frame.data.bytes, ERROR_RX_CMD) == 0){
				statusString.concat(F("Data\nloading\nfailed"));
				return;
			}
		}
	}

	statusString.concat(F("Response\ntimed\nout"));
}

void MapsFormClass::setMapData(){
	byte cmd[sizeof(SET_MOT_DATA_CMD)];
	int r, q;
	Timer timeOut;
	CAN_FRAME frame;
	MotorMap map;

	//Set cmd with 
	memcpy(cmd, SET_MAP_DATA_CMD, sizeof(SET_MAP_DATA_CMD));
	cmd[sizeof(SET_MAP_DATA_CMD)-1] = 0;

	memcpy(map.name, mapCfg.getProperty(MAP * currentListIndex + MAP_NAME).c_str(), sizeof(map.name));
	map.a0 = mapCfg.getProperty(MAP * currentListIndex + MAP_A0).toFloat();
	map.a1 = mapCfg.getProperty(MAP * currentListIndex + MAP_A1).toFloat();
	map.a2 = mapCfg.getProperty(MAP * currentListIndex + MAP_A2).toFloat();
	map.flatLev = mapCfg.getProperty(MAP * currentListIndex + MAP_FLAT_LEV).toFloat();
	map.flatOut = mapCfg.getProperty(MAP * currentListIndex + MAP_FLAT_OUT).toInt();
	map.syncTrh = mapCfg.getProperty(MAP * currentListIndex + MAP_SYNC_TRH).toFloat();
	map.useEnergyRecovery = mapCfg.getProperty(MAP * currentListIndex + MAP_EN_RECOVERY).toInt();
	map.useSyncRect = mapCfg.getProperty(MAP * currentListIndex + MAP_SYNC_RECT).toInt();
	map.useSyncSafe = mapCfg.getProperty(MAP * currentListIndex + MAP_SYNC_SAFE).toInt();
	map.ack = getAck((byte*)&map, sizeof(MotorMap)-1);

	canInterface.send(CAN_DRIVER_CMD, cmd, sizeof(cmd));

	q = sizeof(MotorMap) / 8;
	r = sizeof(MotorMap) % 8;

	for (int i = 0; i < q; i++){
		canInterface.send(CAN_DRIVER_CMD, (byte*)(&map) + 8 * i, 8);
	}

	if (r != 0){
		canInterface.send(CAN_DRIVER_CMD, (byte*)(&map) + 8 * q, r);
	}

	timeOut.setDuration(CMD_TIMEOUT).start();

	while (!timeOut.hasFinished()){
		frame = canInterface.read();
		if (frame.id == CAN_DRIVER_CMD){
			if (strcmp((char*)frame.data.bytes, OK_RX_CMD) == 0){
				statusString.concat(F("Data\nloading\ncompleted"));
				return;
			}
			else if (strcmp((char*)frame.data.bytes, ERROR_RX_CMD) == 0){
				statusString.concat(F("Data\nloading\nfailed"));
				return;
			}
		}
	}

	statusString.concat(F("Response\ntimed\nout"));
}

void MapsFormClass::showConfig(Genie& genie){
	clearString(genie, rightList, RIGHT_TEXT_LIST);

	if (currentState == LOADING_MOT_STATE){
		for (int i = 1; i < MOTOR; i++){
			rightList.concat(motorCfg.getPropertyName(currentListIndex * MOTOR + i));
			rightList.concat(" = ");
			rightList.concat(motorCfg.getProperty(currentListIndex * MOTOR + i));
			rightList.concat('\n');
		}
	}
	else if (currentState == LOADING_MAP_STATE){
		for (int i = 1; i < MAP; i++){
			rightList.concat(mapCfg.getPropertyName(currentListIndex * MAP + i));
			rightList.concat(" = ");
			rightList.concat(mapCfg.getProperty(currentListIndex * MAP + i));
			rightList.concat('\n');
		}
	}

	genie.WriteStr(RIGHT_TEXT_LIST, (char*)rightList.c_str());

}

void MapsFormClass::clearString(Genie &genie, String& list, int index){
	for (int i = 0; i < list.length(); i++){
		if (list.charAt(i) != '\n'){
			list.setCharAt(i, 0x20);
		}
	}

	genie.WriteStr(index, (char*)list.c_str());

	list.remove(0, list.length());
}

byte MapsFormClass::getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}


MapsFormClass mapsForm;

