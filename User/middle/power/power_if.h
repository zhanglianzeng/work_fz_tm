/******************************************************************************
*  Name: power_if.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (POWER_MODULE == 1)

#ifndef POWER_IF_H
#define POWER_IF_H

#include <stdint.h>
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
#define WDO           GPIO(GPIO_PORTB, 15)
#define PMIC_FAULT    GPIO(GPIO_PORTC, 8)   

/*set 12V Converter to output 3.3V */
#define POWER_3_3V    GPIO(GPIO_PORTB, 22)
#define VDD_3V3_PG    GPIO(GPIO_PORTA, 12)   
/*set 12V Converter to output 5V */
#define POWER_5V      GPIO(GPIO_PORTB, 23)
#define HVAC_5V_PG    GPIO(GPIO_PORTA, 13)  

#define VDD_1V1_PG    GPIO(GPIO_PORTC, 10)   
#define VDD_1V8_PG    GPIO(GPIO_PORTC, 11)   

#define BAT_DET_EN    GPIO(GPIO_PORTA, 9)
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
extern void Power_If_Init(void);
extern void Power_If_Standby(void);
extern bool Power_If_Get_Fault_Level(void);
extern bool Power_If_Get_VDD_3V3_PG_Level(void);
extern bool Power_If_Get_HVAC_5V_PG_Level(void);
extern bool Power_If_Get_VDD_1V8_PG_Level(void);
extern void Power_If_EN(void);
extern void Power_If_DIS(void);
extern void Touch_If_Set_Rstn(bool level);
#endif /* _POWER_IF_H_ */
#endif /* POWER_MODULE */
