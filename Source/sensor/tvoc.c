#include <tvoc.h>
#include <delay.h>
#include <gpio.h>
#include <pin.h>
#include <dht11.h>
#include <math.h>

static float sfTvocQueue[TVOC_QUEUE_LENGTH];
static int snTvocQueueIndex = 0;

static __INLINE float TvocValue(float fTvocVoltage)
{
	float fRet,temperature,humi;
	float Formaldehyde,Formaldehyde_ppm,log_Formaldehyde_ppm;																	//only need the Formaldehyde<Modify by @Mida 2015-7-16>
//	float Toluene,Toluene_ppm,log_Toluene_ppm;																	
	
	log_Formaldehyde_ppm = 1.528*fTvocVoltage - 0.125*fTvocVoltage*fTvocVoltage - 2.631;			//only need the Formaldehyde<Modify by @Mida 2015-7-16>
	Formaldehyde_ppm = pow(10,log_Formaldehyde_ppm);
//	log_Toluene_ppm = 2.852*fTvocVoltage - 0.21*fTvocVoltage*fTvocVoltage - 7.071;
//	Toluene_ppm = pow(10,log_Toluene_ppm);
	
	GetDht11(&temperature,&humi);
	
	Formaldehyde = (Formaldehyde_ppm *(273 + temperature)*TVOC_Formaldehyde_M)/(22.4*273);			//only need the Formaldehyde<Modify by @Mida 2015-7-16>
//	Toluene = (Toluene_ppm *(273 + temperature)*TVOC_Toluene_M)/(22.4*273);
	
	fRet = Formaldehyde;               //only need the Formaldehyde<Modify by @Mida 2015-7-16>
	return fRet;
}


static __INLINE void FullTvocQueue()
{
	for (int i = 0; i < TVOC_QUEUE_LENGTH; ++i) {
		GetTvoc();
	}
}

static void InsertTvocQueue(float fTvocValue)
{
	sfTvocQueue[snTvocQueueIndex++] = fTvocValue;
	if (TVOC_QUEUE_LENGTH == snTvocQueueIndex) {
		snTvocQueueIndex = 0;
	}
}

void InitTvoc(void)
{
	FullTvocQueue();
}

float GetTvoc(void)
{
	OpenAdc(TVOC_ADC_NUMBER);
	float fTvocVoltage = GetVoltage(TVOC_ADC_NUMBER)/0.72;
	float fTvocValue = TvocValue(fTvocVoltage);
	InsertTvocQueue(fTvocValue);
	return fTvocValue;
}

float GetAverageTvoc(void)
{
	GetTvoc();
	float fRet = 0.0;
	for (int i = 0; i < TVOC_QUEUE_LENGTH; ++i) {
		fRet += sfTvocQueue[i];
	}
	fRet /= TVOC_QUEUE_LENGTH;
	return fRet;
}



