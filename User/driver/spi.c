/******************************************************************************
*  Name: spi.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#include "gpio.h"
#if (SPI_DRIVER == 1)
#include "../driver/spi.h"
#include "../../Device/samc21n18a.h"
#include "./config/hpl_sercom_config.h"
#include "../driver/delay.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define SERCOMSPI_REGS(n)                                                                                    \
    (((CONF_SERCOM_##n##_SPI_DORD) << SERCOM_SPI_CTRLA_DORD_Pos)                                             \
    | (CONF_SERCOM_##n##_SPI_CPOL << SERCOM_SPI_CTRLA_CPOL_Pos)                                              \
    | (CONF_SERCOM_##n##_SPI_CPHA << SERCOM_SPI_CTRLA_CPHA_Pos)                                              \
    | (CONF_SERCOM_##n##_SPI_AMODE_EN ? SERCOM_SPI_CTRLA_FORM(2) : SERCOM_SPI_CTRLA_FORM(0))                 \
    | (CONF_SERCOM_##n##_SPI_TXPO) << SERCOM_SPI_CTRLA_DOPO_Pos                                              \
    | (CONF_SERCOM_##n##_SPI_RXPO) << SERCOM_SPI_CTRLA_DIPO_Pos                                              \
    | (CONF_SERCOM_##n##_SPI_IBON << SERCOM_SPI_CTRLA_IBON_Pos)                                              \
    | (CONF_SERCOM_##n##_SPI_RUNSTDBY << SERCOM_SPI_CTRLA_RUNSTDBY_Pos)                                      \
    | SERCOM_SPI_CTRLA_MODE(CONF_SERCOM_##n##_SPI_MODE)), /* ctrla */                                        \
    ((CONF_SERCOM_##n##_SPI_RXEN << SERCOM_SPI_CTRLB_RXEN_Pos)                                               \
    | (CONF_SERCOM_##n##_SPI_MSSEN << SERCOM_SPI_CTRLB_MSSEN_Pos)                                            \
    | (CONF_SERCOM_##n##_SPI_SSDE << SERCOM_SPI_CTRLB_SSDE_Pos)                                              \
    | (CONF_SERCOM_##n##_SPI_PLOADEN << SERCOM_SPI_CTRLB_PLOADEN_Pos)                                        \
    | SERCOM_SPI_CTRLB_AMODE(CONF_SERCOM_##n##_SPI_AMODE)                                                    \
    | SERCOM_SPI_CTRLB_CHSIZE(CONF_SERCOM_##n##_SPI_CHSIZE)), /*ctrlb*/                                      \
    ((uint8_t)CONF_SERCOM_##n##_SPI_BAUD_RATE)                                                                
/**********************************************************************************************
* Local types
**********************************************************************************************/
struct spi_config_struct
{
    uint32_t ctrla;
    uint32_t ctrlb;
    uint8_t  baud;
};
/**********************************************************************************************
* Local variables
**********************************************************************************************/
struct spi_config_struct spi_config[2] = {SERCOMSPI_REGS(0),SERCOMSPI_REGS(7)};

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_Master_Set_Enable_Bit
* Visibility  :        public
* Description :        enable spi master mode 
* Parameters  :        void *pSercom
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_Master_Set_Enable_Bit(void *pSercom)
{
    Sercom *module = (Sercom *)pSercom;       
    /*enable spi master mode*/
    while (module->SPI.SYNCBUSY.reg & (SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE));
    module->SPI.CTRLA.bit.ENABLE= 1;  
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_Gclk_Enable_Channel
* Visibility  :        public
* Description :        Enable clock on the given channel with the given clock source 
* Parameters  :        uint8_t sercom_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_Gclk_Enable_Channel(uint8_t sercom_id)
{
    switch (sercom_id)
    {
        case SERCOM_LCD:
            /*Enable clock on the given bus for the given hardware module*/
            MCLK->APBCMASK.reg |= 1 << (((uint32_t)SERCOM0 & 0x0000ff00) >> 10);
            /*spi lcd module*/
            GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE].reg = GCLK_PCHCTRL_GEN(CONF_GCLK_SERCOM0_CORE_SRC) | GCLK_PCHCTRL_CHEN;
            while ((GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE].reg & GCLK_PCHCTRL_CHEN) != GCLK_PCHCTRL_CHEN)
            {
                /* Wait for synchronization */
            }
            break;
        case SERCOM_LCD_BRIDGE:
            MCLK->APBDMASK.reg |= 1 << (((uint32_t)SERCOM7 & 0x0000ff00) >> 10);
            /*spi lcd module*/
            GCLK->PCHCTRL[SERCOM7_GCLK_ID_CORE].reg = GCLK_PCHCTRL_GEN(CONF_GCLK_SERCOM7_CORE_SRC) | GCLK_PCHCTRL_CHEN;
            while ((GCLK->PCHCTRL[SERCOM7_GCLK_ID_CORE].reg & GCLK_PCHCTRL_CHEN) != GCLK_PCHCTRL_CHEN)
            {
                /* Wait for synchronization */
            }
            break;
        default: 
            break;
    }  
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_Master_Init
* Visibility  :        public
* Description :        set spi to master mode and set tranfer mode, baud rate 
* Parameters  :        void *pSercom, uint8_t sercom_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_Master_Init(void *pSercom, uint8_t sercom_id)
{
    Sercom *module = (Sercom *)pSercom;

    SPI_Gclk_Enable_Channel(sercom_id);
    if(SERCOM_LCD == sercom_id)
    {
         /*set cs select control mode and enable Receiver*/
        module->SPI.CTRLA.reg = spi_config[0].ctrla & ~(SERCOM_SPI_CTRLA_IBON | SERCOM_SPI_CTRLA_ENABLE
                          | SERCOM_SPI_CTRLA_SWRST) ;
        /*set cs select control mode and enable Receiver*/
        module->SPI.CTRLB.reg = (spi_config[0].ctrlb & ~(SERCOM_SPI_CTRLB_MSSEN
                          | SERCOM_SPI_CTRLB_AMODE_Msk | SERCOM_SPI_CTRLB_SSDE | SERCOM_SPI_CTRLB_PLOADEN))
                          | (SERCOM_SPI_CTRLB_RXEN); 
        /*set baud rate*/
        module->SPI.BAUD.reg = spi_config[0].baud;
    }
    else if(SERCOM_LCD_BRIDGE == sercom_id)
    {
        /*set cs select control mode and enable Receiver*/
        module->SPI.CTRLA.reg = spi_config[1].ctrla & ~(SERCOM_SPI_CTRLA_IBON | SERCOM_SPI_CTRLA_ENABLE
                          | SERCOM_SPI_CTRLA_SWRST) ;
        /*set cs select control mode and enable Receiver*/
        module->SPI.CTRLB.reg = (spi_config[1].ctrlb & ~(SERCOM_SPI_CTRLB_MSSEN
                          | SERCOM_SPI_CTRLB_AMODE_Msk | SERCOM_SPI_CTRLB_SSDE | SERCOM_SPI_CTRLB_PLOADEN))
                          | (SERCOM_SPI_CTRLB_RXEN); 
        /*set baud rate*/
        module->SPI.BAUD.reg = spi_config[1].baud;
    }
    
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_Master_Deinit
* Visibility  :        public
* Description :        disable spi master mode and software reset 
* Parameters  :        void *pSercom
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_Master_Deinit(void *pSercom)
{
    Sercom *module = (Sercom *)pSercom;

    /*disable spi master mode and Software Reset*/
    while (module->SPI.SYNCBUSY .reg & (SERCOM_SPI_SYNCBUSY_SWRST | SERCOM_SPI_SYNCBUSY_ENABLE));
    module->SPI.CTRLA.bit.ENABLE = 0;  
    while (module->SPI.SYNCBUSY.reg & (SERCOM_SPI_SYNCBUSY_SWRST| SERCOM_SPI_SYNCBUSY_ENABLE));
    module->SPI.CTRLA.bit.SWRST = 0; 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_Master_Write
* Visibility  :        public
* Description :        spi master write interface 
* Parameters  :        void *pSercom, uint8_t *data, uint8_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_Master_Write(void *pSercom,const uint8_t *data, uint16_t length,const uint16_t *td_data)
{
    Sercom *module = (Sercom *)pSercom;
	    
    for(uint16_t i = 0; i < length; i++)
    {
        /*send data*/
        while(module->SPI.INTFLAG.bit.DRE == 0);
        if(SERCOM0 == module)
        {
		    /*lcd td7800 9bit*/
		    module->SPI.DATA.reg = td_data[i];
        }
        else
        {
		    /*lcd bridge ic 8 bit*/
            module->SPI.DATA.reg = data[i];
        } 
        /*dummy read*/
        while(module->SPI.INTFLAG.bit.RXC == 0);
        uint8_t temp = module->SPI.DATA.reg;

        /*Transmit Complete*/	 
        while(module->SPI.INTFLAG.bit.TXC == 0);
        module->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_TXC;              
    }
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_CHSIZE_Set
* Visibility  :        public
* Description :        spi CHSIZE  Set
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_CHSIZE_Set(bool level)
{
    SPI_Master_Deinit(SERCOM0);
	
    SPI_Master_Init(SERCOM0, SERCOM_LCD); 
    SERCOM0->SPI.CTRLB.bit.CHSIZE = level;

    SPI_Master_Set_Enable_Bit(SERCOM0);
	 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        SPI_Master_Read
* Visibility  :        public
* Description :        spi master read interface 
* Parameters  :        void *pSercom, uint8_t *data, uint16_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void SPI_Master_Read(void *pSercom, uint8_t* data, uint16_t length)
{
    Sercom *module = (Sercom *)pSercom;

    if(SERCOM0 == module)
    {
        /*lcd td7800 9bit->8bit*/
        SPI_CHSIZE_Set(false);
    }
    /*read data from addressed register*/
    for(uint16_t i = 0; i < length; i++)
    {
        /*send dummy byte to generate clock*/ 
        while(module->SPI.INTFLAG.bit.DRE == 0);
        module->SPI.DATA.reg = CONF_SERCOM_0_SPI_DUMMYBYTE;	

        while(module->SPI.INTFLAG.bit.RXC == 0);
        /* Check if data is overflown */
        if (module->SPI.STATUS.reg & SERCOM_SPI_STATUS_BUFOVF) 
        {
            /* Clear overflow flag */
            module->SPI.STATUS.reg = SERCOM_SPI_STATUS_BUFOVF;
        }
        data[i] = module->SPI.DATA.reg;
    }
    if(SERCOM0 == module)
    {
        /*lcd td7800 8bit->9bit*/
        SPI_CHSIZE_Set(true);				  
    }
		
}

#endif /* SPI_DRIVER */

