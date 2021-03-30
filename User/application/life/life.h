/******************************************************************************
*  Name: life.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (1 == LIFE_APP)
#include <stdint.h>

#ifndef LIFE_H
#define LIFE_H

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define SLEEP_WAIT_MODE            0   /*sleep mode selection*/
#define LIFE_RECOVER_TIMES         3   /*diagnostic recover times according to the PDD*/
#define LIFE_LOCALWAKEUP_OVERTIME  100 /*slave request to master timeout time when work in local wakeup mode*/

#define WAKEUP_STATUS                1
#define SLEEP_STATUS                 0

typedef void (*Sched_Task)(void);

typedef struct
{
    Sched_Task task;
    uint8_t delay;
    uint16_t period;
}SCHED_TASK;
/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef enum
{
    SLEEP_MODE = 1,       /*sleep mode according to the statemachine*/
    WORK_MODE,            /*work mode according to the statemachine*/
    GOTO_SLEEP,           /*goto sleep mode according to the statemachine*/
    GOTO_WORK             /*goto work mode according to the statemachine*/
} Sleep_Status_Enum;

typedef enum
{
    STEP1,                /*The first step of sequence*/
    STEP2,                /*The second step of sequence*/
    STEP3,                /*The third step of sequence*/
    STEP4,                /*The fourth step of sequence*/
    STEP5,                /*The fifth step of sequence*/
    STEP_END = 0xFE       /*The last step of sequence*/
} ENUM_STEP;
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CLife_Task(void);

#endif /* _LIFE_H_ */
#endif /* LIFE_APP */
