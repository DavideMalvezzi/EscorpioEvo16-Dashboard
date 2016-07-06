
#include "ChannelsConfig.h"
#include "ConsoleForm.h"

boolean ChannelsConfigClass::init(){
	Channel* c;
	Configuration cfg;
	//Load cfg file
	if (cfg.loadFromFile(CHANNELS_CFG_FILE) == FILE_VALID){
		//Resize channel vector
		channels.resize(cfg.getPropertyCount() / Channel::ATTR_COUNT);
		//Load cfg data
		for (int i = 0; i < cfg.getPropertyCount(); i += Channel::ATTR_COUNT){
			c = new Channel;
			c->ID = cfg[i + Channel::CanID].asInt();
			c->name = cfg[i + Channel::Name].asString();
			c->size = cfg[i + Channel::Size].asInt();
			c->type = cfg[i + Channel::Type].asChar();
			channels.append(c);
		}
	}
	else{
		consoleForm.println(cfg.getErrorMsg());
		Log.assert(false, cfg.getErrorMsg());
	}

}

void ChannelsConfigClass::debug(){
	/*
	Channel* c;

	LOGLN(F("========== Channels loaded config: =========="));
	LOG(F("Channels: ")); LOGLN(channels.getSize());
	for (int i = 0; i < channels.getSize(); i++){
		c = channels[i];
		LOG(c->ID); LOG("  "); LOG(c->name); LOG("  "); LOG((char)c->type); LOG("  "); LOGLN((int)c->size);
	}
	LOGLN(F("========================================"));
	*/
}

Channel* ChannelsConfigClass::getChannelByID(unsigned short id){
	return getChannelByIndex(getChannelIndex(id));
}

int ChannelsConfigClass::getChannelIndex(unsigned short id){
	//Binary search channel's index by canID
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
	if (index != -1 && index < channels.getSize()){
		return channels[index];
	}
	return NULL;
}

ChannelsConfigClass channelsConfig;
