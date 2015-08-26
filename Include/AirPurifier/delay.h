#ifndef DELAY_H
#define DELAY_H

#include "nrf_delay.h"

#ifdef __cplusplus
extern "C" {
#endif

#define US_PER_MS             999

void DelayUs(uint32_t us);

void DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif


#endif

