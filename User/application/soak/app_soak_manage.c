/******************************************************************************
*  Name: app_soak_manage.c
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:  
******************************************************************************/
#include "../../config.h"
#if (SOAK_MODULE == 1)
#include "app_soak_manage.h"
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
    
/**********************************************************************************************
* Local types
**********************************************************************************************/
    
/**********************************************************************************************
* Local variables
**********************************************************************************************/
static SOAK_ACTION_STATUS Soak_Status;    /*work mode of SOAK module*/
static uint16_t hSoak_OverTime_5m_Count;  /*5minute overtime count*/
/**********************************************************************************************
* Local functions
**********************************************************************************************/
    
/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        APP/soak/App_soak_manage
* Method      :        CSoakManage_Init(void)
* Visibility  :        public
* Description :        Soak control init function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CSoakManage_Init(void)
{
    hSoak_OverTime_5m_Count = 0;      /*clear 5minute overtime count*/
    Soak_Status = SOAK_INIT_LEVEL;    /*init Soak_Status*/
}
/*=====================================================================================================================
* Upward trac.:        APP/soak/App_soak_manage
* Method      :        CSoakManage_Get_SoakStatus(void)
* Visibility  :        public
* Description :        Get Soak Status
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  SOAK_ACTION_STATUS 
* ===================================================================================================================*/
SOAK_ACTION_STATUS CSoakManage_Get_SoakStatus(void)
{
    return Soak_Status;
}
   
/*=====================================================================================================================
* Upward trac.:        APP/soak/App_soak_manage
* Method      :        CSoakManage_Timeout_Handle(void)
* Visibility  :        public
* Description :        5 minutes timeout handle function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static uint8_t CSoakManage_Timeout_Handle(void)
{    
    uint8_t timeout_flg = false;
    /*judge 5 minutes overtime or not*/
    if(OVERTIME_5M_COUNT_NUM > hSoak_OverTime_5m_Count)
        hSoak_OverTime_5m_Count ++; 
    else
    {
        timeout_flg = true;        /*set 5 minutes overtime flag*/
        hSoak_OverTime_5m_Count = 0;        
    }
    return timeout_flg;    
}
    
/*=====================================================================================================================
* Upward trac.:        APP/soak/CSoakManage_Task
* Method      :        CPowerManage_Soak_Task(void)
* Visibility  :        public
* Description :        soak status switching
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CSoakManage_Task(void)
{
    switch(Soak_Status)  
    {
        case SOAK_INIT_LEVEL:
            Soak_Status = SOAK_FULLY_OPERATION_LEVEL;   /*SOAK work mode, continue 5 minutes when FIDM wakeup or power on*/
            break;
        case SOAK_FULLY_OPERATION_LEVEL:
            if(CSoakManage_Timeout_Handle())
                Soak_Status = SOAK_NOT_OPERATION_LEVEL; /*if 5 minutes timeout, exit soak work mode*/
            else
            {/*do nothing*/}
            break;
        case SOAK_NOT_OPERATION_LEVEL:
        default:
            break;    
    }    
}
    
#endif /* SOAK_MODULE */

