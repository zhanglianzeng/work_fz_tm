/******************************************************************************
*  Name: iic_slave.c
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#include "gpio.h"
#if (IIC_SLAVE_DRIVER == 1)
#include "../../Device/samc21n18a.h"
#include "./config/peripheral_clk_config.h"
#include "./config/hpl_sercom_config.h"
#include "iic_slave.h"
#include <stdlib.h>

/**********************************************************************************************
* External objects
**********************************************************************************************/
 
/**********************************************************************************************
* Global variables
**********************************************************************************************/
static IIC_Slave_Callback IIC_Callback = (IIC_Slave_Callback)(0);
static void IIC_Slave_CLK_Init(uint8_t sercom_id);
/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define I2CS_CONFIGURATION(n)                                                                                  \
        SERCOM_I2CM_CTRLA_MODE_I2C_SLAVE                                                                       \
        | (CONF_SERCOM_##n##_I2CS_RUNSTDBY << SERCOM_I2CS_CTRLA_RUNSTDBY_Pos)                                  \
        | SERCOM_I2CS_CTRLA_SDAHOLD(CONF_SERCOM_##n##_I2CS_SDAHOLD)                                            \
        | (CONF_SERCOM_##n##_I2CS_SEXTTOEN << SERCOM_I2CS_CTRLA_SEXTTOEN_Pos)                                  \
        | (CONF_SERCOM_##n##_I2CS_SPEED << SERCOM_I2CS_CTRLA_SPEED_Pos)                                        \
        | (CONF_SERCOM_##n##_I2CS_SCLSM << SERCOM_I2CS_CTRLA_SCLSM_Pos)                                        \
        | (CONF_SERCOM_##n##_I2CS_LOWTOUT << SERCOM_I2CS_CTRLA_LOWTOUTEN_Pos),                                 \
        SERCOM_I2CS_CTRLB_SMEN | SERCOM_I2CS_CTRLB_AMODE(CONF_SERCOM_##n##_I2CS_AMODE),                        \
        (CONF_SERCOM_##n##_I2CS_GENCEN << SERCOM_I2CS_ADDR_GENCEN_Pos)                                         \
        | SERCOM_I2CS_ADDR_ADDR(CONF_SERCOM_##n##_I2CS_ADDRESS)                                                \
        | (CONF_SERCOM_##n##_I2CS_TENBITEN << SERCOM_I2CS_ADDR_TENBITEN_Pos)                                   \
        | SERCOM_I2CS_ADDR_ADDRMASK(CONF_SERCOM_##n##_I2CS_ADDRESS_MASK)    


#define CMD_ACK     0x03


/**********************************************************************************************
* Local types
**********************************************************************************************/
struct i2cs_config_struct
{
    uint32_t ctrl_a;
    uint32_t ctrl_b;
    uint32_t address;
};

/**********************************************************************************************
* Local variables
**********************************************************************************************/
struct i2cs_config_struct i2cs_config = {I2CS_CONFIGURATION(1)};

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_GPIO_Init
* Visibility  :        public
* Description :        i2c slave gpio init
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_GPIO_Init(void)
{
    GPIO_Set_Function(CTP_SDA, PINMUX_PA16C_SERCOM1_PAD0);  
    GPIO_Set_Function(CTP_SCL, PINMUX_PA17C_SERCOM1_PAD1);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_GPIO_Deinit
* Visibility  :        public
* Description :        i2c slave gpio Deinit
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_GPIO_Deinit(void)
{        
    GPIO_Set_Direction(CTP_SDA, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(CTP_SDA, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(CTP_SDA, false);
    GPIO_Set_Direction(CTP_SCL, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(CTP_SCL, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(CTP_SCL, false); 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_CLK_Init
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
static void IIC_Slave_CLK_Init(uint8_t sercom_id)
{
    if(sercom_id==SLAVE_ID)
    { 
      GCLK->PCHCTRL[SERCOM1_GCLK_ID_CORE].reg = CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
      GCLK->PCHCTRL[SERCOM1_GCLK_ID_SLOW].reg = CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    }
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_Init
* Visibility  :        public
* Description :        set sercom to i2c slave mode
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_Init(Sercom *pSercom, uint8_t sercom_id)
{   
    Sercom *module = (Sercom *)pSercom;
	
    MCLK->APBCMASK.reg |= 1 << (((uint32_t)module & 0x0000ff00) >> 10);
    IIC_Slave_CLK_Init(sercom_id);
   
    module->I2CS.CTRLA.reg = i2cs_config.ctrl_a; 
    module->I2CS.CTRLB.reg = i2cs_config.ctrl_b; 
    module->I2CS.ADDR.reg = i2cs_config.address; 
    module->I2CS.INTENSET.reg = SERCOM_I2CS_INTENCLR_MASK;
    /*Writing '0' to this bit has no effect.*/
    module->I2CS.INTENCLR.reg &= (uint8_t)(~SERCOM_I2CS_INTENCLR_MASK);

    /*enable  irq*/
    if(sercom_id==SLAVE_ID)
    {
        NVIC_DisableIRQ((IRQn_Type)SERCOM1_IRQn);
        NVIC_ClearPendingIRQ((IRQn_Type)SERCOM1_IRQn);
        NVIC_EnableIRQ((IRQn_Type)SERCOM1_IRQn);
		
        IIC_Slave_GPIO_Init();
    }
    
    while (module->I2CS.SYNCBUSY.reg & (SERCOM_I2CS_SYNCBUSY_SWRST | SERCOM_I2CS_SYNCBUSY_ENABLE));
    module->I2CS.CTRLA.bit.ENABLE = 1; 
    module->I2CS.CTRLB.bit.ACKACT = 0; 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_DeInit
* Visibility  :        public
* Description :        disable i2c slave mode and reset module
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_DeInit(Sercom *pSercom, uint8_t sercom_id)
{
    Sercom *module = (Sercom *)pSercom;
    
    module->I2CS.CTRLA.bit.ENABLE = 0; 
    module->I2CS.CTRLA.bit.SWRST = 1; 

    if(sercom_id==SLAVE_ID)
    {
        NVIC_DisableIRQ((IRQn_Type)SERCOM1_IRQn);
        IIC_Slave_GPIO_Deinit();  
    }
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_ReadOneByte
* Visibility  :        public
* Description :        read byte from i2c data reg
* Parameters  :        uint8_t *pData
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_ReadOneByte(Sercom *pSercom,uint8_t *pData)
{
    Sercom *module = (Sercom *)pSercom;
    if (NULL == pData)
    {
        return;
    }

    *pData = module->I2CS.DATA.reg; 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_WriteOneByte
* Visibility  :        public
* Description :        send new byte
* Parameters  :        uint8_t Data
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_WriteOneByte(Sercom *pSercom,uint8_t Data)
{
    Sercom *module = (Sercom *)pSercom;
    module->I2CS.DATA.reg = Data; 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Slave_SetCallBack
* Visibility  :        public
* Description :        send i2c irq callback function
* Parameters  :        IIC_Slave_Callback pCallBack
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void IIC_Slave_SetCallBack( IIC_Slave_Callback pCallBack )
{
    IIC_Callback = pCallBack;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        SERCOM5_Handler
* Visibility  :        public
* Description :        i2c irq function
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void SERCOM1_Handler(void)
{
    uint16_t status = SERCOM1->I2CS.STATUS.reg & (DIR_STATUS | ACK_STATUS); 
    uint8_t flag = SERCOM1->I2CS.INTFLAG.reg;
    
    if(IIC_Callback)
    {
        IIC_Callback(status,flag);
    }
    if(ERROR_FLAG == (flag & ERROR_FLAG))
    {
        SERCOM1->I2CS.INTFLAG.reg = ERROR_FLAG;
        return;
    }
    else if(AMATCH_FLAG == (flag & AMATCH_FLAG))
    {
       SERCOM1->I2CS.INTFLAG.reg = AMATCH_FLAG;
    }
    else if(PREC_FLAG == (flag & PREC_FLAG))
    {
        SERCOM1->I2CS.INTFLAG.reg = PREC_FLAG;	
    }
//    else if(DRDY_FLAG == (flag & DRDY_FLAG))
//    {
//        SERCOM1->I2CS.INTFLAG.reg = DRDY_FLAG;	
//    }
    SERCOM1->I2CS.INTENCLR.reg &= (uint8_t)(~SERCOM_I2CS_INTENCLR_MASK);
}

#endif /* IIC_SLAVE_DRIVER */
