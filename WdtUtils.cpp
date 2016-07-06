
#include "WdtUtils.h"

void disableWDT(){
	WDT_Disable(WDT);
}

void enableWDT(uint32_t timeout){
	//this assumes the slow clock is running at 32.768 kHz watchdog frequency is therefore 32768 / 128 = 256 Hz 
	timeout = timeout * 256 / 1000;
	if (timeout == 0)
		timeout = 1;
	else if (timeout > 0xFFF)
		timeout = 0xFFF;
	//	timeout = WDT_MR_WDRSTEN | WDT_MR_WDRPROC | WDT_MR_WDV(timeout) | WDT_MR_WDD(timeout);
	timeout = WDT_MR_WDRSTEN | WDT_MR_WDV(timeout) | WDT_MR_WDD(timeout);

	WDT_Enable(WDT, timeout);
}

void resetWDT(){
	WDT_Restart(WDT);
}
