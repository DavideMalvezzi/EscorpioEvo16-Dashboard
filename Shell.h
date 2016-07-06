// Shell.h

#ifndef _SHELL_h
#define _SHELL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ByteBuffer.h>
#include <ChannelsBuffer.h>
#include <ChannelsConfig.h>
#include <Logger.h>
#include <SD.h>

#define SHELL_TAG	F("SH")

#define SHELL_RX_BUFFER	32

class ShellClass;

struct ShellCommand{
	char cmdString[10];
	void(ShellClass::*cmdFun)(String&);
};

extern const PROGMEM ShellCommand cmdsList[];

#define CMDS_NUM	(sizeof(cmdsList) / sizeof(ShellCommand))


class ShellClass {

public:
	void init(Stream* serialPort);
	void update();

	//Cmds function
	void analogReadCmd(String&);
	void analogWriteCmd(String&);

	void channelListCmd(String&);
	void channelValueCmd(String&);
	void channelsValuesCmd(String&);

	void digitalReadCmd(String&);
	void digitalWriteCmd(String&);

	void SDMkDirCmd(String&);
	void SDOpenCmd(String&);
	void SDRmCmd(String&);
	void SDRmDirCmd(String&);
	void SDTreeCmd(String&);


private:
	ByteBuffer rxBuffer;
	Stream* serialPort;

	int findCmd(String& line, ShellCommand* cmd);
	String nextParam(String& params);

	void printSDTree(File dir, int indent);

};

extern ShellClass shell;

#endif

