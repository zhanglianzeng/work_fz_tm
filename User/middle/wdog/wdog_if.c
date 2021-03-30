/******************************************************************************
*  Name: wdog_if.c
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (WDOG_MODULE == 1)
#include "../../driver/wdog.h"
#include "wdog_if.h"

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

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        WDog_If_Init(void)
* Visibility  :        public
* Description :        the watchdog module initialization
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void WDog_If_Init(void)
{
    WDOG_Init();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        WDog_If_Feed(void)
* Visibility  :        public
* Description :        Software feeding dog
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void WDog_If_Feed(void)
{
    WDOG_Feed();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        WDog_If_Enable(void)
* Visibility  :        public
* Description :        enable watchdog module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void WDog_If_Enable(void)
{
    WDOG_Enable();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        WDog_If_Disable(void)
* Visibility  :        public
* Description :        disable watchdog module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void WDog_If_Disable(void)
{
    WDOG_Disable();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/WDOG/WDOG_IF
* Method      :        WDog_If_Standby(void)
* Visibility  :        public
* Description :        deinit watchdog module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void WDog_If_Standby(void)
{
    WDOG_Disable();	
}

#endif

