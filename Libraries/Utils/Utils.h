// Utils.h
/*
Macro and utils method
*/

#ifndef _UTILS_h
#define _UTILS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ARDUINO_DEBUG 1

//Shell-like system("pause")
//Pauses Arduino until a character is not send on the serial port
#ifdef ARDUINO_DEBUG 
	#define SYSTEM_PAUSE() \
		Serial.println("Press a key to continue..."); \
		while(!Serial.available()); \
		Serial.read(); 
#else
	#define SYSTEM_PAUSE() ;
#endif

//If the condition is false print the error message and block the arduino
#ifdef ARDUINO_DEBUG
	#define ASSERT(cond, msg) \
			if (!(cond)) {\
				Serial.print(F("Assert failed: ")); Serial.println(msg);             \
				while (1); \
			} 
#else
	#define ASSERT(cond, msg) ;
#endif

//Abbreviation to print on serial
#ifdef ARDUINO_DEBUG 
	#define LOGLN(msg) Serial.println(msg);
#else
	#define LOGLN(msg) ;
#endif

//Abbreviation to print on serial
#ifdef ARDUINO_DEBUG 
	#define LOG(msg) Serial.print(msg);
#else
	#define LOG(msg) ;
#endif

//Print and array in DEC, HEX, OCT or BIN
#ifdef ARDUINO_DEBUG
	#define LOG_ARR(arr, size, mode) \
	for (int i = 0; i < size; i++){ \
		Serial.print(*(arr + i), mode); \
		Serial.print(" "); \
	} \
	Serial.println();
#else
	#define LOG_ARR(arr, size, mode) ;
#endif

//Macro to quickly init a serial port
#define INIT_SERIAL(_Serial_, _baud_) \
	_Serial_.begin(_baud_); \
	while (!_Serial_);

//Macro to quickly init the SD
#define INIT_SD(_SD_, _CS_) \
	pinMode(_CS_, OUTPUT); \
	_SD_.begin(_CS_)

//Type definition
typedef unsigned char byte;
typedef bool boolean;

inline byte getAck(byte* data, int size){
	byte ack = 0;
	for (int i = 0; i < size; i++){
		ack ^= data[i];
	}
	return ack;
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

//Free memory method
inline int freeMemory() {
	char top;
#ifdef __arm__
	return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	
	return &top - __brkval;

#else  // __arm__
	return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

#endif

