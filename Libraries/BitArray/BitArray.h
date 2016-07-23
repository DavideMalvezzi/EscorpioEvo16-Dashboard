// BitArray.h

#ifndef _BITARRAY_h
#define _BITARRAY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Utils.h"

class BitArray {

public:
	BitArray();
	BitArray(int size);
	~BitArray();

	void clearBit(int index);
	void fill(bool value);
	bool getBit(int index);
	int getSize();
	void resize(int size);
	void setBit(int index);
	String toString();

private:
	byte* bits;
	int size;

};

#endif

