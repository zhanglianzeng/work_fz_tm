/******************************************************************************
*  Name: timer_if.c
*  Description:
*  Project: wave2 17.7
*  Author: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "timer_if.h"
#include "../../driver/config/hpl_tc_config.h"
#include "../../../Device/samc21n18a.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static TIMER_STRUCT Timer_List[]={
    {0, TIMER_STOP}, /*default is 0ms, user is DES_TIMER*/
    {0, TIMER_STOP}, /*default is 0ms, user is VOL_TIMER*/
    {0, TIMER_STOP}, /*default is 0ms, user is POWER_TIMER*/
    {0, TIMER_STOP}, /*default is 0ms, user is BOOT_TIMER*/
};

/*periodicity callback function, period is 1ms*/
static TIMER_CALLBACK Timer_Callback_List[TIMER_NUM]={
    {TIMER_TC,   Timer_If_Time_Count}, /*1ms Timer*/
    {DEFAULT_TC, NULL},                /*default inited setting*/
};


/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Time_Count(void)
* Visibility  :        public
* Description :        all timer count
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Timer_If_Time_Count(void)
{
    uint8_t timer_num = sizeof(Timer_List)/sizeof(TIMER_STRUCT);

    for (uint8_t i=0; i<timer_num; i++)
    {
        /*current timer has started*/
        if ((TIMER_RUN==Timer_List[i].run_flag))
        {
            if (Timer_List[i].time>1)
            {
                Timer_List[i].time--;
            }
            else
            {
                /*clear time*/
                Timer_List[i].time = 0;
                /*current timer is timeout*/
                Timer_List[i].run_flag = TIMER_TIMEOUT;
            }
        }else
        { /*do nothing*/}
    }
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Init(void)
* Visibility  :        public
* Description :        Tc module init
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Timer_If_Init(void)
{
    /*current 1ms timer is based on TC4*/
    TC_If_Timer_Init(TC4);
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Deinit(void)
* Visibility  :        public
* Description :        deinit Tc module
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Timer_If_Deinit(void)
{
    /*current 1ms timer is based on TC4*/
    TC_DeInit(TC4);
}

/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        Timer_If_Set_Callback
* Visibility  :        public
* Description :        set periodicity callback function
* Parameters  :        TC_MODULE module, Timer_handling callback
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void Timer_If_Set_Callback(TC_MODULE module, Timer_handling callback)
{
    for (uint8_t i =0; i<TIMER_NUM; i++)
    {
        /*lookup corresponding Timer user*/
        if (module == Timer_Callback_List[i].user)
        {
            Timer_Callback_List[i].P_Handling = callback;
            break;
        }else
        { /*do nothing*/}
    }
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Async_init(void)
* Visibility  :        public
* Description :        asynchronous timer module init
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Timer_If_Async_init(void)
{
    /*current timer is based on TC4*/
    TC4_If_Set_Callback(Timer_Callback_List);
    /*TC Init*/
    Timer_If_Init();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Async_standby(void)
* Visibility  :        public
* Description :        asynchronous timer module deinit
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Timer_If_Async_standby(void)
{
    /*TC DeInit*/
    Timer_If_Deinit();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Set_Timeout(void)
* Visibility  :        public
* Description :        set timer time value
* Parameters  :        TIMER_MODULE module, uint16_t time
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
bool Timer_If_Set_Timeout(TIMER_MODULE module, uint16_t time)
{
    bool set_result = false;

    if (module < MAX_TIMER)
    {
        Timer_List[module].time = time;
        /*start current timer*/
        Timer_List[module].run_flag = TIMER_RUN;
        set_result = true;
    }else
    { /*do nothing*/}

    return set_result;
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Check_Timeout(void)
* Visibility  :        public
* Description :        check current timer timeout
* Parameters  :        TIMER_MODULE module
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
TIMER_STATUS Timer_If_Check_Timeout(TIMER_MODULE module)
{
    TIMER_STATUS timeout = TIMER_STOP;

    if (module < MAX_TIMER)
    {
        timeout = Timer_List[module].run_flag;
        if (TIMER_TIMEOUT==timeout)
        {
            /*stop timer when system find timer is end*/
            Timer_List[module].time = 0;
            Timer_List[module].run_flag = TIMER_STOP;
        }else
        {/*do nothing*/}
    }else
    {
        timeout = TIMER_STOP;
    }

    return timeout;
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        Timer_If_Set_Timeout(void)
* Visibility  :        public
* Description :        set timer time value
* Parameters  :        TIMER_MODULE module, uint16_t time
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Timer_If_Stop_Time(TIMER_MODULE module)
{
    if (module < MAX_TIMER)
    {
        Timer_List[module].time = 0;
        /*stop current timer*/
        Timer_List[module].run_flag = TIMER_STOP;
    }else
    { /*do nothing*/}
}

