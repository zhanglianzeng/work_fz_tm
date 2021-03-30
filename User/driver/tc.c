/******************************************************************************
*  Name: tc.c
*  Description:
*  Project:Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#include "gpio.h"
#if (TC_DRIVER == 1)
#include "./config/hpl_tc_config.h"
#include "tc.h"
#include "qtouch/driven_shield.h"
#include "qtouch/touch.h"


/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define APBC_MASK(value)     1 << (((uint32_t)value & _U(0x0000ff00)) >> 10)

/**
 * \brief Macro is used to fill usart configuration structure based on its
 * number
 *
 * \param[in] n The number of structures
 */
#define TC_CONFIGURATION(n)                                                                                            \
	{                                                                                                                  \
		n, TC##n##_IRQn,                                                                                               \
		    TC_CTRLA_MODE(CONF_TC##n##_MODE) | TC_CTRLA_PRESCSYNC(CONF_TC##n##_PRESCSYNC)                              \
		        | (CONF_TC##n##_RUNSTDBY << TC_CTRLA_RUNSTDBY_Pos) | (CONF_TC##n##_ONDEMAND << TC_CTRLA_ONDEMAND_Pos)  \
		        | TC_CTRLA_PRESCALER(CONF_TC##n##_PRESCALER) | (CONF_TC##n##_ALOCK << TC_CTRLA_ALOCK_Pos),             \
		    (CONF_TC##n##_OVFEO << TC_EVCTRL_OVFEO_Pos) | (CONF_TC##n##_TCEI << TC_EVCTRL_TCEI_Pos)                    \
		        | (CONF_TC##n##_TCINV << TC_EVCTRL_TCINV_Pos) | (CONF_TC##n##_EVACT << TC_EVCTRL_EVACT_Pos)            \
		        | (CONF_TC##n##_MCEO0 << TC_EVCTRL_MCEO0_Pos) | (CONF_TC##n##_MCEO1 << TC_EVCTRL_MCEO1_Pos),           \
		    (CONF_TC##n##_DBGRUN << TC_DBGCTRL_DBGRUN_Pos), CONF_TC##n##_PER, CONF_TC##n##_CC0, CONF_TC##n##_CC1,      \
	}
	
	/**
 * \brief TC configuration type
 */
typedef struct {
    uint8_t        number;
    IRQn_Type       irq;
    uint32_t       ctrl_a;
    uint16_t       event_ctrl;
    uint8_t        dbg_ctrl;
    uint8_t        per;
    uint32_t       cc0;
    uint32_t       cc1;
}tc_configuration;

uint8_t num = 0;
/**
 * \brief Array of TC configurations
 */
static tc_configuration _tcs[] = {
#if CONF_TC0_ENABLE == 1
    TC_CONFIGURATION(0),
#endif
#if CONF_TC1_ENABLE == 1
    TC_CONFIGURATION(1),
#endif
#if CONF_TC2_ENABLE == 1
    TC_CONFIGURATION(2),
#endif
#if CONF_TC3_ENABLE == 1
    TC_CONFIGURATION(3),
#endif
#if CONF_TC4_ENABLE == 1
    TC_CONFIGURATION(4),
#endif
#if CONF_TC5_ENABLE == 1
    TC_CONFIGURATION(5),
#endif
#if CONF_TC6_ENABLE == 1
    TC_CONFIGURATION(6),
#endif
#if CONF_TC7_ENABLE == 1
    TC_CONFIGURATION(7),
#endif
};

static P_TIMER_CALLBACK timer_callback = NULL;
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
* Upward trac.:        user/driver/tc
* Method      :        TC_Get_Num(const void *const tc_id)
* Visibility  :        public
* Description :        get tc number
* Parameters  :        const void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
static int8_t TC_Get_Num(const void *const tc_id)
{	
  /* List of available TC modules. */
	Tc *const tc_modules[TC_INST_NUM] = TC_INSTS;

	/* Find index for TC instance. */
	for (uint32_t i = 0; i < TC_INST_NUM; i++) 
	{
		 if ((uint32_t)tc_id == (uint32_t)tc_modules[i]) 
		 {
			  return i;
		 }
	}
	return (((uint32_t)tc_id - (uint32_t)TC0) >> 10);
}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_Clk_Init(const void *const hw)
* Visibility  :        public
* Description :        configure TC clock
* Parameters  :        const void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_Clk_Init(void *tc_id)
{
    if (tc_id == TC0) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC0;
        GCLK->PCHCTRL[TC0_GCLK_ID].reg = CONF_GCLK_TC0_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tc_id == TC1) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC1;
        GCLK->PCHCTRL[TC1_GCLK_ID].reg = CONF_GCLK_TC1_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tc_id == TC2) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC2;
        GCLK->PCHCTRL[TC2_GCLK_ID].reg = CONF_GCLK_TC2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tc_id == TC3) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC3;
        GCLK->PCHCTRL[TC3_GCLK_ID].reg = CONF_GCLK_TC3_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
	} else if (tc_id == TC4) {
        MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC4;
        GCLK->PCHCTRL[TC4_GCLK_ID].reg = CONF_GCLK_TC4_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
	} else if (tc_id == TC5) {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC5;
        GCLK->PCHCTRL[TC5_GCLK_ID].reg = CONF_GCLK_TC5_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tc_id == TC6) {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC6;
        GCLK->PCHCTRL[TC6_GCLK_ID].reg = CONF_GCLK_TC6_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } else if (tc_id == TC7) {
        MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC7;
        GCLK->PCHCTRL[TC7_GCLK_ID].reg = CONF_GCLK_TC7_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    } 
}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_Pwm_Init(void)
* Visibility  :        public
* Description :        set Tc pwm work mode, freq and duty
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_Pwm_Init(void *tc_id)
{
    Tc * module = (Tc *)tc_id;
    int8_t index = TC_Get_Num(tc_id);  
    module->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (module->COUNT8.SYNCBUSY.reg & (TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE));
    module->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	
    while (module->COUNT8.SYNCBUSY.reg & TC_SYNCBUSY_SWRST) {};
			
    module->COUNT16.CTRLA.reg = _tcs[index].ctrl_a; 
    module->COUNT16.DBGCTRL.reg = _tcs[index].dbg_ctrl;
    module->COUNT16.EVCTRL.reg = _tcs[index].event_ctrl;
    /*config tc mode, waveform generation operation*/
    module->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MPWM_Val;
		
    /*set default duty, duty cycle = cc value/ per value*/
    module->COUNT16.CC[0].reg = _tcs[index].cc0; //top
    module->COUNT16.CC[1].reg = _tcs[index].cc1; //duty
		
    /* Enable the TC module */	
    module->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; 
    while(module->COUNT16.SYNCBUSY.reg & (TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE));
}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_Timer_Init(const void *const hw)
* Visibility  :        public
* Description :        configure TC
* Parameters  :        const void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_Timer_Init(const void *const tc_id)
{
    Tc * module = (Tc *)tc_id;
    int8_t index = TC_Get_Num(tc_id);

    module->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (module->COUNT8.SYNCBUSY.reg & (TC_SYNCBUSY_SWRST | TC_SYNCBUSY_ENABLE)){};
    module->COUNT16.EVCTRL.reg = _tcs[index].event_ctrl;
    module->COUNT16.CC[0].reg = _tcs[index].cc0;
    module->COUNT16.INTENSET.reg = TC_INTENSET_OVF;
    /*config tc mode, waveform generation operation*/
    module->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
		
    NVIC_DisableIRQ((IRQn_Type) _tcs[index].irq);
    NVIC_ClearPendingIRQ((IRQn_Type) _tcs[index].irq);
    NVIC_EnableIRQ((IRQn_Type) _tcs[index].irq);  
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        TC_Set_Duty
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
void TC_Set_Duty(void *tc_id, uint32_t duty)
{
    Tc * module = (Tc *)tc_id;
	  /*set new pwm duty */
    module->COUNT16.CC[1].reg = duty;
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        TC_Set_Period
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
void TC_Set_Period(void *tc_id, uint32_t period)
{
    Tc * module = (Tc *)tc_id;
	
    module->COUNT16.CC[0].reg = period;
}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_Timer_Enable(const void *const hw)
* Visibility  :        public
* Description :        enable TC
* Parameters  :        const void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_Timer_Enable(const void *const hw)
{
    ((Tc *)hw)->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_Init(void)
* Visibility  :        public
* Description :        TC time function init
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_Init(void)
{
    /* TC3 for QTouch Shield*/
    TC_Clk_Init(TC3);
    GPIO_Set_Function(SHILD_PWM, PINMUX_PA19E_TC3_WO1);
#if DEF_ENABLE_DRIVEN_SHIELD == 1
    SHIELD_TC_PWM_init();
#endif

}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_If_Timer_Init(void *tc_id)
* Visibility  :        public
* Description :        TC time function init
* Parameters  :        void *tc_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_If_Timer_Init(void *tc_id)
{
    TC_Clk_Init(tc_id);
    TC_Timer_Init(tc_id);
    TC_Timer_Enable(tc_id);
}	
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC_DeInit(const void *const hw)
* Visibility  :        public
* Description :        TC function deinit
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC_DeInit(const void *const hw)
{
    Tc *HW = (Tc *)hw;
    HW->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
}
/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC4_Handler(void (*point)())
* Visibility  :        public
* Description :        TC4 call back
* Parameters  :        void (*point)()
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC4_Handler(void)
{
    bool OVF_bit;

    if (NULL != timer_callback)
    {
        for (uint8_t i =0; i<TIMER_NUM; i++)
        {
            if (NULL != timer_callback[i].P_Handling)
            {
                timer_callback[i].P_Handling();
            }
        }
    }

    OVF_bit = (TC4->COUNT8.INTFLAG.reg & TC_INTFLAG_OVF) >> TC_INTFLAG_OVF_Pos;
    if (OVF_bit)
    {
        TC4->COUNT8.INTFLAG.reg = TC_INTFLAG_OVF;
    }
}

/*=====================================================================================================================
* Upward trac.:        user/driver/tc
* Method      :        TC4_Set_Callback(Timer_handling callback)
* Visibility  :        public
* Description :        TC5 call back
* Parameters  :        TC_Callback callback
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void
=====================================================================================================================*/
void TC4_If_Set_Callback(P_TIMER_CALLBACK callback)
{
    if (NULL != callback)
    {
        timer_callback = callback;
    }
}

#endif /* TC_DRIVER */

