/******************************************************************************
*  Name: app_thermal_manage.h
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (THERMAL_MODULE == 1)

#ifndef APP_THERMAL_MANAGE_H
#define APP_THERMAL_MANAGE_H

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

#define TEMPER_LED_ADC_BELOW_40C      3897
#define TEMPER_LED_ADC_BELOW_38C      3874
#define TEMPER_LED_ADC_BELOW_30C      3764
#define TEMPER_LED_ADC_ABOVE_75C       483
#define TEMPER_LED_ADC_ABOVE_80C       475
#define TEMPER_LED_ADC_ABOVE_85C       450 
#define TEMPER_LED_ADC_ABOVE_86C       445 /*withdraw value*/
#define TEMPER_LED_ADC_ABOVE_88C       439 
#define TEMPER_LED_ADC_INIT            800


#define TEMPER_PCBA_ADC_BELOW_40C      3897
#define TEMPER_PCBA_ADC_BELOW_38C      3874
#define TEMPER_PCBA_ADC_BELOW_30C      3764
#define TEMPER_PCBA_ADC_ABOVE_75C      483 
#define TEMPER_PCBA_ADC_ABOVE_80C      439 
#define TEMPER_PCBA_ADC_ABOVE_85C      390 
#define TEMPER_PCBA_ADC_ABOVE_86C      380 /*withdraw value*/
#define TEMPER_PCBA_ADC_ABOVE_88C      363 
#define TEMPER_PCBA_ADC_INIT           800


#define LED_ADC_ABOVE_95C              409

#define TEMPER_ADC_CALU_NUM       26 

#define TEMPER_ADC_OPEN    0XF5F
#define TEMPER_ADC_SHORT   0X64
#define TEMPER_COUTER_SPAN 3
#define NTC_RECOVER_INTERVAL 25




/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef enum
{
    THERMAL_FULLY_OPERATION_LEVEL               /*normal work mode, PWM duty cycle value allowable is 65535*/
    , THERMAL_BACKLIGHT_GRADUALLY_OFF_LEVEL     /*derating mode, PWM duty cycle value decreases linearly with the increase of temperature*/
    , THERMAL_PARTIALLY_OPERATION_LEVEL         /*abnormal mode, PWM duty cycle value clear to 0*/
    , THERMAL_INIT_LEVEL                        /*initialization mode*/
} THERMAL_ACTION_STATUS;

typedef enum
{
    THERMAL_FULLY_REGION                /**/
    , THERMAL_DERATING_REGION            /**/
    , THERMAL_WITHDRAW_REGION            /**/
    , THERMAL_PARTIALLY_REGION         
} THERMAL_REGION;

typedef struct
{
    int8_t temp;                                /*temperature value*/
    uint16_t adc_value;                         /*NTC adc value*/
}STRUTEMP_CALC;

typedef struct
{
	uint16_t adc_value;
	uint16_t normal_flag : 1;
	uint16_t short_flag : 1;
	uint16_t open_flag : 1;
	uint16_t short_counter : 2;
	uint16_t open_counter : 2;
	uint16_t recorver_counter : 2;
	uint16_t value_valid_flag : 1;
	uint16_t reserved : 6;
} THERMAL_NTC_STATUS;

typedef struct
{
	uint16_t thermal_value;
	bool lntc_is_active;
} THERMAL_VALUE;
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CThermalManage_Init(void);
extern void CThermalManage_Deinit(void);
extern THERMAL_ACTION_STATUS CThermalManage_Get_ThermalStatus(void);
extern THERMAL_VALUE CThermalManage_Get_ThermalValue(void);
extern int8_t CThermalManage_Get_PCB_NTC_Value(void);
extern int8_t CThermalManage_Get_LED_NTC_Value(void);
extern bool CThermalManage_Get_ThermalStat_ChangeFlg(void);
extern void CThermalManage_Task(void);

#endif /* _APP_THERMAL_MANAGE_H_ */
#endif /* THERMAL_MODULE */
