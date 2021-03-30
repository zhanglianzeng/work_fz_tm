/******************************************************************************
*  Name: Timer_if.h
*  Description:
*  Project: INFO4 11inch
*  Author: zhangqiang
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#ifndef TIMER_IF_H
#define TIMER_IF_H
#include "../../driver/tc.h"
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
typedef enum 
{
    DES_TIMER,                 /*des module*/
    SLEEP_TIMER,                 /*vol monitor*/
    POWER_TIMER,
    BOOT_TIMER,
    MAX_TIMER,
}TIMER_MODULE;

typedef enum 
{
    TIMER_TIMEOUT,             /*Timer has be timeout*/
    TIMER_RUN,                 /*Timer is running*/
    TIMER_STOP,                /*Timer is stopped*/
}TIMER_STATUS;

typedef struct
{
    uint16_t time;             /*timer value, ms*/
    TIMER_STATUS run_flag;     /*timer status flag*/
}TIMER_STRUCT;

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
extern void Timer_If_Async_init(void);
extern void Timer_If_Async_standby(void);
extern void Timer_If_Time_Count(void);
extern void Timer_If_Stop_Time(TIMER_MODULE module);
extern TIMER_STATUS Timer_If_Check_Timeout(TIMER_MODULE module);
extern bool Timer_If_Set_Timeout(TIMER_MODULE module, uint16_t time);
extern void Timer_If_Set_Callback(TC_MODULE module, Timer_handling callback);
#endif 
