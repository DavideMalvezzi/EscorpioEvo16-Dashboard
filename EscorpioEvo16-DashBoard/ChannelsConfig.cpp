#include "ChannelsConfig.h"
#include "ConsoleForm.h"

void ChannelsConfigClass::init(){
	Configuration cfg;
	Channel* c;
	if (cfg.initWithFile(CHANNELS_CFG_FILE)){
		channels.resize(cfg.getPropertiesCount() / CHANNEL);
		for (int i = 0; i < channels.getCapacity(); i++){
			c = new Channel;
			c->ID = cfg[CHANNEL * i + CH_ID].toInt();
			c->name = cfg[CHANNEL * i + CH_NAME];
			c->size = cfg[CHANNEL * i + CH_SIZE].toInt();
			c->type = cfg[CHANNEL * i + CH_TYPE].charAt(0);
			channels.append(c);
		}
	}
	else{
		consoleForm.println("Channels configuration file not found!");
		ASSERT(false, "Channels configuration file not found!");
	}

}

void ChannelsConfigClass::debug(){
	Channel* c;

	LOGLN("========== Channels loaded config: ==========");
	LOG("Channels: "); LOGLN(channels.getSize());
	for (int i = 0; i < channels.getSize(); i++){
		c = channels[i];
		LOG(c->ID); LOG("  "); LOG(c->name); LOG("  "); LOG((char)c->type); LOG("  "); LOGLN((int)c->size);
	}
	LOGLN("========================================");
}

Channel* ChannelsConfigClass::getChannelByID(unsigned short id){
	return getChannelByIndex(getChannelIndex(id));
}

int ChannelsConfigClass::getChannelIndex(unsigned short id){
	int s = 0, d = channels.getSize() - 1;
	int p;
	
	while (s <= d){
		p = (s + d) / 2;

		if (channels[p]->ID == id){
			return p;
		}
		else if (id > channels[p]->ID){
			s = p + 1;
		}
		else{
			d = p - 1;
		}
	}

	return -1;
}


Channel* ChannelsConfigClass::getChannelByIndex(int index){
	if (index != -1){
		return channels[index];
	}
	return NULL;
}

ChannelsConfigClass channelsConfig;
