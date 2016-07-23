
#include "Logger.h"


void LoggerClass::init(UARTClass* serialPort){
	this->serialPort = serialPort;
	this->mode = Dec;
	this->precision = 6;
}

void LoggerClass::setDecimalPrecision(int precision){
	this->precision = precision;
}

void LoggerClass::assert(boolean condition, String msg){
	if (!condition){
		 this->e(ASSERT_TAG) << msg << Endl;
		 serialPort->flush();
		 while (1);
	}
}


LoggerClass& LoggerClass::i(const char* tag){
	(*this) << Info << tag << " ";
	return (*this);
}

LoggerClass& LoggerClass::w(const char* tag){
	(*this) << Warning << tag << " ";
	return (*this);
}

LoggerClass& LoggerClass::e(const char* tag){
	(*this) << Error << tag << " ";
	return (*this);
}

LoggerClass& LoggerClass::i(const __FlashStringHelper* tag){
	(*this) << Info << tag << " ";
	return (*this);
}

LoggerClass& LoggerClass::w(const __FlashStringHelper* tag){
	(*this) << Warning << tag << " ";
	return (*this);
}

LoggerClass& LoggerClass::e(const __FlashStringHelper* tag){
	(*this) << Error << tag << " ";
	return (*this);
}


LoggerClass& LoggerClass::repeat(char c, int times){
	for (int i = 0; i < times; i++){
		Log << c;
	}
	return (*this);
}


//Print
LoggerClass& operator<<(LoggerClass& log, byte value){
	log.serialPort->print(value, log.mode);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, char value){
	log.serialPort->print(value);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, int value){
	log.serialPort->print(value, log.mode);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, unsigned int value){
	log.serialPort->print(value, log.mode);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, long value){
	log.serialPort->print(value, log.mode);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, unsigned long value){
	log.serialPort->print(value, log.mode);
	return log;
}


LoggerClass& operator<<(LoggerClass& log, float value){
	log.serialPort->print(value, log.precision);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, double value){
	log.serialPort->print(value, log.precision);
	return log;
}


LoggerClass& operator<<(LoggerClass& log, const char* value){
	log.serialPort->print(value);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, String value){
	log.serialPort->print(value);
	return log;
}

LoggerClass& operator<<(LoggerClass& log, const __FlashStringHelper* value){
	log.serialPort->print(value);
	return log;
}


LoggerClass& operator<<(LoggerClass& log, LogManip value){
	switch (value){
		case Endl:
			log.mode = Dec;
			log.serialPort->println();
			log.serialPort->flush();
			break;

		case Bin:
		case Oct:
		case Dec:
		case Hex:
			log.mode = value;
			break;

		default:
			log.serialPort->print((char)value);
			break;
	}
	return log;
}

LoggerClass& operator<<(LoggerClass& log, LoggerClass& value){
	return log;
}


LoggerClass& operator<<(LoggerClass& log, ByteBuffer& b){
	LogManip prevLogMode = log.mode;

	Log << Hex << Log.array<byte>(b.data(), b.getSize()) << prevLogMode;
	return log;
}

LoggerClass Log;

