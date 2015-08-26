#include <init.h>

static __INLINE void InitSecondTimer(void)
{
	InitTimer(SECOND_TIMER_NUMBER, BITMODE_32);
	OpenTimer(SECOND_TIMER_NUMBER, ONE_SECOND_PER_MS);
}

void InitSystem(void)
{
	InitLed();
	InitUart();
	BluetoothInit();
	InitAdc();
	InitSensor();
	InitFan();
	InitRtc();
	InitSecondTimer();
}

