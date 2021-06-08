/******************************************************************************
*  Name: tcc.c
*  Description:
*  Project: Wave2 
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#include "gpio.h"
#if (TCC_DRIVER == 1)
#include "tcc.h"
#include "../../Device/samc21n18a.h"
#include "./config/hpl_tcc_config.h"

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
/* brief TCC configuration type*/
struct tcc_cfg
{
    void *         hw; /*!< instance of TCC */
    IRQn_Type      irq;
    uint32_t       ctrl_a;
    uint8_t        ctrl_b;
    uint32_t       per;
    uint32_t       cc0;
    uint32_t       cc1;
    uint32_t       cc2;
    uint32_t       cc3;
    uint32_t       wave; /*Waveform Generation Operation*/
};
/**********************************************************************************************
* Local variables
**********************************************************************************************/
static struct tcc_cfg init_cfgs[3] =
{
    {
      (void *)TCC0,
      TCC0_IRQn,
      CONF_TCC0_CTRLA,
      CONF_TCC0_CTRLB,
	
      CONF_TCC0_PER,  /*default pwm period*/
      CONF_TCC0_CC0,  /*default cc value of channel 0, duty cycle = cc value/ per value */
      CONF_TCC0_CC1,  /*default cc value of channel 1*/
      CONF_TCC0_CC2,  /*default cc value of channel 2*/
      CONF_TCC0_CC3,  /*default cc value of channel 3*/	
      (CONF_TCC0_WAVEGEN << TCC_WAVE_WAVEGEN_Pos) /*TCC mode*/
    },
    {
      (void *)TCC1,
      TCC1_IRQn,
      CONF_TCC1_CTRLA,
      CONF_TCC1_CTRLB,
		
      CONF_TCC1_PER,  /*default pwm period*/ 
      CONF_TCC1_CC0,  /*default cc value of channel 0, duty cycle = cc value/ per value */
      CONF_TCC1_CC1,  /*default cc value of channel 1*/
      0,
      0,	
      (CONF_TCC0_WAVEGEN << TCC_WAVE_WAVEGEN_Pos) /*TCC mode*/
    },
	{
      (void *)TCC2,
      TCC2_IRQn,
      CONF_TCC2_CTRLA,
      CONF_TCC2_CTRLB,
		
      CONF_TCC2_PER,  /*default pwm period*/ 
      CONF_TCC2_CC0,  /*default cc value of channel 0, duty cycle = cc value/ per value */
      CONF_TCC2_CC1,  /*default cc value of channel 1*/
      0,
      0, 
      (CONF_TCC0_WAVEGEN << TCC_WAVE_WAVEGEN_Pos) /*TCC mode*/
    }
};

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        
* Method      :        TCC_Enable_Bus_Clk
* Visibility  :        public
* Description :        Enable clock on the given bus for the given hardware module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void TCC_Enable_Bus_Clk(void *tcc_id)
{
    if (tcc_id == TCC0) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC0;
        GCLK->PCHCTRL[TCC0_GCLK_ID].reg = CONF_GCLK_TCC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tcc_id == TCC1) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC1;
        GCLK->PCHCTRL[TCC1_GCLK_ID].reg = CONF_GCLK_TCC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tcc_id == TCC2) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC2;
        GCLK->PCHCTRL[TCC2_GCLK_ID].reg = CONF_GCLK_TCC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    }		
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        TCC_Init
* Visibility  :        public
* Description :        set Tcc pwm work mode, freq and duty
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void TCC_Init(void *tcc_id, uint8_t tcc_index)
{
    Tcc * module = (Tcc *)tcc_id;
	
    module->CTRLA.reg = ~TCC_CTRLA_ENABLE;
    while(module->SYNCBUSY.reg & (TCC_SYNCBUSY_SWRST | TCC_SYNCBUSY_ENABLE)){};
    module->CTRLA.reg = TCC_CTRLA_SWRST;
			
    while (module->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST) {};
			
    module->CTRLA.reg = init_cfgs[tcc_index].ctrl_a; 
    module->CTRLBSET.reg = init_cfgs[tcc_index].ctrl_a;
   
    /*config tcc mode, waveform generation operation*/
    module->WAVE.reg = init_cfgs[tcc_index].wave;
    
    /*the period time (T) is controlled by Top value
    the TOP value is defined by the Period (PER) register value*/
    module->PER.reg = init_cfgs[tcc_index].per;

    /*set default duty, duty cycle = cc value/ per value*/
    module->CC[0].reg = init_cfgs[tcc_index].cc0; 
    module->CC[1].reg = init_cfgs[tcc_index].cc1; 
	
    if(TCC0 == tcc_id)
    {
        module->CC[2].reg = init_cfgs[tcc_index].cc2; 
        module->CC[3].reg = init_cfgs[tcc_index].cc3;
    }
	
	/*Writing "1" to this bit will enable updating, The double buffering is enable
    CCBx is copied into CCx at TCC update time*/
    module->CTRLBCLR.reg = TCC_CTRLBSET_LUPD;

    /* Enable the TCC module */
    while (module->SYNCBUSY.reg & (TCC_SYNCBUSY_SWRST | TCC_SYNCBUSY_ENABLE));
    module->CTRLA.reg |= TCC_CTRLA_ENABLE;

}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        TCC_Set_Duty
* Visibility  :        public
* Description :        set pwm duty
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void TCC_Set_Duty(void *tcc_id, uint8_t channel, uint32_t duty)
{
    Tcc * module = (Tcc *)tcc_id;

    /*set new pwm duty in channel n*/
    module->CCBUF[channel].reg = duty;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        TCC_Set_Period
* Visibility  :        public
* Description :        set pwm period
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void TCC_Set_Period(void *tcc_id, uint32_t period)
{
    Tcc * module = (Tcc *)tcc_id;
    /*set new pwm period */
    module->PERBUF.reg = period; 
}

#endif /* TCC_DRIVER */
