/* Copyright (c) 2015 SCUT. All Rights Reserved.
** @file <Add by @Mida 2015-8-4>
* @brief Control the 4-lines fan with Softdevice.
* @board BLE_3.1
*/
#include "fan.h"
#include "pin.h"
#include "gpio.h"
#include "nrf_pwm_noglitch.h"

//define in "pin.h"
//#define FAN_PWM_PIN                       4  	//For Fan PWM
//#define FAN_POWER_CONTROL_PIN             5  	//For Fan Power Control
//#define FAN_SPEED_MONITIOR_PIN            6  	//For Fan Speed Monitior


void InitFan(void)													//Initaling the fan
{
	GpioConfig(FAN_POWER_CONTROL_PIN,OUTPUT);
	GpioConfig(FAN_PWM_PIN,OUTPUT);
	
	//Seting the PWM Output for controling the motor
	nrf_pwm_config_t pwm_config = PWM_DEFAULT_CONFIG;						
	pwm_config.mode             = PWM_MODE_MTR_100;							// 0-100 resolution, 20 kHz PWM frequency, 4MHz timer frequency (prescaler 2)
  pwm_config.num_channels     = 1;														//Just one Channel
  pwm_config.gpio_num[0]      = FAN_PWM_PIN;
	nrf_pwm_init(&pwm_config); 																	// Initialize the PWM library
	
	OpenFan(60);														//60-duty-cycle  as default
}

void OpenFan(uint8_t duty_cycle)					//Open the fan and the speed is setted by the duty-cycle
{
	GpioWrite(FAN_POWER_CONTROL_PIN,1);
	PWM_TIMER->TASKS_START = 1;							//Starting the pwm timer
	SetFanSpeed(duty_cycle);							
}

void CloseFan(void)												//Close the fan
{
	GpioWrite(FAN_POWER_CONTROL_PIN,0);
	PWM_TIMER->TASKS_START = 0;							//Stoping the pwm timer
	SetFanSpeed(0);
}

void SetFanSpeed(uint8_t duty_cycle)	 		//Changing the duty-cycle of motor when the fan is openning
{
	nrf_pwm_set_value(0,duty_cycle);
}

uint16_t ReadFanSpeed(void)	 							//Reading the speed of fan. 
{
	uint16_t rpm;
	
	return rpm;
}
