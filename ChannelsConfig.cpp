
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
			c->setID(cfg[i + Channel::CanID].asInt());
			c->setName(cfg[i + Channel::Name].asString());
			c->setSize(cfg[i + Channel::Size].asInt());
			c->setDataType(static_cast<Channel::DataTypes>(cfg[i + Channel::Type].asChar()));

			channels.append(c);
		}

		valid = true;
	}
	else{
		consoleForm.println(cfg.getErrorMsg());
		Log.e(CHBUF_TAG) << cfg.getErrorMsg() << Endl;
		valid = false;
	}

	return valid;
}

void ChannelsConfigClass::debug(){
	
	Channel* c;

	Log << F("========== Channels loaded config: ==========");
	Log << F("Channels: ") << channels.getSize() << Endl;
	for (int i = 0; i < channels.getSize(); i++){
		c = channels[i];
		Log << c->getID() << "  " << c->getName() << "  " << (char)c->getDataType() << "  " << (int)c->getSize() << Endl;
	}
	Log << F("========================================");
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

		if (channels[p]->getID() == id){
			return p;
		}
		else if (id > channels[p]->getID()){
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
