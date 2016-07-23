// Value.h

#ifndef _VALUE_h
#define _VALUE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Value {

public:
	typedef union GenericValue{
		byte b;
		char c;
		short s;
		int i;
		long int li;
		float f;
		void* p;
	}GenericValue;

	Value(){}
	Value(byte b){ this->value.b = b; }
	Value(char c){ this->value.c = c; }
	Value(short s){ this->value.s = s; }
	Value(int i){ this->value.i = i; }
	Value(long int li){ this->value.li = li; }
	Value(float f){ this->value.f = f; }
	Value(void* p){ this->value.p = p; }

	void setByte(byte b){ this->value.b = b; }
	void setChar(char c){ this->value.c = c; }
	void setShort(short s){ this->value.s = s; }
	void setInt(int i){ this->value.i = i; }
	void setLong(long int li){ this->value.li = li; }
	void setFloat(float f){ this->value.f = f; }
	void setPointer(void* p){ this->value.p = p; }

	byte asByte(){ return this->value.b; }
	char asChar(){ return this->value.c; }
	short asShort(){ return this->value.s; }
	int asInt(){ return this->value.i; }
	long int asLong(){ return this->value.li; }
	float asFloat(){ return this->value.f; }
	void* asPointer(){ return this->value.p; }

protected:
	GenericValue value;
};


#endif

