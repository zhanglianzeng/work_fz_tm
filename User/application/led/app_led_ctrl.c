/******************************************************************************
*  Name: app_led_ctrl.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LED_MODULE == 1)
#include <string.h>
#include "../../middle/led/led_if.h"
#include "app_led_ctrl.h"
#include "../../middle/adc/adc_if.h"

#if (POWER_MODULE == 1)
#include "../power/app_power_manage.h"
#endif

#if (SOAK_MODULE == 1)
#include "../soak/app_soak_manage.h"
#endif

#if (THERMAL_MODULE == 1)
#include "../thermal/app_thermal_manage.h"
#endif

#if (DESERIALIZER_MODULE == 1)
#include "../iic/app_iic_comm.h"
#include "../iic/app_i2c_dimming.h"
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
static uint16_t Pwm_Current_Value = PWM_INIT_VALUE;    /*current led backlight PWM duty cycle value*/
static uint16_t Pwm_Expect_Value  = PWM_INIT_VALUE;    /*expect led backlight PWM duty cycle value*/
static uint16_t Pwm_AllowMax_Value = PWM_INIT_VALUE;   /*maximum allowable led backlight PWM duty cycle value*/

static uint16_t set_pwm_duty = PWM_INIT_VALUE;

static Led_Pwm_Str Pwm_Data;                           

static bool Wakeup_Detect_Flag = false;                /*FIDM wakeup time from sleep mode to now upto 1s flag*/      

static bool bBl_En_Status;                             /*BL_EN enable status flag*/

static STRU_TIMEOUT struTimeOut[TIMEOUT_CHANNEL_NUM];
/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void CLedCtrl_Pwm_Calculate(uint16_t pwm_dec);
static void CLedCtrl_Pwm_Refresh(void);
static uint16_t CLedCtrl_Linear(uint32_t temper,bool active);
/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Init(void)
* Visibility  :        public
* Description :        LedCtrl app init funciton
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CLedCtrl_Init(void)
{
#if (DESERIALIZER_MODULE == 1)
    CI2CDimming_Init();
#endif
#if (POWER_MODULE == 1)
    CPowerManage_Init();
#endif
#if (THERMAL_MODULE == 1)
    CThermalManage_Init();
#endif
#if (SOAK_MODULE == 1)
    CSoakManage_Init();
#endif
    Pwm_Current_Value = PWM_INIT_VALUE; /*current PWM value*/
    Pwm_Expect_Value  = PWM_INIT_VALUE; /*expert PWM value*/
    Pwm_AllowMax_Value = PWM_INIT_VALUE;/*PWM maxium value allowable*/
    Pwm_Data.power_flag = POWER_ABNORMAL;/*power normal flag*/
    Pwm_Data.thermal_flag = THERMAL_ABNORMAL; /*thermal normal flag*/
    Pwm_Data.iic_flag = 0;  /*PWM command received flag*/ 

    bBl_En_Status = true;       /*BL_EN enable flag*/
    
    for (uint8_t i=(uint8_t)WAKEUP_1S; i<(uint8_t)TIMEOUT_CHANNEL_NUM; i++)
    { 
        struTimeOut[i].bCount_Enable = 0;
        struTimeOut[i].bTimeout_Flg = 0;
        struTimeOut[i].hCount = 0;
    }
    struTimeOut[WAKEUP_1S].bCount_Enable = 1;     /*1s timeout enable flag*/
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Blen_Ctrl(bool level)
* Visibility  :        public
* Description :        Backlight control
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CLedCtrl_Blen_Ctrl(bool level)
{
    bBl_En_Status = level;       /*set BL_EN enable flag to specified value*/     
    LED_If_Set_EN_Level(level);  /*set BL_EN to specified level*/
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Get_PWMCurrent_Value(void)
* Visibility  :        public
* Description :        Get current output pwm value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
uint16_t CLedCtrl_Get_PWMCurrent_Value(void)
{
    return Pwm_Current_Value;
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Clean_Wakeup_Flag(void)
* Visibility  :        public
* Description :        clean wakeup flag when enter sleep mode
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CLedCtrl_Clean_Wakeup_Flag(void)
{
    Wakeup_Detect_Flag = false;
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        private
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CLedCtrl_Timer_Enable(ENUM_TIMEOUT_CHANNEL TimerChannel)
{
    if(TimerChannel<TIMEOUT_CHANNEL_NUM)
    {
        struTimeOut[TimerChannel].bCount_Enable = 1;             
    }
}
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        private
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/

static void CLedCtrl_Timer_Disable(ENUM_TIMEOUT_CHANNEL TimerChannel)
{
    if(TimerChannel<TIMEOUT_CHANNEL_NUM)
    {
        struTimeOut[TimerChannel].bCount_Enable = 0; 
        struTimeOut[TimerChannel].bTimeout_Flg = 0;       
        struTimeOut[TimerChannel].hCount = 0;            
    }
}
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_OverTime_Handle(void)
* Visibility  :        private
* Description :        1s and 7.5s overtime for backlight brightness
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CLedCtrl_OverTime_Handle(void)                         /*the period is 10ms*/
{
    static const uint16_t ahTimeOut_Threshold[] = {OVERTIME_1S_COUNT_NUM,OVERTIME_7S5_COUNT_NUM};

    for (uint8_t i=0; i<(sizeof(ahTimeOut_Threshold)/sizeof(*ahTimeOut_Threshold)); i++)                  /*The timeout is calculated by polling*/
    {
        if (1u == struTimeOut[i].bCount_Enable)                  /*judge timeout count enable or not*/
        {
            if (struTimeOut[i].hCount < ahTimeOut_Threshold[i])    /*judge timeout count reach threshold or not*/
            {
                struTimeOut[i].hCount ++;
            }
            else
            {
                struTimeOut[i].bTimeout_Flg = 1;                /*occur timeout*/
            }
        }
        else
        {/*do nothing*/}
    }
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_PwmValue_Clear(void)
* Visibility  :        public
* Description :        clear backlight
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CLedCtrl_PwmValue_Clear(void)
{
    Pwm_Current_Value = 0;  
    /*close backlight brightness*/    
    LED_If_Set_PWM_Duty(OUTPUT_PWM_ZERO_VALUE);
}
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Linear(uint32_t temper,bool active)
* Visibility  :        private
* Description :        linear calculate between temperature and PWM value
* Parameters  :        uint32_t temper
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint16_t 
* ===================================================================================================================*/
static uint16_t CLedCtrl_Linear(uint32_t temper,bool active)
{
    static const uint32_t Temper_Led_Input_Para[]={TEMPER_LED_ADC_ABOVE_88C, TEMPER_LED_ADC_ABOVE_85C, TEMPER_LED_ADC_ABOVE_80C, TEMPER_LED_ADC_ABOVE_75C};
    static const uint32_t Temper_Pcba_Input_Para[]={TEMPER_PCBA_ADC_ABOVE_88C, TEMPER_PCBA_ADC_ABOVE_85C, TEMPER_PCBA_ADC_ABOVE_80C, TEMPER_PCBA_ADC_ABOVE_75C};

    uint8_t  point_pos_low = 0;
    uint8_t  point_pos_high = 3;
    uint8_t  point_pos_mid = 1;
    uint32_t pwm_duty ;
    uint32_t temper_diffence_value ;
    uint32_t pwm_diffence_value ;
    uint32_t const * temper_input_para; 
    /*PWM duty cycle: 0,20%,60%,100%*/
    uint32_t const pwm_output_duty[] = {0, 13107, 39321, 65535}; 
    uint32_t temp_min_adc;
    uint32_t temp_max_adc;
    if(active)
    {
        temper_input_para=Temper_Led_Input_Para;
        temp_min_adc=TEMPER_LED_ADC_ABOVE_88C;
        temp_max_adc=TEMPER_LED_ADC_ABOVE_75C;
    }		
    else
    {
        temper_input_para=Temper_Pcba_Input_Para;
        temp_min_adc=TEMPER_PCBA_ADC_ABOVE_88C;
        temp_max_adc=TEMPER_PCBA_ADC_ABOVE_75C;
    }
    /*Find the position of temper in the table by polling,  dichotomy*/
    while(point_pos_low < (point_pos_high - 1u))
    {
        if(temper_input_para[point_pos_mid] == temper)
        {
            point_pos_low = point_pos_mid;
            point_pos_high = point_pos_mid;
        }
        else if(temper_input_para[point_pos_mid] > temper)
        {
            point_pos_high = point_pos_mid;
            point_pos_mid = (point_pos_low + point_pos_high) / 2u;
        }
        else
        {
            point_pos_low = point_pos_mid;
            point_pos_mid = (point_pos_low + point_pos_high) / 2u;
        }
    }

    /* add maximum and minimum limits */
    if (temp_min_adc >= temper)
    {
        point_pos_low  = 0;
        point_pos_high = 0;
    }
    else
    {/*do nothing*/}
    
    if (temp_max_adc <= temper)
    {
        point_pos_low  = 3;
        point_pos_high = 3;
    }
    else
    {/*do nothing*/}
    
    if(point_pos_low == point_pos_high)
    {
        pwm_duty = pwm_output_duty[point_pos_low];
    }
    else
    {
        /*calculate the pwm duty cycle value*/
        temper_diffence_value = temper_input_para[point_pos_high] - temper_input_para[point_pos_low];
        pwm_diffence_value = pwm_output_duty[point_pos_high] - pwm_output_duty[point_pos_low];

        pwm_duty = pwm_output_duty[point_pos_low] + (pwm_diffence_value * (temper - temper_input_para[point_pos_low]) / temper_diffence_value);
    }
    return (uint16_t)pwm_duty;
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Pwm_Calculate(uint16_t pwm_dec)
* Visibility  :        public
* Description :        According to the input parameter, we calculate pwm value to save.
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static void CLedCtrl_Pwm_Calculate(uint16_t pwm_dec)
{
    Pwm_Expect_Value = pwm_dec;
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Pwm_Refresh(void)
* Visibility  :        public
* Description :        this function will be called by the cycle task(50ms)
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static void CLedCtrl_Pwm_Refresh(void)
{
    int32_t pwm_value_diff;
    SOAK_ACTION_STATUS     soak_sta = CSoakManage_Get_SoakStatus();
    THERMAL_ACTION_STATUS  thermal_sta = CThermalManage_Get_ThermalStatus();
    int32_t expect_val = (int32_t)Pwm_Expect_Value;
    int32_t current_val =(int32_t)Pwm_Current_Value;

    /* set pwm value step by step for display */    
    #if ((POWER_MODULE == 1) && (SOAK_MODULE == 1) && (THERMAL_MODULE == 1))
    if((SOAK_FULLY_OPERATION_LEVEL == soak_sta)||(THERMAL_PARTIALLY_OPERATION_LEVEL > thermal_sta))
    {
        if(expect_val != current_val)
        {             
            /*smooth algorithm*/ 
            pwm_value_diff = expect_val - current_val;
            current_val += pwm_value_diff * (BRIGHTNESS_LAG_FILTER_RANGE - BRIGHTNESS_LAG_FILTER_CONST) / 
                                  BRIGHTNESS_LAG_FILTER_RANGE;
            if((current_val + 6) == expect_val)
            {
                current_val = expect_val;
            }
            else if((current_val - 6) == expect_val)
            {
                current_val = expect_val;
            }
            else  
            {/* do nothing */}
            /*set pwm duty cycle*/
            LED_If_Set_PWM_Duty((uint16_t)current_val);
        }
        else
        {/*do nothing*/}
    }
    else
    {
        if(expect_val != current_val) 
        {  
            current_val = expect_val;
            LED_If_Set_PWM_Duty((uint16_t)current_val);
        }
        else
        {/*do nothing*/}
    }
    Pwm_Current_Value = (uint16_t)current_val;
    #endif
}
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        public
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/

static void CLedCtrl_PowerMode_UpStatus(void)
{
#if (POWER_MODULE == 1)
    switch(CPowerManage_Get_PowerStatus())
    {
        case FULLY_OPERATION_LEVEL:
            Pwm_Data.power_flag = POWER_NORMAL;         /*normal mode*/
            Pwm_AllowMax_Value = OUTPUT_PWM_FULL_VALUE;
            break;
        case PARTIALLY_OPERATION_LOW:
            Pwm_Data.power_flag = POWER_PARITAL_LOW;     /*iic keep open*/
            Pwm_AllowMax_Value = OUTPUT_PWM_FULL_VALUE;  /*Guarantee PWM_Expect_Value not cleared 0*/
            break;
        default:
            Pwm_Data.power_flag = POWER_ABNORMAL;        /*abnormal mode*/
            Pwm_AllowMax_Value = OUTPUT_PWM_ZERO_VALUE;
            break; 
    }
#endif
}
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        public
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/

static void CLedCtrl_SoakMode_UpStatus(void)
{
#if ((POWER_MODULE == 1)&&(SOAK_MODULE == 1))
    uint16_t tem;
    SOAK_ACTION_STATUS soak_sta = CSoakManage_Get_SoakStatus();

    if(Pwm_Data.power_flag == POWER_NORMAL)
    {
        switch(soak_sta)
        {
            case SOAK_INIT_LEVEL:
            case SOAK_FULLY_OPERATION_LEVEL:
                Pwm_Data.soak_flag = SOAK_ENTRY;                 /*enable soak function*/
                tem = ADC_If_Get_LED_NTC2();  /*not sure user LEDNTC1 or LEDNTC2*/
                if(tem<LED_ADC_ABOVE_95C)
                {
                    Pwm_AllowMax_Value = PWM_SOAK_MAX_VALUE;   /*maximum 75%*/
                }
                else
                {
                    Pwm_AllowMax_Value=OUTPUT_PWM_FULL_VALUE;
                }
                break;           
            default:
                Pwm_Data.soak_flag = SOAK_EXIT;               /*disable soak function*/
                break;
        }
    }
#endif
}
/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        public
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static void CLedCtrl_ThermalMode_UpStatus(void)
{
#if ((POWER_MODULE == 1)&&(THERMAL_MODULE == 1))
    THERMAL_VALUE value;
    THERMAL_ACTION_STATUS thermal_sta = CThermalManage_Get_ThermalStatus();

    if((Pwm_Data.power_flag == POWER_NORMAL)&&(Pwm_Data.soak_flag == SOAK_EXIT))
    {                           
        switch(thermal_sta)
        {
            case THERMAL_BACKLIGHT_GRADUALLY_OFF_LEVEL:                      
                value=CThermalManage_Get_ThermalValue();
                Pwm_AllowMax_Value = CLedCtrl_Linear((uint32_t)value.thermal_value,value.lntc_is_active);  /*derating mode*/
            break;
            case THERMAL_PARTIALLY_OPERATION_LEVEL:      /*partilaly mode*/
                Pwm_AllowMax_Value = OUTPUT_PWM_ZERO_VALUE;
            break;
            case THERMAL_INIT_LEVEL:
            break;
            default:
                Pwm_AllowMax_Value = OUTPUT_PWM_FULL_VALUE;  
            break; 
        }
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        public
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static void CLedCtrl_LED_TimeOutUpVal(void)
{
    bool reflg = CI2CDimming_Get_PWM_RefreshFlg();
    if(POWER_PARITAL_LOW <= Pwm_Data.power_flag)          
    {    
        /*judge new backlight PWM data recieved from master or not*/      
        if(reflg)
        {          
            CI2CDimming_Clear_PWM_RefreshFlg();
            set_pwm_duty = CI2CDimming_Get_PWM_Value();       /*get PWM balue*/
            Wakeup_Detect_Flag = true;
            CLedCtrl_Timer_Disable(WAKEUP_1S);
            CLedCtrl_Timer_Disable(BACKLIGHT_7S5);
        }
        else
        {
            if (1u == struTimeOut[WAKEUP_1S].bTimeout_Flg)
            {
                set_pwm_duty = PWM_DEFAULT_VALUE;
                Pwm_AllowMax_Value = PWM_DEFAULT_VALUE;
                CLedCtrl_Timer_Disable(WAKEUP_1S);
                Wakeup_Detect_Flag = true;
            }
            else
            {/*do nothing*/}           
            /*When it wakes up for 1s, no II command is received, or no IIC command is received for 7.5S, 
            set PWM duty cycle as default value*/
            if((true  == Wakeup_Detect_Flag) && (1u == struTimeOut[BACKLIGHT_7S5].bTimeout_Flg))
            {
                set_pwm_duty = PWM_DEFAULT_VALUE;
            }
            else
            {/*do nothing*/}
            
            CLedCtrl_Timer_Enable(BACKLIGHT_7S5); /*enable 7.5s timeout count when pwm value have not refresh*/
        }
    }                            
    else    
    {   
        if(reflg)    /*if i2c refresh, clear it, do not deal with the pwm value*/
        {
            CI2CDimming_Clear_PWM_RefreshFlg();
        }
        set_pwm_duty = PWM_INIT_VALUE; 
        CLedCtrl_Timer_Disable(BACKLIGHT_7S5);           
    }

}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        
* Visibility  :        public
* Description :        
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static void CLedCtrl_LED_UpTask(void)
{
    if(false == Wakeup_Detect_Flag)
    {
        Pwm_AllowMax_Value = PWM_INIT_VALUE;
    }
    else
    {/*do nothing*/}
    /*do not allow PWM duty cycle value over Pwm_AllowMax_Value*/
    if(Pwm_AllowMax_Value < set_pwm_duty)
    {
        set_pwm_duty = Pwm_AllowMax_Value;
    }
    else
    {/*do nothing*/}
    /*pwm value less than OUTPUT_PWM_MIN_PRECNET, should clear to 0 */
    if (OUTPUT_PWM_MIN_PRECNET > set_pwm_duty)
    {
        set_pwm_duty = OUTPUT_PWM_ZERO_VALUE;
    }
    else
    {/*do nothing*/}
    /*pwm value bigger than OUTPUT_PWM_MAX_PRECNET, should set to 100%*/
    if (OUTPUT_PWM_MAX_PRECNET < set_pwm_duty)
    {
        set_pwm_duty = OUTPUT_PWM_FULL_VALUE;
    }
    else
    {/*do nothing*/}
                     
    CLedCtrl_Pwm_Calculate(set_pwm_duty);    /*calculate led_expect_value*/

    if (true == bBl_En_Status)               /*It can only be executed when bl_en is at high level*/
    {
        CLedCtrl_Pwm_Refresh();              /*refresh PWM duty cycle value*/
    }
    else
    {/*do nothing*/}

}

/*=====================================================================================================================
* Upward trac.:        APP/LED/App_led_ctl
* Method      :        CLedCtrl_Task(void)
* Visibility  :        public
* Description :        LedCtrl app task funciton
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CLedCtrl_Task(void)
{              
    CLedCtrl_PowerMode_UpStatus();
    CLedCtrl_SoakMode_UpStatus();
    CLedCtrl_ThermalMode_UpStatus();

    /*work in FULLY_OPERATION_LEVEL and POWER_PARITAL_LOW mode*/
    if(POWER_PARITAL_LOW <= Pwm_Data.power_flag)          
    {
        CLedCtrl_OverTime_Handle();  /*calculate 7.5s timeout and 1s timeout*/
    }
    
    CLedCtrl_LED_TimeOutUpVal();        
    CLedCtrl_LED_UpTask();
	
}

#endif /* LED_MODULE */
