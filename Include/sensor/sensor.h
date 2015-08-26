#ifndef SENSOR_H
#define SENSOR_H

//INCLUDE
#include <rtc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float pm2_5;
	float tvoc;
	float temperature;
	float humidity;
	CalenderTime local_rtc;
} SensorData;

void InitSensor(void);

void CloseSensor(void);

void GetSensorData(SensorData *SRet);

void LcdDisplaySensorData(SensorData sensor ,unsigned char LcdShowFlag);

#ifdef __cplusplus
}
#endif


#endif


