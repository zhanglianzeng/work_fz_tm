/******************************************************************************
*  Name: app_power_manage.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (POWER_MODULE == 1)
#include "app_power_manage.h"

#if (ADC_MODULE == 1)
#include "../../middle/adc/adc_if.h"
#endif


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
static POWER_ACTION_STATUS Power_Status;            /*the current power status*/
static POWER_ACTION_STATUS Power_Status_Last;       /*the last power status*/
static POWER_STATUS_TENDENCY Power_Status_Tendency; /*current power status change tendency*/
static bool bPowerStat_Change_Flg = true;           /*power status change flag*/
static bool bPowerTendency_End_Flg;                 /*power status change time sequence end flag*/
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Init(void)
* Visibility  :        public
* Description :        This function will initialize module.
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CPowerManage_Init(void)
{
    Power_Status = INIT_LEVEL;
    Power_Status_Tendency = INIT_TENDENCY;
    bPowerTendency_End_Flg = true;
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Get_PowerStatus(void)
* Visibility  :        public
* Description :        This function get current Power Status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  POWER_ACTION_STATUS 
* ===================================================================================================================*/
POWER_ACTION_STATUS CPowerManage_Get_PowerStatus(void)
{
    return Power_Status;  
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_PowerStatus_Tendency(POWER_ACTION_STATUS power_status_curr)
* Visibility  :        private
* Description :        This function calculate Power_Status_Tendency
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  POWER_STATUS_TENDENCY 
* ===================================================================================================================*/
static void CPowerManage_PowerStatus_Tendency(POWER_ACTION_STATUS power_status_curr)
{
    static const POWER_STATUS_TENDENCY Power_Tendency_Map[TENDENCY_MAP_LENGTH] = 
    {
        NOL_TO_POL,POL_TO_FO,FO_TO_POH,POH_TO_NOH,   /*UP*/
        POL_TO_NOL,FO_TO_POL,POH_TO_FO,NOH_TO_POH,   /*DN*/
    };

    if(power_status_curr > Power_Status_Last)        /*up*/ 
    {     
        Power_Status_Tendency = Power_Tendency_Map[(uint16_t)power_status_curr-2u];       /*Look-up table*/
    }
    else /*down*/
    {
        Power_Status_Tendency = Power_Tendency_Map[(uint16_t)power_status_curr+3u]; 
    }
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Get_PowerStatus_Tendency(void)
* Visibility  :        public
* Description :        This function get Power Status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  POWER_STATUS_TENDENCY 
* ===================================================================================================================*/
POWER_STATUS_TENDENCY CPowerManage_Get_PowerStatus_Tendency(void)
{    
    return Power_Status_Tendency;  
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Get_PowerStat_ChangeFlg(void)
* Visibility  :        public
* Description :        This function get Power Status change or not 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool CPowerManage_Get_PowerStat_ChangeFlg(void)
{
    return bPowerStat_Change_Flg;  
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Clear_PowerStat_ChangeFlg(void)
* Visibility  :        public
* Description :        This function clear Power Status change flag 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
void CPowerManage_Clear_PowerStat_ChangeFlg(void)
{
    bPowerStat_Change_Flg = false;  
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Status(uint32_t power_ADC)
* Visibility  :        public
* Description :        This function manages display status based on the power
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  POWER_ACTION_STATUS 
* ===================================================================================================================*/
static POWER_ACTION_STATUS CPowerManage_Status(uint32_t power_ADC)
{   
    POWER_ACTION_STATUS Power_Status_Curr;  
    /*After modifying the sequence, the QAC alarm no longer appears*/
    /*Different power voltage is corresponding to different status*/  
    if ((POWER_INPUT_ADC_26_5V < power_ADC) && (POWER_INPUT_ADC_27V > power_ADC))   /*Return difference interval*/
    {
        if(PARTIALLY_OPERATION_HIGH > Power_Status)           /*up Tendency*/
            Power_Status_Curr = PARTIALLY_OPERATION_HIGH;     /* Tendency change */
        else                                                  /*down Tendency*/
            Power_Status_Curr = Power_Status;                 /* Tendency unchange */
    }	
    else if((POWER_INPUT_ADC_18_5V <= power_ADC) && (POWER_INPUT_ADC_26_5V >= power_ADC))
    {
        Power_Status_Curr = PARTIALLY_OPERATION_HIGH;  
    }
    else if((POWER_INPUT_ADC_18V < power_ADC) && (POWER_INPUT_ADC_18_5V > power_ADC))  /*Return difference interval*/
    {
        if(FULLY_OPERATION_LEVEL >= Power_Status)              /*up Tendency*/
            Power_Status_Curr = FULLY_OPERATION_LEVEL;         /* Tendency change */
        else                                                   /*down Tendency*/
            Power_Status_Curr = PARTIALLY_OPERATION_HIGH;
    }		
    else if((POWER_INPUT_ADC_6_5V <= power_ADC) && (POWER_INPUT_ADC_18V >= power_ADC))                 
    {
        Power_Status_Curr = FULLY_OPERATION_LEVEL;
    }
    else if ((POWER_INPUT_ADC_6V < power_ADC) && (POWER_INPUT_ADC_6_5V > power_ADC))  /*Return difference interval*/
    {
        if(PARTIALLY_OPERATION_LOW >= Power_Status)           /*up Tendency*/
            Power_Status_Curr = PARTIALLY_OPERATION_LOW;      /* Tendency change */
        else                                                  /*down Tendency*/
            Power_Status_Curr = FULLY_OPERATION_LEVEL;
    }	
    else if((POWER_INPUT_ADC_4_5V <= power_ADC) && (POWER_INPUT_ADC_6V >= power_ADC))         
    {
        Power_Status_Curr = PARTIALLY_OPERATION_LOW;
    }

    else if ((POWER_INPUT_ADC_4_2V < power_ADC) && (POWER_INPUT_ADC_4_5V > power_ADC))  /*Return difference interval*/           
    {
        if(PARTIALLY_OPERATION_LOW < Power_Status)            /*up Tendency*/
            Power_Status_Curr = PARTIALLY_OPERATION_LOW;      /* Tendency change */
        else                                                  /*down Tendency*/
            Power_Status_Curr = Power_Status;                 /* Tendency unchange */
    } 
    else if(POWER_INPUT_ADC_4_2V >= power_ADC)
    {
        Power_Status_Curr = NOT_OPERATIONAL_LOW; 
    }
    else
    {
        Power_Status_Curr = NOT_OPERATIONAL_HIGH;
    }
    return Power_Status_Curr;
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Get_TendencyEndFlg(void)
* Visibility  :        private
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool CPowerManage_Get_TendencyEndFlg(void) 
{
    return bPowerTendency_End_Flg;   
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Get_TendencyEndFlg(void)
* Visibility  :        private
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
void CPowerManage_Set_TendencyEndFlg(bool value) 
{
   bPowerTendency_End_Flg = value;   
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_StatusChangeAllow(void)
* Visibility  :        private
* Description :        power status changes, allowing time sequence execution
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CPowerManage_StatusChangeAllow(void) 
{
    Power_Status_Last = Power_Status;   /*store the previous power status*/
	
    bPowerStat_Change_Flg = true;       /*power status change*/
    bPowerTendency_End_Flg = false;     /*start power mode manage*/    
}

/*=====================================================================================================================
* Upward trac.:        APP/power/App_power_manage
* Method      :        CPowerManage_Task(void)
* Visibility  :        public
* Description :        This function contain the task, it will be called by the cycle task.
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CPowerManage_Task(void)
{
#if (1 == ADC_MODULE)
    uint16_t adc_val = ADC_If_Get_Input_Vol();     
    POWER_ACTION_STATUS Power_Status_Curr = CPowerManage_Status(adc_val);

    /*INIT_LEVEL only appears in power on or wakeup initialization, and all module will init at this time*/
    if(INIT_LEVEL == Power_Status)
    {
        Power_Status = FULLY_OPERATION_LEVEL;
    }
    else
    {/*do nothing*/} 
    
    
    if((Power_Status != Power_Status_Curr)&& (true == bPowerTendency_End_Flg))
    { 
        CPowerManage_StatusChangeAllow();
        if(Power_Status > Power_Status_Curr)  /*down tendency*/
        {
           /*The management of the previous round is over before the next round can be started*/
            Power_Status --;                        
        }
        else    /*up tendency*/
        {
            Power_Status ++;            
        } 
        CPowerManage_PowerStatus_Tendency(Power_Status); 
    }
    else
    {/*do nothing*/}      
#endif
}

#endif /* POWER_MODULE */
