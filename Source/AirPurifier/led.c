#include <led.h>

uint8_t off_stat,on_stat;

static __INLINE void InitWarnLed(void)   
{
		GpioConfig(WARN_LED_RED_PIN,OUTPUT);
		GpioConfig(WARN_LED_GREEN_PIN,OUTPUT);
}

static __INLINE void InitAQILed(void)   
{
	  GpioConfig(AQI_LED_RED_PIN,OUTPUT);
		GpioConfig(AQI_LED_GREEN_PIN,OUTPUT);
		GpioConfig(AQI_LED_BLUE_PIN,OUTPUT);
}

static __INLINE void CloseWarnLed(void)   
{
	 GpioWrite(WARN_LED_RED_PIN, off_stat);
	 GpioWrite(WARN_LED_GREEN_PIN,off_stat);
}

static __INLINE void CloseAQILed(void)   
{
	 GpioWrite(AQI_LED_RED_PIN, off_stat);
	 GpioWrite(AQI_LED_GREEN_PIN,off_stat);
	 GpioWrite(AQI_LED_BLUE_PIN,off_stat);
}

static __INLINE void OpenRGBLed(LED_TYPE led,COLOR_TYPE color)   
{
		uint8_t rgb_color;
		RgbLed rgbled;
		if(HIGH_LEVEL_TO_LIGHT_LED)
			rgb_color = color & 0x07;
		else
			rgb_color = (~color) & 0x07;
			
		rgbled.Red 		= (rgb_color & 0x04) >> 2;
		rgbled.Green 	= (rgb_color & 0x02) >> 1;
		rgbled.Blue 	= (rgb_color & 0x01);
		
		switch(led)
		{
			case WARN_LED:	GpioWrite(WARN_LED_RED_PIN, rgbled.Red);
											GpioWrite(WARN_LED_GREEN_PIN,rgbled.Green);break;
			case AQI_LED :	GpioWrite(AQI_LED_RED_PIN, rgbled.Red);
											GpioWrite(AQI_LED_GREEN_PIN,rgbled.Green);
											GpioWrite(AQI_LED_BLUE_PIN,rgbled.Blue);break;
			default 		 : 	break;
		} 
}

void InitLed(void)
{
		if(HIGH_LEVEL_TO_LIGHT_LED)
		{
			on_stat 	= 1;
			off_stat 	= 0;
		}
		else
		{
			on_stat 	= 0;
			off_stat 	= 1;
		}
						
    GpioConfig(TEST_LED0,OUTPUT);
    GpioConfig(TEST_LED1,OUTPUT);
		GpioWrite(TEST_LED0,off_stat);
		GpioWrite(TEST_LED1,off_stat);
		
		InitWarnLed();
		CloseWarnLed();
			
		InitAQILed();
		CloseAQILed();
}

void CloseLed(LED_TYPE led)
{
		switch(led)
		{
			case CONNECT_LED 	: 	GpioWrite(APP_CONNECT_LED,off_stat);break;
			case CONTROL_LED 	:		GpioWrite(PURIFIER_LED_PIN_NO,off_stat);break;
			case WARN_LED 		:		CloseWarnLed();break;
			case AQI_LED 			: 	CloseAQILed();break;
			case ALL_LED 			:		GpioWrite(APP_CONNECT_LED,off_stat);
														GpioWrite(PURIFIER_LED_PIN_NO,off_stat);
														CloseWarnLed();CloseAQILed();break;
		}
}

void OpenLed(LED_TYPE led, COLOR_TYPE color)
{
		switch(led)
		{
			case CONNECT_LED 	: 	GpioWrite(APP_CONNECT_LED,on_stat);break;
			case CONTROL_LED 	:		GpioWrite(PURIFIER_LED_PIN_NO,on_stat);break;
			case WARN_LED 		:		OpenRGBLed(WARN_LED,color);break;
			case AQI_LED 			: 	OpenRGBLed(AQI_LED ,color);break;
			default 					:		break;
		}
	
}

void ToggleLed(LED_TYPE led)
{
		switch(led)
		{
			case CONNECT_LED 	: 	GpioToggle(APP_CONNECT_LED);break;
			case CONTROL_LED 	:		GpioToggle(PURIFIER_LED_PIN_NO);break;
			case WARN_LED 		:		break;
			case AQI_LED 			: 	break;
			default 					:		break;
		}
}


