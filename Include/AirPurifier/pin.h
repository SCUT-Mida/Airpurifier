#ifndef PIN_H
#define PIN_H
                 

	//Pin for BLE_3.1 
	#define PM25_PULSE_PIN                    0   //For PM2.5 PULSE
	#define PM25_ADC_PIN                      1   //For PM2.5 ADC
	#define TVOC_ADC_PIN                      2   //For TVOC ADC
	#define DHT11_PIN                         3   //For DHT11 ADC
	#define FAN_PWM_PIN                       4  	//For Fan PWM
	#define FAN_POWER_CONTROL_PIN             5  	//For Fan Power Control
	#define FAN_SPEED_MONITIOR_PIN            6  	//For Fan Speed Monitior
	#define FLASH_CS_PIN                      7   //For Flash Chis Select
	#define FLASH_SPI_CLK_PIN                 8   //For Flash SPI Clock
	#define FLASH_SPI_SI_PIN                  9   //For Flash SPI Serial Input
	#define FLASH_SPI_SO_PIN                  10  //For Flash SPI Serial Ouput
	#define DS1302_SCLK_PIN                   11  //For DS1302 Clock
	#define DS1302_IO_PIN                     12  //For DS1302 IO
	#define DS1302_RST_PIN                    13  //For DS1302 Reset
	#define AQI_LED_RED_PIN                   14  //For AQI LED Red
	#define AQI_LED_GREEN_PIN                 15  //For AQI LED Green
	#define AQI_LED_BLUE_PIN                  16  //For AQI LED Blue
	#define WARN_LED_RED_PIN                  18  //For Warning LED Red
	#define WARN_LED_GREEN_PIN                17  //For Warning LED Blue          //<Modify by @Mida 2015-8-7>Because the Led board are exchanged.
	#define TEST_LED0                         19  //For Testing LED0 
	#define TEST_LED1                         20  //For Testing LED1
	#define TOUCH_TEST                        21  //For Testing the touch button
	#define LCD_CSB_PIN                    		22  //For LCD13232 CSB
	#define LCD_RESB_PIN                     	23  //For LCD13232 Reset
	#define LCD_A0_PIN                   			24  //For LCD13232 A0
	#define LCD_SCLK_PIN                    	25  //For LCD13232 Clock
	#define LCD_SID_PIN                     	26  //For LCD13232 SID
	#define FASTCHG                  					27  //For Fast Charging
	#define FUEL_GAUGE_ALERT                  28  //For Fuel Gauge Alert Interrupt
	#define FUEL_GAUGE_SDA                    29  //For Fuel Gauge I2C SDA
	#define FUEL_GAUGE_CLK                    30  //For Fuel Gauge I2C CLK   
	
/*                
	//Pin for BLE_3.0 
	#define PM25_PULSE_PIN                    0   //For PM2.5 PULSE
	#define PM25_ADC_PIN                      1   //For PM2.5 ADC
	#define TVOC_ADC_PIN                      2   //For TVOC ADC
	#define DHT11_PIN                         3   //For DHT11 ADC
	#define FAN_PWM_PIN                       4  	//For Fan PWM
	#define FAN_POWER_CONTROL_PIN             5  	//For Fan Power Control
	#define FAN_SPEED_MONITIOR_PIN            6  	//For Fan Speed Monitior
	#define FLASH_CS_PIN                      7   //For Flash Chis Select
	#define FLASH_SPI_CLK_PIN                 8   //For Flash SPI Clock
	#define FLASH_SPI_SI_PIN                  9   //For Flash SPI Serial Input
	#define FLASH_SPI_SO_PIN                  10  //For Flash SPI Serial Ouput
	//#define                                 11  //unused (GPIO)
	//#define                                 12  //unused (GPIO)
	//#define                                 13  //unused (GPIO)
	#define DS1302_SCLK_PIN                   14  //For DS1302 Clock
	#define DS1302_IO_PIN                     15  //For DS1302 IO
	#define DS1302_RST_PIN                    16  //For DS1302 Reset
	//#define AQI_LED_RED_PIN                   17  //For AQI LED Red              //Mapping the Led board
	#define AQI_LED_GREEN_PIN                 17  //For AQI LED Green
	#define AQI_LED_BLUE_PIN                  17  //For AQI LED Blue
	#define WARN_LED_BLUE_PIN                	17  //For Warning LED Red
	#define WARN_LED_RED_PIN                  20  //For Warning LED Red
	//#define                                 21  //unused (GPIO)
	#define LCD_CSB_PIN                    		22  //For LCD13232 CSB
	#define LCD_RESB_PIN                     	23  //For LCD13232 Reset
	#define LCD_A0_PIN                   			24  //For LCD13232 A0
	#define LCD_SCLK_PIN                    	25  //For LCD13232 Clock
	#define LCD_SID_PIN                     	26  //For LCD13232 SID
	#define FASTCHG                  					27  //For Fast Charging
	#define FUEL_GAUGE_ALERT                  28  //For Fuel Gauge Alert Interrupt
	#define FUEL_GAUGE_SDA                    29  //For Fuel Gauge I2C SDA
	#define FUEL_GAUGE_CLK                    30  //For Fuel Gauge I2C CLK        
*/

#endif              //End For Define pin.h


