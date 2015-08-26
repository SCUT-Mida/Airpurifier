#ifndef PM25_H
#define PM25_H

//INCLUDE
#include <adc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PM25_PULSE_ON                1
#define PM25_PULSE_OFF               0
#define PM25_ADC_NUMBER              ADC_CONFIG_PSEL_AnalogInput2  //P0.01
#define PM25_INIT_TIME               1000                 //uint(ms) 1000ms
#define PM25_SAMPLE_TIME             280
#define PM25_SAMPLE_LEFT             40
#define PM25_LOW_TIME                9680
#define PM25_VOLTAGE_WITH_NO_DUST    0.40
#define PM25_K                       153.85
#define PM25_MAX                     500
#define PM25_MIN                     0
#define PM25_QUEUE_LENGTH            10
#define PM25_BUF_SIZE                3  //xxxμg/m³

void InitPm25(void);

float GetPm25(void);

float GetAveragePM25(void);


#ifdef __cplusplus
}
#endif


#endif
