/******************************************************************************
*  Name: scheduler.c
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (SCHEDULER_APP == 1)

#include <stdio.h>
#include "../../../Device/samc21n18a.h"
#include "scheduler.h"
#include "../../driver/rtc.h"

#if (FILTER_MODULE == 1)
#include "../../middle/filter/filter_if.h"
#endif
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
/*scheduled task queue*/
static task_queue_struct task_queue[MAX_TASKS] = {
#if (FILTER_MODULE == 1)
    /*filter task is first running task when wakeup*/
    {Filter_Fast_Task, 0, 3, 0},
#else
    {NULL, 0, 0, 0},
#endif
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
    {NULL, 0, 0, 0},
};

#if (CPU_LOAD_APP == 1)

float cpu_load_rate;        /*CPU load rate*/
uint8_t  stamp_nest = 0;    /*Scheduler task status*/
uint64_t start_time = 0;    /*Scheduler module start Timestamp*/
uint64_t load_time  = 0;    /*CPU load time*/
uint64_t enter_time = 0;    /*CPU load enter time*/
uint64_t leave_time = 0;    /*CPU load leave time*/

#endif /* CPU_LOAD_APP */

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void CScheduler_Update(void);
static bool CScheduler_Delete_Task_Index(uint16_t index);

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief    scheduler module initialization function.
* @param    void
* @return   void
*
************************************************************************************************/
void CScheduler_Init(void)
{   
    //RTC_Init(); //Put RTC init at bsp init for touch
    RTC_Stop();
    NVIC_DisableIRQ(RTC_IRQn);
    RTC_Set_Callback(CScheduler_Update);
    RTC_Set_COMP_VAL_to_original();
    RTC_Start();
    NVIC_EnableIRQ(RTC_IRQn);
    
}

/***********************************************************************************************
*
* @brief    scheduler module start up function.
* @param    void
* @return   void
*
************************************************************************************************/
void CScheduler_Start(void)
{
#if (1 == CPU_LOAD_APP)
    CScheduler_Add_Task(CScheduler_CPULoad_Task, 1500, 1000);  
    start_time = RTC_Read_Timestamp();
#endif /* CPU_LOAD_APP */
  
    //RTC_Start(); Put RTC Start at init for touch
}

/***********************************************************************************************
*
* @brief    scheduler module stop function.
* @param    void
* @return   void
*
************************************************************************************************/
void CScheduler_Stop(void)
{  
    //RTC_Stop(); //Don't stop RTC for touch.

    /* clear run count value */
    for(uint16_t i = 0; i < MAX_TASKS; i++)
    {
        if((0u < task_queue[i].run_count) && (NULL != task_queue[i].p_task) )
        {
            task_queue[i].run_count = 0;
        }
        else
        {/*do nothing*/}
    }
}

/***********************************************************************************************
*
* @brief    scheduler module task dispatch function.
* @param    void
* @return   void
*
************************************************************************************************/
void CScheduler_Dispatch(void)
{
    for(uint16_t i = 0; i < MAX_TASKS; i++)
    {
        if( (task_queue[i].run_count > 0u) && (NULL != task_queue[i].p_task) )
        {
#if (1 == CPU_LOAD_APP)
            if(CScheduler_CPULoad_Task != task_queue[i].p_task)
            {
                CScheduler_Enter_Task();
            }
            else
            {/*do nothing*/}
#endif /* CPU_LOAD_APP */

            task_queue[i].p_task();
            task_queue[i].run_count -= 1u;

            if(0u == task_queue[i].period)
            {
                (void)CScheduler_Delete_Task_Index(i);
            }
            else
            {/*do nothing*/}
#if (CPU_LOAD_APP == 1)
            if(CScheduler_CPULoad_Task != task_queue[i].p_task)
            {
                CScheduler_Leave_Task();
            }
            else
            {/*do nothing*/}
#endif /* CPU_LOAD_APP */
        }
        else
        {/*do nothing*/}
    }
}

/***********************************************************************************************
*
* @brief    scheduler module update task struct function, this function as RTC's callback function.
* @param    void
* @return   void
*
************************************************************************************************/
static void CScheduler_Update(void)
{
#if (1 == CPU_LOAD_APP)
    CScheduler_Enter_Task();
#endif /* CPU_LOAD_APP */

    for(uint16_t i = 0; i < MAX_TASKS; i++)
    {
        if (NULL != task_queue[i].p_task)
        {
            if (1u >= task_queue[i].delay)          
            {
                task_queue[i].run_count += 1u;      /*run_count > 0, task_queue[i].task need execute*/
                if (0u != task_queue[i].period)
                {
                    task_queue[i].delay = task_queue[i].period;
                }
                else
                {/*do nothing*/}
            }
            else
            {
                task_queue[i].delay -= 1u;
            }
        }
        else
        {/*do nothing*/}
    }

#if (1 == CPU_LOAD_APP)
    CScheduler_Leave_Task();
#endif /* CPU_LOAD_APP */
}

/***********************************************************************************************
*
* @brief    scheduler module add task function.
* @param    task pointer, delay value, period value.
* @return   int16_t, task index.
*
************************************************************************************************/
int16_t CScheduler_Add_Task(void (*p_task)(void), uint16_t delay, uint16_t period)
{
    int16_t return_val;
    uint16_t index = 0;
    if(NULL == p_task)
    {
        return_val = -1;
    }
    else
    {
        while ((MAX_TASKS > index) && (NULL != task_queue[index].p_task)) 
        {
            index++;        
        }
        if (MAX_TASKS == index)
        {
            return_val = -1;
        }
        else
        {
            task_queue[index].p_task    = p_task;
            task_queue[index].delay     = delay;
            task_queue[index].period    = period;
            task_queue[index].run_count = 0;
            return_val = (int16_t)index;
        }                    

    }   
    return return_val;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/SCHEDULER/scheduler.c
* Method      :        CScheduler_Delete_Task(void (*p_task)())
* Visibility  :        public
* Description :        scheduler module delete task function.
* Parameters  :        task pointer.
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        ibool, task delete successful or not.
* ===================================================================================================================*/
bool CScheduler_Delete_Task(void (*p_task)(void))
{
    bool return_val;
    uint16_t index = 0;
    if(NULL == p_task)
    {
        return_val = false;
    }
    else
    {
        while ((task_queue[index].p_task != p_task) && (MAX_TASKS > index))
        {
            index++;
        }
        if (MAX_TASKS == index)
        {
            return_val = false;
        }
        else
        {
            task_queue[index].p_task    = NULL;
            task_queue[index].delay     = 0;
            task_queue[index].period    = 0;
            task_queue[index].run_count = 0;
            return_val = true;
        }    
    }
    return return_val;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/SCHEDULER/scheduler.c
* Method      :        CScheduler_Delete_Task_Index(uint16_t index)
* Visibility  :        public
* Description :        scheduler module delete task function.
* Parameters  :        task index.
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        bool, task delete successful or not.
* ===================================================================================================================*/
static bool CScheduler_Delete_Task_Index(uint16_t index)
{
    bool retun_val = true;
    if (MAX_TASKS <= index)
    {
        retun_val = false;
    }
    else
    {
        task_queue[index].p_task    = NULL;
        task_queue[index].delay     = 0;
        task_queue[index].period    = 0;
        task_queue[index].run_count = 0;
    }
    
    return retun_val;
}

#if (1 == CPU_LOAD_APP)
/***********************************************************************************************
*
* @brief    CPU load module record enter task or interrupt timestamp function.
* @param    void
* @return   void
*
************************************************************************************************/
void CScheduler_Enter_Task(void)
{
    if(0 == stamp_nest)
    {
        enter_time = RTC_Read_Timestamp();
    }
    else
    {/*do nothing*/}

    stamp_nest++;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/SCHEDULER/scheduler.c
* Method      :        CScheduler_Leave_Task(void)
* Visibility  :        public
* Description :        CPU load module record leave task or interrupt timestamp function.
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void CScheduler_Leave_Task(void)
{
    stamp_nest--;

    if(0 == stamp_nest)
    {
        leave_time = RTC_Read_Timestamp();
        load_time += leave_time - enter_time;
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/SCHEDULER/scheduler.c
* Method      :        CScheduler_Get_CPULoad(void)
* Visibility  :        public
* Description :        CPU load module get load rate function.
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        float, CPU load rate.
* ===================================================================================================================*/
float CScheduler_Get_CPULoad(void)
{
    return cpu_load_rate;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/SCHEDULER/scheduler.c
* Method      :        CScheduler_CPULoad_Task(void)
* Visibility  :        public
* Description :        CPU load module task for calculate load rate function.
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void CScheduler_CPULoad_Task(void)         /*Calculate CPU load*/
{
    __disable_irq();

    if(0 == stamp_nest)
    {
        cpu_load_rate = (float)(load_time) / (float)(leave_time - start_time);
        start_time = leave_time;
        load_time = 0;
    }
    else
    {/*do nothing*/}

    __enable_irq();

#if (1 == CPU_LOAD_PRINTF)
    printf("cpu load: %04.02f%% .\n", CScheduler_Get_CPULoad() * 100);
#endif /* CPU_LOAD_PRINTF */
}

#endif /* CPU_LOAD_APP */


#if (1 == SCHEDULER_TEST)

/***********************************************************************************************
*
* @brief    scheduler module test task function.
* @param    void
* @return   void
*
************************************************************************************************/
void Test_Task_A(void)
{
    printf("Test task A running.\n");
}

/***********************************************************************************************
*
* @brief    scheduler module test task function.
* @param    void
* @return   void
*
************************************************************************************************/
void Test_Task_End(void)
{
    printf("Test task end running.\n");
    printf("====  Test pattern case 5: delete all task.  ====\n");

    bool ret = CScheduler_Delete_Task(Test_Task_A);

    if (true == ret)
    {
        printf("Task A delete successful.\n");
    }
    else
    {
        printf("Task A delete fail.\n");
    }

    ret = CScheduler_Delete_Task(Test_Task_End);
    if (true == ret)
    {
        printf("Task end itself delete successful.\n");
        printf("====  Test pattern case 5 is pass.  ====\n");
        printf("********  Scheduler test pattern is complete.  ********\n");
    }
    else
    {
        printf("====  Test pattern case 5 is fail.  ====\n");
    }
}

/***********************************************************************************************
*
* @brief    scheduler module test task function.
* @param    void
* @return   void
*
************************************************************************************************/
void Test_Task_C(void)
{
    printf("Test task C running.\n");
}

/***********************************************************************************************
*
* @brief    scheduler module test task function.
* @param    void
* @return   void
*
************************************************************************************************/
void Test_Task_B(void)
{
    printf("One time test task B running.\n");
    printf("====  Test pattern case 3: delete task.  ====\n");
    bool ret = CScheduler_Delete_Task(Test_Task_C);
    if (true == ret)
    {
        printf("Delete task C successful.\n");
        printf("====  Test pattern case 3 is pass.  ====\n");
    }
    else
    {
        printf("====  Test pattern case 3 is fail.  ====\n");
    }

    printf("====  Test pattern case 4: insert task.  ====\n");
    int16_t ret_val = CScheduler_Add_Task(Test_Task_End, 10, 0);
    if (-1 == ret_val)
    {
        printf("====  Test pattern case 4 is fail.  ====\n");
    }
    else
    {
        printf("Insert end task successful.\n");
        printf("====  Test pattern case 4 is pass.  ====\n");
    }
}

/***********************************************************************************************
*
* @brief    scheduler module test task function.
* @param    void
* @return   void
*
************************************************************************************************/
void Test_Task_D(void)
{
    printf("====  Test pattern case 2: one time task.  ====\n");

    printf("Test task D running.\n");
    printf("Test task D is one time task, not run again.\n");

    printf("====  Test pattern case 2 is pass.  ====\n");
}

/***********************************************************************************************
*
* @brief    scheduler module test pattern function.
* @param    void
* @return   void
*
************************************************************************************************/
void CScheduler_Test_Pattern(void)
{
    printf("********  Scheduler test pattern start.  ********\n");

    CScheduler_Init();
    CScheduler_Start();

    printf("Scheduler initialization is complete.\n");

    printf("====  Test pattern case 1: add task.  ====\n");

    int16_t ret;

    ret = CScheduler_Add_Task(Test_Task_A, 0, 50);
    if (-1 == ret)
    {
        printf("Test task A add fail.\n");
        return;
    }
    else
    {
        printf("Test task A add successful.\n");
    }

    ret = CScheduler_Add_Task(Test_Task_B, 100, 0);
    if (-1 == ret)
    {
        printf("Test task B add fail.\n");
        return;
    }
    else
    {
        printf("Test task B add successful.\n");
    }

    ret = CScheduler_Add_Task(Test_Task_C, 10, 30);
    if (-1 == ret)
    {
        printf("Test task C add fail.\n");
        return;
    }
    else
    {
        printf("Test task C add successful.\n");
    }

    ret = CScheduler_Add_Task(Test_Task_D, 5, 0);
    if (-1 == ret)
    {
        printf("Test task D add fail.\n");
        return;
    }
    else
    {
        printf("Test task D add successful.\n");
    }

    printf("====  Test pattern case 1 is pass.  ====\n");
}

#endif /* SCHEDULER_TEST */

#endif /* SCHEDULER_APP */
