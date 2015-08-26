#ifndef INIT_H
#define INIT_H

//INCLUDE
#include <led.h>
#include <bluetooth.h>
#include <adc.h>
#include <sensor.h>
#include <fan.h>
#include <rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

//TIMER NUMBER For Counting Second
#define SECOND_TIMER_NUMBER          0
//NRF TIMER NUMBER for Counting Second
#define NRF_SECOND_TIMER             NRF_TIMER0

void InitSystem(void);

#ifdef __cplusplus
}
#endif


#endif
