#include <pm25.h>
#include <delay.h>
#include <gpio.h>
#include <pin.h>

static float sfPm25Queue[PM25_QUEUE_LENGTH];
static int snPm25QueueIndex = 0;

static __INLINE float Pm25Value(float fPm25Voltage)
{
	float fRet = (fPm25Voltage - PM25_VOLTAGE_WITH_NO_DUST) * PM25_K;
	if (fRet > PM25_MAX)
		fRet = PM25_MAX;
	else if (fRet < PM25_MIN)
		fRet = PM25_MIN;
	return fRet;
}

static __INLINE void FullPm25Queue()
{
	for (int i = 0; i < PM25_QUEUE_LENGTH; ++i) {
		GetPm25();
	}
}

static __INLINE void InsertPm25Queue(float fPm25Value)
{
	sfPm25Queue[snPm25QueueIndex++] = fPm25Value;
	if (PM25_QUEUE_LENGTH == snPm25QueueIndex) {
		snPm25QueueIndex = 0;
	}
}

void InitPm25(void)
{
	GpioConfig(PM25_PULSE_PIN, OUTPUT);
	GpioWrite(PM25_PULSE_PIN, ON);
	DelayMs(PM25_INIT_TIME);
	FullPm25Queue();
}

float GetPm25(void)
{
	GpioWrite(PM25_PULSE_PIN, PM25_PULSE_ON);
	DelayUs(PM25_SAMPLE_TIME);
	OpenAdc(PM25_ADC_NUMBER);
	uint32_t uPm25Adc = GetAdcValue();
	DelayUs(PM25_SAMPLE_LEFT);
	GpioWrite(PM25_PULSE_PIN, PM25_PULSE_OFF);
	DelayUs(PM25_LOW_TIME);
	float fPm25Voltage = GetVoltageByValue(uPm25Adc);
	float fPm25Value = Pm25Value(fPm25Voltage);
	InsertPm25Queue(fPm25Value);
	return fPm25Value;
}

float GetAveragePM25(void)
{
	GetPm25();
	float fRet = 0.0;
	for (int i = 0; i < PM25_QUEUE_LENGTH; ++i) {
		fRet += sfPm25Queue[i];
	}
	fRet /= PM25_QUEUE_LENGTH;
	return fRet;
}

