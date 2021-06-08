/******************************************************************************
*  Name: filter_if.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"

#if (FILTER_MODULE == 1)
#include <stdint.h>
#include <stdio.h>
#include "../../../Device/samc21n18a.h"
#include "filter_if.h"
#include "../../driver/gpio.h"
#include "../../driver/delay.h"

#if (SLEEP_MODULE == 1)
#include "../sleep/sleep_if.h"
#endif

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
/*master wakeup*/
Str_Filter Master_Wake = {0, 1};              /*signal default level and stablization count for Master_Wake*/
static bool bFilter_Stablization_Flg = false; /*remote wakeup and local wakeup signal stabilization flag*/
/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define FILTER_WAKE_TIME_IN_MS  5
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
* Upward trac.:        Middle/filter/filter_if
* Method      :        Filter_Get_StablizetionFlg(void)
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        bool
* ===================================================================================================================*/
bool Filter_Get_StablizetionFlg(void)
{
    return bFilter_Stablization_Flg; /*return wakeup signal stablization flag*/
}

/*=====================================================================================================================
* Upward trac.:        Middle/filter/filter_if
* Method      :        Filter_Clear_StablizationFlg(void)
* Visibility  :        public
* Description :        Clear remote wakeup and local wakeup signal stabilization flag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Filter_Clear_StablizationFlg(void)
{
    bFilter_Stablization_Flg = false; /*Clear wakeup signal stablization flag*/
}

/*=====================================================================================================================
* Upward trac.:        Middle/filter/filter_if
* Method      :        Filter_Fast_GetLevel(Str_Filter *pData, uint8_t gpio)
* Visibility  :        private
* Description :        Get remote and local wakeup signal, judge whether the signal changes, and count.
* Parameters  :        Str_Filter *pData, uint8_t gpio
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        bool
* ===================================================================================================================*/
static bool Filter_Fast_GetLevel(Str_Filter *pData, uint8_t gpio)
{
    bool sample_level;
    /*get gpio signal level*/
    sample_level = GPIO_Get_Level(gpio);
    /*judge the signal value change or not*/
    if (pData->default_level != sample_level)
    {
        pData->default_level = sample_level;
        pData->cnt = 1; /* One more time */
    }
    else
    {
        if (FILTER_COUNT_NUM > pData->cnt) /*if not change, and cnt < 3, cnt ++*/
            pData->cnt++;
        else
            pData->cnt = FILTER_COUNT_NUM;
    }
    /*return stabilization level*/
    return (pData->default_level);
}

/*=====================================================================================================================
* Upward trac.:        Middle/filter/filter_if
* Method      :        Filter_Filter_ButtonCheck(Str_Filter *pData, uint8_t gpio)
* Visibility  :        private
* Description :        Judge whether the remote and local wakeup signal stabilization.
* Parameters  :        Str_Filter *pData, uint8_t gpio
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void Filter_Fast_ButtonCheck(Str_Filter *pData, uint8_t gpio) /*redtip #task 119*/
{
    bool status_temp;
    /*get current default level*/
    status_temp = Filter_Fast_GetLevel(pData, gpio);
    /*judge master or local wakeup signal cnt over 3 times or not, if all not, exit*/
    if (FILTER_COUNT_NUM > Master_Wake.cnt)
    {
        return;
    }
    else
    { /*do nothing*/
    }

/*set button status for IIC command */
#if (SLEEP_MODULE == 1)
    switch (gpio)
    {
    case MASTER_WAKE_SIGNAL:
        bFilter_Stablization_Flg = true; /*Set master and local wakeup signal stablization flag to 1*/
        if (0 != status_temp)
        {
#if (SLEEP_MODULE == 1)
            Sleep_If_Handle_RCV(); /*Set master wakeup flag to 1*/
#endif
        }
        else
        {
#if (SLEEP_MODULE == 1)
            Sleep_If_Clear_MasterWake_Flag(); /*Clear master wakeup flag to 0*/
#if (SWITCHPACK_MODULE == 1)
            SwitchPack_If_Disable_Interrupt(); /*close switchpack interrupt*/
#endif
#endif
        }
        break;
    default:
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        Middle/filter/filter_if
* Method      :        Filter_Task(void)
* Visibility  :        public
* Description :        Remote wakeup and local wakeup fast filter task function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Filter_Fast_Task(void)
{
#if (SLEEP_MODULE == 1)
    /*check master wakeup signal*/
    Filter_Fast_ButtonCheck(&Master_Wake, MASTER_WAKE_SIGNAL);
#endif
}

#endif /*FILTER_MODULE*/


/*=====================================================================================================================
* Upward trac.:        Middle/filter/filter_if
* Method      :        Filter_Wake_for_Sleeping(void)
* Visibility  :        public
* Description :        Remote wakeup and local wakeup fast filter task function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool Filter_Wake_for_Sleeping(void)
{
    /* Filter time in ms */
    int i = 0;
    for (i = 0; i < FILTER_WAKE_TIME_IN_MS; i++)
    {
        Delay_Ms(1);
        /* If Wake signal pulled down within filter_time_ms*/
        if (EIC_Get_WakeRCV_GPIO_Status() == false)
        {
            /* Set Master_Wake_Flag = 0 and return false */
            Sleep_If_Clear_MasterWake_Flag();
            return false;
        }
    }
    /* If Wake signal did not pulled down with in filter_time_ms */
    if (i == FILTER_WAKE_TIME_IN_MS)
    {
        /* Filter succeed. Set Master_Wake_Flag = 1 and return false*/
        Sleep_If_Handle_RCV();
        return true;
    }
    return false;
}