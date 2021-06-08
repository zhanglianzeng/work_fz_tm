/******************************************************************************
*  Name: uart.c
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include <stdio.h>
#include "../config.h"
#if(UART_DRIVER == 1)
#include "../../Device/samc21n18a.h"
#include "./config/peripheral_clk_config.h"
#include "./config/hpl_sercom_config.h"
#include "gpio.h"
#include "uart.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define SERCOM_CONFIGURATION(n)                                                                              \
                                                                                                             \
    n,                                                                                                       \
    SERCOM_USART_CTRLA_MODE(CONF_SERCOM_##n##_USART_MODE)                                                    \
        | (CONF_SERCOM_##n##_USART_RUNSTDBY << SERCOM_USART_CTRLA_RUNSTDBY_Pos)                              \
        | (CONF_SERCOM_##n##_USART_IBON << SERCOM_USART_CTRLA_IBON_Pos)                                      \
        | SERCOM_USART_CTRLA_SAMPR(CONF_SERCOM_##n##_USART_SAMPR)                                            \
        | SERCOM_USART_CTRLA_TXPO(CONF_SERCOM_##n##_USART_TXPO)                                              \
        | SERCOM_USART_CTRLA_RXPO(CONF_SERCOM_##n##_USART_RXPO)                                              \
        | SERCOM_USART_CTRLA_SAMPA(CONF_SERCOM_##n##_USART_SAMPA)                                            \
        | SERCOM_USART_CTRLA_FORM(CONF_SERCOM_##n##_USART_FORM)                                              \
        | (CONF_SERCOM_##n##_USART_CMODE << SERCOM_USART_CTRLA_CMODE_Pos)                                    \
        | (CONF_SERCOM_##n##_USART_CPOL << SERCOM_USART_CTRLA_CPOL_Pos)                                      \
        | (CONF_SERCOM_##n##_USART_DORD << SERCOM_USART_CTRLA_DORD_Pos),                                     \
    SERCOM_USART_CTRLB_CHSIZE(CONF_SERCOM_##n##_USART_CHSIZE)                                                \
        | (CONF_SERCOM_##n##_USART_SBMODE << SERCOM_USART_CTRLB_SBMODE_Pos)                                  \
        | (CONF_SERCOM_##n##_USART_CLODEN << SERCOM_USART_CTRLB_COLDEN_Pos)                                  \
        | (CONF_SERCOM_##n##_USART_SFDE << SERCOM_USART_CTRLB_SFDE_Pos)                                      \
        | (CONF_SERCOM_##n##_USART_ENC << SERCOM_USART_CTRLB_ENC_Pos)                                        \
        | (CONF_SERCOM_##n##_USART_PMODE << SERCOM_USART_CTRLB_PMODE_Pos)                                    \
        | (CONF_SERCOM_##n##_USART_TXEN << SERCOM_USART_CTRLB_TXEN_Pos)                                      \
        | (CONF_SERCOM_##n##_USART_RXEN << SERCOM_USART_CTRLB_RXEN_Pos),                                     \
        (uint16_t)(CONF_SERCOM_##n##_USART_BAUD_RATE), CONF_SERCOM_##n##_USART_FRACTIONAL,                   \
        CONF_SERCOM_##n##_USART_RECEIVE_PULSE_LENGTH, CONF_SERCOM_##n##_USART_DEBUG_STOP_MODE,               \

/**********************************************************************************************
* Local types
**********************************************************************************************/
struct usart_config_struct
{
    uint8_t    number;
    uint32_t   ctrl_a;
    uint32_t   ctrl_b;
    uint16_t   baud;
    uint8_t    fractiona;
    uint8_t    rxpl;
    uint8_t    debug_ctrl;
};

/**********************************************************************************************
* Local variables
**********************************************************************************************/
struct usart_config_struct uart_config = {SERCOM_CONFIGURATION(3)};

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void Set_Uart_clk(uint8_t sercom_id);
/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief
* @param    none
* @return   none
*
************************************************************************************************/
void UART_GPIO_Init(void)
{
    GPIO_Set_Function(UART_TX, PINMUX_PB20C_SERCOM3_PAD0);
    GPIO_Set_Function(UART_RX, PINMUX_PB21C_SERCOM3_PAD1);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        UART_Init
* Visibility  :        public
* Description :        set USART INIT
* Parameters  :        void *pSercom, uint8_t sercom_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void UART_Init(void *pSercom,uint8_t sercom_id)
{
    Sercom *module = (Sercom *)pSercom;
	  /*Enable clock on the given bus for the given hardware module*/
    Set_Uart_clk(sercom_id);
	
    uint32_t mode = uart_config.ctrl_a & SERCOM_USART_CTRLA_MODE_Msk;
    while (module->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_ENABLE){};
    module->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST | mode; 	
    while (module->USART.SYNCBUSY.reg & SERCOM_USART_SYNCBUSY_SWRST) {};
			
    module->USART.CTRLA.reg = uart_config.ctrl_a; 
    module->USART.CTRLB.reg = uart_config.ctrl_b;  
    module->USART.BAUD.reg = uart_config.baud; 
    module->USART.RXPL.reg = uart_config.rxpl; 
    module->USART.DBGCTRL.reg = uart_config.debug_ctrl;
			
    module->USART.INTENSET.reg |= SERCOM_USART_INTENCLR_RXC | SERCOM_USART_INTENCLR_RXS;	
			
    UART_GPIO_Init();
    if(sercom_id == SERCOM_UART)
    {
        NVIC_DisableIRQ((IRQn_Type)SERCOM3_IRQn);
        NVIC_ClearPendingIRQ((IRQn_Type)SERCOM3_IRQn);
        NVIC_EnableIRQ((IRQn_Type)SERCOM3_IRQn);
    }
    module->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Set_Uart_clk
* Visibility  :        void
* Description :        Enable clock on the given channel with the given clock source
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void Set_Uart_clk(uint8_t sercom_id)
{
    switch (sercom_id)
    {
        case SERCOM_UART:
        /*UART module*/
        MCLK->APBCMASK.reg |= 1 << (((uint32_t)SERCOM3 & 0x0000ff00) >> 10);
        GCLK->PCHCTRL[SERCOM3_GCLK_ID_CORE].reg = CONF_GCLK_SERCOM3_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
        GCLK->PCHCTRL[SERCOM3_GCLK_ID_SLOW].reg = CONF_GCLK_SERCOM3_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
		    
            break;
        default: 
            break;
    }  
	  
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        UART_Write
* Visibility  :        public
* Description :        USART write interface 
* Parameters  :        Sercom *hardware, uint8_t data
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void UART_Write(Sercom *hardware, uint8_t data)
{ 
    while(hardware->USART.INTFLAG.bit.DRE == 0); 
    hardware->USART.DATA.reg = data;
    while(hardware->USART.INTFLAG.bit.TXC == 0); 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        UART_Read
* Visibility  :        public
* Description :        USART Read interface 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint8_t
* =====================================================================================================================*/
uint8_t UART_Read(void)
{ 
    return SERCOM3->USART.DATA.reg;	 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        SERCOM5_Handler
* Visibility  :        public
* Description :        USART irq function
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void SERCOM6_Handler(void)
{    	  
    SERCOM6->USART.INTENCLR.reg &= (uint8_t)(~SERCOM_USART_INTENCLR_MASK);
}
/***********************************************************************************************
*
* @brief
* @param    none
* @return   none
*
************************************************************************************************/
/* Li Yanfeng: Where is "FILE" come from? (Build error using IAR)
int fputc(int ch, FILE *f)
{
    UART_Write(SERCOM3, (uint8_t)ch);
    return (ch);
}
*/
/*=====================================================================================================================
* Upward trac.:        
* Method      :        UART_if_Iint(void)
* Visibility  :        public
* Description :        the uart module initialization
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void UART_if_Iint(void)
{
    UART_Init(UART_SERCOM,SERCOM_UART);
}
#endif /*UART_DRIVER*/

