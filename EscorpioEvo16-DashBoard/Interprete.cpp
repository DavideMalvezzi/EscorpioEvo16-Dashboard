// Classe Interprete

#include "Interprete.h"
#include "ConsoleForm.h"

void Interprete::init(){
	kfirstlap = TFirstLap[TrackData[0]]/TrackData[4]/100.0;
	klap = TLap[TrackData[0]]/TrackData[5]/100.0;
	klastlap = TLastLap[TrackData[1]]/TrackData[6]/100.0;

	// Correzione dei profili con i dati settings
	unsigned int i;
	for (i=0; i<TrackData[0]; i++) TFirstLap[i] = TFirstLap[i]/kfirstlap;
	for (i=0; i<TrackData[0]; i++) SFirstLap[i] = SFirstLap[i]*kfirstlap;

	for (i=0; i<TrackData[0]; i++) TLap[i] = TLap[i]/klap;
	for (i=0; i<TrackData[0]; i++) SLap[i] = SLap[i]*klap;

	for (i=0; i<TrackData[1]; i++) TLastLap[i] = TLastLap[i]/klastlap;
	for (i=0; i<TrackData[1]; i++) SLastLap[i] = SLastLap[i]*klastlap;

	output = 0;
	GAP = 0;

	LOGLN("STRATEGY_INIT");
	consoleForm.println("STRATEGY_INIT");
}

// laps, relpos in m*10e-2, reltime in s*10e-2, speed in km/h *10e-2  
unsigned char Interprete::step(unsigned char FullLaps, unsigned long RelativePosition, unsigned long RelativeTime, unsigned short Speed)
{
	unsigned char strat = output;
	//********** Seleziono il profilo giusto ***********//
	unsigned short *Sprofile, *Tprofile;
	if (FullLaps==0)
	{
		Sprofile = SFirstLap;
		Tprofile = TFirstLap;
	} else if (FullLaps==(TrackData[2]-1))
	{
		Sprofile = SLastLap;
		Tprofile = TLastLap;
	} else
	{
		Sprofile = SLap;
		Tprofile = TLap;
	}
	//*********************************************//

	// Interpolazione lineare
	long i;
	for (i=0; i<TrackData[0]; i++)
		if((i*100)>RelativePosition) break;
	float coeff = ((i*100)-RelativePosition)/(100.0);
	float ProfileCurrentSpeed = (Sprofile[i]*coeff + (Sprofile[i-1]*(1-coeff)));

	/*
	for (i=0; i<TrackData[0]; i++)
			if((Tprofile[i])>RelativeTime) break;
	coeff = ((Tprofile[i])-RelativeTime)/(Tprofile[i]-Tprofile[i-1])*100.0;
	float Steorico = ((i*coeff + ((i-1)*(1-coeff))) + (FullLaps*TrackData[0]))*100;
	*/
	
	//Determinazione Steorico
	float Steorico;
	if (RelativeTime>TrackData[4]*100) //TFirstLap[TrackData[0]-1]) 
	{
		RelativeTime-= TrackData[4]*100; //TFirstLap[TrackData[0]-1];
		int lp;
		for(lp=0; lp<(TrackData[2]-2); lp++)
		{
			if (RelativeTime > TrackData[5]*100) //TLap[TrackData[0]-1])
			{
				RelativeTime-= TrackData[5]*100;//TLap[TrackData[0]-1];
			} else break;
		}
		if (lp>=(TrackData[2]-2))
		{
			// Ultimo giro
			for (i=0; i<TrackData[0]; i++)
				if((TLastLap[i])>RelativeTime) break;
			coeff = ((TLastLap[i])-RelativeTime)/((float)(TLastLap[i]-TLastLap[i-1]));
			Steorico = (((i-1)*coeff + (i*(1-coeff))) + ((1+lp)*TrackData[0]))*100;
		}
		else
		{
			//Giro tipo
			for (i=0; i<TrackData[0]; i++)
				if((TLap[i])>RelativeTime) break;
			coeff = ((TLap[i])-RelativeTime)/((float)(TLap[i]-TLap[i-1]));
			Steorico = (((i-1)*coeff + (i*(1-coeff))) + ((1+lp)*TrackData[0]))*100;
		}
	}
	else
	{
		for (i=0; i<TrackData[0]; i++)
			if((TFirstLap[i])>RelativeTime) break;
		coeff = ((TFirstLap[i])-RelativeTime)/((float)(TFirstLap[i]-TFirstLap[i-1]));
		Steorico = (((i-1)*coeff + (i*(1-coeff))))*100;
	}

	// Aggiustamento con coefficiente k
	unsigned long RaceLenght = ((TrackData[2]-1)*TrackData[0]+TrackData[1])*100;
	if ((RaceLenght-Steorico)<0)
			Steorico = RaceLenght;
	float Smancante = (RaceLenght-(RelativePosition+(FullLaps*TrackData[0]*100)));
	float SmancanteTeorico = (RaceLenght-Steorico);
	k = Smancante / SmancanteTeorico;
	ProfileCurrentSpeed = ProfileCurrentSpeed*k;

	unsigned long RaceTime = (TrackData[4]*100 + (TrackData[2]-2)*TrackData[5]*100 + TrackData[6]*100);


	/*
	float NeededAcc = 1;//(ProfileSpeed[i]-CurrentSpeed)/(ProfileTime[i]-CurrentTime); 
	float OptimalAcc = 0;

#define hysteresisON 1.1
#define hysteresisONcruise 1.02
#define hysteresisOFF 1.05
		if (strat == 1)
		{
			if(Speed<=ProfileCurrentSpeed*hysteresisON) 
			{
				if ((NeededAcc) < (OptimalAcc*0.3))
					strat = 2;
				else
					strat=1;
			}
			else
				strat = 0;
		}
		else if (strat == 0)
		{
			if(Speed*hysteresisOFF<=ProfileCurrentSpeed) 
			{
					strat = 1;
			}
			else
				strat = 0;
		}
		else if (strat == 2)
		{
			if(Speed<=ProfileCurrentSpeed*hysteresisONcruise) 
			{
				strat = 2;
			}
			else
				strat = 0;
		}

		*/
	/*
#define hysteresisON 1.05
#define hysteresisOFF 1.08
		if (strat != 0)
		{
			if(Speed<=ProfileCurrentSpeed*hysteresisON) 
				strat=1;
			else
				strat = 0;
		}
		else if (strat == 0)
		{
			if(Speed*hysteresisOFF<=ProfileCurrentSpeed) 
					strat = 1;
			else
				strat = 0;
		}
		*/
#define hysteresisON 0.4*360
#define hysteresisOFF 0.4*360
		if (strat != 0)
		{
			if(Speed>=(ProfileCurrentSpeed+hysteresisON)) 
				strat=0;
			else
				strat = 1;
		}
		else
		{
			if((Speed+hysteresisOFF)<=ProfileCurrentSpeed) 
				strat = 1;
			else
				strat = 0;
		}
		

		// Stima del ritardo/anticipo
		/*
		float Telpased = 0;
		if (FullLaps==0) Telpased = RelativeTime;
		else Telpased = TrackData[4]*100 + (FullLaps-1)*TrackData[5]*100 + RelativeTime;
		float AVGSpeedToEnd = (float)Smancante/(float)(RaceTime-Telpased); //in m/s 
		*/
		//Test: salvo la velocità media da qui alla fine
		//GAP = (long)((Steorico-(RaceLenght-Smancante))/(AVGSpeedToEnd*100) *1000);
		//GAP =((Steorico-(RaceLenght-Smancante)));
		GAP = (long)((Steorico-(RaceLenght-Smancante))/(RaceLenght/RaceTime*100) *1000);

		//GAP = (long)((Steorico-(RaceLenght-Smancante))/Speed*1000);
		output = strat;
		return strat;
		
}

Interprete strategy;
