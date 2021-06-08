
/******************************************************************************
*  Name: main.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "bsp/bsp_if.h"

#if (SCHEDULER_APP == 1)
#include "application/scheduler/scheduler.h"
#endif

#if (BOOT_APP == 1)
#include "application/boot/boot.h"
#endif

#if (LIFE_APP == 1)
#include "application/life/life.h"
#endif

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        main
* Method      :        main(void)
* Visibility  :        public
* Description :        Program entry function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
int main(void)
{     
    Bsp_If_Init();                                    /*board support package*/
	
    while(1)
    {

        #if (1 == SCHEDULER_APP)
        CScheduler_Dispatch();     /*call task function when run_count not 0*/
        #endif
        
        #if (1 == BOOT_APP)
        CBoot_Check();            /*check bootloader command whether received*/
        #endif
        
        #if (1 == LIFE_APP)
        CLife_Task();            /*lifecycle task, sleep-gotowork-work-gotosleep-sleep*/
        #endif

    }	  
}

