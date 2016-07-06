/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Arduino Due (Programming Port), Platform=sam, Package=arduino
*/

#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
#define printf iprintf
#define F_CPU 84000000L
#define ARDUINO 10608
#define ARDUINO_SAM_DUE
#define ARDUINO_ARCH_SAM
#define __SAM3X8E__
#define USB_VID 0x2341
#define USB_PID 0x003e
#define USBCON
#define __cplusplus
#define __ARM__
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __volatile__
#define __SIZE_TYPE__ long

#define __ICCARM__
#define __ASM
#define __INLINE
#define __builtin_va_list void
//#define _GNU_SOURCE 
//#define __GNUC__ 0
//#undef  __ICCARM__
//#define __GNU__

typedef long Pio;
typedef long Efc;
typedef long Adc;
typedef long Pwm;
typedef long Rtc;
typedef long Rtt;
typedef long pRtc;
typedef long Spi;
typedef long spi;
typedef long Ssc;
//typedef long p_scc;
typedef long Tc;
//typedef long pTc;
typedef long Twi;
typedef long Wdt;
//typedef long pTwi;
typedef long Usart;
typedef long Pdc;
typedef long Rstc;

extern const int ADC_MR_TRGEN_DIS = 0;
extern const int ADC_MR_TRGSEL_ADC_TRIG0 = 0;
extern const int ADC_MR_TRGSEL_Pos = 0;

extern const int ADC_MR_TRGSEL_Msk = 0;
extern const int ADC_MR_TRGEN = 0;
extern const int ADC_MR_TRGSEL_ADC_TRIG1 = 0;
extern const int ADC_MR_TRGSEL_ADC_TRIG2 = 0;
extern const int ADC_MR_TRGSEL_ADC_TRIG3 = 0;



#define __ARMCC_VERSION 400678
#define __attribute__(noinline)

#define prog_void
#define PGM_VOID_P int


typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}



#include <arduino.h>
#include <pins_arduino.h> 
#include <variant.h> 
#undef F
#define F(string_literal) ((const PROGMEM char *)(string_literal))
#undef PSTR
#define PSTR(string_literal) ((const PROGMEM char *)(string_literal))
#undef cli
#define cli()
#define pgm_read_byte(address_short)
#define pgm_read_word(address_short)
#define pgm_read_word2(address_short)
#define digitalPinToPort(P)
#define digitalPinToBitMask(P) 
#define digitalPinToTimer(P)
#define analogInPinToBit(P)
#define portOutputRegister(P)
#define portInputRegister(P)
#define portModeRegister(P)
#include <..\REV16\DashBoard.ino>
#include <..\REV16\BMSInterface.cpp>
#include <..\REV16\BMSInterface.h>
#include <..\REV16\CanInterface.cpp>
#include <..\REV16\CanInterface.h>
#include <..\REV16\Channel.cpp>
#include <..\REV16\Channel.h>
#include <..\REV16\ChannelsBuffer.cpp>
#include <..\REV16\ChannelsBuffer.h>
#include <..\REV16\ChannelsConfig.cpp>
#include <..\REV16\ChannelsConfig.h>
#include <..\REV16\ConsoleForm.cpp>
#include <..\REV16\ConsoleForm.h>
#include <..\REV16\DataLogger.cpp>
#include <..\REV16\DataLogger.h>
#include <..\REV16\DebugForm.cpp>
#include <..\REV16\DebugForm.h>
#include <..\REV16\DisplayInterface.cpp>
#include <..\REV16\DisplayInterface.h>
#include <..\REV16\EarthDistance.cpp>
#include <..\REV16\EarthDistance.h>
#include <..\REV16\GpsInterface.cpp>
#include <..\REV16\GpsInterface.h>
#include <..\REV16\HWConfig.h>
#include <..\REV16\Interprete.cpp>
#include <..\REV16\Interprete.h>
#include <..\REV16\LCDForm.h>
#include <..\REV16\LCDStringList.cpp>
#include <..\REV16\LCDStringList.h>
#include <..\REV16\LCDStringMsg.cpp>
#include <..\REV16\LCDStringMsg.h>
#include <..\REV16\Logger.cpp>
#include <..\REV16\Logger.h>
#include <..\REV16\MainForm.cpp>
#include <..\REV16\MainForm.h>
#include <..\REV16\MapSelector.cpp>
#include <..\REV16\MapSelector.h>
#include <..\REV16\MapsForm.cpp>
#include <..\REV16\MapsForm.h>
#include <..\REV16\PhoneInterface.cpp>
#include <..\REV16\PhoneInterface.h>
#include <..\REV16\Shell.cpp>
#include <..\REV16\Shell.h>
#include <..\REV16\SteerSensor.cpp>
#include <..\REV16\SteerSensor.h>
#include <..\REV16\StrategySettings.cpp>
#include <..\REV16\StrategySettings.h>
#include <..\REV16\TelemetryInterface.cpp>
#include <..\REV16\TelemetryInterface.h>
#include <..\REV16\WayPoint.cpp>
#include <..\REV16\WayPoint.h>
#include <..\REV16\WdtUtils.h>
#include <..\REV16\WheelSensor.cpp>
#include <..\REV16\WheelSensor.h>
#endif
