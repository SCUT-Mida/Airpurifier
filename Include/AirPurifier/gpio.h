#ifndef AIRPURIFIER_GPIO_H
#define AIRPURIFIER_GPIO_H

//INCLUDE
#include <stdbool.h>
#include <stdint.h>
#include <nrf.h>
#include <nrf_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	INPUT = 0,
	OUTPUT = 1
} GPIO_DIR;

typedef enum {
	OFF = 0,
	ON = 1
} GPIO_STATUS;

uint32_t GpioRead(uint32_t uPinNumber);

void GpioWrite(uint32_t uPinNumber, uint32_t nValue);

void GpioToggle(uint32_t uPinNumber);

void GpioConfig(uint32_t uPinNumber, GPIO_DIR dir);


#ifdef __cplusplus
}
#endif

#endif
