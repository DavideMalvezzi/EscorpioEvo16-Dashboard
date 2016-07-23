
#include <Timer.h>
#include <Vector.h>
#include <Configuration.h>
#include <SPI.h>
#include <SD.h>
#include <Utils.h>

void setup(){
	INIT_SERIAL(Serial, 9600);
	INIT_SD(SD, 28);

	//Configuration cfg;
	//cfg.loadFromFile("MOTOR.CFG");
	//cfg.debug();
	//LOGLN("________________");
	Configuration cfg2;
	cfg2.loadFromFile("MAPSET.CFG");
	cfg2.debug();
	
	
	LOGLN("________________");
	Configuration cfg3;
	cfg3.loadFromProperty(cfg2.getProperty(0));
	cfg3.debug();
	

}

void loop(){

  /* add main program code here */

}
