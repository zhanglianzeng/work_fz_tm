/******************************************************************************
*  Name: power_if.c
*  Description:
*  Project: 
*  Auther:
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (POWER_MODULE == 1)
#include "../../driver/gpio.h"
#include "../../driver/delay.h"
#include "../../driver/tc.h"

#if (DESERIALIZER_MODULE == 1)
#include "../deserializer/deserializer_if.h"
#endif

#include "power_if.h"

/*Touch Reset : In order to meet the timing requirements, put it here temporarily when touch module off*/
#define TOUCH_RESET  1

#if (1 == TOUCH_RESET)
#define TP_EXT_RSTN            GPIO(GPIO_PORTA, 24)
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
static volatile uint8_t byTc_Counter = 0;
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
#if (1 == TOUCH_RESET)
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Touch_If_Set_Rstn
* Visibility  :        public
* Description :        set lcd bridge pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Touch_If_Set_Rstn(bool level)
{
	/*set bridge RSTN pin to output mode*/
    GPIO_Set_Direction(TP_EXT_RSTN, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(TP_EXT_RSTN, GPIO_PIN_FUNCTION_OFF);
    if(level)
        GPIO_Set_Level(TP_EXT_RSTN, true);   
    else
        GPIO_Set_Level(TP_EXT_RSTN, false);  
}
#endif
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_WD_Clk_Out(void)    
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_WD_Clk_Out(void)
{
    GPIO_Set_Direction(WDO, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(WDO, GPIO_PIN_FUNCTION_H);
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_WD_Clk_Close(void)    
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_WD_Clk_Close(void)
{
	/*Power_3v3_WD disable*/
    GPIO_Set_Direction(WDO, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(WDO, GPIO_PIN_FUNCTION_OFF);  
    GPIO_Set_Level(WDO, false);
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Set_5VEN_Level(bool level)   
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_Set_5VEN_Level(bool level)
{
    /*set 5V Converter to output for 11.3inch and 11ICS project*/
    GPIO_Set_Direction(POWER_5V, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(POWER_5V, GPIO_PIN_FUNCTION_OFF);

    if(level)
    {
        GPIO_Set_Level(POWER_5V, true);
    }
    else
    {
        GPIO_Set_Level(POWER_5V, false);
    }
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Set_5VEN_Level(bool level)   
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_Set_3_3VEN_Level(bool level)
{
    /*set 5V Converter to output for 11.3inch and 11ICS project*/
    GPIO_Set_Direction(POWER_3_3V, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(POWER_3_3V, GPIO_PIN_FUNCTION_OFF);

    if(level)
    {
        GPIO_Set_Level(POWER_3_3V, true);
    }
    else
    {
        GPIO_Set_Level(POWER_3_3V, false);
    }
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Set_5VEN_Level(bool level)   
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_Set_BAT_DET_EN_Level(bool level)
{
    GPIO_Set_Direction(BAT_DET_EN, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(BAT_DET_EN, GPIO_PIN_FUNCTION_OFF);

    if(level)
    {
        GPIO_Set_Level(BAT_DET_EN, true);
    }
    else
    {
        GPIO_Set_Level(BAT_DET_EN, false);
    }
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Power_If_Set_PDB_Level
* Visibility  :        public
* Description :        Set Des PDB level   
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Power_If_Set_PDB_Level(bool level)
{
#if (DESERIALIZER_MODULE == 1)
    Des_If_Set_PDB_Level(level);
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Power_If_EN
* Visibility  :        public
* Description :        Set Des PDB level   
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Power_If_EN(void)
{
    Power_If_Set_3_3VEN_Level(true);
    Power_If_WD_Clk_Out();
    Power_If_Set_5VEN_Level(true);
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Power_If_DIS
* Visibility  :        public
* Description :        Set Des PDB level   
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Power_If_DIS(void)
{
    Power_If_Set_3_3VEN_Level(false);
    Power_If_WD_Clk_Close();
    Power_If_Set_5VEN_Level(false);
}


/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Init(void)   
* Visibility  :        public
* Description :        PMIC initialization
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_Init(void)
{
    GPIO_Set_Direction(PMIC_FAULT, GPIO_DIRECTION_IN); 
    GPIO_Set_Pull_Mode(PMIC_FAULT, GPIO_PULL_UP);
    GPIO_Set_Function(PMIC_FAULT, GPIO_PIN_FUNCTION_OFF);

    Delay_Ms(1);
	Power_If_EN();
    Delay_Ms(5);
   
    Power_If_Set_BAT_DET_EN_Level(true);
	  
}

/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Standby(void)   
* Visibility  :        public
* Description :        This function is to deinitialize the gpio pin
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Power_If_Standby(void)
{
    /*according to HSIS20210309*/
    Delay_Ms(10);
	 
    /*Power disable*/ 
    Power_If_DIS();
    #if (1 == TOUCH_RESET)
    Touch_If_Set_Rstn(false); 
    #endif
	
    Power_If_Set_BAT_DET_EN_Level(false);
	
    GPIO_Set_Direction(PMIC_FAULT, GPIO_DIRECTION_OFF);           
    GPIO_Set_Pull_Mode(PMIC_FAULT, GPIO_PULL_OFF);
    GPIO_Set_Function(PMIC_FAULT, GPIO_PIN_FUNCTION_OFF);    
}

/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Get_Fault_Level(void)   
* Visibility  :        public
* Description :        Get PMIC fault status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool Power_If_Get_Fault_Level(void)
{
    return GPIO_Get_Level(PMIC_FAULT);   /*Low Level Effective*/
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Get_Fault_Level(void)   
* Visibility  :        public
* Description :        Get power manage fault
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool Power_If_Get_VDD_3V3_PG_Level(void)
{
    return GPIO_Get_Level(VDD_3V3_PG);   /*Low Level Effective*/
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Get_Fault_Level(void)   
* Visibility  :        public
* Description :        Get power manage fault
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool Power_If_Get_HVAC_5V_PG_Level(void)
{
    return GPIO_Get_Level(HVAC_5V_PG);   /*Low Level Effective*/
}
/*=====================================================================================================================
* Upward trac.:        Middle/power/power_if
* Method      :        Power_If_Get_Fault_Level(void)   
* Visibility  :        public
* Description :        Get power manage fault
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool Power_If_Get_VDD_1V8_PG_Level(void)
{
    return GPIO_Get_Level(VDD_1V8_PG);   /*Low Level Effective*/
}

#endif /* POWER_MODULE */
