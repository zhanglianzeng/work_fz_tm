/******************************************************************************
*  Name: bsp_if.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#ifndef BSP_IF_H
#define BSP_IF_H

#include "../config.h"

#if (CLOCK_MODULE==1)
#include "../middle/clock/clk_if.h"
#endif
#if (POWER_MODULE==1)
#include "../middle/power/power_if.h"
#endif
#if (ADC_MODULE==1)
#include "../middle/adc/adc_if.h"
#endif
#if (SLEEP_MODULE==1)
#include "../middle/sleep/sleep_if.h"
#endif
#if (EEPROM_MODULE==1)
#include "../application/eeprom/app_eeprom.h"   /*called upper layer*/
#endif
#if (POST_MODULE==1)
#include "../middle/post/post_if.h"
#endif
#if (FILTER_MODULE==1)
#include "../middle/filter/filter_if.h"
#endif
#if (WDOG_MODULE==1)
#include "../middle/wdog/wdog_if.h"
#endif
#if (DESERIALIZER_MODULE==1)
#include "../middle/deserializer/deserializer_if.h"
#include "../application/iic/app_iic_comm.h"  
#endif

#include "./../driver/tc.h"
#include "./../driver/tcc.h"
/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Bsp_If_Init(void);

#endif /* _BSP_IF_H_ */
