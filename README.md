# EscorpioEvo16 Dashboard
This is the Arduino Due firmware loaded inside the Escoprio Evo 16 DashBoard during the Shell Eco-Marathon 2016 in London.
The hardware is composed by an Arduino Due plus 2 custom made boards placed on the Arduino top. 
The 2 boards have:
 - a [4D system 4.3'' LCD display](http://www.4dsystems.com.au/product/uLCD_43/)
 - a bluetooth module
 - a SD card slot
 - 3 Can Bus port 
 - multiple digital/analog port to connect sensors and buttons

##Modules
### Datalogger
One of the many modules of this firmware is to log on the SD all the data received via Can Bus. 
To make this modules work is needed to place inside the SD card two configurations file:
- DATALOG.CFG
- CHANNELS.CFG

The first one contains the datalogger properties, such as the log period, the log file name and the extension.
If this file is missing then the default configuration is loaded.
The second one contains all the usable Can ID from which the datalogger is expecting to receive data.
For each Can ID is also provided the associated name, packets size and data type, to allow the datalogger to convert the byte packets to a human comprensible value.
In case the second configuration file is missing this module won't work.
The file created by the datalogger software module is a CSV file and can be easily opened with programs such as Excel.

### Telemetry
This module allows to stream the datalogger values to a remote server through internet. This is only possible if the (Online Telemetry App)[https://github.com/DavideMalvezzi/EscorpioEvo16-OnlineTelemetry] is installed on an Android smartphone and placed near the DashBoard.
The telemetry module will take control of the bluetooth module TX channel and send all the datalogger data to the smartphone.

### Phone and GPS
If the DashBoard is connected through bluetooth to the (Online Telemetry App)[https://github.com/DavideMalvezzi/EscorpioEvo16-OnlineTelemetry], it will receive information from the smartphone sensor.
The smartphone data consist of:
- date and time
- gps position
- accelometer 

Also is possible to start a call from the phone simply pressing a button on the DashBoard. The called number is setted in the PHONE.CFG file in the SD card. 
