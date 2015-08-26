#include <car_air_purifier.h>

void InitAirPurifier(void)										//Initialing the AirPurifier.
{
		InitLed();
		InitLCD();
		InitRtc();
		InitSensor();
		InitFan();
}


void SmartAdapt(SensorData sensor)						//According to the sensor data, intelligently modify the LEDs and duty-cycle of the motor.
{
	float Pm25 = sensor.pm2_5;
	if((Pm25>=0)&&(Pm25<35))	
		OpenLed(AQI_LED,GREEN);		//0~35ug/m3,excellent==>Light Green.
	else if (Pm25<75)
		OpenLed(AQI_LED,BLUE);		//35~75ug/m3,good==>Light Blue.
	else if(Pm25<115)
		OpenLed(AQI_LED,CYAN);		//75~115ug/m3,mild level pollution==>Light Cyan.
	else if(Pm25<150)
		OpenLed(AQI_LED,YELLOW);	//115~150ug/m3,middle level pollution==>Light Yellow.
	else if(Pm25<250)
		OpenLed(AQI_LED,MAGENTA);	//150~250ug/m3,heavy level pollution==>Light Magenta.
	else
		OpenLed(AQI_LED,RED);		  //>250ug/m3,severe level pollution==>Light Red.

	float Formaldehyde = sensor.tvoc;
	if((Formaldehyde>=0)&&(Formaldehyde<0.08))
		OpenLed(WARN_LED,GREEN);								//0~0.08mg/m3,excellent==>Light Green.
	else if(Formaldehyde<=0.12)
		OpenLed(WARN_LED,YELLOW);								//0.08~0.12mg/m3,so so==>Light Yellow.
	else
		OpenLed(WARN_LED,RED);									//>0.12mg/m3,bad==>Light Red.
}

