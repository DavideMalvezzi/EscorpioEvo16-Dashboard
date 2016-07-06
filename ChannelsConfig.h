// ChannelsConfig.h

#ifndef _CHANNELSCONFIG_h
#define _CHANNELSCONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Channel.h"
#include "Configuration.h"
#include "Vector.h"

#define CHANNELS_CFG_FILE	"CHANNELS.CFG"

class ChannelsConfigClass {

public:
	boolean init();
	void debug();

	Channel* getChannelByID(unsigned short id);
	Channel* getChannelByIndex(int index);
	int getChannelIndex(unsigned short id);
	int getChannelCount(){ return channels.getSize(); }
	boolean isValid(){ return valid; }

private:
	boolean valid;
	Vector<Channel*> channels;

};

extern ChannelsConfigClass channelsConfig;


#endif

