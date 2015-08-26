//INCLUDE
#include <sensor.h>
#include <dht11.h>
#include <pm25.h>
#include <tvoc.h>
#include <lcd.h>

void InitSensor(void)
{
		InitAdc();        //Initializing the ADC
		InitDht11(); 			//Initializing the DHT11
		InitPm25();				//Initializing the PM2.5
		InitTvoc();				//Initializing the TVOC
}

void CloseSensor(void)
{
	
}

void GetSensorData(SensorData *SRet)
{	
		GetAverageDht11(&SRet->temperature,&SRet->humidity);
		SRet->pm2_5 = GetAveragePM25();
		SRet->tvoc = GetAverageTvoc();
		GetCalenderTime(&SRet->local_rtc);
}


void LcdDisplaySensorData(SensorData sensor ,unsigned char LcdShowFlag)
{
	switch(LcdShowFlag)
	{
		case FENchen: LcdDisplayPM25(sensor.pm2_5);break;	
					 
		case jiaQUAN: LcdDisplayFormaldehyde(sensor.tvoc);break;
			
		case WENdu : LcdDisplayTemp(sensor.temperature); break; 			
		 
		case SHIdu : LcdDisplayHumi(sensor.humidity);break;		

		case SHIjian :LcdDisplayTime(sensor.local_rtc); break;
								 
//		case ZHUANsu: LcdDisplayFanSpeed(sensor.fan.rpm);		
//					break;					   		
	}
}

