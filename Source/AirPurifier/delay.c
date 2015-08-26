#include <delay.h>

void DelayUs(volatile uint32_t us)
{
	nrf_delay_us(us);
}

void DelayMs(volatile uint32_t ms)
{
	while(ms != 0) {
        ms--;
        nrf_delay_us(US_PER_MS);
    }
}
