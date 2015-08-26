#ifndef ADC_H
#define ADC_H

//INCLUDE
#include <stdbool.h>
#include <stdint.h>
#include <nrf.h>
#include <nrf_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	ADC_IDLE = 0,
	ADC_BUSY = 1,
} ADC_STATUS;


#define ADC_COUNT_MAX           1024.0
#define ADC_COUNT_MAX_VOLTAGE   1.2
#define ADC_PRESCALING          3.0

void InitAdc(void);

void OpenAdc(uint32_t uAdcNumber);

uint32_t GetAdcNumber(void);

uint32_t GetAdcValue(void);

float GetVoltageByValue(uint32_t uAdcValue);

float GetVoltage(uint32_t uAdcNumber);


#ifdef __cplusplus
}
#endif


#endif
