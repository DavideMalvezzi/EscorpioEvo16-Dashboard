// ChannelsConfig.h

#ifndef _CHANNELSCONFIG_h
#define _CHANNELSCONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Configuration.h"
#include "Vector.h"

#define CHANNELS_CFG_FILE	"Channels.cfg"

#define CHANNEL		4
#define CH_ID		0
#define CH_NAME		1
#define CH_SIZE		2
#define CH_TYPE		3

#define BIT_FLAG	'B'
#define	U_INTEGER	'U'
#define INTEGER		'I'
#define DECIMAL		'D'
#define STRING		'S'

class Channel{

public:
	byte size, type;
	unsigned short ID;
	String name;
};

class ChannelsConfigClass {

public:
	void init();
	void debug();

	Channel* getChannelByID(unsigned short id);
	Channel* getChannelByIndex(int index);
	int getChannelIndex(unsigned short id);
	int getChannelCount(){ return channels.getSize(); }

private:
	Vector<Channel*> channels;

};

extern ChannelsConfigClass channelsConfig;


#endif

