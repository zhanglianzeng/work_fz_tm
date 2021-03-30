/******************************************************************************
*  Name: rtc.c
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "../config.h"
#if (RTC_DRIVER == 1)
#include "../../Device/samc21n18a.h"
#include "./config/peripheral_clk_config.h"
#include "./config/hpl_rtc_config.h"
#include "rtc.h"

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
void (*callback)();

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief    RTC module initialization function
* @param    void
* @return   void
*
************************************************************************************************/
void RTC_Init(void)
{
    MCLK->APBAMASK.reg |= 1 << (((uint32_t)RTC & 0x0000ff00) >> 10);

    RTC->MODE0.CTRLA.reg = RTC_MODE0_CTRLA_PRESCALER(CONF_RTC_PRESCALER) | RTC_MODE0_CTRLA_COUNTSYNC | RTC_MODE0_CTRLA_MATCHCLR;
    while (RTC->MODE0.SYNCBUSY.reg & (RTC_MODE0_SYNCBUSY_SWRST | RTC_MODE0_SYNCBUSY_ENABLE | RTC_MODE0_SYNCBUSY_COUNTSYNC))
    {
    };

    RTC->MODE0.COMP[0].reg = CONF_RTC_COMP_VAL;
    RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0; 
    //NVIC_EnableIRQ(RTC_IRQn);
}

/***********************************************************************************************
*
* @brief    RTC module start up counter function
* @param    void
* @return   void
*Not to deal with temporarily
************************************************************************************************/
void RTC_Start(void)
{
    RTC->MODE0.COUNT.reg = 0;
    while (RTC->MODE0.SYNCBUSY.reg & RTC_MODE0_SYNCBUSY_COUNT);
    RTC->MODE0.CTRLA.reg |= RTC_MODE0_CTRLA_ENABLE; 
//    NVIC_EnableIRQ(RTC_IRQn);
}

/***********************************************************************************************
*
* @brief    RTC module stop counter function
* @param    void
* @return   void
*
************************************************************************************************/
void RTC_Stop(void)
{
    RTC->MODE0.COUNT.reg = 0;
    while (RTC->MODE0.SYNCBUSY.reg & RTC_MODE0_SYNCBUSY_COUNT);
    RTC->MODE0.CTRLA.bit.ENABLE = 0;
}

/***********************************************************************************************
*
* @brief    RTC module set interrupt callback function
* @param    callback function pointer
* @return   void
*
************************************************************************************************/
void RTC_Set_Callback(void (*point)())
{
    callback = point;
}

/***********************************************************************************************
*
* @brief    RTC module interrupt serve routine
* @param    void
* @return   void
*
************************************************************************************************/
void RTC_Handler(void)
{
    callback();
    RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0;
}

/***********************************************************************************************
*
* @brief    RTC module interrupt serve routine
* @param    void
* @return   void
*
************************************************************************************************/
void RTC_Set_COMP_VAL_to_640(void)
{
    RTC->MODE0.COMP[0].reg = 640;
}


/***********************************************************************************************
*
* @brief    RTC module interrupt serve routine
* @param    void
* @return   void
*
************************************************************************************************/
void RTC_Set_COMP_VAL_to_original(void)
{
    RTC->MODE0.COMP[0].reg = CONF_RTC_COMP_VAL;
}

#endif /* RTC_DRIVER */
