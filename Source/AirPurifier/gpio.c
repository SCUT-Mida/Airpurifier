#include <gpio.h>

uint32_t GpioRead(uint32_t uPinNumber)
{
	return nrf_gpio_pin_read(uPinNumber);
}

void GpioWrite(uint32_t uPinNumber, uint32_t nValue)
{
	nrf_gpio_pin_write(uPinNumber, nValue);
}

void GpioToggle(uint32_t uPinNumber)
{
    nrf_gpio_pin_toggle(uPinNumber);
}

void GpioConfig(uint32_t uPinNumber, GPIO_DIR dir)
{
	switch (dir) {
	case INPUT:
		nrf_gpio_cfg_input(uPinNumber, NRF_GPIO_PIN_NOPULL);
		break;
	case OUTPUT:
		nrf_gpio_cfg_output(uPinNumber);
		break;
	}
}


