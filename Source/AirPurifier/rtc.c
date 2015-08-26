#include <rtc.h>

static __INLINE void RtcShortDelay(void)
{
	DelayUs(1);
}

static __INLINE void RtcLongDelay(void)
{
	DelayUs(4);
}

static __INLINE void EnableRtc(void)
{
	GpioWrite(DS1302_RST_PIN, ON);
}

static __INLINE void DisableRtc(void)
{
	GpioWrite(DS1302_SCLK_PIN, OFF);
	GpioWrite(DS1302_RST_PIN, OFF);
}

static __INLINE void RtcWriteIo(uint8_t is)
{
	GpioWrite(DS1302_IO_PIN, (uint32_t)is);
}

static __INLINE IO_STATUS RtcReadIo(void)
{
	return (IO_STATUS)GpioRead(DS1302_IO_PIN);
}

static __INLINE void RtcWriteClk(CLK_STATUS cs)
{
	GpioWrite(DS1302_SCLK_PIN, cs);
}

static __INLINE void RtcWriteByte(uint8_t nByte)
{
	int i;
	GpioConfig(DS1302_IO_PIN, OUTPUT);
	for (i = 0; i < BYTE_BIT_COUNT; ++i) {
		RtcWriteClk(LOW);
		RtcShortDelay();
		RtcWriteIo(nByte & 0x01);
		nByte = nByte >> 1;
		RtcWriteClk(HIGH);
		RtcShortDelay();
	}
}

static __INLINE uint8_t RtcReadByte(void)
{
	int i;
	GpioConfig(DS1302_IO_PIN, INPUT);
	uint8_t nByte = 0;
	for (i = 0; i < BYTE_BIT_COUNT; ++i) {
		nByte = nByte >> 1;
		RtcWriteClk(LOW);
		RtcShortDelay();
		if (UP == RtcReadIo())
			nByte += 0x80;
		RtcWriteClk(HIGH);
		RtcShortDelay();
	}
	return nByte;
}

static __INLINE uint8_t RtcReadTime(RTC_READ_COMMAND_TYPE command)
{
	DisableRtc();
	RtcShortDelay();
	EnableRtc();
	RtcLongDelay();
	RtcWriteByte(command);
	uint8_t nTime = RtcReadByte();
	DisableRtc();
	RtcShortDelay();
	return nTime;
}

static __INLINE void RtcWriteTime(RTC_WRITE_COMMAND_TYPE command, uint8_t nTime)
{
	DisableRtc();
	RtcShortDelay();
	EnableRtc();
	RtcLongDelay();
	RtcWriteByte(command);
	RtcWriteByte(nTime);
	DisableRtc();
	RtcShortDelay();
}

int GetSecond(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_SECOND);
	int nRet = ((nTime >> 4) & 0x07) * 10 + (nTime & 0x0F);
	return nRet;
}

int GetMinute(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_MINUTE);
	int nRet = ((nTime >> 4) & 0x07) * 10 + (nTime & 0x0F);
	return nRet;
}

int GetHour(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_HOUR);
	int nRet = ((nTime >> 4) & 0x03) * 10 + (nTime & 0x0F);
	return nRet;
}

WEEK_DAY_TYPE GetWeekDay(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_DAY);
	WEEK_DAY_TYPE nRet = (WEEK_DAY_TYPE)(nTime & 0x0F);
	return nRet;
}

int GetDate(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_DATE);
	int nRet = ((nTime >> 4) & 0x03) * 10 + (nTime & 0x0F);
	return nRet;
}

int GetMonth(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_MONTH);
	int nRet = ((nTime >> 4) & 0x01) * 10 + (nTime & 0x0F);
	return nRet;
}

int GetYear(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_YEAR);
	int nRet = ((nTime >> 4) & 0x0F) * 10 + (nTime & 0x0F);
	return nRet;
}

static __INLINE void RtcEnableWrite(void)
{
	RtcWriteTime(RTC_WRITE_WP, RTC_ENABLE_WRITE);
}

/*
static __INLINE void RtcDisableWrite(void)
{
	RtcWriteTime(RTC_WRITE_PROTECT, RTC_DISABLE_WRITE);
}
*/

int SetSecond(int nSec)
{
	if (nSec < 0 || nSec > 60)
		return -1;
	uint8_t nTime = GET_RTC_TIME_8BIT_FORMAT(nSec);
	RtcWriteTime(RTC_WRITE_SECOND, nTime);
	return 0;
}

int SetMinute(int nMin)
{
	if (nMin < 0 || nMin > 60)
		return -1;
	uint8_t nTime = GET_RTC_TIME_8BIT_FORMAT(nMin);
	RtcWriteTime(RTC_WRITE_MINUTE, nTime);
	return 0;
}

int SetHour(int nHour)
{
	if (nHour < 0 || nHour > 23)
		return -1;
	uint8_t nTime = GET_RTC_TIME_8BIT_FORMAT(nHour);
	RtcWriteTime(RTC_WRITE_HOUR, nTime);
	return 0;
}

int SetWeekDay(WEEK_DAY_TYPE nDay)
{
	if (nDay < 1 || nDay > 7)
		return -1;
	uint8_t nTime = nDay;
	RtcWriteTime(RTC_WRITE_DAY, nTime);
	return 0;
}

int SetDate(int nDate)
{
	if (nDate < 1 || nDate > 31)
		return -1;
	uint8_t nTime = GET_RTC_TIME_8BIT_FORMAT(nDate);
	RtcWriteTime(RTC_WRITE_DATE, nTime);
	return 0;
}

int SetMonth(int nMonth)
{
	if (nMonth < 1 || nMonth > 12)
		return -1;
	uint8_t nTime = GET_RTC_TIME_8BIT_FORMAT(nMonth);
	RtcWriteTime(RTC_WRITE_MONTH, nTime);
	return 0;
}

int SetYear(int nYear)
{
	if (nYear < 0 || nYear > 99)
		return -1;
	uint8_t nTime = GET_RTC_TIME_8BIT_FORMAT(nYear);
	RtcWriteTime(RTC_WRITE_YEAR, nTime);
	return 0;
}

void SetCalenderTime(CalenderTime *ct)
{
	SetYear(ct->year);
	SetMonth(ct->month);
	SetDate(ct->date);
	SetHour(ct->hour);
	SetMinute(ct->minute);
	SetSecond(ct->second);
//	RtcDisableWrite();
}

void GetCalenderTime(CalenderTime *ct)
{
	ct->second = (uint8_t)GetSecond();
	ct->minute = (uint8_t)GetMinute();
	ct->hour   = (uint8_t)GetHour();
	ct->date   = (uint8_t)GetDate();
	ct->month  = (uint8_t)GetMonth();
	ct->year   = (uint8_t)GetYear();
}

int GetCalenderTimeStr(char *pBuf, int nLen)
{
	if (NULL == pBuf || nLen < CALENDER_TIME_STR_LEN)
		return -1;
	CalenderTime ct;
	GetCalenderTime(&ct);
	int i;
	uint8_t nTime;
	uint8_t *pCalenderTime = (uint8_t *)&ct;
	for (i = 0; i < CALENDER_TIME_COUNT; ++i) {
		nTime = *pCalenderTime++;  //依次取到year-->month-->date-->hour-->minute-->second
		*pBuf++ = (nTime / 10) + '0';  //先转换十位
		*pBuf++ = (nTime % 10) + '0';  //后转换个位
	}
	*pBuf = 0;
	return 0;
}

RTC_STATUS CheckRtcStatus(void)
{
	uint8_t nTime = RtcReadTime(RTC_READ_SECOND);
	return (0 == (nTime & 0x80)) ? RTC_RUNNING : RTC_STOPPED;
}

static __INLINE void StartRtcClock(void)
{
	int nSec = GetSecond();
	SetSecond(nSec);
}

static __INLINE void DiableCharge(void)
{
	RtcWriteTime(RTC_WRITE_CHARGE, 0x00);
}

void InitRtc(void)
{
	GpioConfig(DS1302_RST_PIN, OUTPUT);
	GpioConfig(DS1302_SCLK_PIN, OUTPUT);
	RtcEnableWrite();
	RTC_STATUS nRtcStaus = CheckRtcStatus();
	if (RTC_STOPPED == nRtcStaus) {
		DiableCharge();
		StartRtcClock();
	}
}

