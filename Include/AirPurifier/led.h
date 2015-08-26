#ifndef LED_H
#define LED_H

#include <pin.h>
#include <gpio.h>

/*******@Mida<Pin for Testing Led>***********/
#define APP_ADVERTISING_LED               TEST_LED1        				/**< Is on when device is advertising. */
#define APP_CONNECT_LED                 	TEST_LED1               /**< Is on when device has connected. */
#define APP_ASSERT_LED                    WARN_LED_GREEN_PIN     		/**< Is on when application has asserted. */
#define PURIFIER_LED_PIN_NO               TEST_LED0               /**< Is on when application has turned on purifier. */
#define SEND_PACKET_FINISH_LED_PIN_NO   	WARN_LED_RED_PIN      /**< Is on when device is advertising. */
/*******@Mida<Pin for Testing Led>***********/

#ifdef __cplusplus
extern "C" {
#endif

#define HIGH_LEVEL_TO_LIGHT_LED  (bool)true             //If low-level to light the leds,set this as 'false'

typedef enum{
	DEFAULT = 0x00,       //0000 0000B
	RED     = 0x04,     	//0000 0100B,R
	GREEN   = 0x02,     	//0000 0010B,G
	BLUE    = 0x01,     	//0000 0001B,B
	YELLOW  = 0x06,  			//0000 0110B,R+G
	CYAN    = 0x03, 			//0000 0011B,G+B
	MAGENTA = 0x05,     	//0000 0101B,R+B
	WHITE   = 0x07      	//0000 0111B,R+G+B
} COLOR_TYPE;

typedef enum {
	CONNECT_LED = 0,
	CONTROL_LED,
	WARN_LED,
	AQI_LED,
	ALL_LED
} LED_TYPE;

typedef struct {
	bool Red;
	bool Green;
	bool Blue;
} RgbLed;


void InitLed(void);

void CloseLed(LED_TYPE led);

void OpenLed(LED_TYPE led, COLOR_TYPE color);

void ToggleLed(LED_TYPE led);

#ifdef __cplusplus
}
#endif



#endif

