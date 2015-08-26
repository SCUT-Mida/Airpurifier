#ifndef RTC_H
#define RTC_H

#include <pin.h>
#include <gpio.h>
#include <delay.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE_BIT_COUNT                  8
#define GET_RTC_TIME_8BIT_FORMAT(n)     (n % 10) + ((n / 10) << 4)
#define CALENDER_TIME_STR_LEN           13  // 规定时间字符串为"YYMMDDhhmmss\0"
#define CALENDER_TIME_FORMAT_LEN        2   // "YY"
#define CALENDER_TIME_COUNT             6   // 年月日时分秒，一共6种

typedef enum {
	RTC_READ_SECOND   = 0x81,
	RTC_READ_MINUTE   = 0x83,
	RTC_READ_HOUR     = 0x85,
	RTC_READ_DATE     = 0x87,
	RTC_READ_MONTH    = 0x89,
	RTC_READ_DAY      = 0x8B,
	RTC_READ_YEAR     = 0x8D,
	RTC_READ_WP       = 0x8F,  //WP(write protection): 写保护
	RTC_READ_CHARGE   = 0x91   //CHARGE: 充电
} RTC_READ_COMMAND_TYPE;

typedef enum {
	RTC_WRITE_SECOND  = 0x80,
	RTC_WRITE_MINUTE  = 0x82,
	RTC_WRITE_HOUR    = 0x84,
	RTC_WRITE_DATE    = 0x86,
	RTC_WRITE_MONTH   = 0x88,
	RTC_WRITE_DAY     = 0x8A,
	RTC_WRITE_YEAR    = 0x8C,
	RTC_WRITE_WP      = 0x8E,  //WP(write protection): 写保护
	RTC_WRITE_CHARGE  = 0x90   //CHARGE: 充电
} RTC_WRITE_COMMAND_TYPE;

typedef enum {
	RTC_ENABLE_WRITE  = 0x00,
	RTC_DISABLE_WRITE = 0x80
} RTC_WRITE_PROTECT_SETTING;

typedef enum {
	RTC_RUNNING = true,
	RTC_STOPPED = false
} RTC_STATUS;

typedef enum {
	SUNDAY = 1,
	MONDAY = 2,
	TUESDAY = 3,
	WEDNESDAY = 4,
	THURSDAY = 5,
	FRIDAY = 6,
	SATURDAY= 7
} WEEK_DAY_TYPE;

typedef enum {
	RTC_WRITE = 0,
	RTC_READ  = 1
} RTC_COMMAND_TYPE;

typedef enum {
	DOWN = 0,
	UP   = 1
} IO_STATUS;

typedef enum {
	LOW  = 0,
	HIGH = 1
} CLK_STATUS;

typedef struct {
	uint8_t year;     /* year since 2000, 0 to 99*/
	uint8_t month;    /* month since January, 0 to 11 */
	uint8_t date;     /* date of the month, 1 to 31 */
	uint8_t hour;     /* hour since midnight, 0 to 23 */
	uint8_t minute;   /* minute after the hour, 0 to 59 */
	uint8_t second;   /* second after the minute, 0 to 60 */
}CalenderTime;


void InitRtc(void);

RTC_STATUS CheckRtcStatus(void);

void SetCalenderTime(CalenderTime *ct);

void GetCalenderTime(CalenderTime *ct);

int GetCalenderTimeStr(char *pBuf, int nLen);

int GetSecond(void);

int GetMinute(void);

int GetHour(void);

WEEK_DAY_TYPE GetWeekDay(void);

int GetDate(void);

int GetMonth(void);

int GetYear(void);

int SetSecond(int nSec);

int SetMinute(int nMin);

int SetHour(int nHour);

int SetWeekDay(WEEK_DAY_TYPE nDay);

int SetDate(int nDate);

int SetMonth(int nMonth);

int SetYear(int nYear);

#ifdef __cplusplus
}
#endif

#endif


