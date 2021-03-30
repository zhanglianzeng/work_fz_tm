/******************************************************************************
*  Name: adc_if.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (ADC_MODULE == 1)
#include "adc_if.h"
#include "../../driver/adc.h"
#include "../../../Device/samc21n18a.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
/* Parameter coefficient is enlarged by 10000 */
static const uint32_t ADC_FirB[FIR_ORDER] = {144, 439, 1202, 2025, 2380, 2025, 1202, 439, 144};

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static uint16_t ADC_NTC1_Value_Buff[ADC_BUFFER_LENGTH] = {0};
static uint8_t ADC_NTC1_Buff_Number = 0;

static uint16_t ADC_NTC2_Value_Buff[ADC_BUFFER_LENGTH] = {0};
static uint8_t ADC_NTC2_Buff_Number = 0;

static uint16_t ADC_Power_Value_Buff[FIR_ORDER] = {0};
static uint8_t ADC_Power_Buff_Number = 0;

static uint16_t ADC_Temp_Value_Buff[FIR_ORDER] = {0};
static uint8_t ADC_Temp_Buff_Number = 0;
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Init(void)
* Visibility  :        public
* Description :        Init of adc module 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
void ADC_If_Init(void)
{
    ADC_Init(ADC0,0);
    ADC_Init(ADC1,1);
	
    ADC_Enable(ADC0);
    ADC_Enable(ADC1);
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Standby(void)
* Visibility  :        public
* Description :        Deinit of adc module 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
void ADC_If_Standby(void)
{
    ADC_Deinit(ADC0);
    ADC_Deinit(ADC1);
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Get_LED_NTC1(void)
* Visibility  :        public
* Description :        This function is to obtain the filtered LED NTC value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
uint32_t ADC_If_Get_LED_NTC1(void)
{
    uint32_t ADC_NTC_Value = 0;
    uint8_t i;
    uint16_t adc_buff[FIR_ORDER],adc_max = 0,adc_min = 0xFFFF;
    uint8_t adc_max_index, adc_min_index,index = 0;

    ADC_NTC1_Value_Buff[ADC_NTC1_Buff_Number++] = ADC_Read(ADC0,ADC_LED_NTC1_CH);

    if((ADC_NTC1_Buff_Number < ADC_BUFFER_LENGTH) && (ADC_NTC1_Value_Buff[ADC_NTC1_Buff_Number] == 0))
    {
        for(i = ADC_NTC1_Buff_Number; i < ADC_BUFFER_LENGTH; i++)
            ADC_NTC1_Value_Buff[i] = ADC_NTC1_Value_Buff[i - 1];
    }

    if(ADC_NTC1_Buff_Number == ADC_BUFFER_LENGTH)
    {
        ADC_NTC1_Buff_Number = 0;
    }
    
    for(i = 0; i < ADC_BUFFER_LENGTH; i++)
    {
        if(adc_max < ADC_NTC1_Value_Buff[i])
        {            
            adc_max = ADC_NTC1_Value_Buff[i];
            adc_max_index = i;
        }
        
        if (adc_min > ADC_NTC1_Value_Buff[i])
        {
            adc_min = ADC_NTC1_Value_Buff[i];
            adc_min_index = i;
        }
    }
    
    for(i = 0; i < ADC_BUFFER_LENGTH; i++)
    {
        if((i != adc_max_index) && (i != adc_min_index))
        {
            adc_buff[index++] = ADC_NTC1_Value_Buff[i];
        }
    }
    
    for(i = 0; i < FIR_ORDER; i++)
    {
        ADC_NTC_Value += ADC_FirB[FIR_ORDER - 1 - i] *
                         adc_buff[(ADC_NTC1_Buff_Number + i) % FIR_ORDER];
    }

    ADC_NTC_Value /= 10000;

    return ADC_NTC_Value;
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Get_LED_NTC2(void)
* Visibility  :        public
* Description :        This function is to obtain the filtered LED NTC value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
uint32_t ADC_If_Get_LED_NTC2(void)
{
    uint32_t ADC_NTC_Value = 0;
    uint8_t i;
    uint16_t adc_buff[FIR_ORDER],adc_max = 0,adc_min = 0xFFFF;
    uint8_t adc_max_index, adc_min_index,index = 0;

    ADC_NTC2_Value_Buff[ADC_NTC2_Buff_Number++] = ADC_Read(ADC1,ADC_LED_NTC2_CH);

    if((ADC_NTC2_Buff_Number < ADC_BUFFER_LENGTH) && (ADC_NTC2_Value_Buff[ADC_NTC2_Buff_Number] == 0))
    {
        for(i = ADC_NTC2_Buff_Number; i < ADC_BUFFER_LENGTH; i++)
            ADC_NTC2_Value_Buff[i] = ADC_NTC2_Value_Buff[i - 1];
    }

    if(ADC_NTC2_Buff_Number == ADC_BUFFER_LENGTH)
    {
        ADC_NTC2_Buff_Number = 0;
    }
    
    for(i = 0; i < ADC_BUFFER_LENGTH; i++)
    {
        if(adc_max < ADC_NTC2_Value_Buff[i])
        {            
            adc_max = ADC_NTC2_Value_Buff[i];
            adc_max_index = i;
        }
        
        if (adc_min > ADC_NTC2_Value_Buff[i])
        {
            adc_min = ADC_NTC2_Value_Buff[i];
            adc_min_index = i;
        }
    }
    
    for(i = 0; i < ADC_BUFFER_LENGTH; i++)
    {
        if((i != adc_max_index) && (i != adc_min_index))
        {
            adc_buff[index++] = ADC_NTC2_Value_Buff[i];
        }
    }
    
    for(i = 0; i < FIR_ORDER; i++)
    {
        ADC_NTC_Value += ADC_FirB[FIR_ORDER - 1 - i] *
                         adc_buff[(ADC_NTC2_Buff_Number + i) % FIR_ORDER];
    }

    ADC_NTC_Value /= 10000;

    return ADC_NTC_Value;
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Get_Input_Vol(void)
* Visibility  :        public
* Description :        This function is to obtain the filtered voltage value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   uint32_t
* ===================================================================================================================*/
uint32_t ADC_If_Get_Input_Vol(void)
{
    uint32_t ADC_Power_Value = 0;
    uint8_t i;


    ADC_Power_Value_Buff[ADC_Power_Buff_Number++] = ADC_Read(ADC0,ADC_POWER_CH);

    if((ADC_Power_Buff_Number < FIR_ORDER) && (ADC_Power_Value_Buff[ADC_Power_Buff_Number] == 0))
    {
        for(i = ADC_Power_Buff_Number; i < FIR_ORDER; i++)
            ADC_Power_Value_Buff[i] = ADC_Power_Value_Buff[i - 1];
    }

    if(ADC_Power_Buff_Number == (FIR_ORDER))
    {
        ADC_Power_Buff_Number = 0;
    }

    for(i = 0; i < FIR_ORDER; i++)
    {
        ADC_Power_Value += ADC_FirB[FIR_ORDER - 1 - i] *
                           ADC_Power_Value_Buff[(ADC_Power_Buff_Number + i) % FIR_ORDER];
    }

    ADC_Power_Value /= 10000;

    return ADC_Power_Value;
}
/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Get_Vref_Vol(void)
* Visibility  :        public
* Description :        This function is to obtain the filtered voltage value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   uint32_t
* ===================================================================================================================*/
uint32_t ADC_If_Get_Vref_Vol(void)
{
    uint32_t ADC_Power_Value = 0;
    uint8_t i;

    ADC_Power_Value_Buff[ADC_Power_Buff_Number++] = ADC_Read(ADC0,ADC_REF_CH);

    if((ADC_Power_Buff_Number < FIR_ORDER) && (ADC_Power_Value_Buff[ADC_Power_Buff_Number] == 0))
    {
        for(i = ADC_Power_Buff_Number; i < FIR_ORDER; i++)
            ADC_Power_Value_Buff[i] = ADC_Power_Value_Buff[i - 1];
    }

    if(ADC_Power_Buff_Number == (FIR_ORDER))
    {
        ADC_Power_Buff_Number = 0;
    }

    for(i = 0; i < FIR_ORDER; i++)
    {
        ADC_Power_Value += ADC_FirB[FIR_ORDER - 1 - i] *
                           ADC_Power_Value_Buff[(ADC_Power_Buff_Number + i) % FIR_ORDER];
    }

    ADC_Power_Value /= 10000;

    return ADC_Power_Value;
}

/*=====================================================================================================================
* Upward trac.:        MIDDLE/ADC/ADC_IF
* Method      :        ADC_If_Get_PCB_NTC(void)
* Visibility  :        public
* Description :        This function is to obtain the filtered PCB NTC value
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   uint32_t
* ===================================================================================================================*/
uint32_t ADC_If_Get_PCB_NTC(void)
{
    uint32_t ADC_Temp_Value = 0;
    uint8_t i;

    ADC_Temp_Value_Buff[ADC_Temp_Buff_Number++] = ADC_Read(ADC1,ADC_PCB_NTC_CH);

    if((ADC_Temp_Buff_Number < FIR_ORDER) && (ADC_Temp_Value_Buff[ADC_Temp_Buff_Number] == 0))
    {
        for(i = ADC_Temp_Buff_Number; i < FIR_ORDER; i++)
            ADC_Temp_Value_Buff[i] = ADC_Temp_Value_Buff[i - 1];
    }

    if(ADC_Temp_Buff_Number == (FIR_ORDER))
    {
        ADC_Temp_Buff_Number = 0;
    }

    for(i = 0; i < FIR_ORDER; i++)
    {
        ADC_Temp_Value += ADC_FirB[FIR_ORDER - 1 - i] *
                           ADC_Temp_Value_Buff[(ADC_Temp_Buff_Number + i) % FIR_ORDER];
    }

    ADC_Temp_Value /= 10000;

    return ADC_Temp_Value;
}

#endif /* ADC_MODULE */
