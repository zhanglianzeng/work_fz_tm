/******************************************************************************
*  Name: .c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#include "../../Device/samc21n18a.h"
#include "bsp_if.h"
#include "../driver/tc.h"
#include "../driver/tcc.h"
/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        BSP/Bsp_if
* Method      :        Bsp_If_Init(void)
* Visibility  :        public
* Description :        init bsp hardware module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
void Bsp_If_Init(void)
{
#if (1 == CLOCK_MODULE)
    Clk_If_Init();
#endif
#if (ADC_MODULE == 1)
    ADC_If_Init();
#endif
#if (EEPROM_MODULE == 1)
    CEeprom_Init();
#endif
#if (POST_MODULE == 1)
    Post_RamInit();
#endif
#if ((FILTER_MODULE == 1) && (SWITCHPACK_MODULE == 1))
    Filter_GpioInit();
#endif
#if (SLEEP_MODULE == 1)
    Sleep_If_Init();
#endif
#if (DESERIALIZER_MODULE == 1)
    CI2CComm_Init();
#endif
#if (WDOG_MODULE == 1)
    WDog_If_Init();
#endif
#if (DMAC_MODULE == 1)
    DMAC_Init();
#endif
#if (EVSYS_MODULE == 1)
    EVSYS_init();
#endif
#if (PTC_MODULE == 1)
    Touch_button_init();
    RTC_Init();
    RTC_Start();
    TC_Init();
#endif
}
