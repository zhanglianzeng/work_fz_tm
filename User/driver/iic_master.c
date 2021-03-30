/******************************************************************************
*  Name: iic_master.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#if (IIC_MASTER_DRIVER == 1)
#include "./config/hpl_sercom_config.h"
#include "../../Device/samc21n18a.h"
#include "iic_master.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
// The below down "I2CM_CONFIGURATION(n)" n=SERCOM number.
#define I2CM_CONFIGURATION(n)                                               \
    {                                                                       \
        (SERCOM_I2CM_CTRLA_MODE_I2C_MASTER)                                 \
      | (CONF_SERCOM_##n##_I2CM_RUNSTDBY << SERCOM_I2CM_CTRLA_RUNSTDBY_Pos) \
      | (CONF_SERCOM_##n##_I2CM_SPEED << SERCOM_I2CM_CTRLA_SPEED_Pos)       \
      | (CONF_SERCOM_##n##_I2CM_MEXTTOEN << SERCOM_I2CM_CTRLA_MEXTTOEN_Pos) \
      | (CONF_SERCOM_##n##_I2CM_SEXTTOEN << SERCOM_I2CM_CTRLA_SEXTTOEN_Pos) \
      | (CONF_SERCOM_##n##_I2CM_INACTOUT << SERCOM_I2CM_CTRLA_INACTOUT_Pos) \
      | (CONF_SERCOM_##n##_I2CM_LOWTOUT << SERCOM_I2CM_CTRLA_LOWTOUTEN_Pos) \
      | (CONF_SERCOM_##n##_I2CM_SDAHOLD << SERCOM_I2CM_CTRLA_SDAHOLD_Pos)   \
      | (CONF_SERCOM_##n##_I2CS_SCLSM << SERCOM_I2CM_CTRLA_SCLSM_Pos),      \
      (uint32_t)(CONF_SERCOM_##n##_I2CM_BAUD_RATE)                          \
    }

#define CMD_STOP 0x3
#define CMD_ACK 0x2
#define I2C_IDLE 0x1
#define I2C_SM 0x0
#define I2C_FM 0x1
#define I2C_HS 0x2
#define I2C_MASTER_MODE 0x5
#define TEN_ADDR_FRAME 0x78
#define TEN_ADDR_MASK 0x3ff
#define SEVEN_ADDR_MASK 0x7f

#define ERROR_FLAG (1 << 7)
#define SB_FLAG (1 << 1)
#define MB_FLAG (1 << 0)

/**********************************************************************************************
* Local types
**********************************************************************************************/
struct i2cm_config_struct
{
    uint32_t   ctrl_a;
    uint32_t   baud;
};
/**********************************************************************************************
* Local variables
**********************************************************************************************/
struct i2cm_config_struct i2cm_config[2] = {I2CM_CONFIGURATION(5), I2CM_CONFIGURATION(4)}; //1:DES, 2:LED

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void NVIC_Disable_Irq(uint8_t sercom_id);
static void Set_Sercom_clk(Sercom *pSercom,uint8_t sercom_id);

/**********************************************************************************************
* Global functions
**********************************************************************************************/ 

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Master_Enable_Bit
* Visibility  :        void
* Description :        enable secrom i2c master mode
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void IIC_Master_Enable_Bit(void *pSercom)
{
    Sercom *module = (Sercom *)pSercom;
    
    /* enabled i2c peripheral */
    while (module->I2CM.SYNCBUSY.reg & (SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE));
    module->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_MODE(I2C_MASTER_MODE); 
    while (module->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_SYSOP);
    module->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(I2C_IDLE);
    while((module->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk) >> SERCOM_I2CM_STATUS_BUSSTATE_Pos != I2C_IDLE); 
    module->I2CM.INTFLAG.reg = ERROR_FLAG | SB_FLAG | MB_FLAG;
    module->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Master_Init
* Visibility  :        void
* Description :        init secrom to i2c master mode
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void IIC_Master_Init(Sercom *pSercom, uint8_t sercom_id)
{
    Sercom *module = (Sercom *)pSercom;
    /*set clk*/
    Set_Sercom_clk(module, sercom_id);

    module->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;

    module->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_SMEN;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Master_DeInit
* Visibility  :        void
* Description :        Deinitialize sercom i2c module
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void IIC_Master_DeInit(void *pSercom, uint8_t sercom_id)
{
    Sercom *module = (Sercom *)pSercom;
    NVIC_Disable_Irq(sercom_id);
    while (module->I2CM.SYNCBUSY.reg & (SERCOM_I2CM_SYNCBUSY_SWRST | SERCOM_I2CM_SYNCBUSY_ENABLE));
    module->I2CM.CTRLA.bit.ENABLE = 0; 
    module->I2CM.CTRLA.bit.SWRST = 1; 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Set_Sercom_clk
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
static void Set_Sercom_clk(Sercom *pSercom,uint8_t sercom_id)
{
    Sercom *module = (Sercom *)pSercom;
    if(sercom_id==SERCOM_LED)
    { 
      GCLK->PCHCTRL[SERCOM4_GCLK_ID_CORE].reg = CONF_GCLK_SERCOM4_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);		
      GCLK->PCHCTRL[SERCOM4_GCLK_ID_SLOW].reg = CONF_GCLK_SERCOM4_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    
      MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM4;
		  
      module->I2CM.CTRLA.reg = i2cm_config[1].ctrl_a; 											 
      module->I2CM.BAUD.reg = i2cm_config[1].baud; 
   }
   if(sercom_id==SERCOM_DES)
   {   
      GCLK->PCHCTRL[SERCOM5_GCLK_ID_CORE].reg = CONF_GCLK_SERCOM5_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
      GCLK->PCHCTRL[SERCOM5_GCLK_ID_SLOW].reg = CONF_GCLK_SERCOM5_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);
    
      MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM5;
		 
      module->I2CM.CTRLA.reg = i2cm_config[0].ctrl_a; 											 
      module->I2CM.BAUD.reg = i2cm_config[0].baud; 
   }
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Sercom_Send_Stop_Signal
* Visibility  :        void
* Description :        secrom i2c send stop signal
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void Sercom_Send_Stop_Signal(Sercom * module)
{
    while (module->I2CM.SYNCBUSY.bit.SYSOP);
    module->I2CM.CTRLB.bit.CMD |= CMD_STOP; 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        NVIC_Disable_Irq
* Visibility  :        void
* Description :        disable secrom irq
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void NVIC_Disable_Irq(uint8_t sercom_id)
{
    switch (sercom_id)
    {
        case SERCOM_DES:
            NVIC_DisableIRQ((IRQn_Type)(SERCOM5_IRQn));
            break;
        case SERCOM_LED:
            NVIC_DisableIRQ((IRQn_Type)(SERCOM2_IRQn));
            break;
        default: 
            break;
    }
}


/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Master_Wait_Busy
* Visibility  :        void
* Description :        Wait for bus response
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static enum I2c_Status IIC_Master_Wait_Busy(Sercom *module, uint32_t *flags)
{
    uint32_t timeout = 65535;

    do 
    {
        *flags = module->I2CM.INTFLAG.reg;

        if (timeout-- == 0) 
        {
            return STATUS_TIMEOUT;
        }
    } while (!(*flags & MB_FLAG) && !(*flags & SB_FLAG));

    return STATUS_NULL;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Master_Write
* Visibility  :        void
* Description :        write data to addressed slave
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
enum I2c_Status IIC_Master_Write(Sercom *module, uint8_t slave_addr, uint8_t *data, uint16_t length, uint8_t send_stop)
{
    uint32_t flags = 0;
    enum I2c_Status tmp_status = STATUS_NULL;

    while(module->I2CM.SYNCBUSY.bit.SYSOP);
	
    module->I2CM.ADDR.bit.ADDR = (slave_addr<<1) | I2C_WRITE;
    /*delay and check master or slave*/
    tmp_status = IIC_Master_Wait_Busy(module,&flags);
    if(STATUS_TIMEOUT==tmp_status )
    {
        return tmp_status;
    }

    for(uint16_t i = 0; i < length; i++)
    {
        /*This bit is set when any error is detected. Errors that will set this flag have corresponding status bits in the
          STATUS register*/
        if(module->I2CM.INTFLAG.bit.ERROR)/*error found*/
        {
            Sercom_Send_Stop_Signal(module);
            module->I2CM.INTFLAG.reg = MB_FLAG | SB_FLAG | ERROR_FLAG;
            return STATUS_ERR;
        }
        /*RXNACK=0,Slave responded with ACK*/
        if(module->I2CM.STATUS.bit.RXNACK == 0) 
        {
            module->I2CM.DATA.reg = data[i];/*continue to send data*/   
        }
        else
        {   
            /* Slave rejects to receive more data */
            if (send_stop)
            {
                /*send stop signal*/
                Sercom_Send_Stop_Signal(module);
            }
            module->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_MB;
            return STATUS_ERR;
        }
        tmp_status = IIC_Master_Wait_Busy(module,&flags);
        if (STATUS_TIMEOUT==tmp_status)
        {
            if (send_stop)
            {
                /*send stop signal*/
                Sercom_Send_Stop_Signal(module);
            }
            return tmp_status;
        }
    }
    if (send_stop)
    {
        /*send stop signal*/
        Sercom_Send_Stop_Signal(module);
    }
    /*writing 1 to this bit location will clear the MB flag.*/
    module->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_MB;
    return STATUS_NULL;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        IIC_Master_Read
* Visibility  :        void
* Description :        read data from addressed slave
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
enum I2c_Status IIC_Master_Read(Sercom *module, uint8_t slave_addr, uint8_t *data, uint16_t length, uint8_t send_stop)
{
    uint32_t flags = 0;
    uint16_t status = 0;
    uint16_t counter = 0;
    uint8_t sclsm_flag = 0;
    uint16_t data_len = length;
    enum I2c_Status tmp_status = STATUS_NULL;

    sclsm_flag = module->I2CM.CTRLA.bit.SCLSM;
    if (((data_len == 1) && sclsm_flag))
    {
        module->I2CM.CTRLB.bit.ACKACT = 1;/*send NACK*/
    }else
    {
        module->I2CM.CTRLB.bit.ACKACT = 0;/*send ACK*/
    }
    module->I2CM.ADDR.bit.ADDR = (slave_addr << 1)|I2C_READ;

    while (data_len)
    {
        /*SB will be auto set by hardware when recevied first byte*/
        tmp_status = IIC_Master_Wait_Busy(module,&flags);
        if (STATUS_TIMEOUT==tmp_status)
        {
            /* Set action to NACK */
            module->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
            if (send_stop)/*send_stop*/ 
            {
                /* Send stop command unless arbitration is lost. */
                Sercom_Send_Stop_Signal(module);
            }
            return tmp_status;
        }
		
        /* Check that no SB interrupt has occurred.*/
        if (flags & SB_FLAG)
        {
            while (module->I2CM.SYNCBUSY.bit.SYSOP);
            status = module->I2CM.STATUS.reg;
            /* Read data buffer. */
            if ((data_len--) && !(status & SERCOM_I2CM_STATUS_RXNACK)) 
            {
                if ((data_len == 0 && !sclsm_flag)||(sclsm_flag && (data_len == 1))) 
                {
                    /* Set action to NACK */
                    module->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
                }
                if (data_len == 0)
                { 
                    if (send_stop)/*send_stop*/ 
                    {
                        /* Send stop command unless arbitration is lost. */
                        Sercom_Send_Stop_Signal(module);
                    }
                }
                /* Save data to buffer. */
                while (module->I2CM.SYNCBUSY.bit.SYSOP);
                data[counter++] = module->I2CM.DATA.reg;
            }
            else
            {
                /*clear SB interrupt flag*/
                module->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;
                return STATUS_ERR;
            }
            /*clear SB interrupt flag*/
            module->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;
        }
        else
        {
            /*writing 1 to this bit location will clear the MB flag.*/
            module->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_MB;
        }
    }
    return STATUS_NULL;
}

#endif /* IIC_MASTER_DRIVER */
