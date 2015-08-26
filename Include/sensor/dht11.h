#ifndef DHT11_H
#define DHT11_H

#include <gpio.h>
#include <pin.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DHT11_QUEUE_LENGTH            10                          //As the Length for calculate the average
#define DHT11_INIT_TIME               100													//100ms  	

void InitDht11(void);                                             //Initializing the DHT11

void GetDht11(float *temperature, float *humidity);

void GetAverageDht11(float *temperature, float *humidity);				//Get the average of DHT11


#ifdef __cplusplus
}
#endif

#endif


