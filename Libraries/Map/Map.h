// Map.h

#ifndef _MAP_h
#define _MAP_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Vector.h"

template <typename K, typename V>
class Map {

public:
	Map<K, V>(){}

	Map<K, V>(int capacity){
		keys.resize(capacity);
		values.resize(capacity);
	}

	void clear(){
		keys.clear();
		values.clear();
	}

	boolean contains(K key){
		return keys.indexOf(key) != -1;
	}

	void debug(){
		LOG("Map");
		for (int i = 0; i < keys.getSize(); i++){
			Serial.print(keys[i]); Serial.print(" = "); Serial.println(values[i]);
		}
		Serial.println();
	}

	void insert(K key, V value){
		keys.append(key);
		values.append(value);
	}

	void remove(K key){
		int index = keys.indexOf(key);
		keys.remove(index);
		values.remove(index);
	}

	void resize(int capacity){
		keys.resize(capacity);
		values.resize(capacity);
	}

	V value(K key){
		int index = keys.indexOf(key);
		return values[index];
	}

	Vector<K> getKeys(){
		return keys;
	}

	Vector<V> getValues(){
		return values;
	}

	int getSize(){
		return keys.getSize();
	}

	int getCapacity(){
		return keys.getCapacity();
	}

	V& operator[](K key){
		int index = keys.indexOf(key);
		return values[index];
	}

protected:
	Vector<K> keys;
	Vector<V> values;
};

#endif

