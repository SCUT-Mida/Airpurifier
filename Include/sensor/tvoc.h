#ifndef TVOC_H
#define TVOC_H

//INCLUDE
#include <adc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TVOC_ADC_NUMBER              ADC_CONFIG_PSEL_AnalogInput3
#define TVOC_QUEUE_LENGTH            10
#define TVOC_Formaldehyde_M          30
#define TVOC_Toluene_M               92
#define TVOC_BUF_SIZE                4

void InitTvoc(void);

float GetTvoc(void);

float GetAverageTvoc(void);


#ifdef __cplusplus
}
#endif


#endif
