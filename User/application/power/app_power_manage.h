/******************************************************************************
*  Name: app_power_manage.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (POWER_MODULE == 1)

#ifndef APP_POWER_MANAGE_H
#define APP_POWER_MANAGE_H

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

/* input voltage, theoretical value of ADC = input voltage*(42.2K/(300K+42.2K))/(3.3V/2^12)= 153.077*input voltage */

#define POWER_INPUT_ADC_4V            0x248    /*ADC value corresponding to 4V voltage*/
#define POWER_INPUT_ADC_4_2V          0x266    /*ADC value corresponding to 4.2V voltage*/
#define POWER_INPUT_ADC_4_5V          0x292    /*ADC value corresponding to 4.5V voltage*/
#define POWER_INPUT_ADC_5_5V          0x324    /*ADC value corresponding to 5.5V voltage*/
#define POWER_INPUT_ADC_6V            0x36D    /*ADC value corresponding to 6V voltage*/
#define POWER_INPUT_ADC_6_5V          0x3B6    /*ADC value corresponding to 6.5V voltage*/
#define POWER_INPUT_ADC_18V           0xA48    /*ADC value corresponding to 18V voltage*/
#define POWER_INPUT_ADC_18_5V         0xA91    /*ADC value corresponding to 18.5V voltage*/
#define POWER_INPUT_ADC_26V           0xED9    /*ADC value corresponding to 26V voltage*/
#define POWER_INPUT_ADC_26_5V         0xF23    /*ADC value corresponding to 26.5V voltage*/
#define POWER_INPUT_ADC_27V           0xF6C    /*ADC value corresponding to 27V voltage*/

#define POWER_INPUT_ADC_8_5V          0x4DA    /*ADC value corresponding to 8.5V voltage*/  

#define TENDENCY_MAP_LENGTH           8        /*power volatge mode number*/

/**********************************************************************************************
* Local types
**********************************************************************************************/

typedef enum
{
    INIT_LEVEL,                        /*init value when FIDM power on or wakeup*/
    NOT_OPERATIONAL_LOW,               /*not operation low mode*/
    PARTIALLY_OPERATION_LOW,           /*partially operation low mode*/
    FULLY_OPERATION_LEVEL,             /*fully operation mode*/
    PARTIALLY_OPERATION_HIGH,          /*partially operation high mode*/
    NOT_OPERATIONAL_HIGH,              /*not operation high mode*/
} POWER_ACTION_STATUS;

typedef enum
{
    INIT_TENDENCY,                     /*init value when FIDM power on or wakeup*/
    NOL_TO_POL,                        /*change not operation low mode to partially operation low mode*/
    POL_TO_FO,                         /*change partially operation low mode to fully operation mode*/
    FO_TO_POH,                         /*change fully operation mode to partially operation high mode*/
    POH_TO_NOH,                        /*change partially operation high mode to not operation high mode*/
    NOH_TO_POH,                        /*change not operation high mode to partially operation high mode*/
    POH_TO_FO,                         /*change partially operation high mode to fully operation mode*/
    FO_TO_POL,                         /*change fully operation mode to partially operation low mode*/   
    POL_TO_NOL,                        /*change partially operation low mode to not operation low mode*/
} POWER_STATUS_TENDENCY;
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CPowerManage_Init(void);
extern void CPowerManage_Deinit(void);
extern POWER_ACTION_STATUS CPowerManage_Get_PowerStatus(void);
extern POWER_ACTION_STATUS CPowerManage_Get_LastPowerStatus(void);
extern POWER_STATUS_TENDENCY CPowerManage_Get_PowerStatus_Tendency(void);
extern bool CPowerManage_Get_PowerStat_ChangeFlg(void);
extern void CPowerManage_Clear_PowerStat_ChangeFlg(void);
extern bool CPowerManage_Get_TendencyEndFlg(void);
extern void CPowerManage_Set_TendencyEndFlg(bool value);
extern void CPowerManage_Task(void);
#endif /* APP_POWER_MANAGE_H */
#endif /* POWER_MODULE */
