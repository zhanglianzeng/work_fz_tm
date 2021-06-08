/******************************************************************************
*  Name: scheduler.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (SCHEDULER_APP == 1)

#ifndef SCHEDULER_H
#define SCHEDULER_H

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
#define MAX_TASKS       20u      /*Maximum number of tasks*/

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef struct task_queue_struct_type
{
    void (*p_task)();                       /*task pointer*/
    uint16_t delay;                         /*delay time*/
    uint16_t period;                        /*task period*/
    uint8_t run_count;                      /*Number of times not executed*/
} task_queue_struct, *p_task_queue_struct;

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CScheduler_Init(void);
extern void CScheduler_Start(void);
extern void CScheduler_Stop(void);
extern void CScheduler_Dispatch(void);
extern int16_t CScheduler_Add_Task(void (*p_task)(), uint16_t delay, uint16_t period);
extern bool CScheduler_Delete_Task(void (*p_task)());
#if (1 == CPU_LOAD_APP)
extern void CScheduler_Enter_Task(void);
extern void CScheduler_Leave_Task(void);
extern float CScheduler_Get_CPULoad(void);
extern void CScheduler_CPULoad_Task(void);
#endif /* CPU_LOAD_APP */

#if (1 == SCHEDULER_TEST)
extern void CScheduler_Test_Pattern(void);
#endif /* SCHEDULER_TEST */

#endif /* _SCHEDULER_H_ */
#endif /* SCHEDULER_APP */
