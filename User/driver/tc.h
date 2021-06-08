/******************************************************************************
*  Name: tc.h
*  Description:
*  Project: Wave 2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#ifndef TC_H
#define TC_H

#include <stddef.h>
#include <stdint.h>
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define TC_HW_BASE_ADDR   ((uint32_t)TC0)


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
typedef void (*TC_Callback)();
#define TIMER_NUM   2	
#define INDONOF_PWM                  GPIO(GPIO_PORTB, 31)
#define REDLED_PWM                   GPIO(GPIO_PORTA, 7)
#define WTLED1_PWM                   GPIO(GPIO_PORTA, 1)
#define AMBLED_PWM                   GPIO(GPIO_PORTB, 3)
#define WTLED2_PWM                   GPIO(GPIO_PORTB, 1)
#define BL_PWM                       GPIO(GPIO_PORTB, 11)
#define SHILD_PWM                    GPIO(GPIO_PORTA, 19)
/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef void (*Timer_handling)(void);


typedef enum
{
    TIMER_TC,       /*Timer count, period is 1ms*/
    SWITCHPACK_TC,  /*switchpack period poll 1ms*/
    DEFAULT_TC,
}TC_MODULE;

typedef struct
{
    TC_MODULE       user;
    Timer_handling  P_Handling;
}TIMER_CALLBACK,*P_TIMER_CALLBACK;

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void TC_Clk_Init(void *tc_id);
extern void TC_Pwm_Init(void *tc_id);
extern void TC_Timer_Init(const void *const tc_id);
extern void TC_Init(void);
extern void TC_Set_Duty(void *tc_id, uint32_t duty);
extern void TC_Set_Period(void *tc_id, uint32_t period);
extern void TC_Timer_Enable(const void *const hw);
extern void TC_DeInit(const void *const hw);
extern void TC4_If_Set_Callback(P_TIMER_CALLBACK callback);
extern void TC_If_Timer_Init(void *tc_id);
//extern void TC3_If_Set_Callback(P_TIMER_CALLBACK callback);
#endif /* _TC_H_ */
