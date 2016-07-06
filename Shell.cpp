
#include "Shell.h"

//MUST be in alphabetical order
const PROGMEM ShellCommand cmdsList[] = {
	//analog pins cmds
	{ "ar",			&ShellClass::analogReadCmd		},	//analogRead							ar	<pin>
	{ "aw",			&ShellClass::analogWriteCmd		},	//analogWrite							aw	<pin>	<value>

	//channels cmds
	{ "chlist",		&ShellClass::channelListCmd		},	//list all loaded channels				chlist
	{ "chvalue",	&ShellClass::channelValueCmd	},	//list a specific channel last value	chvalue <id>
	{ "chvalues",	&ShellClass::channelsValuesCmd	},	//list all last channel values			chvalues

	//digital pins cmds
	{ "dr",			&ShellClass::digitalReadCmd		},	//digitalRead							dr	<pin>
	{ "dw",			&ShellClass::digitalWriteCmd	},	//digitalWrite							dw	<pin>	<value>

	//SD cmds
	{ "sdmkdir",	&ShellClass::SDMkDirCmd			},	//make a dir							sdmkdir <path>
	{ "sdopen",		&ShellClass::SDOpenCmd			},	//read all file's content				sdopen	<path>
	{ "sdrm",		&ShellClass::SDRmCmd			},	//remove a file							sdrm	<path>
	{ "sdrmdir",	&ShellClass::SDRmDirCmd			},	//remove a dir							sdrmdir	<path>
	{ "sdtree",		&ShellClass::SDTreeCmd			}	//print all files and dirs				sdtree
};


void ShellClass::init(Stream* serialPort){
	this->serialPort = serialPort;
	this->rxBuffer.resize(SHELL_RX_BUFFER);
}

void ShellClass::update(){
	int index;
	String line;
	ShellCommand cmd;

	if (serialPort->available()){

		while (serialPort->available() && rxBuffer.getSize() < rxBuffer.getCapacity()){
			rxBuffer.append(serialPort->read());
		}

		index = rxBuffer.indexOf('\n');
		if (index != -1){
			line = rxBuffer.toString(0, index);
			line.trim();

			if (findCmd(line, &cmd) != -1){
				(this->*cmd.cmdFun)(line);
				//Log.e(SHELL_TAG) << ("Cmd executed") << Endl;
			}
			else{
				Log.e(SHELL_TAG) << F("Command not found") << Endl;
			}

			rxBuffer.clear();
		}

	}
}


//Cmds
//A
void ShellClass::analogReadCmd(String& params){
	int pin = A0 + nextParam(params).toInt();
	uint32_t attr = g_APinDescription[pin].ulPinAttribute;

	if ((attr & PIN_ATTR_ANALOG) != PIN_ATTR_ANALOG){
		Log.e(SHELL_TAG) << F("Pin number A") << pin << F(" is not a valid analog I/O pin for this board") << Endl;
	}
	else{
		pinMode(pin, INPUT);
		Log.i(SHELL_TAG) << F("Readed ") << analogRead(pin) << Endl;
	}
}

void ShellClass::analogWriteCmd(String& params){
	int pin = A0 + nextParam(params).toInt();
	int value = nextParam(params).toInt();

	uint32_t attr = g_APinDescription[pin].ulPinAttribute;

	if ((attr & PIN_ATTR_ANALOG) != PIN_ATTR_ANALOG && (attr & PIN_ATTR_PWM) != PIN_ATTR_PWM){
		Log.e(SHELL_TAG) << F("Pin number A") << pin << F(" is not a valid analog I/O pin for this board") << Endl;
	}
	else{
		pinMode(pin, OUTPUT);
		analogWrite(pin, value);
		Log.i(SHELL_TAG) << F("OK") << Endl;
	}
}

//C
void ShellClass::channelListCmd(String& params){
	Channel* c;

	Log.i(SHELL_TAG) << F("Channel number: ") << channelsConfig.getChannelCount() << Endl;
	Log.i(SHELL_TAG) << F("ID\tName\tSize\tType") << Endl;

	for (int i = 0; i < channelsConfig.getChannelCount(); i++){
		c = channelsConfig.getChannelByIndex(i);
		Log.i(SHELL_TAG) << F("0x") << Hex << c->ID << "\t" << c->name << "\t" << c->size << "\t" << (char)c->type << Endl;
	}
}

void ShellClass::channelValueCmd(String& params){
	int id = nextParam(params).toInt();
	Channel* c = channelsConfig.getChannelByID(id);

	if (c == NULL){
		Log.e(SHELL_TAG) << F("Channel not found") << Endl;
	}
	else{
		Log.i(SHELL_TAG) << c->name << "\t" << channelsBuffer.getValueAsString(id) << "\t" << channelsBuffer.getValueAsByteArray(id).toHexString() << Endl;
	}
}

void ShellClass::channelsValuesCmd(String& params){

}

//D
void ShellClass::digitalReadCmd(String& params){
	int pin = nextParam(params).toInt();

	if (g_APinDescription[pin].ulPinType == PIO_NOT_A_PIN){
		Log.e(SHELL_TAG) << F("Pin number ") << pin << F(" is not a valid digital I/O pin for this board") << Endl;
	}
	else{
		pinMode(pin, INPUT);
		Log.i(SHELL_TAG) << F("Readed ") << digitalRead(pin) << Endl;
	}
}

void ShellClass::digitalWriteCmd(String& params){
	int pin = nextParam(params).toInt();
	int level = nextParam(params).toInt();

	if (g_APinDescription[pin].ulPinType == PIO_NOT_A_PIN){
		Log.e(SHELL_TAG) << F("Pin number ") << pin << F(" is not a valid digital I/O pin for this board") << Endl;
	}
	else{
		pinMode(pin, OUTPUT);
		digitalWrite(pin, level);
		Log.i(SHELL_TAG) << F("OK") << Endl;
	}
}

//S
void ShellClass::SDMkDirCmd(String& params){
	String path = nextParam(params);

	if (SD.mkdir(path)){
		Log.i(SHELL_TAG) << F("OK") << Endl;
	}
	else{
		Log.e(SHELL_TAG) << F("Error creating new directory: is SD inserted?") << Endl;
	}
}

void ShellClass::SDOpenCmd(String& params){
	String path = nextParam(params);

	if (SD.exists(path)){
		File f = SD.open(path);
		f.setTimeout(100);
		if (f){
			while (f.available()){
				Log.i(SHELL_TAG) << f.readStringUntil('\n') << Endl;
			}
			f.close();
		}
		else{
			Log.e(SHELL_TAG) << F("Error opening file") << Endl;
		}
	}
	else{
		Log.e(SHELL_TAG) << F("File not found") << Endl;
	}
}

void ShellClass::SDRmCmd(String& params){
	String path = nextParam(params);

	if (SD.exists(path)){
		if (SD.remove(path)){
			Log.i(SHELL_TAG) << F("OK") << Endl;
		}
		else{
			Log.e(SHELL_TAG) << F("Error deleting file") << Endl;
		}
	}
	else{
		Log.e(SHELL_TAG) << F("File not found") << Endl;
	}
}

void ShellClass::SDRmDirCmd(String& params){
	String path = nextParam(params);

	if (SD.exists(path)){
		if (SD.rmdir(path)){
			Log.i(SHELL_TAG) << F("OK") << Endl;
		}
		else{
			Log.e(SHELL_TAG) << F("Error deleting directory") << Endl;
		}
	}
	else{
		Log.e(SHELL_TAG) << F("Directory not found") << Endl;
	}

}

void ShellClass::SDTreeCmd(String&){
	File root = SD.open("/");
	root.rewindDirectory();

	if (!root){
		Log.e(SHELL_TAG) << F("Error opening SD root: is SD inserted?") << Endl;
	}
	else{
		printSDTree(root, 0);
	}
}

void ShellClass::printSDTree(File dir, int indent){
	File entry = dir.openNextFile();

	while (entry) {

		Log.i(SHELL_TAG) << Log.repeat('\t', indent) << entry.name();

		if (entry.isDirectory()) {
			Log << '/' << Endl;
			printSDTree(entry, indent + 1);
		}
		else {
			Log << '\t' << entry.size() << Endl;
		}

		entry.close();
		entry = dir.openNextFile();
	}
}


//Cmd utils

int ShellClass::findCmd(String& line, ShellCommand* cmd){
	//Binary search channel's index by canID
	int s = 0, d = CMDS_NUM - 1;
	int p, cmp;

	int index = line.indexOf(' ');
	String cmdLine;

	//Remove \n and \r
	line.remove(line.indexOf('\n'), 1);
	line.remove(line.indexOf('\r'), 1);

	//Get the cmd
	cmdLine = line.substring(0, index == -1 ? line.length() : index);

	//Remove cmd string and leave only the params
	line.remove(0, index + 1);


	while (s <= d){
		p = (s + d) / 2;

		//Load cmd from flash
		mempcpy(cmd, &cmdsList[p], sizeof(ShellCommand));
		//Compare loaded cmd string and searched cmd line
		cmp = strcmp(cmd->cmdString, cmdLine.c_str());

		if (cmp == 0){
			//Log << Hex << Log.array<byte>((byte*)line.c_str(), line.length()) << Endl;

			return p;
		}
		else if (cmp < 0){
			s = p + 1;
		}
		else{
			d = p - 1;
		}
	}
	
	cmd = NULL;
	return -1;
}

String ShellClass::nextParam(String& params){
	int index = params.indexOf(' ');
	String p;

	if (index != -1){
		p = params.substring(0, index);
		params.remove(0, index + 1);
		return p;
	}
	//if not found the params string is empty or contains the last param
	return params;
}

ShellClass shell;

