#include <adc.h>
#include <nrf_soc.h>

void InitAdc(void)
{
	NRF_ADC->ENABLE = 1;
	NRF_ADC->EVENTS_END = ADC_IDLE;
}
void OpenAdc(uint32_t uAdcNumber)
{
	uint32_t p_is_running = 0;	
	sd_clock_hfclk_request();
	while(! p_is_running) {  							//wait for the hfclk to be available
		sd_clock_hfclk_is_running((&p_is_running));
	}   
	
	while (ADC_BUSY == NRF_ADC->EVENTS_END);
	NRF_ADC->CONFIG = (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos)
	                | (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling << ADC_CONFIG_INPSEL_Pos)
									| (ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos)
									| (uAdcNumber << ADC_CONFIG_PSEL_Pos);
	NRF_ADC->TASKS_START = 1;
}

uint32_t GetAdcNumber(void)
{
	return ((NRF_ADC->CONFIG & ADC_CONFIG_PSEL_Msk) >> ADC_CONFIG_PSEL_Pos);
}

uint32_t GetAdcValue(void)
{
	NRF_ADC->EVENTS_END = ADC_BUSY;
	while (ADC_IDLE == NRF_ADC->EVENTS_END);
	uint32_t uRet = NRF_ADC->RESULT & ADC_RESULT_RESULT_Msk;
	NRF_ADC->TASKS_STOP = 1;
	NRF_ADC->EVENTS_END = 0;
	
	//Release the external crystal
	sd_clock_hfclk_release();
	return uRet;
}

float GetVoltageByValue(uint32_t uAdcValue)
{
	float fAdcVoltage = (float)uAdcValue / ADC_COUNT_MAX * ADC_COUNT_MAX_VOLTAGE * ADC_PRESCALING;
	return fAdcVoltage;
}

float GetVoltage(uint32_t uAdcNumber)
{
	OpenAdc(uAdcNumber);
	uint32_t uAdcValue = GetAdcValue();
	float fAdcVoltage = ((float)uAdcValue / ADC_COUNT_MAX) * ADC_COUNT_MAX_VOLTAGE * ADC_PRESCALING;
	return fAdcVoltage;
}


