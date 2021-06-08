/******************************************************************************
*  Name: gpio.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../Device/samc21n18a.h"
#include "gpio.h"

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
* Local function prototypes
*********************************************************************************************/

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
* Upward trac.: 	   
* Method	  : 	   GPIO_Set_Direction
* Visibility  : 	   public
* Description : 	   set gpio input or output or disable
* Parameters  : 	   const uint8_t pin, const enum gpio_direction direction
* ---------------------------------------------------------------------------------------------------------------------
*	  Type		| Name(Direction) | 									 Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status	 | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type : 	   void
* ===================================================================================================================*/
void GPIO_Set_Direction(const uint8_t pin, const enum gpio_direction direction)
{
    enum gpio_port port = (enum gpio_port)GPIO_PORT(pin);
    const uint32_t mask = (uint32_t) (1U << GPIO_PIN(pin));

    switch(direction)
    {
    case GPIO_DIRECTION_OFF:
        ((Port *)PORT_IOBUS)->Group[port].DIRCLR.reg = mask; 
        ((Port *)PORT)->Group[port].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | (mask & 0xffff); 
        ((Port *)PORT)->Group[port].WRCONFIG.reg = PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | ((mask & 0xffff0000) >> 16); 
        break;
    case GPIO_DIRECTION_IN:
        ((Port *)PORT_IOBUS)->Group[port].DIRCLR.reg = mask;
        ((Port *)PORT)->Group[port].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (mask & 0xffff);
        ((Port *)PORT)->Group[port].WRCONFIG.reg = PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN
                                                   | ((mask & 0xffff0000) >> 16);
        break;
    case GPIO_DIRECTION_OUT:
        ((Port *)PORT_IOBUS)->Group[port].DIRSET.reg = mask;
        ((Port *)PORT)->Group[port].WRCONFIG.reg = PORT_WRCONFIG_WRPINCFG | (mask & 0xffff); 
        ((Port *)PORT)->Group[port].WRCONFIG.reg = PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | ((mask & 0xffff0000) >> 16);
        break;
    default:
        break;
    }
}

/*=====================================================================================================================
* Upward trac.: 	   
* Method	  : 	   GPIO_Set_Function
* Visibility  : 	   public
* Description : 	   set gpio to new mode
* Parameters  : 	   const uint32_t gpio, const uint32_t function
* ---------------------------------------------------------------------------------------------------------------------
*	  Type		| Name(Direction) | 									 Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status	 | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type : 	   void
* ===================================================================================================================*/
void GPIO_Set_Function(const uint32_t gpio, const uint32_t function)
{
    uint8_t port = GPIO_PORT(gpio);
    uint8_t pin  = GPIO_PIN(gpio);
    uint8_t tmp;

    if(function == GPIO_PIN_FUNCTION_OFF)
    {
        tmp = ((Port *)PORT)->Group[port].PINCFG[pin].reg;
        tmp &= ~PORT_PINCFG_PMUXEN;
        ((Port *)PORT)->Group[port].PINCFG[pin].reg = tmp;
    }
    else
    {
        tmp = ((Port *)PORT)->Group[port].PINCFG[pin].reg;
        tmp &= ~PORT_PINCFG_PMUXEN;
        tmp |= 1U << PORT_PINCFG_PMUXEN_Pos;
        ((Port *)PORT)->Group[port].PINCFG[pin].reg = tmp;
        if(pin & 1)
        {
            /*Odd numbered pin*/
            tmp = ((Port *)PORT)->Group[port].PMUX[pin >> 1].reg;
            tmp &= ~PORT_PMUX_PMUXO_Msk;
            tmp |= PORT_PMUX_PMUXO(function & 0xffff);
            ((Port *)PORT)->Group[port].PMUX[pin >> 1].reg = tmp;
        }
        else
        {
            /*Even numbered pin*/
            tmp = ((Port *)PORT)->Group[port].PMUX[pin >> 1].reg;
            tmp &= ~PORT_PMUX_PMUXE_Msk;
            tmp |= PORT_PMUX_PMUXE(function & 0xffff);
            ((Port *)PORT)->Group[port].PMUX[pin >> 1].reg = tmp;
        }
    }
}

/*=====================================================================================================================
* Upward trac.: 	   
* Method	  : 	   GPIO_Set_Pull_Mode
* Visibility  : 	   public
* Description : 	   set gpio to pull up or pull down
* Parameters  : 	   const uint8_t gpio, const enum gpio_pull_mode pull_mode
* ---------------------------------------------------------------------------------------------------------------------
*	  Type		| Name(Direction) | 									 Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status	 | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type : 	   void
* ===================================================================================================================*/
void GPIO_Set_Pull_Mode(const uint8_t gpio, const enum gpio_pull_mode pull_mode)
{
    enum gpio_port port = (enum gpio_port)GPIO_PORT(gpio);
    const uint8_t pin = gpio & 0x1f;

    switch(pull_mode)
    {
    case GPIO_PULL_OFF:
        ((Port *)PORT)->Group[port].PINCFG[pin].reg &= ~PORT_PINCFG_PULLEN;
        break;
    case GPIO_PULL_UP:
        ((Port *)PORT_IOBUS)->Group[port].DIRCLR.reg = 1U << pin; 
        ((Port *)PORT)->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PULLEN;
        ((Port *)PORT_IOBUS)->Group[port].OUTSET.reg = 1U << pin;
        break;
    case GPIO_PULL_DOWN:
        ((Port *)PORT_IOBUS)->Group[port].DIRCLR.reg = 1U << pin; 
        ((Port *)PORT)->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PULLEN;
        ((Port *)PORT_IOBUS)->Group[port].OUTCLR.reg = 1U << pin;
        break;
    default:
        break;
    }
}

/*=====================================================================================================================
* Upward trac.: 	   
* Method	  : 	   GPIO_Set_Level
* Visibility  : 	   public
* Description : 	   set gpio to output high or low
* Parameters  : 	   const uint8_t gpio, const bool level
* ---------------------------------------------------------------------------------------------------------------------
*	  Type		| Name(Direction) | 									 Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status	 | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type : 	   void
* ===================================================================================================================*/
void GPIO_Set_Level(const uint8_t gpio, const bool level)
{
    enum gpio_port port = (enum gpio_port)GPIO_PORT(gpio);
    const uint32_t mask = 0x01U << GPIO_PIN(gpio);

    if(level)
    {
        ((Port *)PORT_IOBUS)->Group[port].OUTSET.reg = mask;
    }
    else
    {
        ((Port *)PORT_IOBUS)->Group[port].OUTCLR.reg = mask;
    }
}

/*=====================================================================================================================
* Upward trac.: 	   
* Method	  : 	   GPIO_Toggle_Level
* Visibility  : 	   public
* Description : 	   Writing '1' to a bit will toggle the corresponding bit in the DIR register, which reverses the direction of the I/O pin.
* Parameters  : 	   const uint8_t gpio
* ---------------------------------------------------------------------------------------------------------------------
*	  Type		| Name(Direction) | 									 Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status	 | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type : 	   void
* ===================================================================================================================*/
void GPIO_Toggle_Level(const uint8_t gpio)
{
    enum gpio_port port = (enum gpio_port)GPIO_PORT(gpio);
    const uint32_t mask = 0x01U << GPIO_PIN(gpio);

    ((Port *)PORT_IOBUS)->Group[port].OUTTGL.reg = mask;
}

/*=====================================================================================================================
* Upward trac.: 	   
* Method	  : 	   GPIO_Get_Level
* Visibility  : 	   public
* Description : 	   get corresponding gpio status
* Parameters  : 	   const uint8_t gpio
* ---------------------------------------------------------------------------------------------------------------------
*	  Type		| Name(Direction) | 									 Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status	 | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type : 	   void
* ===================================================================================================================*/
bool GPIO_Get_Level(const uint8_t gpio)
{
    uint32_t tmp;
    enum gpio_port port = (enum gpio_port)GPIO_PORT(gpio);
    const uint32_t mask = 0x01U << GPIO_PIN(gpio);

    uint32_t dir_tmp = ((Port *)PORT_IOBUS)->Group[port].DIR.reg;

    tmp = ((Port *)PORT)->Group[port].IN.reg & ~dir_tmp;
    tmp |= ((Port *)PORT_IOBUS)->Group[port].OUT.reg & dir_tmp;

    return (bool)(tmp & mask);
}

