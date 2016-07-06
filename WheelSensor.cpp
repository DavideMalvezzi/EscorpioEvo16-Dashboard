
#include "WheelSensor.h"


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

//Timer event related
void Timer5_Handler ()
{
	// Each ms
	if (wheelSensor.Space != 0)  // Faccio partire il conteggio quando il veicolo si muove
	{
		wheelSensor.TimeMillis++;
		wheelSensor.Energy += wheelSensor.power/1000.0;

		if (millis()>wheelSensor.speedtmr+2000) // Azzero la velocità se sono fermo
			wheelSensor.Speed = 0;
	}
}

volatile boolean l;
volatile uint32_t prevrb=0;

void TC6_Handler()
{
 //reads the interrupt. necessary to clear the interrupt flag.
 const uint32_t status=TC_GetStatus(TC2, 0);
 //has the timer overflowed?
 const bool overflowed=status & TC_SR_COVFS;
 //input capture
 const bool inputcaptureA=status & TC_SR_LDRAS;
 const bool inputcaptureB=status & TC_SR_LDRBS;
 //loading overrun
 const bool loadoverrun=status & TC_SR_LOVRS;
 //read LDRA. If we dont, we will get overflow (TC_SR_LOVRS)
 if(inputcaptureA) {
   const uint32_t ra= TC2->TC_CHANNEL[0].TC_RA;
 }
 //read LDRB. If we dont, we will get overflow (TC_SR_LOVRS)
 if(inputcaptureB) {
   const uint32_t rb = TC2->TC_CHANNEL[0].TC_RB;
   float gap = (float)(rb-prevrb)/42000000; // gap in sec
   //Serial.print("GAP = ");
   //Serial.println((float)(rb-prevrb)/42000);
   prevrb = rb;

   // Entro qui se si verifica un fronte sul sensore
   if ( gap >= 0.05) // Ignoro impulsi minori di 50ms (100km/h)
   {
	wheelSensor.Space += WHEEL_CFR;           // Incremento lo spazio totale
	wheelSensor.RelativeSpace += WHEEL_CFR;   // Incremento lo spazio relativo
	// Gestisco i giri non da gps
	if (wheelSensor.RelativeSpace >= (float)strategySettings.getTrackData().trackLenght)
	{ // Se ho superato il giro
		wheelSensor.RelativeSpace = 0.0;
		wheelSensor.LastRelativeMillis = wheelSensor.getRelativeMillis(); // Salvo il tempo precedente
		wheelSensor.LastFinishTime = wheelSensor.TimeMillis; // Salvo l'istante di inizio giro
		wheelSensor.FullLaps++;
	}
	// Aggiorno la velocità media
	if ( (wheelSensor.TimeMillis!=0) && (wheelSensor.Space >= 20) )
			wheelSensor.AvgSpeed = wheelSensor.Space / wheelSensor.TimeMillis * 1000.0;
	else
		wheelSensor.AvgSpeed = 0;

	wheelSensor.Speed = WHEEL_CFR/gap;  // Aggiorno la velocità istantanea
	wheelSensor.speedtmr = millis();
	
   }

 }  

}

void WheelSensorClass::processWayPoint (unsigned char WayPointIndex)
{
	if (strategySettings.getWayPoint(WayPointIndex).isReference()) // se si tratta di un waypoint di riferimento
	{
		if(WayPointIndex == 0) // start waypoint: è lo 0-esimo
		{
			// Devo considerare il waypoint di start solo dopo 
			// aver percorso un giro, non devo considerarlo alla partenza
			if (Space>200) // Devo aver percorso almeno 200 metri!
			{
				RelativeSpace = strategySettings.getWayPoint(WayPointIndex).getSpaceReference();
				LastRelativeMillis = getRelativeMillis(); // Salvo il tempo precedente
				LastFinishTime = TimeMillis; // Salvo l'istante di inizio giro
				FullLaps++;
			}
		}
		else
		{
			// Normalissimo waypoint di riferimento
			RelativeSpace = strategySettings.getWayPoint(WayPointIndex).getSpaceReference();
		}
	}
}


void WheelSensorClass::startTimer(Tc *tc, uint32_t channel, IRQn_Type irq) {

 //see 37.7.9
 REG_TC2_WPMR=0x54494D00;

 //enable configuring the io registers. see 32.7.42
 REG_PIOC_WPMR=0x50494F00;


 //we need to configure the pin to be controlled by the right peripheral.
 //pin 5 is port C. PIOC_PDR is defined in hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/instance/instance_pioc.h
 //and PIO_PDR_P25 is defined in hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/component/component_pio.h 
 //this disables the pio from controlling the pin. see 32.7.2

 REG_PIOC_PDR |= PIO_PDR_P25;

 //next thing is to assign the io line to the peripheral. See 32.7.24.
 //we need to know which peripheral we should use. Read table 37-4 in section 37.5.1.
 //TIOA6 is peripheral B, so we want to set that bit to 1.
 //REG_PIOC_ABSR is defined in hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/instance/instance_pioc.h
 //PIO_ABSR_P25 is defined in hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/component/component_pio.h
 REG_PIOC_ABSR |= PIO_ABSR_P25;


 //allow configuring the clock.
 pmc_set_writeprotect(false);

 /*
  Every peripheral in the SAM3X is off by default (to save power) and
  should be turned on. 
  */
 pmc_enable_periph_clk(ID_TC6);

 /*
  configure the timer. All this is about setting TC_CMRx, see 37.7.10 in atmel pdf.
  We use CLOCK1 at 42 MHz to get the best possible resolution.
  We want input capture on TIOA6 (pin 5). Nothing else should be necessary, BUT there is a caveat:
  As mentioned in 37.6.8, we only get the value loaded in RA if not loaded since the last trigger,
  or RB has been loaded. Since I do not want to trigger as that sets the timer value to 0, I
  instead let register B be loaded when the pulse is going low.
  */
 TC_Configure(tc, channel,  TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_LDRA_RISING | TC_CMR_LDRB_FALLING);



 //set the interrupt flags. We want interrupt on overflow and TIOA6 (pin 5) going high.
 const uint32_t flags=TC_IER_COVFS  | TC_IER_LDRAS;
 tc->TC_CHANNEL[channel].TC_IER=flags;
 tc->TC_CHANNEL[channel].TC_IDR=~flags;//assume IER and IDR are equally defined.

 NVIC_EnableIRQ(irq);

 //read away the status.
 // TC_GetStatus(tc, channel);

 //start the timer
 TC_Start(tc,channel);
}

void WheelSensorClass::init(){
	power = 0;
	speedtmr = millis();
	startTimer(TC2, 0, TC6_IRQn); //TC2 channel 0, the IRQ for that channel
	Timer5.attachInterrupt(Timer5_Handler).setFrequency(1000).start();

	reset();
}

void WheelSensorClass::reset(){
	this->Speed = 0;
	this->RelativeSpace = 0;
	this->FullLaps = 0;
	this->AvgSpeed = 0;
	this->Space = 0;
	this->TimeMillis = 0;
	this->LastFinishTime = 0;
	this->LastRelativeMillis = 0;
	this->Energy = 0;
}

void WheelSensorClass::update(){
	channelsBuffer.setValue<byte>(CanID::LAP, getLap());
	channelsBuffer.setValue<float>(CanID::REL_SPACE, getRelativeSpace());
	channelsBuffer.setValue<float>(CanID::DISTANCE, getSpace());

	channelsBuffer.setValue<float>(CanID::IST_VEL, getSpeed());
	channelsBuffer.setValue<float>(CanID::AVG_VEL, getAvgSpeed());

	channelsBuffer.setValue<unsigned long>(CanID::REL_TIME, getRelativeMillis());
	channelsBuffer.setValue<unsigned long>(CanID::LEFT_TIME, getLeftMillis());
	channelsBuffer.setValue<unsigned long>(CanID::LAST_TIME, getLastRelativeMillis());

	channelsBuffer.setValue<float>(CanID::ENERGY, getEnergy());

}

WheelSensorClass wheelSensor;