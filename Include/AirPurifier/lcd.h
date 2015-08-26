#ifndef __LCD13232_H
#define __LCD13232_H

#include <stdbool.h>
#include <stdint.h>
#include <rtc.h>

#define START_PAGE_ADDRESS  0xB3    //开始页地址，从LCD屏上到下是0xB3->0xB0

typedef enum{
	FENchen  	= 0 ,	 //"粉"
	jiaQUAN  	= 1 ,	 //"醛"	
	WENdu    	= 2 ,	 //"温"
	SHIdu    	= 3 ,	 //"湿"
	SHIjian   = 4 ,	 //"时"	
	ZHUANsu		= 5	,	 //"转"
	wenshiDU  = 6 ,	 //"度"
	fenCHEN   = 7 ,	 //"尘"
	JIAquan  	= 8 ,	 //"甲"
	zhuanSU   = 9 ,  //"速"	
	shiJIAN   = 10,  //"间"
} CODE_FLAG;

void InitLCD(void);    						 //初始化显示屏
void ClearScreen(void);						 //清屏
void LcdDisplayInit(void);            //显示“初始化...”界面 

/***字高为32，字宽可以为根据字符数组设置，正常为4、8、12、16、24、32***/
uint16_t DisplayStrH32(uint16_t StrWidth,uint16_t column,uint8_t *StrCode);

void LcdDisplayTemp(float Temp);    				//LCD显示温度
void LcdDisplayHumi(float Humi);    				//LCD显示湿度
void LcdDisplayPM25(float PM25);    				//LCD显示粉尘
void LcdDisplayFormaldehyde(float formaldehyde);    //LCD显示甲醛
void LcdDisplayTime(CalenderTime rtc);    			//LCD显示时间
void LcdDisplayFanSpeed(uint16_t speed);    			//LCD显示电机速度
    
#endif
