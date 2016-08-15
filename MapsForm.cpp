
#include "MapsForm.h"

//Form
void MapsFormClass::init(Genie& genie){
	//Init string-list objs
	propList.init(PROP_TEXT_LIST, LIST_BUFFER_SIZE, &genie);
	detailList.init(DETAIL_TEXT_LIST, LIST_BUFFER_SIZE, &genie);
	valueList.init(VALUE_TEXT_LIST, LIST_BUFFER_SIZE, &genie); 
	statusMsg.init(STATUS_STRING, &genie);
}
//OK
void MapsFormClass::update(Genie& genie){
	//Nothing to do here
}
//OK
void MapsFormClass::onEnter(Genie& genie){
	//Clear all the strings
	clearAll();
	
	//Load motor cfgs if not already loaded
	if (motorCfg.getPropertyCount() == 0){
		if (motorCfg.loadFromFile(MOTOR_CFG_FILE) != FILE_VALID){
			Log.e(LCD_TAG) << motorCfg.getErrorMsg() << Endl;
		}
	}

	//Load mapsets cfgs if not already loaded
	if (mapCfg.getPropertyCount() == 0){
		if (mapCfg.loadFromFile(MAPS_CFG_FILE) != FILE_VALID){
			Log.e(LCD_TAG) << mapCfg.getErrorMsg() << Endl;
		}
	}

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
					if (motorCfg.getPropertyCount() > 0){
						onLoadMotorButtonPressed(genie);
					}
					else{
						statusMsg.setMessage(F("Invalid motor cfg"));
					}
					break;

				case LOAD_MAP_BUTTON:
					if (mapCfg.getPropertyCount() > 0){
						onLoadMapButtonPressed(genie);
					}
					else{
						statusMsg.setMessage(F("Invalid mapset cfg"));
					}
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

				case BACK_BUTTON:
					displayInterface.setCurrentForm(&debugForm);
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


//Get
//OK
void MapsFormClass::getMotorData(){
	Motor m;
	String value;
	CanStreamResult sResult;

	//Start the stream
	statusMsg.setMessage(F("Waiting for response..."));
	//Send a GET_MOTOR_DATA_CMD request over can bus with id CanID::DRIVER_SETTINGS_CMD
	//The response will be save inside &m and is expected to be sizeof(Motor) bytes length
	//sResult will contain the request outcome
	sResult = canInterface.waitForStreamOverCan(CanID::DRIVER_SETTINGS_CMD, GET_MOTOR_DATA_CMD, (byte*)&m, sizeof(Motor));
	//Clear display
	clearAll();

	//Print result
	switch (sResult){
		//If the request is successful
		case SUCCES:
			//Change state
			currentState = GETTING_MOTOR_STATE;
			statusMsg.setMessage(F("Successful transfer!"));
			workingList = &detailList;

			//Print the response
			for (int i = 0; i < Motor::ATTR_COUNT; i++){
				value.remove(0, value.length());
			
				switch (i){
					case Motor::Name:
						value += F("NAME = ");
						value += m.name;
						value += '\0';
						break;

					case Motor::DefaultMap:
						value += F("DEF_MAP = ");
						value += (int)m.defaultMap;
						break;

					case Motor::Friction:
						value += F("FRICTION = ");
						value += m.friction;
						break;

					case Motor::FrictionGrad:
						value += F("FRICTION_GRAD = ");
						value += String(m.frictionGrad, 6);
						break;

					case Motor::SpeedConst:
						value += F("SPEED_CONST = ");
						value += m.speedConst;
						break;

					case Motor::SpeedTorqueGrad:
						value += F("SPEED_TORQUE_GRAD = ");
						value += String(m.speedTorqueGrad, 3);
						break;

					case Motor::TorqueConst:
						value += F("TORQUE_CONST = ");
						value += m.torqueConst;
						break;

					case Motor::GearTrain:
						value += F("GEAR_TRAIN = ");
						value += m.gearTrain;
						break;
				}

				detailList.addElement(value);
			}

			detailList.repaint();
			break;

		//If the request is not successfull
		case ERROR:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage(F("Error on transfer!"));
			break;

		case TIMEOUT:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage(F("Transfer timed out!"));
			break;

		case WRONG_ACK:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage(F("Wrong ack!"));
			break;
	}

}
//OK
void MapsFormClass::getMapSetData(){
	String value;
	CanStreamResult sResult;

	//Start the stream
	statusMsg.setMessage(F("Waiting for response..."));
	sResult = canInterface.waitForStreamOverCan(CanID::DRIVER_SETTINGS_CMD, GET_MAPSET_DATA_CMD, (byte*)&mapSet, sizeof(MotorMap)* MAPS_PER_SET);
	//Clear the display
	clearAll();

	//Print result
	switch (sResult){
		case SUCCES:
			currentState = GETTING_MAPSET_STATE;
			statusMsg.setMessage(F("Successful transfer!"));
			workingList = &propList;
			for (int i = 0; i < MAPS_PER_SET; i++){
				propList.addElement(mapSet[i].name);
			}
			propList.repaint();
			break;

		case ERROR:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage(F("Error on transfer!"));
			break;

		case TIMEOUT:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage(F("Transfer timed out!"));
			break;

		case WRONG_ACK:
			currentState = NOTHING_LOADED;
			statusMsg.setMessage(F("Wrong ack!"));
			break;

	}

}


//Set
//OK
CanStreamResult MapsFormClass::setMotorData(){
	Motor motor;
	//Load struct from cfg
	statusMsg.setMessage(F("Transfer in progress..."));
	motorCfg.toStruct((byte*)&motor, MOTOR_TYPES, propList.getCurrentElement() * Motor::ATTR_COUNT);
	motor.defaultMap -= '0';	//Convert char to int by Magneto
	//Stream
	return canInterface.streamOverCan(CanID::DRIVER_SETTINGS_CMD, SET_MOT_DATA_CMD, (byte*)&motor, sizeof(Motor));

}
//OK
CanStreamResult MapsFormClass::setMapData(){
	Configuration maps;
	//Load selected mapset
	statusMsg.setMessage(F("Transfer in progress..."));
	maps.loadFromProperty(mapCfg.getProperty(propList.getCurrentElement()));

	if (maps.getPropertyCount() / MotorMap::ATTR_COUNT == MAPS_PER_SET){
		//Load struct from cfg
		for (int i = 0; i < MAPS_PER_SET; i++){
			maps.toStruct((byte*)&mapSet[i], MAPS_TYPES, i * MotorMap::ATTR_COUNT);
		}

		//Stream
		return canInterface.streamOverCan(CanID::DRIVER_SETTINGS_CMD, SET_MAP_DATA_CMD, (byte*)mapSet, sizeof(MotorMap)* MAPS_PER_SET);
	}

	return ABORT;
}


//Event
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

		case GETTING_MAPSET_STATE:
			if (workingList == &propList){
				loadGetMapSetValues();
				workingList->repaint();
			}
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

		case GETTING_MAPSET_STATE:
			//If showing something in detailList go back else do nothing
			if (workingList == &detailList){
				workingList->clear();
				workingList = &propList;
			}
			break;
	}
	
}
//OK
void MapsFormClass::onOkButtonPressed(Genie& genie){
	CanStreamResult sResult;

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
			statusMsg.setMessage(F("Successful transfer!"));
			break;

		case ERROR:
			statusMsg.setMessage(F("Error on transfer!"));
			break;

		case TIMEOUT:
			statusMsg.setMessage(F("Transfer timed out!"));
			break;

		case ABORT:
			statusMsg.setMessage(F("Map number not match!"));
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
//OK
void MapsFormClass::loadGetMapSetValues(){
	int mapIndex = propList.getCurrentElement();
	String e;
	//Load current map prop's values
	for (int i = MotorMap::Name; i < MotorMap::ATTR_COUNT; i++){
		e.remove(0, e.length());
		switch (i){
			case MotorMap::Name:
				e += F("NAME = ");
				e += mapSet[mapIndex].name;
				//LOGLN(mapSet[mapIndex].name);
				break;

			case MotorMap::A0:
				e += F("A0 = ");
				e += mapSet[mapIndex].a0;
				//LOGLN(mapSet[mapIndex].a0);
				break;

			case MotorMap::A1:
				e += F("A1 = ");
				e += mapSet[mapIndex].a1;
				//LOGLN(mapSet[mapIndex].a1);
				break;

			case MotorMap::A2:
				e += F("A2 = ");
				e += mapSet[mapIndex].a2;
				//LOGLN(mapSet[mapIndex].a2);
				break;

			case MotorMap::SyncTrh:
				e += F("SYNC_ITRH = ");
				e += mapSet[mapIndex].syncTrh;
				//LOGLN(mapSet[mapIndex].syncTrh);
				break;

			case MotorMap::FlatLev:
				e += F("FLAT_LEV = ");
				e += mapSet[mapIndex].flatLev;
				//LOGLN(mapSet[mapIndex].flatLev);
				break;

			case MotorMap::MaxSpeed:
				e += F("MAX_SPEED = ");
				e += mapSet[mapIndex].flatLev;
				//LOGLN(mapSet[mapIndex].maxSpeed);
				break;

			case MotorMap::FlatOut:
				e += F("FLAT_OUT = ");
				e += mapSet[mapIndex].flatOut;
				//LOGLN((int)mapSet[mapIndex].flatOut);
				break;

			case MotorMap::UseSyncRect:
				e += F("SYNC_RECT = ");
				e += mapSet[mapIndex].useSyncRect;
				//LOGLN((int)mapSet[mapIndex].useSyncRect);
				break;

			case MotorMap::UseSyncSafe:
				e += F("SYNC_SAFE = ");
				e += mapSet[mapIndex].useSyncSafe;
				//LOGLN((int)mapSet[mapIndex].useSyncSafe);
				break;

			case MotorMap::UseEnergyRecovery:
				e += F("EN_RECOVERY = ");
				e += mapSet[mapIndex].useEnergyRecovery;
				//LOGLN((int)mapSet[mapIndex].useEnergyRecovery);
				break;
			
		}
		detailList.addElement(e);

	}

	//Change working list
	workingList = &detailList;
}


MapsFormClass mapsForm;

