// Configuration.h
/*
This is due the compability with the majority of the text editor.
The Configuration.h library is created to provide a simply and usefull interface to parse and create 
key-value text file to store program's configuration. 
The file first line must contain the key-value record as follow -> PROPS=<number of props in the file>.
This is used to know how much space is needed to parse the file. 
REMEMBER to modify this number if properties in the file are added/removed.
After the first line each line can contain a KEY=VALUE string that define a property.
In the parsing each line is trimmed (white space and tabulation removed) as well as blank line.
The comment are identified by the # character at the sentence's beginning and the comments could be on 
a single line or on the same line of the property.
It possible to create a sub-configuration in a single property. It's usefull to create a set of properties 

Example:
PROPS=2
SAMPLE=12 #random property

SAMPLESET={
	#random property2
	PROP1=VALUE1
	PROP2=VALUE2	#random property

	#tabulation, white space, blank line and this comment are removed during the parsing
}

A set is created defining a property folloed by { } with inside the KEY=VALUE properties.
Each set of properties count as ONLY ONE property in the total count. See the example above.
It's recommended to not create a set in an another set (for now) because the actual behavior has not been tested yet.

To load a cfg file from SD use loadFromFile method.
After the parsing each property is accessible with the getProperty method or [] operator using as parameter the 
property index. Property 0 is the first after the PROPS line and so on. This method return a Property object containing
the conversion methods and the property KEY name.
At first a properties set is load as a normal property. To get the set's properties it's needed to create another Configuration
instance and using the loadFromProperty method passing as parameter the Property containing the set.
This new instance is used to access to each set's property as a normal Configuration instance.

To modify a loaded cfg file use the setValue methods and to save the file on the SD use the saveChanges method.
It's NOT possible, for now, to modify a property inside a set.
*/

#ifndef _CONFIGURATION_h
#define _CONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define PROPS_COUNT			"PROPS"
#define BACKUP_FILE_FORMAT	"BAK"

#define COMMENT_CHAR	'#'
#define VALUE_CHAR		'='
#define TAB_CHAR		'\t'
#define ENDLINE_CHAR	'\n'
#define CARRIAGE_CHAR	'\r'
#define BEGIN_ARR_CHAR	'{'
#define END_ARR_CHAR	'}'
#define SPACE_CHAR		' '

#define TYPE_BOOL		'b'
#define TYPE_CHAR		'c'
#define TYPE_INT		'i'
#define TYPE_FLOAT		'f'
#define TYPE_CHAR_ARR	's'

#include <SPI.h>
#include <SD.h>
#include <Utils.h>
#include <Vector.h>

class Configuration;

enum ParseResult : byte{
	FILE_NOT_FOUND,
	FILE_NOT_OPENED,
	FILE_EMPTY,
	FILE_INVALID,
	PROP_NUM_MISMATCH,
	FILE_VALID,

	END_OF_FILE,
	VALID_PROP,
	INVALID_PROP
};

class Property{
friend class Configuration;

public:
	//Get the property value's name
	String getName(){ return name; }
	//Conversion to boolean
	boolean asBoolean(){ return value.charAt(0); }
	//Conversion to char
	char asChar(){ return value.charAt(0); }
	//Conversion to int
	int asInt(){ return value.toInt(); }
	//Conversion to float
	float asFloat(){ return value.toFloat(); }
	//Conversion to string
	String asString(){ return value; }

private:
	String name, value;

};

class Configuration {
	
public:

	Configuration();
	//Load from a file on SD the cfg file and parse all the properties
	ParseResult loadFromFile(const char* fileName);
	//Load from a file on SD the cfg file and parse all the properties
	ParseResult loadFromBackup();
	//Load from a property another set of properties
	//Used to parse an array of "objects"
	ParseResult loadFromProperty(Property& prop);
	//Save on file the modified cfg file
	void saveChanges();

	//Save on instance the properties to struct conversion
	//Each struct value is formatted with the types string
	//Conversion available are:
	//	- b as boolean
	//	- c as char
	//	- i as int
	//	- f as float
	//	- s[size] as const char string (need to put the max number of characters the string can contains instead of [size])
	//Example is "bcfbs10" which is a struct with 1 boolean, 1 char, 1 float, 1 boolean and max 10 char string
	//The order of parameter in the types string need to be exactly the same as the order of the struct's parameters
	//The conversion start from the propIndex properties
	void toStruct(byte* instance, String types, int propIndex = 0);

	//Get the property at index
	Property& getProperty(int index){ return props[index]; }
	//Get the property at index array-likely
	Property& operator[](int index){ return props[index]; }

	//Set property value at index at index property
	void setValue(unsigned int index, const char *value){ props[index].value = String(value); }
	//Set property value at index at index property
	void setValue(unsigned int index, const String &value){ props[index].value = value; }

	//Set property value at index
	void setValue(unsigned int index, char value){ props[index].value = String(value); }
	//Set property value at index
	void setValue(unsigned int index, unsigned char value, unsigned char base = 10){ props[index].value = String(value, base); }
	//Set property value at index
	void setValue(unsigned int index, int value, unsigned char base = 10){ props[index].value = String(value, base); }
	//Set property value at index
	void setValue(unsigned int index, unsigned int value, unsigned char base = 10){ props[index].value = String(value, base); }
	//Set property value at index
	void setValue(unsigned int index, long value, unsigned char base = 10){ props[index].value = String(value, base); }
	//Set property value at index
	void setValue(unsigned int index, unsigned long value, unsigned char base = 10){ props[index].value = String(value, base); }
	//Set property value at index
	void setValue(unsigned int index, float value, unsigned char decimalPlaces = 2){ props[index].value = String(value, decimalPlaces); }
	//Set property value at index
	void setValue(unsigned int index, double value, unsigned char decimalPlaces = 2){ props[index].value = String(value, decimalPlaces); }


	//Get error message
	String getErrorMsg(){ return errMsg; }
	//Get count
	int getPropertyCount(){ return props.getCapacity(); }
	//Debug on serial the parsed properties
	void debug();

private:
	boolean loadedFromBackup;
	char configFile[32];
	String errMsg;
	Vector<Property> props;

	void parseNextConfig(String& cfgLine, String& name, String& value);
	ParseResult parseNextConfig(File& cfgFile, String& name, String& value);
	void clearString(String& value);
	void clearArray(String& value);
	void removeChar(String& string, char c, boolean removeSingleChar);
	
	void updateCfg();
	void updateBackUp();
	void printOnFile(File& f);
};


#endif

