#ifndef __FAN_H
#define __FAN_H

#include <stdint.h>

void InitFan(void);												//Initaling the fan

void OpenFan(uint8_t duty_cycle);					//Open the fan and the speed is setted by the duty-cycle

void CloseFan(void);											//Close the fan

void SetFanSpeed(uint8_t duty_cycle);	 		//Changing the duty-cycle of motor when the fan is openning

uint16_t ReadFanSpeed(void);	 						//Reading the speed of fan. 

#endif
