/*
 * gpio.c
 *
 *  Created on: Nov 13, 2019
 *      Author: Eskil
 */

#include "gpio.h"

#include "xparameters.h"
#include "xgpio.h"


#if CPU_INSTANCE == 0
static const u8 gpio_device_id = XPAR_GPIO_0_DEVICE_ID;
#else
static const u8 gpio_device_id = XPAR_GPIO_1_DEVICE_ID;
#endif

static XGpio gpio;

int gpio_initialize()
{
	int status;
	status = XGpio_Initialize(&gpio, gpio_device_id);
	if(status != XST_SUCCESS)
	{
		//log_uart("cpu%u - gpio init failed!\n", gpio_device_id);
		return XST_FAILURE;
	}

	// Set direction of gpio pins for channel 1
	XGpio_SetDataDirection(&gpio, 1, GPIO_INPUTS);

	return XST_SUCCESS;
}

void gpio_set(gpio_pins_t pin)
{
	XGpio_DiscreteSet(&gpio, 1, 1 << pin);
}

void gpio_clear(gpio_pins_t pin)
{
	XGpio_DiscreteClear(&gpio, 1, 1 << pin);
}

u8 gpio_read(gpio_pins_t pin)
{
	return (XGpio_DiscreteRead(&gpio, 1) >> pin) & 1;
}
