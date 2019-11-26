/*
 * gpio.h
 *
 *  Created on: Nov 13, 2019
 *      Author: Eskil
 */

#ifndef SRC_DRIVERS_GPIO_H_
#define SRC_DRIVERS_GPIO_H_

#include "xil_types.h"

typedef enum {
	LED_GREEN_0,
	LED_GREEN_1,
	LED_RED_0,
	LED_RED_1,
	LED_YELLOW_0,
	LED_ORANGE_0,
	SIG_GD_NOT_FAULT,
	SIG_GD_READY,
	SIG_GD_RST
} gpio_pins_t;


// 1 defines input, 0 defines output
#define GPIO_INPUTS 0b000000000

int gpio_initialize(void);

void gpio_set(gpio_pins_t pin);

void gpio_clear(gpio_pins_t pin);

u8 gpio_read(gpio_pins_t pin);

#endif /* SRC_DRIVERS_GPIO_H_ */
