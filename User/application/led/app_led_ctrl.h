/******************************************************************************
*  Name: app_led_ctrl.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LED_MODULE == 1)

#ifndef APP_LED_CTRL_H
#define APP_LED_CTRL_H

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
#define PWM_INIT_VALUE               0u     /* Remove the burr of PWM waveform after power on   reditip-101*/
#define PWM_DEFAULT_VALUE            9830u   /*PWM default value*/
#define PWM_DEFAULT_CRC_VALUE        153u   /*CRC 3 times, PWM default value*/
#define PWM_SOAK_MAX_VALUE           49151u /* PWM duty cycle is 75%*/
#define BRIGHTNESS_LAG_FILTER_CONST  810   /* the data divided by 1000 is the effective value*/
#define BRIGHTNESS_LAG_FILTER_RANGE  1000  /* the range data*/

#define OUTPUT_PWM_ZERO_VALUE  0u          /*0% PWM duty cycle value*/
#define OUTPUT_PWM_FULL_VALUE  65535u      /*100% PWM duty cycle  value*/

#define OUTPUT_PWM_MIN_PRECNET 1310u       /*2% pwm duty */
#define OUTPUT_PWM_MAX_PRECNET 64224u      /*98% pwm duty */
#define OUTPUT_PWM_DEFAULT     10026u      /*15.3% == 100nits  default value 100nit (100/650 = 15.38%) ,1000 pwm cycle. */

#define OVERTIME_7S5_COUNT_NUM 749u        /*7.5s timeout count threshold*/
#define OVERTIME_1S_COUNT_NUM  99u         /*1s timeout count threshold*/           

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef enum
{
    SIGNAL_FROM_IIC                         /*indicate information from iic communication*/
    , SIGNAL_FROM_POWER                     /*indicate information from power voltage management*/
    , SIGNAL_FROM_THERMAL                   /*indicate information from thermal management*/
    , SIGNAL_FROM_SOAK                      /*indicate information from soak management*/
    , SIGNAL_ABNORMAL                       /*indicate information abnormal*/
} SIGNAL_STATUS;

typedef enum
{
    THERMAL_ABNORMAL                        /*thermal abnormal state*/
    , THERMAL_NORMAL                        /*thermal normal state*/
    , THERMAL_BACKLIGHT                     /*thermal derating state*/
} THERMAL_STATUS;

typedef enum
{
    POWER_ABNORMAL                          /*power abnormal state*/
    , POWER_PARITAL_LOW                     /*power paritally mode state*/
    , POWER_NORMAL                          /*power fully mode state*/
} POWER_STATUS;
    
typedef enum
{
    SOAK_EXIT
    ,SOAK_ENTRY
} SOAK_STATUS;

typedef enum
{
    WAKEUP_1S                               /*wakeup 1s timeout*/
    , BACKLIGHT_7S5                         /*PWM command missing more than 7.5s */
    , TIMEOUT_CHANNEL_NUM                   /*timeout channel number  7*/
} ENUM_TIMEOUT_CHANNEL;

typedef struct 
{
    POWER_STATUS power_flag     ;               /*current power voltage status*/
    THERMAL_STATUS thermal_flag ;               /*current thermal status*/
    SOAK_STATUS soak_flag       ;               /*current soak status*/
    uint8_t iic_flag            ;               /*reception of current i2c command*/
} Led_Pwm_Str;

typedef struct
{
    uint16_t hCount;                        /*timeout count*/
    uint8_t  bCount_Enable : 1;             /*timeout count enable*/
    uint8_t  bTimeout_Flg  : 1;             /*timeout flag*/
} STRU_TIMEOUT;


/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern uint16_t CLedCtrl_Get_PWMCurrent_Value(void);
extern void CLedCtrl_Clean_Wakeup_Flag(void);
extern void CLedCtrl_Init(void);
extern void CLedCtrl_Task(void);
extern void CLedCtrl_Blen_Ctrl(bool level);
extern void CLedCtrl_PwmValue_Clear(void);
#endif /* _APP_LED_CTRL_H_ */
#endif /* LED_MODULE */
