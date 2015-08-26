#ifndef CAR_AIR_PURIFIER_H
#define CAR_AIR_PURIFIER_H

#include <stdint.h>
#include "boards.h"

//Include the Header File of Control the Air Purifier******<@Mida>
#include "pin.h"
#include "gpio.h"
#include "led.h"
#include "lcd.h"
#include "rtc.h"
#include "sensor.h"
#include "fan.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t							ble_length_t;

#define BLE_SENSOR_SIZE						(uint16_t)4 	// The value size of sensor data which can be described by float.
#define SENSOR_SAMPLE_TIMER_INTERVAL   6        //(6/2)*5 = 15s; 
#define LCD_SHOW_PAGE     						 5				//The number of change lcd pages


// Macros functions
#define CHECK_BIT(byte, bit)				(((byte)>>bit)&(1U))
#define SET_BIT(byte, bit)					((byte)=((byte)|((1U)<<bit)))

void InitAirPurifier(void);										//Initialing the AirPurifier.
void SmartAdapt(SensorData sensor);						//According to the sensor data, intelligently modify the LEDs and duty-cycle of the motor.	

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

