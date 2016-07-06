
#include "Configuration.h"
#include <Timer.h>

Configuration::Configuration(){
	loadedFromBackup = false;
}


//Load
ParseResult Configuration::loadFromFile(const char* fileName){
	int propsCount;
	File cfgFile;
	String name, value;
	
	strcpy(configFile, fileName);


	//File exists
	if (SD.exists(configFile)){
		cfgFile = SD.open(configFile, O_READ);
		cfgFile.setTimeout(0);	//Reduce parsing time drastically
		//File opened
		if(cfgFile){
			//File not empty
			if(cfgFile.available()){
				//Read props count
				if(parseNextConfig(cfgFile, name, value) == VALID_PROP){
					//If first prop is PROP_COUNT
					if(name.equals(PROPS_COUNT)){
						propsCount = value.toInt();
						props.resize(propsCount);
				
						//Parse props
						for (int i = 0; i < propsCount; i++){
							if(parseNextConfig(cfgFile, props[i].name, props[i].value) != VALID_PROP){
								cfgFile.close();
								errMsg = String(configFile) + F(" properties count doesn't match with PROPS value");
								return PROP_NUM_MISMATCH;
							}
						}

						cfgFile.close();
						return FILE_VALID;
					}
				}
				cfgFile.close();
				errMsg = String(configFile) + F(" doesn't contain PROPS count");
				return FILE_INVALID;
			}
			cfgFile.close();
			errMsg = String(configFile) + F(" is empty");
			return FILE_EMPTY;
		}
		errMsg = String(configFile) + F(" error on opening");
		return FILE_NOT_OPENED;
	}
	errMsg = String(configFile) + F(" file not found");
	return FILE_NOT_FOUND;
}

ParseResult Configuration::loadFromProperty(Property& prop){
	String value = prop.value, temp;
	int index, prevIndex, propsCount = 0;

	index = value.indexOf(ENDLINE_CHAR);
	while (index != -1){
		propsCount++;
		index = value.indexOf(ENDLINE_CHAR, index + 1);
	}

	if (propsCount > 0){

		props.resize(propsCount);

		prevIndex = 0;
		for (int i = 0; i < propsCount; i++){
			index = value.indexOf(ENDLINE_CHAR, prevIndex);
			temp = value.substring(prevIndex, index);

			parseNextConfig(temp, props[i].name, props[i].value);
			prevIndex = index + 1;
		}

		return FILE_VALID;
	}

	errMsg = F("found 0 properties");
	return FILE_INVALID;
}

ParseResult Configuration::loadFromBackup(){
	String original;
	String backUpPath;

	original += configFile;
	backUpPath+= configFile;

	backUpPath.remove(backUpPath.indexOf('.') + 1);
	backUpPath.concat(BACKUP_FILE_FORMAT);
	
	ParseResult res = loadFromFile((char*)backUpPath.c_str());
	if(res == FILE_VALID){
		loadedFromBackup = true;
	}
	
	strcpy(configFile, (char*)original.c_str());	//Set the original file path
	
	return res;
}


//Save
void Configuration::saveChanges(){
	if(loadedFromBackup){
		updateCfg();
		updateBackUp();
	}
	else{
		updateBackUp();
		updateCfg();
	}
}

void Configuration::printOnFile(File& cfg){
	cfg.print(PROPS_COUNT);
	cfg.print(VALUE_CHAR);
	cfg.println(props.getCapacity());

	for (int i = 0; i < props.getCapacity(); i++){
		cfg.print(props[i].name);
		cfg.print(VALUE_CHAR);
		cfg.println(props[i].value);
	}
}

void Configuration::updateBackUp(){
	File cfg;
	
	String backUpPath = configFile;
	backUpPath.remove(backUpPath.indexOf('.') + 1);
	backUpPath.concat(BACKUP_FILE_FORMAT);

	SD.remove((char*)backUpPath.c_str());	//Remove old backup if present
	cfg = SD.open((char*)backUpPath.c_str(), O_WRITE | O_CREAT);
	printOnFile(cfg);
	cfg.close();
}

void Configuration::updateCfg(){
	File cfg;

	SD.remove(configFile);	//Remove old backup if present
	cfg = SD.open(configFile, O_WRITE | O_CREAT);
	printOnFile(cfg);
	cfg.close();
}


//Clear
ParseResult Configuration::parseNextConfig(File& cfgFile, String& name, String& value){
	int index = 0;
	
	value = cfgFile.readStringUntil(ENDLINE_CHAR);

	clearString(value);
		
	//If blank line skip
	if (value.length() == 0){
		if(cfgFile.available() > 0){
			//LOGLN("FOUND_BLANK");
			return parseNextConfig(cfgFile, name, value);
		}
		else{
			return END_OF_FILE;
		}
	}

	/*
	//If comment skip
	if (value.charAt(0) == COMMENT_CHAR){
		//LOGLN("FOUND_COMMENT");
		return parseNextConfig(cfgFile, name, value);
	}
	*/
	
	//Search for name=value
	index = value.indexOf(VALUE_CHAR);
	name = value.substring(0, index);
	name.trim();

	//if found { is an array
	if (value.charAt(index + 1) == BEGIN_ARR_CHAR){
		//LOGLN("FOUND_ARRAY");
		value = cfgFile.readStringUntil(END_ARR_CHAR);
		clearArray(value);
	}
	else{
		//LOGLN("FOUND_VALUE");
		value = value.substring(index + 1);
		value.trim();
	}
	
	return VALID_PROP;
}

void Configuration::parseNextConfig(String& cfgLine, String& name, String& value){
	int index;

	index = cfgLine.indexOf(VALUE_CHAR);
	name = cfgLine.substring(0, index);
	value = cfgLine.substring(index + 1);

	name.trim();
	value.trim();
}

void Configuration::removeChar(String& string, char c, boolean removeSingleChar){
	int index;

	index = string.indexOf(c);
	while (index != -1){
		if (removeSingleChar){
			string.remove(index, 1);	//Remove only the found char
		}
		else{
			string.remove(index);	//Remove until the end of the string
		}
		index = string.indexOf(c, index);
	}
}

void Configuration::clearString(String& value){
	removeChar(value, COMMENT_CHAR, false);
	removeChar(value, SPACE_CHAR, true);
	removeChar(value, TAB_CHAR, true);
	removeChar(value, ENDLINE_CHAR, true);
	removeChar(value, CARRIAGE_CHAR, true);
	value.trim();	
}

void Configuration::clearArray(String& value){
	int index;

	removeChar(value, SPACE_CHAR, true);
	removeChar(value, TAB_CHAR, true);
	removeChar(value, CARRIAGE_CHAR, true);

	
	index = value.indexOf(COMMENT_CHAR);
	while(index != -1){
		while (value.charAt(index) != ENDLINE_CHAR){
			value.remove(index, 1);
		}
		index = value.indexOf(COMMENT_CHAR, index);
	}

	
	index = value.indexOf("\n\n");
	while(index != -1){
		value.remove(index, 1);
		index = value.indexOf("\n\n", index);
	}
}


//Debug
void Configuration::debug(){
	for (int i = 0; i < props.getCapacity(); i++){
		LOG(props[i].name); LOG("  "); LOGLN(props[i].value);
	}
}


//Get
void Configuration::toStruct(byte* instance, String types, int propIndex){
		byte* memIndex = instance;
		int d;
		float f;
		String arrSize;
		for (int i = 0; i < types.length(); i++){
			switch (types.charAt(i)){
				case TYPE_BOOL:
					*memIndex = getProperty(propIndex).asBoolean();
					memIndex += sizeof(boolean);
					break;

				case TYPE_CHAR:
					*memIndex = getProperty(propIndex).asChar();
					memIndex += sizeof(char);
					break;

				case TYPE_INT:
					d = getProperty(propIndex).asInt();
					memcpy(memIndex, &d, sizeof(int));
					memIndex += sizeof(int);
					break;

				case TYPE_FLOAT:
					f = getProperty(propIndex).asFloat();
					memcpy(memIndex, &f, sizeof(float));
					memIndex += sizeof(float);
					break;

				case TYPE_CHAR_ARR:
					d = 1;
					while (types.charAt(i + d) >= '0' && types.charAt(i + d) <= '9'){
						arrSize.concat(types.charAt(i + d));
						d++;
					}
					
					memcpy(memIndex, getProperty(propIndex).asString().c_str(), arrSize.toInt());
					memIndex += arrSize.toInt();
					i += d - 1;
					break;					
			}
			propIndex++;
		}
	}