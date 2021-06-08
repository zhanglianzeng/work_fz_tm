/******************************************************************************
*  Name: app_thermal_manage.c
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#include <stdlib.h>

#if (THERMAL_MODULE == 1)
#include "app_thermal_manage.h"

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
#define ADC_SENSITIVITY_THRESHOLD  11
/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static THERMAL_ACTION_STATUS Thermal_Status = THERMAL_INIT_LEVEL;
THERMAL_VALUE Thermal_Value;  /*led ntc adc value*/
static int8_t LED_TempValue;    /*led temperature value*/
static int8_t PCB_TempValue;    /*pcb temperature value*/

const uint16_t Led_Region_Value[]={TEMPER_ADC_OPEN,TEMPER_LED_ADC_ABOVE_75C,TEMPER_LED_ADC_ABOVE_86C,TEMPER_LED_ADC_ABOVE_88C,TEMPER_ADC_SHORT};
const uint16_t Pcba_Region_Value[]={TEMPER_ADC_OPEN,TEMPER_PCBA_ADC_ABOVE_75C,TEMPER_PCBA_ADC_ABOVE_86C,TEMPER_PCBA_ADC_ABOVE_88C,TEMPER_ADC_SHORT};
const THERMAL_REGION Region_Value[]={THERMAL_FULLY_REGION,THERMAL_DERATING_REGION,THERMAL_WITHDRAW_REGION,THERMAL_PARTIALLY_REGION};
  
bool Temper_Display_Close_Flag = false;
THERMAL_NTC_STATUS Thermal_Led_Status = { 0,true,false,false,0,0,0,false };
THERMAL_NTC_STATUS Thermal_Pcba_Status = { 0,true,false,false,0,0,0,false };


#if (ETM_MODULE == 1)
/*table of temperature value and NTC adc value*/
static const STRUTEMP_CALC struTemp_Calc[TEMPER_ADC_CALU_NUM] = 
{
    {127, 0},{127, 0xC5}, {120, 0xE6},{110, 0x120},{105, 0x143},{103, 0x152},{101, 0x163},{99, 0x174},
    {97, 0x186},{95, 0x199},{90, 0x1CC},{80, 0x249},{70, 0x2EA},{60, 0x3B4},{50, 0x4B3},{40, 0x5E5},{30, 0x743},
    {20, 0x8C0},{10, 0xA45},{0, 0xBB3},{-10, 0xCF3},{-20, 0xDF4},{-30, 0xEB3},{-38, 0xF22},{-40, 0xF38},{-41, 0xFFF},   
};
#endif
/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void CThermalManage_check_Ntc(void);
static void CThermalManage_recover_Ntc(THERMAL_NTC_STATUS* ntc_status);
static void CThermalManage_update_Ntcsta(THERMAL_NTC_STATUS* ntc_status);
static THERMAL_ACTION_STATUS CThermalManage_LED_Status(uint16_t temper_ADC);
static THERMAL_ACTION_STATUS CThermalManage_PCBA_Status(uint16_t temper_ADC);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Init(void)
* Visibility  :        public
* Description :        App thermal init function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CThermalManage_Init(void)
{
    Thermal_Status = THERMAL_INIT_LEVEL;
}
/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Set_ThermalStatus(THERMAL_ACTION_STATUS Thermal_Stat)
* Visibility  :        private
* Description :        Set Thermal Status
* Parameters  :        THERMAL_ACTION_STATUS Thermal_Stat
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CThermalManage_Set_ThermalStatus(THERMAL_ACTION_STATUS Thermal_Stat)
{
    Thermal_Status = Thermal_Stat;
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Get_ThermalStatus(void)
* Visibility  :        public
* Description :        Get Thermal Status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  THERMAL_ACTION_STATUS 
* ===================================================================================================================*/
THERMAL_ACTION_STATUS CThermalManage_Get_ThermalStatus(void)
{
    return Thermal_Status;
}
/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        public
* Description :        Set led backlight ntc adc Value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint16_t 
* ===================================================================================================================*/
static void CThermalManage_Set_ThermalValue(uint16_t value,bool led_act)
{
    Thermal_Value.thermal_value=value;
    Thermal_Value.lntc_is_active = led_act;
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Get_ThermalValue(void)
* Visibility  :        public
* Description :        Get led backlight ntc adc Value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint16_t 
* ===================================================================================================================*/
THERMAL_VALUE CThermalManage_Get_ThermalValue(void)
{
    return Thermal_Value;
}

#if (ETM_MODULE == 1)
/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Get_LED_NTC_Value(void)
* Visibility  :        public
* Description :        Get LED Thermal Value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int8_t 
* ===================================================================================================================*/
int8_t CThermalManage_Get_LED_NTC_Value(void)
{
    return LED_TempValue;
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Get_PCB_NTC_Value(void)
* Visibility  :        public
* Description :        Get PCB Thermal Value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int8_t 
* ===================================================================================================================*/
int8_t CThermalManage_Get_PCB_NTC_Value(void)
{
    return PCB_TempValue;
}
/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int8_t
* ===================================================================================================================*/
static uint16_t CThermalManage_TempSenCalc(uint16_t last_ADC,uint16_t current_ADC)
{
    int16_t diff = 0;
    uint16_t value = 0;
    diff = (int16_t)last_ADC - (int16_t)current_ADC;
    if(abs(diff) > ADC_SENSITIVITY_THRESHOLD)
    {
        value = current_ADC;
    }
    else
    {
        value = last_ADC;
    }
    return value;
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_TempCalc(uint16_t temper_ADC)
* Visibility  :        private
* Description :        Calculate Thermal Value according to the input temper_ADC value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int8_t
* ===================================================================================================================*/
static int8_t CThermalManage_TempCalc(uint16_t temper_ADC)
{
    int8_t temp = 0;    /*Add for QAC test, Using value of uninitialized automatic object*/
    uint16_t diff;
    /*Find the position of temper_ADC in the table by polling*/    	
    for(uint8_t i = 0; i < TEMPER_ADC_CALU_NUM; i++)
    {
        if(struTemp_Calc[i].adc_value >= temper_ADC)
        {
            if(0 == i)  /*temper_ADC == 0*/
            {
                temp = struTemp_Calc[i].temp;
                break;
            }
            else
            {
                /*calculate temperature value*/
                diff = (struTemp_Calc[i].adc_value - temper_ADC)*10 /(struTemp_Calc[i].adc_value - struTemp_Calc[i-1].adc_value);                 
                temp = struTemp_Calc[i].temp + diff * (struTemp_Calc[i-1].temp - struTemp_Calc[i].temp) / 10;						
                break;   
            }							
        }
        else
        {/*do nothing*/}          
    }
				 
    return temp;
}
#endif

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        static
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  
* ===================================================================================================================*/
static THERMAL_REGION CThermalManage_Set_LED_ADC_Region(uint16_t temper_ADC)
{
    uint8_t index = 0;
    while(temper_ADC<=Led_Region_Value[index])
    {
        index++;
        if(index==sizeof(Led_Region_Value)/sizeof(*Led_Region_Value))
        {
            index-=1;
            break;
        }
    }
    if(index==0)
    {
        index+=1;
    }
    return Region_Value[index-1];     
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        static
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  
* ===================================================================================================================*/
static THERMAL_REGION CThermalManage_Set_PCBA_ADC_Region(uint16_t temper_ADC)
{
    uint8_t index = 0;
    while(temper_ADC<=Pcba_Region_Value[index])
    {
        index++;
        if(index==sizeof(Pcba_Region_Value)/sizeof(*Pcba_Region_Value))
        {
            index-=1;
            break;
        }        
    }
    if(index==0)
    {
        index+=1;
    }
    return Region_Value[index-1];        
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        static
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  THERMAL_ACTION_STATUS 
* ===================================================================================================================*/

static THERMAL_ACTION_STATUS CThermalManage_Region_Set_Thermalta(THERMAL_REGION region)
{
    THERMAL_ACTION_STATUS Thermal_Status_Curr;
    switch(region)
    {
        case THERMAL_FULLY_REGION:
            Thermal_Status_Curr=THERMAL_FULLY_OPERATION_LEVEL;
            break;       
        case THERMAL_PARTIALLY_REGION:
            Thermal_Status_Curr=THERMAL_PARTIALLY_OPERATION_LEVEL;            
            break;           
        default:
            Thermal_Status_Curr=THERMAL_BACKLIGHT_GRADUALLY_OFF_LEVEL;
        break;
    }
    return Thermal_Status_Curr;
}


/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        static
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  THERMAL_ACTION_STATUS 
* ===================================================================================================================*/

static THERMAL_ACTION_STATUS CThermalManage_LED_Status(uint16_t temper_ADC)
{
    static THERMAL_REGION last_region = THERMAL_FULLY_REGION ;
    static uint16_t last_adc = TEMPER_LED_ADC_INIT;
    THERMAL_REGION curren_region;    
    uint16_t current_adc;    
    THERMAL_ACTION_STATUS Thermal_Status_Curr;	
    current_adc=temper_ADC;
    curren_region=CThermalManage_Set_LED_ADC_Region(current_adc); 
    if(THERMAL_WITHDRAW_REGION==last_region)
    {
        if(THERMAL_WITHDRAW_REGION==curren_region)
        {
            current_adc = CThermalManage_TempSenCalc(last_adc,current_adc);
        }
    }
    else if(THERMAL_PARTIALLY_REGION==last_region)
    {
        if(THERMAL_WITHDRAW_REGION==curren_region)
        {
            curren_region=THERMAL_PARTIALLY_REGION;
            current_adc = last_adc;
        }
    }
    Thermal_Status_Curr=CThermalManage_Region_Set_Thermalta(curren_region);
    CThermalManage_Set_ThermalValue(current_adc,true);
    last_adc=current_adc;
    return Thermal_Status_Curr;
}
/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Status(uint32_t temper_ADC)
* Visibility  :        static
* Description :        This function manages display status based on temperature
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  THERMAL_ACTION_STATUS 
* ===================================================================================================================*/
static THERMAL_ACTION_STATUS CThermalManage_PCBA_Status(uint16_t temper_ADC)
{
    static THERMAL_REGION last_region = THERMAL_FULLY_REGION ;
    static uint16_t last_adc = TEMPER_PCBA_ADC_INIT;
    THERMAL_REGION curren_region;    
    uint16_t current_adc;    
    THERMAL_ACTION_STATUS Thermal_Status_Curr;	
    current_adc=temper_ADC;
    curren_region=CThermalManage_Set_PCBA_ADC_Region(current_adc); 
    if(THERMAL_WITHDRAW_REGION==last_region)
    {
        if(THERMAL_WITHDRAW_REGION==curren_region)
        {
            current_adc = CThermalManage_TempSenCalc(last_adc,current_adc);
        }
    }
    else if(THERMAL_PARTIALLY_REGION==last_region)
    {
        if(THERMAL_WITHDRAW_REGION==curren_region)
        {
            curren_region=THERMAL_PARTIALLY_REGION;
            current_adc = last_adc;
        }
    }
    Thermal_Status_Curr=CThermalManage_Region_Set_Thermalta(curren_region);
    CThermalManage_Set_ThermalValue(current_adc,false);
    last_adc=current_adc;
    return Thermal_Status_Curr;

}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/

static void CThermalManage_recover_Ntc(THERMAL_NTC_STATUS* ntc_status)
{
    uint16_t value = 0;
    if(ntc_status==&Thermal_Led_Status)
        value = ADC_If_Get_LED_NTC2();
    else
        value = ADC_If_Get_PCB_NTC();		
    if (false == ntc_status->normal_flag)
    {
        if((value > TEMPER_ADC_SHORT)&&(value < TEMPER_ADC_OPEN))
        {
            ntc_status->recorver_counter++;
            if (ntc_status->recorver_counter == TEMPER_COUTER_SPAN)
            {
                /*Has recovered*/
                ntc_status->recorver_counter=0;
                ntc_status->normal_flag = true;
                Temper_Display_Close_Flag = false;
            }
        }
        else
        {
             /*No recovery*/
             ntc_status->recorver_counter = 0;
        }
			
    }
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CThermalManage_update_Ntcsta(THERMAL_NTC_STATUS *ntc_status)
{			
    if(ntc_status->adc_value <= TEMPER_ADC_SHORT)
    { 
    /*The obtained adc value points to a short*/
        if (false==ntc_status->short_flag)
        {
            ntc_status->short_flag = true;
            ntc_status->short_counter = 0;
        }
        ntc_status->short_counter++;
        if (ntc_status->short_counter == TEMPER_COUTER_SPAN)
        {
            ntc_status->short_counter = 0;
            ntc_status->normal_flag = false;
        }
        ntc_status->value_valid_flag = false;
    }
    else
    {
        ntc_status->short_flag = false;
        if (ntc_status->adc_value >= TEMPER_ADC_OPEN)
        {
            if (false == ntc_status->open_flag)
            {
                ntc_status->open_flag = true;
                ntc_status->open_counter = 0;
            }
            ntc_status->open_counter++;
            if (ntc_status->open_counter == TEMPER_COUTER_SPAN)
            {
                ntc_status->open_counter = 0;
                ntc_status->normal_flag = false;
            }
            ntc_status->value_valid_flag = false;
        }
        else
        {
            ntc_status->open_flag = false;
            ntc_status->value_valid_flag = true;
        }
    }	
}	

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        
* Visibility  :        
* Description :        
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CThermalManage_check_Ntc(void)
{
    if (Thermal_Led_Status.normal_flag)
    {
        /*read the adc value*/
        Thermal_Led_Status.adc_value=ADC_If_Get_LED_NTC2();
        CThermalManage_update_Ntcsta(&Thermal_Led_Status);
    }
    else
    {
        if (Thermal_Pcba_Status.normal_flag)
        {
            Thermal_Pcba_Status.adc_value = ADC_If_Get_PCB_NTC();
            CThermalManage_update_Ntcsta(&Thermal_Pcba_Status);
        }
        else
        {
            if (Temper_Display_Close_Flag == false)
            {
                Temper_Display_Close_Flag = true;
                /*close the display*/
                CThermalManage_Set_ThermalStatus(THERMAL_PARTIALLY_OPERATION_LEVEL);
            }
            else
            {
                /*It has been shut down! Do not repeat off!*/
            }				
        }
    }
}

/*=====================================================================================================================
* Upward trac.:        APP/thermal/App_thermal_manage
* Method      :        CThermalManage_Task(void)
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
void CThermalManage_Task(void)
{ 
#if	(ADC_MODULE == 1)
    THERMAL_ACTION_STATUS current_sta;
    static uint16_t beat_number=0;
    CThermalManage_check_Ntc();
    if(Thermal_Led_Status.value_valid_flag)
    {
        current_sta=CThermalManage_LED_Status(Thermal_Led_Status.adc_value);
        CThermalManage_Set_ThermalStatus(current_sta);
    }
    else
    {
        if(Thermal_Pcba_Status.value_valid_flag)
        {
            current_sta=CThermalManage_PCBA_Status(Thermal_Pcba_Status.adc_value);
            CThermalManage_Set_ThermalStatus(current_sta);
        }
        else
        {
            /*adc value is not valid,not used to set thermal mode*/
        }
    }
    /*500ms check if recover*/
    beat_number++;
    if(beat_number>=NTC_RECOVER_INTERVAL)
    {
        beat_number=0;
        CThermalManage_recover_Ntc(&Thermal_Led_Status);
        CThermalManage_recover_Ntc(&Thermal_Pcba_Status);
    }   
#if (ETM_MODULE == 1)
    /*calculate backlight temperature*/
    LED_TempValue = CThermalManage_TempCalc(ADC_If_Get_LED_NTC2());
    /*calculate PCB board temperature*/
    PCB_TempValue = CThermalManage_TempCalc(ADC_If_Get_PCB_NTC());
    #endif
#endif
}

#endif /* THERMAL_MODULE */

