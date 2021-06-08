/******************************************************************************
*  Name: sleep_if.c
*  Description:
*  Project: wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (SLEEP_MODULE == 1)
#include "sleep_if.h"
#include "../../driver/gpio.h"
#include "../../driver/delay.h"
#include "../../middle/sys/sys_if.h"
#include "../../driver/eic.h"
#include "../../middle/timer/timer_if.h"

#if (LED_MODULE == 1)
#include "../led/led_if.h"
#endif

#if (DESERIALIZER_MODULE == 1)
#include "../deserializer/deserializer_if.h"
#endif

#if (LCD_MODULE == 1)
#include "../lcd/lcd_if.h"
#endif

#if (POWER_MODULE == 1)
#include "../power/power_if.h"
#endif

#if (ADC_MODULE == 1)
#include "../adc/adc_if.h"
#endif

#if (WDOG_MODULE == 1)
#include "../wdog/wdog_if.h"
#endif

#if (FILTER_MODULE == 1)
#include "../filter/filter_if.h"
#endif


#if (SWITCHPACK_MODULE==1)
#include "../switchpack/switchpack_if.h"
#endif

#if (ETM_MODULE == 1)
#include "../../application/etm/app_i2c_etm.h"
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
static uint8_t Master_Wake_Flag  = 0;    /*remote wake flag, need master wake signal stablization 6-9ms*/
#if (LOCALWAKEUP_STATUS == 1)
static uint8_t byLocal_Wake_Flag  = 0;   /*local wake flag, need wakeSP signal stablization 6-9ms*/
#endif
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Clear_Eic_Callback(void)
* Visibility  :        static
* Description :        disable EXTINT9
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Clear_Eic_Callback(void)
{
    EIC_Irq_Disable(EIC_CHANNEL_RCV);   /*disable EXTINT9 when FIDM wakeup from sleep mode*/
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Init(void)
* Visibility  :        public
* Description :        initialize sleep interface function 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Init(void)
{
    EIC_Init();       /*initialize master wakeup*/ 
    EIC_Irq_Register(Sleep_If_Clear_Eic_Callback, EIC_CHANNEL_RCV);  /*declare callback function for external interrupt*/
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Handle_RCV(void)
* Visibility  :        public
* Description :        Set remote wake-up flags to 1
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Handle_RCV(void)
{
    Master_Wake_Flag = 1;    /*remote wakeup signal stablization flag*/
}

#if (LOCALWAKEUP_STATUS == 1)
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Handle_WakeSP(void)
* Visibility  :        public
* Description :        Set local wake-up flags to 1
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Handle_WakeSP(void)
{
    byLocal_Wake_Flag = 1;    /*local wakeup signal stablization flag*/
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Get_WakeSPSig(void)
* Visibility  :        public
* Description :        get WakeSP Signal level
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint8_t 
* ===================================================================================================================*/
bool Sleep_If_Get_WakeSPSig(void)
{
    //return GPIO_Get_Level(NMI_WAKESP_SIGNAL);   /*return current wakeSP gpio signal data*/
	return 0;//Ptc_Wake_Flg;
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Get_WakeSP_Flag(void)
* Visibility  :        public
* Description :        get local wakeup signal stabilization  flag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint8_t 
* ===================================================================================================================*/
bool Sleep_If_Get_WakeSP_Flag(void)
{
    return byLocal_Wake_Flag;   /*local wakeup signal stablization flag*/
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Get_WakeSP_Flag(void)
* Visibility  :        public
* Description :        clear local wakeup signal stabilization flag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Clear_WakeSP_Flag(void)
{
    byLocal_Wake_Flag = 0;  /*clear local wakeup signal stablization flag*/
}
#endif

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Get_MasterWakeSig(void)
* Visibility  :        public
* Description :        get remote wakeup signal level
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint8_t 
* ===================================================================================================================*/
bool Sleep_If_Get_MasterWakeSig(void)
{
    return GPIO_Get_Level(MASTER_WAKE_SIGNAL);  /*remote wakeup signal level*/
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Get_MasterWake_Flag(void)
* Visibility  :        public
* Description :        get remote wakeup signal stabilization flag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
bool Sleep_If_Get_MasterWake_Flag(void)
{
    return Master_Wake_Flag;   /*remote wakeup signal stabilization flag*/
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Clear_MasterWake_Flag(void)
* Visibility  :        public
* Description :        clear remote wakeup signal stabilization flag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Clear_MasterWake_Flag(void)
{
    Master_Wake_Flag = 0;    /*remote wakeup signal stabilization flag*/
}

#if (LOCALWAKEUP_STATUS == 1)
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Set_SlvReq(void)
* Visibility  :        public
* Description :        enable slave request to master
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Set_SlvReq(void)
{
    GPIO_Set_Direction(SLAVE_REQUIRE_SIGNAL, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SLAVE_REQUIRE_SIGNAL, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SLAVE_REQUIRE_SIGNAL, true);   /*set slave request to hign level*/
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Not_WakeUp_Req(void)
* Visibility  :        public
* Description :        disable slave request to master
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Clear_WakeUp_Req(void)
{
    GPIO_Set_Direction(SLAVE_REQUIRE_SIGNAL, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SLAVE_REQUIRE_SIGNAL, false);  /*clear slave request to low level*/
}
#endif
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Power_Iint(void)
* Visibility  :        public
* Description :        init power module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Power_Init(void)
{
#if (1 == POWER_MODULE)
    Power_If_Init();	
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Module_Standby(void)
* Visibility  :        public
* Description :        enter into sleep mode, execute sleep time sequence
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Power_Standby(void)
{
#if (1 == POWER_MODULE)
    Power_If_Standby();	
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_ADC_Init(void)
* Visibility  :        public
* Description :        init ADC module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_ADC_Init(void)
{
#if (1 == ADC_MODULE)
    ADC_If_Init();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_ADC_Standby(void)
* Visibility  :        public
* Description :        ADC enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_ADC_Standby(void)
{
#if (1 == ADC_MODULE)
    ADC_If_Standby();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_ETM_Standby(void)
* Visibility  :        public
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_ETM_Standby(void)
{
#if (1 == ETM_MODULE)
    CI2CEtm_ClearEtmMode();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Des_Init(void)
* Visibility  :        public
* Description :        init Des module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Des_Init(void)
{
#if (1 == DESERIALIZER_MODULE)
    Des_If_Init();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Des_Standby(void)
* Visibility  :        public
* Description :        Des enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Des_Standby(void)
{
#if (1 == DESERIALIZER_MODULE)
    Des_If_Standby();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Lcd_Step1_Init(void)
* Visibility  :        public
* Description :        init lcd module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Lcd_Step1_Init(void)
{
#if (1== LCD_MODULE)
    Lcd_If_Step1_Init();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Lcd_Step2_Init(void)
* Visibility  :        public
* Description :        init lcd module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Lcd_Step2_Init(void)
{
#if (1== LCD_MODULE)
    Lcd_If_Step2_Init();
#endif
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Lcd_Standby(void)
* Visibility  :        public
* Description :        lcd enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Lcd_Standby(void)
{
#if (1 == LCD_MODULE)
    Lcd_If_Standby();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Led_Init(void)
* Visibility  :        public
* Description :        init led module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Led_Init(void)
{
#if(1 == LED_MODULE)
    LED_If_Init();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Led_Standby(void)
* Visibility  :        public
* Description :        led enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Led_Standby(void)
{
#if(1 == LED_MODULE)
    LED_If_Standby();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Wdog_Init(void)
* Visibility  :        public
* Description :        init wdog module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Wdog_Init(void)
{
#if (1 == WDOG_MODULE)
    WDog_If_Init();
#endif
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Wdog_Standby(void)
* Visibility  :        public
* Description :        wdog enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_Wdog_Standby(void)
{
#if (WDOG_MODULE == 1)
    WDog_If_Standby();
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Wdog_Init(void)
* Visibility  :        public
* Description :        init wdog module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_SwitchPack_Init(void)
{
#if (SWITCHPACK_MODULE == 1)
#if (FILTER_MODULE == 1)
    Filter_GpioInit();    /*switchpack button gpio and external interrupt init*/
#endif
    SwitchPack_If_Init(); /*switchpack knob and INT_MCU gpio init*/
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_SwitchPack_Standby(void)
* Visibility  :        public
* Description :        wdog enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void Sleep_If_SwitchPack_Standby(void)
{
#if (SWITCHPACK_MODULE==1)
    #if (FILTER_MODULE == 1)             
    Filter_Gpio_Standby();   /*switchpack button gpio deinit*/
    #endif
    SwitchPack_If_GPIO_Standby();  /*switchpack knob and INT_MCU gpio deinit*/
#endif
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_LocalWakeUp_Standby(void)
* Visibility  :        public
* Description :        wdog enter into sleep mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Gpio_Standby(void)
{
#if (SWITCHPACK_MODULE==1)
    #if (FILTER_MODULE == 1)               
    Filter_Gpio_Standby();
    #endif
    SwitchPack_If_GPIO_Standby();
#endif

#if (LOCALWAKEUP_STATUS == 1)
    GPIO_Set_Direction(SLAVE_REQUIRE_SIGNAL, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SLAVE_REQUIRE_SIGNAL, false);
#endif
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Module_Standby(void)
* Visibility  :        public
* Description :        enter into sleep mode, execute sleep time sequence
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Module_Standby(void)
{
    /*Peripheral enter into standby when MCU sleep*/
    Sleep_If_Led_Standby();
    Sleep_If_Lcd_Standby();
    Sleep_If_Des_Standby();
    Sleep_If_Power_Standby();
    Sleep_If_ADC_Standby();

    Sleep_If_ETM_Standby();
    
    
    Sleep_If_SwitchPack_Standby();
   
    /*disable asynchronous timer*/
    Timer_If_Async_standby();
    /*disable Wdog module*/
    Sleep_If_Wdog_Standby();

    Sleep_If_Gpio_Standby();
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/SLEEP/SLEEP_IF
* Method      :        Sleep_If_Module_StandbyTowork(void)
* Visibility  :        public
* Description :        begin to wake from sleep mode, execute wakeup time sequence according to HSIS
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Sleep_If_Module_StandbyTowork(void)
{    
	
	/*init extern pmic when MCU wakeup*/
    Sleep_If_Power_Init();
    /*init asynchronous timer*/
    Timer_If_Async_init(); 
    /*init peripheral when MCU wakeup*/
    Touch_If_Set_Rstn(true); 
    Sleep_If_Lcd_Step1_Init();
    Sleep_If_Des_Init();
    Sleep_If_Led_Init();
    Sleep_If_Lcd_Step2_Init();
    
    Sleep_If_ADC_Init();
    Sleep_If_SwitchPack_Init();
	
    /*watch dog enable*/
    Sleep_If_Wdog_Init();

}
#endif /* SLEEP_MODULE */
