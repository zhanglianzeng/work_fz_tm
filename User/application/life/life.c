/******************************************************************************
*  Name: life.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LIFE_APP == 1)
#include <stdbool.h>
#include <stddef.h>
#include "../../../Device/samc21n18a.h"
#include "../../driver/delay.h"
#include "life.h"
#include "../../middle/timer/timer_if.h"

#if (SCHEDULER_APP == 1)
#include "../scheduler/scheduler.h"
#endif

#if (SLEEP_MODULE == 1)
#include "../../middle/sleep/sleep_if.h"
#endif

#if (LED_MODULE == 1)
#include "../led/app_led_ctrl.h"
#include "../../middle/led/led_if.h"
#endif

#if (LCD_MODULE == 1)
#include "../../middle/lcd/lcd_if.h"
#endif

#if (EEPROM_MODULE == 1)
#include "../eeprom/app_eeprom.h"
#endif

#if (POST_MODULE == 1)
#include "../../middle/post/post_if.h"
#endif

#if (DESERIALIZER_MODULE == 1)
#include "../iic/app_iic_comm.h"
#include "../iic/app_i2c_dimming.h"
#endif

#if (ETM_MODULE == 1)
#include "../etm/app_i2c_etm.h"
#endif

#if (FILTER_MODULE == 1)
#include "../../middle/filter/filter_if.h"
#endif

#if (WDOG_MODULE == 1)
#include "../../middle/wdog/wdog_if.h"
#endif

#if (POWER_MODULE == 1)
#include "../power/app_power_manage.h"
#include "../../middle/power/power_if.h"
#endif

#if (THERMAL_MODULE == 1)
#include "../thermal/app_thermal_manage.h"
#endif

#if (SOAK_MODULE == 1)
#include "../soak/app_soak_manage.h"
#endif

#if (DIAGNOSTIC_MODULE == 1)
#include "../diagnostic/app_diagnostic_manage.h"
#endif

#if (TOUCH_MODULE == 1)
#include "../../middle/touch/touch_if.h"
#endif

#if (RTC_DRIVER == 1)
#include "../../driver/rtc.h"
#endif

#include "../../driver/gpio.h"

#include "../../driver/qtouch/touch.h"
#include "../../middle/touch/touch_example.h"

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
static Sleep_Status_Enum Sleep_Status = SLEEP_MODE; /*lifecycle mode*/

static bool bSleepToWake_flg = true; /*wakeup from sleep mode flag*/
static uint8_t byPower_Step;         /*power volatge mode sequential step count*/


#if (POWER_MODULE == 1)
static POWER_STATUS_TENDENCY byPower_Tendency_New;                  /*the newest power voltage mode change tendency*/
static POWER_STATUS_TENDENCY byPower_Tendency_Curr = INIT_TENDENCY; /*the current power voltage mode change tendency*/
#endif

/*sched task list when system running*/
static SCHED_TASK const Sched_Task_List[] = {
#if (POWER_MODULE == 1)
    {CPowerManage_Task, 2, 20},
#endif

#if (THERMAL_MODULE == 1)
    {CThermalManage_Task, 3, 20},
#endif

#if (SOAK_MODULE == 1)
    {CSoakManage_Task, 4, 20},
#endif

#if (DESERIALIZER_MODULE == 1)
    {CI2CComm_Task, 5, 10},
#endif

#if (LED_MODULE == 1)
    {CLedCtrl_Task, 6, 10},
#endif

#if (DIAGNOSTIC_MODULE == 1)
    {CDiagnositcManage_Task, 7, 500},
#endif

#if (WDOG_MODULE == 1)
    {WDog_If_Feed, 1, 100},
#endif

#if (PTC_MODULE == 1)
    {Touch_scan_task, 8, 20},
    {touch_timer_handler, 9, 20},
#endif
};
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

#if (POWER_MODULE == 1)
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_NOL_To_POL(void)
* Visibility  :        private
* Description :        Manage power sequence from NOT_OPERATIONAL_LOW mode to PARTIALLY_OPERATION_LOW mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_NOL_To_POL(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
        case STEP1:
            byPower_Step++;
            #if (POWER_MODULE == 1)
            /*enable 3_v 5v WDo*/
            Power_If_EN();
            #endif
            #if (LCD_MODULE == 1)
            Lcd_If_Gpio_Init();
            #endif
            /*TP_EXT_RSTN set high*/
            Touch_If_Set_Rstn(true);
            /*enable tc time*/    
            Timer_If_Set_Timeout(POWER_TIMER, 5);
        break;
        case STEP2:
            if(TIMER_STOP==Timer_If_Check_Timeout(POWER_TIMER))   /*5ms*/
            {
                #if (DESERIALIZER_MODULE == 1)
                Des_If_Set_PDB_Level(true);      /*enable PDB*/          
                #endif
                #if (LCD_MODULE == 1)
                Lcd_If_Set_Reset_Level(true);   /*enable LCD_RESET*/
                #endif
                #if (LED_MODULE == 1)
                LED_If_I2C_Init();
                #endif                    
                #if (DESERIALIZER_MODULE == 1)
                Des_If_Slave_Init();    
                Des_If_Master_Init();                    
                Des_If_Register_Config();        /*configuare 988, can cause restart when bridge board power is 12V*/
                #endif
                byPower_Step = STEP_END;
            }
            else
            {/*do nothing*/}
        break;
    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_POL_To_FO(void)
* Visibility  :        private
* Description :        Manage power sequence form PARTIALLY_OPERATION_LOW mode to FULLY_OPERATION_LEVEL mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_POL_To_FO(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
    case STEP1:
#if (LCD_MODULE == 1)
        Lcd_If_Set_Disp_Level(true);
        Lcd_If_Set_Pon_Level(true);
#endif
        /*enable tc time*/
        Timer_If_Set_Timeout(POWER_TIMER, 61);
        byPower_Step++;
        break;
        case STEP2:  
            if(TIMER_STOP==Timer_If_Check_Timeout(POWER_TIMER))    /*need review*/
            { 
				CLedCtrl_Blen_Ctrl(true);        /*enable BL_EN*/
             	LED_If_IC_Config();              /*LP8863 config*/               
                byPower_Step = STEP_END; 
            }
            else
            {/*do nothing*/}              
        break;

    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_NOH_To_POH(void)
* Visibility  :        private
* Description :        Manage power sequence form NOT_OPERATIONAL_HIGH mode to PARTIALLY_OPERATION_HIGH mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_NOH_To_POH(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
    case STEP1:
        byPower_Step++;
#if (POWER_MODULE == 1)
        Power_If_EN();
#endif
        /*TP_EXT_RSTN set high*/
        Touch_If_Set_Rstn(true);
        /*enable tc time*/
        Timer_If_Set_Timeout(POWER_TIMER, 5);
        break;
    case STEP2:
        if (TIMER_STOP == Timer_If_Check_Timeout(POWER_TIMER)) /*5ms*/
        {
#if (LCD_MODULE == 1)
            Lcd_If_Set_Reset_Level(true); /*enable LCD_RESET*/
#endif
            byPower_Step = STEP_END; /*STEP end*/
        }
        else
        { /*do nothing*/
        }
        break;
    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_POH_To_FO(void)
* Visibility  :        private
* Description :        Manage power sequence form PARTIALLY_OPERATION_HIGH mode to FULLY_OPERATION_LEVEL mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_POH_To_FO(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
        case STEP1:        
            #if (DESERIALIZER_MODULE == 1)
            Des_If_Set_PDB_Level(true);     /*enable PDB*/  
            Des_If_Slave_Init();           /*init iic_slave*/
            Des_If_Master_Init();          /*init iic_master*/          
            //Des_If_Register_Config();      /*988 config*/
            #endif
            #if (LED_MODULE == 1)
            LED_If_I2C_Init();
            #endif
            /*enable tc time*/    
            Timer_If_Set_Timeout(POWER_TIMER, 5);
            byPower_Step++;
        break;
            case STEP2:  
            if(TIMER_STOP==Timer_If_Check_Timeout(POWER_TIMER))   
            { 
                #if (LCD_MODULE == 1)
                Lcd_If_Set_Pon_Level(true);
                Lcd_If_Set_Disp_Level(true);
                #endif	
                /*enable tc time*/    
                Timer_If_Set_Timeout(POWER_TIMER, 61);                              
                byPower_Step++; 
            }
            else
            {/*do nothing*/}              
        break;			
        case STEP3:  
            if(TIMER_STOP==Timer_If_Check_Timeout(POWER_TIMER))   /*need review*/
            {
                CLedCtrl_Blen_Ctrl(true);       /*enable BL_EN*/
                LED_If_IC_Config();             /*LP8863 config*/	
                byPower_Step = STEP_END;        /*STEP end*/   
            }
            else
            {/*do nothing*/}              
        break;

    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_POL_To_NOL(void)
* Visibility  :        private
* Description :        Manage power sequence form PARTIALLY_OPERATION_LOW mode to NOT_OPERATIONAL_LOW mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_POL_To_NOL(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
        case STEP1:
            byPower_Step++;		
            #if (LCD_MODULE == 1)
            Lcd_If_Set_Reset_Level(false);   /*DISable LCD_RESET*/
			Lcd_If_Gpio_Deinit();            /*close LCD GPIO*/
            #endif
            #if (DESERIALIZER_MODULE == 1)
            Des_If_Set_PDB_Level(false);      /*disable PDB*/                
            Des_If_Slave_DeInit();    
            Des_If_Master_DeInit();
            #endif 
            Timer_If_Set_Timeout(POWER_TIMER, 10);
        break;
    case STEP2:
        if (TIMER_STOP == Timer_If_Check_Timeout(POWER_TIMER))
        {
#if (POWER_MODULE == 1)
            /*Power disable*/
            Power_If_DIS();
#endif
#if (LED_MODULE == 1)
            LED_If_I2c_Deinit();
#endif
            /*TP_EXT_RSTN set high*/
            Touch_If_Set_Rstn(false);
            byPower_Step = STEP_END;
        }
        else
        { /*do nothing*/
        }
        break;
    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_POH_To_NOH(void)
* Visibility  :        private
* Description :        Manage power sequence form PARTIALLY_OPERATION_HIGH mode to NOT_OPERATIONAL_HIGH mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_POH_To_NOH(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
        case STEP1:
            #if	(LCD_MODULE == 1)
            Lcd_If_Set_Reset_Level(false);        /*disable LCD_RESET*/
			Lcd_If_Gpio_Deinit();            /*close LCD GPIO*/
            #endif
            Timer_If_Set_Timeout(POWER_TIMER, 10);
            byPower_Step++;
        break;
        case STEP2:
            if(TIMER_STOP==Timer_If_Check_Timeout(POWER_TIMER))    /*10ms*/
            {
                #if (POWER_MODULE == 1)
                /*Power disable*/  
                Power_If_DIS();
                #endif
                #if(LED_MODULE == 1)
                LED_If_I2c_Deinit();  
                #endif 
                /*TP_EXT_RSTN set high*/
                Touch_If_Set_Rstn(false);  
                byPower_Step = STEP_END;          /*step end*/
            }           
        break;
    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_FO_To_POL(void)
* Visibility  :        private
* Description :        Manage power sequence form FULLY_OPERATION_LEVEL mode to PARTIALLY_OPERATION_LOW mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_FO_To_POL(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
    case STEP1:
        byPower_Step++;
#if (LED_MODULE == 1)
        CLedCtrl_PwmValue_Clear(); /*close backlight and switchpack brightness*/
        CLedCtrl_Blen_Ctrl(false); /*disable BL_EN*/
#endif
#if (LCD_MODULE == 1)
        Lcd_If_Set_Disp_Level(false);
        Lcd_If_Set_Pon_Level(false);
#endif
        byPower_Step = STEP_END;
        break;
    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Power_FO_To_POL(void)
* Visibility  :        private
* Description :        Manage power sequence form FULLY_OPERATION_LEVEL mode to PARTIALLY_OPERATION_HIGH mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Power_FO_To_POH(void)
{
#if (POWER_MODULE == 1)
    switch (byPower_Step)
    {
    case STEP1:
        byPower_Step++;
#if (LCD_MODULE == 1)
        Lcd_If_Set_Disp_Level(false);
        Lcd_If_Set_Pon_Level(false);
#endif
#if (LED_MODULE == 1)
        CLedCtrl_PwmValue_Clear(); /*close backlight and switchpack brrightness*/
        LED_If_I2c_Deinit();
        CLedCtrl_Blen_Ctrl(false); /*disable BL_EN*/
#endif
        Timer_If_Set_Timeout(POWER_TIMER, 70);
        break;
    case STEP2:
        if (TIMER_STOP == Timer_If_Check_Timeout(POWER_TIMER)) /*70ms*/
        {
#if (DESERIALIZER_MODULE == 1)
            Des_If_Slave_DeInit();
            Des_If_Master_DeInit();
            Des_If_Set_PDB_Level(false); /*disable PDB*/
#endif
            byPower_Step = STEP_END;
        }
        else
        { /*do nothing*/
        }
        break;

    default:
        byPower_Step = STEP1;
        break;
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_PowerStatus_Manage(void)
* Visibility  :        private
* Description :        Manage sequence according to the power voltage mode change tendency
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_PowerStatus_Manage(void)
{
#if (POWER_MODULE == 1)
    if (true == CPowerManage_Get_PowerStat_ChangeFlg()) /*judge power status change or not*/
    {
        byPower_Tendency_New = CPowerManage_Get_PowerStatus_Tendency();

        /*byPower_Step is STEP1 means time the last time sequence is end*/
        if ((STEP1 == byPower_Step) && (byPower_Tendency_New != byPower_Tendency_Curr))
        {
            byPower_Tendency_Curr = byPower_Tendency_New;
        }
        else
        { /*do nothing*/
        }

        switch (byPower_Tendency_Curr)
        {
        case NOL_TO_POL:
            CLife_Power_NOL_To_POL(); /*NOT_OPERATIONAL_LOW to PARTIALLY_OPERATION_LOW*/
            break;
        case POL_TO_FO:
            CLife_Power_POL_To_FO(); /*PARTIALLY_OPERATION_LOW to FULLY_OPERATION_LEVEL*/
            break;
        case FO_TO_POH:
            CLife_Power_FO_To_POH(); /*FULLY_OPERATION_LEVEL to PARTIALLY_OPERATION_HIGH*/
            break;
        case POH_TO_NOH:
            CLife_Power_POH_To_NOH(); /*PARTIALLY_OPERATION_HIGH to NOT_OPERATIONAL_HIGH*/
            break;
        case NOH_TO_POH:
            CLife_Power_NOH_To_POH(); /*NOT_OPERATIONAL_HIGH to PARTIALLY_OPERATION_HIGH*/
            break;
        case POH_TO_FO:
            CLife_Power_POH_To_FO(); /*PARTIALLY_OPERATION_HIGH to FULLY_OPERATION_LEVEL*/
            break;
        case FO_TO_POL:
            CLife_Power_FO_To_POL(); /*FULLY_OPERATION_LEVEL to PARTIALLY_OPERATION_LOW*/
            break;
        case POL_TO_NOL:
            CLife_Power_POL_To_NOL(); /*PARTIALLY_OPERATION_LOW to NOT_OPERATIONAL_LOW*/
            break;
        default:
            byPower_Step = STEP_END;
            break;
        }

        if (STEP_END == byPower_Step)
        {
            byPower_Step = STEP1;
            CPowerManage_Clear_PowerStat_ChangeFlg(); /*clear power status change flag*/
            CPowerManage_Set_TendencyEndFlg(true);    /*time senquence end*/
#if (ETM_MODULE == 1)
            CI2CEtm_ClearEtmMode(); /*redtip527,ETM only applicable for one power cycle (wake up- sleep). In another power cycle the EMT should be re-triggered*/
#endif
        }
        else
        { /*do nothing*/
        }
    }
#endif
}
#endif /*POWER_MODULE*/

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Led_Init(void)
* Visibility  :        private
* Description :        init led app module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Led_Init(void)
{
#if (LED_MODULE == 1)
    /*init the global variables in application file*/
    CLedCtrl_Init();
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Led_Clean_Wakeup_Flag(void)
* Visibility  :        private
* Description :        clean led wakeup flag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Led_Clean_Wakeup_Flag(void)
{
#if (LED_MODULE == 1)
    CLedCtrl_Clean_Wakeup_Flag();
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Eeprom_Store(void)
* Visibility  :        private
* Description :        store data when FIDM enter sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Eeprom_Store(void)
{
#if (EEPROM_MODULE == 1)
    Factory_Config *pConfig_Init;

    pConfig_Init = CEeprom_Get_Factory_Config();
    /*store data when mcu enter sleep mode*/
    CEeprom_Write_Factory_Config(pConfig_Init);
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_MemoryCheck(void)
* Visibility  :        private
* Description :        check memory operation
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_MemoryCheck(void)
{
#if (POST_MODULE == 1)
    if (false == Post_RamTest())
    {
        return;
    }
    else
    { /*do nothing*/
    }
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Power_Counter_Inc(void)
* Visibility  :        private
* Description :        counter powerup times
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Power_Counter_Inc(void)
{
#if (EEPROM_MODULE == 1)
    CEeprom_Power_Counter_Inc();
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Sleep_If_Init(void)
* Visibility  :        private
* Description :        init EIC module when sleep
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Sleep_If_Init(void)
{
#if (SLEEP_MODULE == 1)
    Sleep_If_Init(); /*init master wakeup signal and callback function*/
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Module_Standby(void)
* Visibility  :        private
* Description :        hardware init enter standby
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Module_Standby(void)
{
#if (SLEEP_MODULE == 1)
    Sleep_If_Module_Standby(); /*deinit of bsp_init()*/
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Module_StandbyTowork(void)
* Visibility  :        private
* Description :        hardware init from standby to work
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Module_StandbyTowork(void)
{
#if (SLEEP_MODULE == 1)
    Sleep_If_Module_StandbyTowork();
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Timer_Start(void)
* Visibility  :        private
* Description :        scheduler and Timer start
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Timer_Start(void)
{
#if (SCHEDULER_APP == 1)
    CScheduler_Start();
#endif
    /*init asynchronous timer*/
//    Timer_If_Async_init();
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Timer_Stop(void)
* Visibility  :        private
* Description :        scheduler and Timer end
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Timer_Stop(void)
{
#if (SCHEDULER_APP == 1)
    CScheduler_Stop();
#endif
    /*disable asynchronous timer*/
//    Timer_If_Async_standby();
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Filter_StablizetionFlg(void)
* Visibility  :        private
* Description :        check master or local wakeup signal whether stablized 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static bool CLife_If_Filter_StablizetionFlg(void)
{
#if (FILTER_MODULE == 1)
    /*check master or local wakeup signal whether stablized*/
    return Filter_Get_StablizetionFlg();
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Filter_StablizetionFlg(void)
* Visibility  :        private
* Description :        clear Stablization Flag 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Clear_StablizationFlg(void)
{
#if (FILTER_MODULE == 1)
    /*clear Stablization Flag*/
    Filter_Clear_StablizationFlg();
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Get_MasterWake_Flag(void)
* Visibility  :        private
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static uint8_t CLife_If_Get_MasterWake_Flag(void)
{
#if (1 == SLEEP_MODULE)
    /*master wakeup flag*/
    return Sleep_If_Get_MasterWake_Flag();
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Diagnositc_Task(void)
* Visibility  :        private
* Description :        Diagnositc Task
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Diagnositc_Task(void)
{
#if (DIAGNOSTIC_MODULE == 1)
    CDiagnositcManage_Task();
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Clear_LocalWakeup(void)
* Visibility  :        private
* Description :        Clear local wakeup relevant variables and status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Clear_LocalWakeup(void)
{
#if (SLEEP_MODULE == 1)
    //Sleep_If_Clear_WakeUp_Req();              /*clear slave request signal*/
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        Clife_If_Add_Task(void)
* Visibility  :        private
* Description :        Add task to scheduler queue 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Add_Task(void)
{
#if (SCHEDULER_APP == 1)
    for (uint8_t i = 0u; i < (sizeof(Sched_Task_List) / sizeof(SCHED_TASK)); i++)
    {
        (void)CScheduler_Add_Task(Sched_Task_List[i].task, Sched_Task_List[i].delay, Sched_Task_List[i].period);
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        Clife_If_Delete_Task(void)
* Visibility  :        private
* Description :        Delete task from scheduler queue
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Delete_Task(void)
{
#if (SCHEDULER_APP == 1)
    for (uint8_t i = 0; i < (sizeof(Sched_Task_List) / sizeof(SCHED_TASK)); i++)
    {
        (void)CScheduler_Delete_Task(Sched_Task_List[i].task);
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Wait_Command(void)
* Visibility  :        private
* Description :        go to sleep wait mode 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void CLife_Wait_Command(void)
{
    /* Configure PM */
    PM->SLEEPCFG.reg = 0x02;
    /* Ensure that SLEEPMODE bits are configured with the given value */
    //while(!(PM->SLEEPCFG.reg & 0x02));
    /* Wait for interrupt instruction execution */
    __WFI();
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Stop_Command(void)
* Visibility  :        private
* Description :        go to sleep stop mode function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Stop_Command(void)
{
    /* Configure PM */
    PM->STDBYCFG.reg = PM_STDBYCFG_VREGSMOD(2);
    /* Configure Standby Sleep */
    PM->SLEEPCFG.reg = 0x04;

    /* Ensure that SLEEPMODE bits are configured with the given value */
    while (!(PM->SLEEPCFG.reg & 0x04))
        ;
    /* Wait for interrupt instruction execution */
    __WFI();
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Sleeping(void)
* Visibility  :        private
* Description :        Fake sleeping mode for PTC touch button scanning
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void CLife_If_Sleeping(void)
{
    /* Prepare RTC for fake sleeping mode*/
    NVIC_DisableIRQ(RTC_IRQn);
    RTC_Stop();
    RTC_Set_COMP_VAL_to_640();
    Set_RTC_CallBack_To_Touch();
    RTC_Start();
    NVIC_EnableIRQ(RTC_IRQn);

    /* Clear Request GPIO */
    Sleep_If_Clear_WakeUp_Req();

    while (1)
    {
        /* PTC Scan Touch button */
        if (Touch_scan())
        {
            /* If touch button pressed, pull Request GPIO */
            Sleep_If_Set_SlvReq();
            for (int i = 0; i < 200; i++)
            {
                /* Request signal stay at high max for 200ms */
                Delay_Ms(1);
                if (EIC_Get_WakeRCV_GPIO_Status() == true)
                {
                    /* If remote WAKE signal detected within this 200ms, pull down request and go on */
                    break;
                }
            }
            /* Pull down request GPIO */
            Sleep_If_Clear_WakeUp_Req();
        }

        /* If remote WAKE signal detected */
        if (EIC_Get_WakeRCV_GPIO_Status())
        {
            /* Filter WAKE signal for several ms. If stable, return this funtion(exit sleeping mode) and go on (GOTO WORK) */
            if (Filter_Wake_for_Sleeping())
                return;
        }
        /* If wakeup(GOTO WORK) conditions did not met, MCU will enter deep sleep until next RTC wake it up */
        CLife_Stop_Command();
    }
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_Enter_Sleep(void)
* Visibility  :        private
* Description :        go to sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_Enter_Sleep(void)
{
#if (SLEEP_WAIT_MODE == 1)
    CLife_Wait_Command();
#else
    /*mcu enter fake sleeping mode (PTC Touch button scaning)*/
    CLife_If_Sleeping();
#endif
}

/*****************************************************PTC******************************************************************/

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Sleep(void)
* Visibility  :        private
* Description :        Lifecycle Sleep mode function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Sleep(void)
{
#if (SLEEP_MODULE == 1)
    Sleep_Status = GOTO_WORK;
    static bool Cool_PowerUp = true; /*first PowerUp*/
    uint8_t WakeUp_Status = SLEEP_STATUS;
    uint8_t wakeSP_Status = SLEEP_STATUS;

    // WakeUp_Status = Sleep_If_Get_MasterWakeSig();
    // wakeSP_Status = Sleep_If_Get_WakeSPSig();

    /* Enter into sleep when wakeup signal is low*/
    if ((SLEEP_STATUS == WakeUp_Status)
#if (LOCALWAKEUP_STATUS == 1)
        && (SLEEP_STATUS == wakeSP_Status)
#endif
    )
    {
        /*Sched task need to start*/
        bSleepToWake_flg = true;
        /*stop shced task when sleep mode*/
        CLife_If_Timer_Stop();

        //CLife_If_Clear_StablizationFlg();
        //CLife_If_Led_Clean_Wakeup_Flag();

        // CLife_If_Eeprom_Store();

        /*init extern EIC wakeup function*/
        CLife_If_Sleep_If_Init();
        CLife_If_Enter_Sleep();
    }
    else
    { /*do nothing*/
    }
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_GoToSleep(void)
* Visibility  :        private
* Description :        Lifecycle gotosleep mode function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_GoToSleep(void)
{
    /*sleep time sequence*/
    CLife_If_Module_Standby();
    /*delete task from scheduler list*/
    CLife_If_Delete_Task();
    /*change Sleep_Status to SLEEP_MODE according to state machine*/
    Sleep_Status = SLEEP_MODE;
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Work(void)
* Visibility  :        private
* Description :        Lifecycle work mode function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_Work(void)
{
    /*power voltage mode change management*/
    CLife_PowerStatus_Manage();

#if (SLEEP_MODULE == 1)
    /*master wakeup signal change to low level, and stable continue 9ms*/
    if (0u == Sleep_If_Get_MasterWake_Flag()) //Filter not ready, use this instead temporary
    {
        /*mcu enter sleep mode*/
        Sleep_Status = GOTO_SLEEP;
    }
    else
    { /*do nothing*/
    }
#endif
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_If_PrepareToWork(void)
* Visibility  :        private
* Description :        Lifecycle Prepare To Work
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_If_PrepareToWork(void)
{
    /*init led app moule*/
    CLife_If_Led_Init();

    CLife_If_Module_StandbyTowork();

    /*For Memory_Check do not affect sequence time*/
    CLife_If_MemoryCheck();

    /*hardware diag when wakeup*/
    CLife_If_Diagnositc_Task();

    /*Powerup_counter update*/
    CLife_If_Power_Counter_Inc();

    /*init task list when mcu wakeup*/
    CLife_If_Add_Task();
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_GoToWork(void)
* Visibility  :        private
* Description :        Lifecycle gotowork mode management
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CLife_GoToWork(void)
{
#if (1 == SCHEDULER_APP)
    CScheduler_Init();
#endif
    CLife_If_Timer_Start();
    CLife_If_PrepareToWork();
    Sleep_Status = WORK_MODE;
#if 0
#if (SLEEP_MODULE == 1)
    TIMER_STATUS timer_status = TIMER_STOP;

    /*Task start when mcu wakeup from sleep mode*/
    if(true == bSleepToWake_flg) 
    {
	    CLife_If_Timer_Start();
        bSleepToWake_flg = false;
    }
    else
    {/*do nothing*/}

    if((true == CLife_If_Filter_StablizetionFlg())
#if (PTC_MODULE == 1)
        || (true == CLife_If_Local_Filter_StablizetionFlg)
#endif
	  )
    {
        if(1 == Sleep_If_Get_MasterWake_Flag())
        {              
            CLife_If_PrepareToWork();
            Sleep_Status = WORK_MODE;
        }
#if (LOCALWAKEUP_STATUS == 1)      
        else if( (true == CLife_If_Local_Filter_StablizetionFlg) ) /*local wakeup flag:"P0" according to state machine*/
        {
            /*set slave request signal to master*/
            Sleep_If_Set_SlvReq();

            timer_status = Timer_If_Check_Timeout(SLEEP_TIMER);
            if(TIMER_STOP == timer_status)
            {
                /*start 100ms timer*/
                Timer_If_Set_Timeout(SLEEP_TIMER,100);                /*time out function 100 ms*/
            }
            else
            {/*do nothing*/}
            
            /*master wakeup signal not change to high level in 100ms*/
            if(TIMER_TIMEOUT == timer_status)                                   
            {
                CLife_If_Clear_LocalWakeup();                
                Sleep_Status = SLEEP_MODE;                            /*P2 high level signal is not detected within 100ms*/
            }
            /*master wakeup signal change to high level in 100ms*/
            else if(1 == Sleep_If_Get_MasterWakeSig())
            {
                CLife_If_Clear_LocalWakeup();
                CLife_If_PrepareToWork();
                Sleep_Status = WORK_MODE;               
            }
            else
            {/*do nothing*/}          
        }
#endif
        /*master and local wakeup signal are all low level*/        
        else        
        {
            CLife_If_Clear_LocalWakeup();         
            Sleep_Status = SLEEP_MODE;                     
        }     
    }

	if (GOTO_WORK != Sleep_Status)
	{
        Timer_If_Stop_Time(SLEEP_TIMER);
    }
#endif
#endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CLife_Task(void)
* Visibility  :        public
* Description :        Lifecycle Task function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void CLife_Task(void)
{
    switch (Sleep_Status)
    {
    case WORK_MODE: /*WORK_MODE according to state machine*/
        CLife_Work();
        break;
    case GOTO_SLEEP: /*GOTO_SLEEP according to state machine*/
        CLife_GoToSleep();
        break;
    case SLEEP_MODE: /*SLEEP_MODE according to state machine*/
        CLife_Sleep();
        break;
    case GOTO_WORK: /*GOTO_WORK according to state machine*/
        CLife_GoToWork();
        break;
    default:
        break;
    }
}

#endif /* LIFE_APP */
