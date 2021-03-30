/******************************************************************************
*  Name: eic.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include <stdlib.h>
#include "../config.h"
#if (EIC_DRIVER == 1)
#include "../../Device/samc21n18a.h"
#include "./config/peripheral_clk_config.h"
#include "./config/hpl_eic_config.h"
#include "gpio.h"
#include "eic.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define EIC_CHANNEL_NUMBER      16

#define EIC_RCV                 GPIO(GPIO_PORTC, 17)
#define NMI_WAKE                GPIO(GPIO_PORTA, 8)


/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef void (*callback_type)();

/**********************************************************************************************
* Local variables
**********************************************************************************************/
callback_type eic_callback[EIC_CHANNEL_NUMBER] = {0};
#if (LOCALWAKEUP_STATUS == 1)
callback_type nmi_callback = NULL;
#endif

/**********************************************************************************************
* Local functions
**********************************************************************************************/


/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_GPIO_Init(void)
* Visibility  :        public
* Description :        EIC Init
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_GPIO_Init(void)
{
    GPIO_Set_Direction(EIC_RCV, GPIO_DIRECTION_IN);
    GPIO_Set_Pull_Mode(EIC_RCV, GPIO_PULL_DOWN);
    GPIO_Set_Function(EIC_RCV, PINMUX_PC17A_EIC_EXTINT9);
	/*
    GPIO_Set_Direction(NMI_WAKE, GPIO_DIRECTION_IN);
    GPIO_Set_Pull_Mode(NMI_WAKE, GPIO_PULL_OFF);          
    GPIO_Set_Function(NMI_WAKE, PINMUX_PA08A_EIC_NMI);
    */
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Init(void)
* Visibility  :        public
* Description :        Configuring external interrupt
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Init(void)
{
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = CONF_GCLK_EIC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
	  
    EIC_GPIO_Init();

    while (EIC->SYNCBUSY.reg); 

    if (EIC->CTRLA.reg & EIC_CTRLA_ENABLE)
    {
        EIC->CTRLA.reg = 0; 
        while (EIC->SYNCBUSY.reg); 
    }
    EIC->CTRLA.reg = EIC_CTRLA_SWRST;

    while (EIC->SYNCBUSY.reg);
    EIC->CTRLA.reg |= CONF_EIC_CKSEL<<EIC_CTRLA_CKSEL_Pos;

    //#if (LOCALWAKEUP_STATUS == 1)
    //EIC->NMICTRL.reg = (CONF_EIC_NMIFILTEN << EIC_NMICTRL_NMIFILTEN_Pos) | EIC_NMICTRL_NMISENSE(CONF_EIC_NMISENSE); /*enable nmi interrupt*/
    //#endif
	
    EIC->EVCTRL.reg = (CONF_EIC_EXTINTEO0 << 0) | (CONF_EIC_EXTINTEO1 << 1) | (CONF_EIC_EXTINTEO2 << 2)
                    | (CONF_EIC_EXTINTEO3 << 3) | (CONF_EIC_EXTINTEO4 << 4) | (CONF_EIC_EXTINTEO5 << 5)
                    | (CONF_EIC_EXTINTEO6 << 6) | (CONF_EIC_EXTINTEO7 << 7) | (CONF_EIC_EXTINTEO8 << 8)
                    | (CONF_EIC_EXTINTEO9 << 9) | (CONF_EIC_EXTINTEO10 << 10) | (CONF_EIC_EXTINTEO11 << 11)
                    | (CONF_EIC_EXTINTEO12 << 12) | (CONF_EIC_EXTINTEO13 << 13)
                    | (CONF_EIC_EXTINTEO14 << 14) | (CONF_EIC_EXTINTEO15 << 15) | 0;

    EIC->ASYNCH.reg = (CONF_EIC_ASYNCH0 << 0) | (CONF_EIC_ASYNCH1 << 1) | (CONF_EIC_ASYNCH2 << 2)
                    | (CONF_EIC_ASYNCH3 << 3) | (CONF_EIC_ASYNCH4 << 4) | (CONF_EIC_ASYNCH5 << 5)
                    | (CONF_EIC_ASYNCH6 << 6) | (CONF_EIC_ASYNCH7 << 7) | (CONF_EIC_ASYNCH8 << 8)
                    | (CONF_EIC_ASYNCH9 << 9) | (CONF_EIC_ASYNCH10 << 10) | (CONF_EIC_ASYNCH11 << 11)
                    | (CONF_EIC_ASYNCH12 << 12) | (CONF_EIC_ASYNCH13 << 13) | (CONF_EIC_ASYNCH14 << 14)
                    | (CONF_EIC_ASYNCH15 << 15) | 0;
										
    EIC->DEBOUNCEN.reg = (CONF_EIC_DEBOUNCE_ENABLE0 << 0) | (CONF_EIC_DEBOUNCE_ENABLE1 << 1) | (CONF_EIC_DEBOUNCE_ENABLE2 << 2)
                       | (CONF_EIC_DEBOUNCE_ENABLE3 << 3) | (CONF_EIC_DEBOUNCE_ENABLE4 << 4) | (CONF_EIC_DEBOUNCE_ENABLE5 << 5)
                       | (CONF_EIC_DEBOUNCE_ENABLE6 << 6) | (CONF_EIC_DEBOUNCE_ENABLE7 << 7) | (CONF_EIC_DEBOUNCE_ENABLE8 << 8)
                       | (CONF_EIC_DEBOUNCE_ENABLE9 << 9) | (CONF_EIC_DEBOUNCE_ENABLE10 << 10) | (CONF_EIC_DEBOUNCE_ENABLE11 << 11)
                       | (CONF_EIC_DEBOUNCE_ENABLE12 << 12) | (CONF_EIC_DEBOUNCE_ENABLE13 << 13)
                       | (CONF_EIC_DEBOUNCE_ENABLE14 << 14) | (CONF_EIC_DEBOUNCE_ENABLE15 << 15) | 0;

    EIC->DPRESCALER.reg = (EIC_DPRESCALER_PRESCALER0(CONF_EIC_DPRESCALER0)) | (CONF_EIC_STATES0 << EIC_DPRESCALER_STATES0_Pos)
                        | (EIC_DPRESCALER_PRESCALER1(CONF_EIC_DPRESCALER1)) | (CONF_EIC_STATES1 << EIC_DPRESCALER_STATES1_Pos)
                        | CONF_EIC_TICKON << EIC_DPRESCALER_TICKON_Pos | 0;
	 
	 
    EIC->CONFIG[0].reg = (CONF_EIC_FILTEN0 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE0)
                       | (CONF_EIC_FILTEN1 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE1)
                       | (CONF_EIC_FILTEN2 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE2)
                       | (CONF_EIC_FILTEN3 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(CONF_EIC_SENSE3)
                       | (CONF_EIC_FILTEN4 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE4)
                       | (CONF_EIC_FILTEN5 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE5)
                       | (CONF_EIC_FILTEN6 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE6)
                       | (CONF_EIC_FILTEN7 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE7)
                       | 0;

    EIC->CONFIG[1].reg = (CONF_EIC_FILTEN8 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE8)
                       | (CONF_EIC_FILTEN9 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE9)
                       | (CONF_EIC_FILTEN10 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE10)
                       | (CONF_EIC_FILTEN11 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(CONF_EIC_SENSE11)
                       | (CONF_EIC_FILTEN12 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE12)
                       | (CONF_EIC_FILTEN13 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE13)
                       | (CONF_EIC_FILTEN14 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE14)
                       | (CONF_EIC_FILTEN15 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE15)
                       | 0;

    while (EIC->SYNCBUSY.reg);
    EIC->CTRLA.reg |= EIC_CTRLA_ENABLE;
   
    NVIC_DisableIRQ(EIC_IRQn);
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
		
    #if (LOCALWAKEUP_STATUS == 1)
    //EIC_Enable_NMI();
    #endif
}

#if (LOCALWAKEUP_STATUS == 1)
/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        NMI_Set_Callback(void (*point)())
* Visibility  :        public
* Description :        NMI call back
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void NMI_Set_Callback(void (*point)())
{
    nmi_callback = point;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        NMI_Clear_Callback(void (*point)())
* Visibility  :        public
* Description :        NMI call back
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void NMI_Clear_Callback(void)
{
    nmi_callback = NULL;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Enable_NMI(void)
* Visibility  :        public
* Description :        Enable nmi interrupt
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Enable_NMI(void)
{
    EIC->NMICTRL.reg = (CONF_EIC_NMIFILTEN << EIC_NMICTRL_NMIFILTEN_Pos) | EIC_NMICTRL_NMISENSE(EIC_NMICTRL_NMISENSE_BOTH_Val);
    while (EIC->SYNCBUSY.reg);
    EIC->NMIFLAG.reg = EIC_NMIFLAG_NMI;
}
#endif

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Irq_Enable(void)
* Visibility  :        public
* Description :        EIC interrupt enable
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Irq_Enable(const uint8_t reg)
{
    EIC->INTENCLR.reg = 1 << reg;
    EIC->INTFLAG.reg = 1 << reg;
    EIC->INTENSET.reg = 1 << reg;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Irq_Disable(void)
* Visibility  :        public
* Description :        EIC interrupt disable
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Irq_Disable(const uint8_t reg)
{
    EIC->INTENCLR.reg = 1 << reg;
    EIC->INTFLAG.reg = 1 << reg;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Irq_Register(void)
* Visibility  :        public
* Description :        EIC interrupt register
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Irq_Register(void (*point)(), uint8_t channel)
{
    eic_callback[channel] = point;
    EIC_Irq_Enable(channel);
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Irq_Deinit(void)
* Visibility  :        public
* Description :        EIC interrupt Deinit
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Irq_Deinit(void)
{
    NVIC_DisableIRQ(EIC_IRQn);
    EIC->CTRLA.reg &= ~EIC_CTRLA_ENABLE;
    EIC->CTRLA.reg |= EIC_CTRLA_SWRST;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Handler(void)
* Visibility  :        public
* Description :        EIC interrupt handle function
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void EIC_Handler(void)
{
    volatile uint32_t flags = EIC->INTFLAG.reg & EIC->INTENSET.reg; 

    for(uint8_t i = 0; i < EIC_CHANNEL_NUMBER; i++)
    {
        if((flags >> i) & 0x1)
        {
            if(NULL != eic_callback[i])
            {
                eic_callback[i]();
            }
        }
    }

    EIC->INTFLAG.reg = flags;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        EIC_Get_WakeRCV_GPIO_Status(void)
* Visibility  :        public
* Description :        EIC Get WakeRCV GPIO Status
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool EIC_Get_WakeRCV_GPIO_Status(void)
{
    return GPIO_Get_Level(EIC_RCV);
}


#if (LOCALWAKEUP_STATUS == 1)
/*=====================================================================================================================
* Upward trac.:        DRIVER/EIC
* Method      :        NMI_Handler(void)
* Visibility  :        public
* Description :        NMI interrupt handle function
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void NMI_Handler(void)
{
    EIC->NMIFLAG.reg = EIC_NMIFLAG_NMI;

    if(NULL != nmi_callback)
    {
        nmi_callback();
    }
    else
    {
        //EIC_Irq_Deinit();  //switchpack is not add
    }
}
#endif 

#endif /*EIC_DRIVER*/
