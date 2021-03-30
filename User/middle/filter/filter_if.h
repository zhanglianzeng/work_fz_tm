/******************************************************************************
*  Name: filter_if.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"

#if (FILTER_MODULE == 1)
#ifndef FILTER_IF_H
#define FILTER_IF_H

#include <stdbool.h>
#include <stdint.h>
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
typedef struct 
{
    bool    default_level;    /*for VCAST*/
    uint8_t cnt;
}Str_Filter;

/*define Filter_Button_Bits as bellow for 11.3 and 11ICS*/
typedef struct 
{
    uint8_t Button1    : 1;
    uint8_t Button2    : 1;
    uint8_t Button3    : 1;
    uint8_t Button4    : 1;
    uint8_t Button5    : 1;
    uint8_t Button6    : 1;
    uint8_t WakeSP     : 1;
    uint8_t Reserved   : 1;
} Filter_Button_Bits;

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define FILTER_COUNT_NUM       3

#if (SWITCHPACK_MODULE == 1)
//#define BUTTON_S1        GPIO(GPIO_PORTA, 9)   	
//#define BUTTON_S2        GPIO(GPIO_PORTA, 11)  	
//#define BUTTON_S3        GPIO(GPIO_PORTB, 14)
//#define BUTTON_S4        GPIO(GPIO_PORTB, 15)
//#define BUTTON_S5        GPIO(GPIO_PORTB, 8)	
//#define BUTTON_S6        GPIO(GPIO_PORTA, 21)
#endif
/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Filter_Init(void);
extern void Filter_Task(void);
extern void Filter_GpioInit(void);
extern void Filter_Gpio_Standby(void);
extern bool Filter_Get_StablizetionFlg(void);
extern void Filter_Clear_StablizationFlg(void);
extern void Filter_Fast_Task(void);
#endif /* _FILTER_IF_H_ */
#endif
