/******************************************************************************
*  Name: adc.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#if (ADC_DRIVER == 1)
#include "../../Device/samc21n18a.h"
#include "./config/hpl_gclk_config.h"
#include "./config/hpl_mclk_config.h"
#include "./config/hpl_sercom_config.h"
#include "./config/hpl_adc_config.h"
#include "adc.h"
#include "gpio.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
#define ADC_LINEARITY_POS  (0)
#define ADC_LINEARITY_Msk   (0x7 << ADC_LINEARITY_POS)

#define ADC_BIASCAL_POS  (3)
#define ADC_BIASCAL_Msk   (0x7 << ADC_BIASCAL_POS)

#define ADC_INPUTCTRL_MUXNEG_GND_Val        _U_(0x18) 
#define ADC_INPUTCTRL_MUXNEG_GND   (ADC_INPUTCTRL_MUXNEG_GND_Val << ADC_INPUTCTRL_MUXNEG_Pos) /**< (ADC_INPUTCTRL) Internal Ground Position  */

#ifndef CONF_ADC_0_ENABLE
#define CONF_ADC_0_ENABLE 1
#endif
#ifndef CONF_ADC_1_ENABLE
#define CONF_ADC_1_ENABLE 0
#endif
/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define ADC_CONFIGURATION(n)                                                                                           \
	{                                                                                                                  \
        (n),                                                                                                             \
		    (CONF_ADC_##n##_RUNSTDBY << ADC_CTRLA_RUNSTDBY_Pos) | (CONF_ADC_##n##_ONDEMAND << ADC_CTRLA_ONDEMAND_Pos)  \
		        | (CONF_ADC_##n##_SLAVEEN << ADC_CTRLA_SLAVEEN_Pos),                                                   \
		    ADC_CTRLB_PRESCALER(CONF_ADC_##n##_PRESCALER),                                                             \
		    (CONF_ADC_##n##_REFCOMP << ADC_REFCTRL_REFCOMP_Pos) | ADC_REFCTRL_REFSEL(CONF_ADC_##n##_REFSEL),           \
		    (CONF_ADC_##n##_WINMONEO << ADC_EVCTRL_WINMONEO_Pos)                                                       \
		        | (CONF_ADC_##n##_RESRDYEO << ADC_EVCTRL_RESRDYEO_Pos)                                                 \
		        | (CONF_ADC_##n##_STARTINV << ADC_EVCTRL_STARTINV_Pos)                                                 \
		        | (CONF_ADC_##n##_FLUSHINV << ADC_EVCTRL_FLUSHINV_Pos)                                                 \
		        | (CONF_ADC_##n##_STARTEI << ADC_EVCTRL_STARTEI_Pos)                                                   \
		        | (CONF_ADC_##n##_FLUSHEI << ADC_EVCTRL_FLUSHEI_Pos),                                                  \
		    ADC_INPUTCTRL_MUXNEG(CONF_ADC_##n##_MUXNEG) | ADC_INPUTCTRL_MUXPOS(CONF_ADC_##n##_MUXPOS),                 \
		    ADC_CTRLC_DUALSEL(CONF_ADC_##n##_DUALSEL) | ADC_CTRLC_WINMODE(CONF_ADC_##n##_WINMODE)                      \
		        | (CONF_ADC_##n##_R2R << ADC_CTRLC_R2R_Pos) | ADC_CTRLC_RESSEL(CONF_ADC_##n##_RESSEL)                  \
		        | (CONF_ADC_##n##_CORREN << ADC_CTRLC_CORREN_Pos) | (CONF_ADC_##n##_FREERUN << ADC_CTRLC_FREERUN_Pos)  \
		        | (CONF_ADC_##n##_LEFTADJ << ADC_CTRLC_LEFTADJ_Pos)                                                    \
		        | (CONF_ADC_##n##_DIFFMODE << ADC_CTRLC_DIFFMODE_Pos),                                                 \
		    ADC_AVGCTRL_ADJRES(CONF_ADC_##n##_ADJRES) | ADC_AVGCTRL_SAMPLENUM(CONF_ADC_##n##_SAMPLENUM),               \
		    (CONF_ADC_##n##_OFFCOMP << ADC_SAMPCTRL_OFFCOMP_Pos) | ADC_SAMPCTRL_SAMPLEN(CONF_ADC_##n##_SAMPLEN),       \
		    ADC_WINLT_WINLT(CONF_ADC_##n##_WINLT), ADC_WINUT_WINUT(CONF_ADC_##n##_WINUT),                              \
		    ADC_GAINCORR_GAINCORR(CONF_ADC_##n##_GAINCORR), ADC_OFFSETCORR_OFFSETCORR(CONF_ADC_##n##_OFFSETCORR),      \
		    CONF_ADC_##n##_DBGRUN << ADC_DBGCTRL_DBGRUN_Pos, ADC_SEQCTRL_SEQEN(CONF_ADC_##n##_SEQEN),                  \
	}



/**********************************************************************************************
* Local types
**********************************************************************************************/
struct adc_configuration
 {
    uint32_t                 number;
    uint32_t                 ctrl_a;
    uint32_t                 ctrl_b;
    uint32_t                 ref_ctrl;
    uint32_t                 ev_ctrl;
    uint32_t                 input_ctrl;
    uint16_t                 ctrl_c;
    uint32_t                 avg_ctrl;
    uint32_t                 samp_ctrl;
    uint16_t                 win_lt;
    uint16_t                 win_ut;
    uint16_t                 gain_corr;
    uint16_t                 offset_corr;
    uint32_t                 dbg_ctrl;
    uint32_t                 seq_ctrl;
};
 
/**********************************************************************************************
* Local variables
**********************************************************************************************/
static const struct adc_configuration adc_conf[] =
 {
    #if CONF_ADC_0_ENABLE == 1
      ADC_CONFIGURATION(0),
    #endif
    #if CONF_ADC_1_ENABLE == 1
      ADC_CONFIGURATION(1),
    #endif
};
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC0_GPIO_Init
* Visibility  :        public
* Description :        set gpio to ADC0 mode
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void ADC0_GPIO_Init(void)
{
    /*ADC0 CH6*/
    GPIO_Set_Direction(ADC_LED_NTC1, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(ADC_LED_NTC1, PINMUX_PA06B_ADC0_AIN6);
    /*ADC0 CH5*/
    GPIO_Set_Direction(ADC_POWER, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(ADC_POWER, PINMUX_PA05B_ADC0_AIN5);
    /*ADC0 VREFP*/
    GPIO_Set_Direction(ADC_REF, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(ADC_REF, PINMUX_PA03B_ADC0_VREFP);  
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC1_GPIO_Init
* Visibility  :        public
* Description :        set gpio to ADC mode
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void ADC1_GPIO_Init(void)
{
    /*ADC1 CH0*/
    GPIO_Set_Direction(ADC_PCB_NTC, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(ADC_PCB_NTC, PINMUX_PB00B_ADC1_AIN0);
    /*ADC1 CH2*/
    GPIO_Set_Direction(ADC_LED_NTC2, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(ADC_LED_NTC2, PINMUX_PB02B_ADC1_AIN2);
   
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC0_GPIO_DeInit
* Visibility  :        public
* Description :        deinit gpio from ADC0 mode to output mode
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void ADC0_GPIO_DeInit(void)
{
    /*ADC0 CH6*/
    GPIO_Set_Pull_Mode(ADC_LED_NTC1, GPIO_PULL_OFF);         
    GPIO_Set_Function(ADC_LED_NTC1, GPIO_PIN_FUNCTION_OFF);
    /*ADC0 CH5*/
    GPIO_Set_Pull_Mode(ADC_POWER, GPIO_PULL_OFF);           
    GPIO_Set_Function(ADC_POWER, GPIO_PIN_FUNCTION_OFF);  
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC1_GPIO_DeInit
* Visibility  :        public
* Description :        deinit gpio from ADC0 mode to output mode
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void ADC1_GPIO_DeInit(void)
{
    /*ADC1 CH0*/
    GPIO_Set_Pull_Mode(ADC_PCB_NTC, GPIO_PULL_OFF);         
    GPIO_Set_Function(ADC_PCB_NTC, GPIO_PIN_FUNCTION_OFF);

    /*ADC1 CH2*/
    GPIO_Set_Pull_Mode(ADC_LED_NTC2, GPIO_PULL_OFF);         
    GPIO_Set_Function(ADC_LED_NTC2, GPIO_PIN_FUNCTION_OFF);
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC_Init
* Visibility  :        public
* Description :        set ADC to normal work mode
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void ADC_Init(void *hw,const uint8_t adc_id)
{
    Adc *module = (Adc *)hw;
    if( 0 == adc_id)
    {
	/*ADC0 CLK Init*/
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_ADC0;
        GCLK->PCHCTRL[ADC0_GCLK_ID].reg =  CONF_GCLK_ADC0_SRC | GCLK_PCHCTRL_CHEN;
        while ((GCLK->PCHCTRL[ADC0_GCLK_ID].reg & GCLK_PCHCTRL_CHEN) != GCLK_PCHCTRL_CHEN);
			  
        ADC0_GPIO_Init();	  
    }
    else if(1 == adc_id)
    {
        /*ADC1 CLK Iint*/
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_ADC1;
        GCLK->PCHCTRL[ADC1_GCLK_ID].reg = CONF_GCLK_ADC1_SRC | GCLK_PCHCTRL_CHEN;
        while ((GCLK->PCHCTRL[ADC1_GCLK_ID].reg & GCLK_PCHCTRL_CHEN) != GCLK_PCHCTRL_CHEN);
        ADC1_GPIO_Init();
    }  
    if (!module->SYNCBUSY.reg & ADC_SYNCBUSY_SWRST )
    {
        if (module->CTRLA.reg & ADC_CTRLA_ENABLE) 
        {
            module->CTRLA.reg = 0;
            while (module->SYNCBUSY.reg & ADC_CTRLA_ENABLE); 
        }
        module->CTRLA.reg = ADC_CTRLA_SWRST; 
    }
    while (module->SYNCBUSY.reg & ADC_SYNCBUSY_SWRST); 
    /*Init CTRLB*/
    module->CTRLB.reg = adc_conf[adc_id].ctrl_b;
    /*Init REFCTRL*/
    module->REFCTRL.reg = adc_conf[adc_id].ref_ctrl;
    /*Init EVCTRL*/
    module->EVCTRL.reg = adc_conf[adc_id].ev_ctrl;
    /*Init INPUTCTRL*/
    module->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;
    /*Init CTRLC*/
    module->CTRLC.reg = adc_conf[adc_id].ctrl_c;
    /*Init AVGCTRL*/
    module->AVGCTRL.reg = adc_conf[adc_id].avg_ctrl;
    /*Init SAMPCTRL*/
    module->SAMPCTRL.reg = adc_conf[adc_id].samp_ctrl;
    /*Configure WINLT*/
    module->WINLT.reg = adc_conf[adc_id].win_lt;
    /*Configure WINUT*/
    module->WINUT.reg = adc_conf[adc_id].win_ut;
    /*Init GAINCORR*/
    module->GAINCORR.reg = adc_conf[adc_id].gain_corr;
    /*Init offsetcorr*/
    module->OFFSETCORR.reg = adc_conf[adc_id].offset_corr;
    /*Init DBRCTRL*/
    module->DBGCTRL.reg = adc_conf[adc_id].dbg_ctrl;
    /*Init SERCTRL*/
    module->SEQCTRL.reg = adc_conf[adc_id].seq_ctrl;
    /*Init CTRLA*/
    module->CTRLA.reg = adc_conf[adc_id].ctrl_a;

}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC_Deinit
* Visibility  :        public
* Description :        De-initialize ADC module and reset
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void ADC_Deinit(void *hw)
{
    Adc *module = (Adc *)hw;
    module->CTRLA.reg &= ~ADC_CTRLA_ENABLE; 
    module->CTRLA.reg |= ADC_CTRLA_SWRST;
	
    if(ADC0 == module)
    {
        ADC0_GPIO_DeInit(); 
    }
    else if(ADC1 == module)
    {
        ADC1_GPIO_DeInit();
    }			    
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC_Enable
* Visibility  :        public
* Description :        Enable ADC module
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void ADC_Enable(void *hw)
{
    Adc *module = (Adc *)hw;
    module->CTRLA.reg |= ADC_CTRLA_ENABLE;
    while (module->SYNCBUSY.reg & (ADC_SYNCBUSY_SWRST | ADC_SYNCBUSY_ENABLE)); 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC_Disable
* Visibility  :        public
* Description :        disable ADC module
* Parameters  :        void *const
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void ADC_Disable(void *hw)
{
    Adc *module = (Adc *)hw;
    module->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    while (module->SYNCBUSY.reg & (ADC_SYNCBUSY_SWRST | ADC_SYNCBUSY_ENABLE)); 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        ADC_Read
* Visibility  :        public
* Description :        read ADC value
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
uint16_t ADC_Read(void *hw, uint8_t channel)
{
    Adc *module = (Adc *)hw;
    uint16_t result;
    module->INPUTCTRL.bit.MUXPOS=ADC_INPUTCTRL_MUXPOS(channel);
    while(module->SYNCBUSY.reg&ADC_SYNCBUSY_INPUTCTRL);
   
    module->SWTRIG.reg|=ADC_SWTRIG_START;
    while(module->SYNCBUSY.reg&ADC_SYNCBUSY_SWTRIG);
    
    while(!(module->INTFLAG.reg&ADC_INTFLAG_RESRDY))
    {
    /*Wait for the conversion result is available*/
    }
    result = module->RESULT.reg;
        
    return result;  
}


#endif /* ADC_DRIVER */
