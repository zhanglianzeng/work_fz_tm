/******************************************************************************
*  Name: gpio.h
*  Description: gpio driver
*  Project:
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "../../Device/samc21n18a.h"
#include <stdbool.h>

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define GPIO_PIN(n) (((n)&0x1Fu) << 0)
#define GPIO_PORT(n) ((n) >> 5)
#define GPIO(port, pin) ((((port)&0x7u) << 5) + ((pin)&0x1Fu))
#define GPIO_PIN_FUNCTION_OFF 0xffffffff
#define GPIO_PIN_FUNCTION_H 7
#define SLAVE_REQUIRE   GPIO(GPIO_PORTC, 16)

/**********************************************************************************************
* Local types
**********************************************************************************************/
enum gpio_pull_mode { GPIO_PULL_OFF, GPIO_PULL_UP, GPIO_PULL_DOWN };
enum gpio_direction { GPIO_DIRECTION_OFF, GPIO_DIRECTION_IN, GPIO_DIRECTION_OUT };
enum gpio_port { GPIO_PORTA, GPIO_PORTB, GPIO_PORTC, GPIO_PORTD, GPIO_PORTE };

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void GPIO_Set_Direction(const uint8_t pin, const enum gpio_direction direction);
extern void GPIO_Set_Function(const uint32_t gpio, const uint32_t function);
extern void GPIO_Set_Pull_Mode(const uint8_t gpio, const enum gpio_pull_mode pull_mode);
extern void GPIO_Set_Level(const uint8_t gpio, const bool level);
extern void GPIO_Toggle_Level(const uint8_t gpio);
extern bool GPIO_Get_Level(const uint8_t gpio);
#endif /* GPIO_DRIVER_H */
