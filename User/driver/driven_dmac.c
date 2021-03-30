/******************************************************************************
*  Name: iic_master.c
*  Description:
*  Project: INFO4 11inch
*  Auther: Jun Gao
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../Device/samc21n18a.h"
#include "../config.h"
#include <stdint.h>
#include <stddef.h>
#include "config/hpl_dmac_config.h"
#include "driven_dmac.h"

#if CONF_DMAC_ENABLE
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
/* This macro DMAC configuration */
#define DMAC_CHANNEL_CFG(n)                                                                                         \
	{(CONF_DMAC_ENABLE_##n << DMAC_CHCTRLA_ENABLE_Pos),                                                                \
	 DMAC_CHCTRLB_TRIGACT(CONF_DMAC_TRIGACT_##n) | DMAC_CHCTRLB_TRIGSRC(CONF_DMAC_TRIGSRC_##n)                         \
	     | DMAC_CHCTRLB_LVL(CONF_DMAC_LVL_##n) | (CONF_DMAC_EVOE_##n << DMAC_CHCTRLB_EVOE_Pos)                         \
	     | (CONF_DMAC_EVIE_##n << DMAC_CHCTRLB_EVIE_Pos) | DMAC_CHCTRLB_EVACT(CONF_DMAC_EVACT_##n),                    \
	 DMAC_BTCTRL_STEPSIZE(CONF_DMAC_STEPSIZE_##n) | (CONF_DMAC_STEPSEL_##n << DMAC_BTCTRL_STEPSEL_Pos)                 \
	     | (CONF_DMAC_DSTINC_##n << DMAC_BTCTRL_DSTINC_Pos) | (CONF_DMAC_SRCINC_##n << DMAC_BTCTRL_SRCINC_Pos)         \
	     | DMAC_BTCTRL_BEATSIZE(CONF_DMAC_BEATSIZE_##n) | DMAC_BTCTRL_BLOCKACT(CONF_DMAC_BLOCKACT_##n)                 \
	     | DMAC_BTCTRL_EVOSEL(CONF_DMAC_EVOSEL_##n)}

#define COMPILER_PRAGMA(arg) _Pragma(#arg)
#if defined __GNUC__
#define COMPILER_ALIGNED(a) __attribute__((__aligned__(a)))
#elif defined __ICCARM__
#define COMPILER_ALIGNED(a) COMPILER_PRAGMA(data_alignment = a)
#elif defined __CC_ARM
#define COMPILER_ALIGNED(a) __attribute__((__aligned__(a)))
#endif

/**********************************************************************************************
* Local types
**********************************************************************************************/
/* Section containing first descriptors for all DMAC channels */
COMPILER_ALIGNED(16)
DmacDescriptor descriptor_section[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR;

/* Section containing current descriptors for all DMAC channels */
COMPILER_ALIGNED(16)
DmacDescriptor write_back_section[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR;

/* Array containing callbacks for DMAC channels*/
static struct DMAC_Resource resources[DMAC_CH_NUM];
/* DMAC channel configuration */
typedef struct dmac_channel_cfg 
{
  uint8_t  ctrla;
  uint32_t ctrlb;
  uint16_t btctrl;
} DMAC_CONFIG;

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/
DMAC_CONFIG  cfgs[] = {
                       DMAC_CHANNEL_CFG(0),
                       DMAC_CHANNEL_CFG(1),
                       DMAC_CHANNEL_CFG(2),
                       DMAC_CHANNEL_CFG(3),
                       DMAC_CHANNEL_CFG(4),
                       DMAC_CHANNEL_CFG(5),
                       DMAC_CHANNEL_CFG(6),
                       DMAC_CHANNEL_CFG(7),
                       DMAC_CHANNEL_CFG(8),
                       DMAC_CHANNEL_CFG(9),
                       DMAC_CHANNEL_CFG(10),
                       DMAC_CHANNEL_CFG(11),
                      }; 
static inline bool Dmacdescriptor_Get_BTCTRL_DSTINC_Bit(const void *const hw);
static inline bool Dmacdescriptor_Get_BTCTRL_SRCINC_Bit(const void *const hw);
static inline bool Dmacdescriptor_Get_BTCTRL_STEPSEL_Bit(const void *const hw);
static inline uint16_t DMAC_Read_INTPEND_ID_Bf(const void *const hw);
static inline uint8_t  DMAC_Get_CHINTFLAG_Reg(const void *const hw, uint8_t mask);
static inline void _dmac_handler(void);

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        static inline void Dmacdescriptor_Get_BTCTRL_DSTINC_Bit(const void *const hw);
* Visibility  :        static inline
* Description :        Get BTCTRL DSTINC Bit   
* Parameters  :        void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static inline bool Dmacdescriptor_Get_BTCTRL_DSTINC_Bit(const void *const hw)
{
  uint16_t tmp;
  tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
  tmp = (tmp & DMAC_BTCTRL_DSTINC) >> DMAC_BTCTRL_DSTINC_Pos;
  return (bool)tmp;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        static inline bool Dmacdescriptor_Get_BTCTRL_SRCINC_Bit(const void *const hw)
* Visibility  :        static inline
* Description :        Get BTCTRL SRCINC Bit   
* Parameters  :        void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static inline bool Dmacdescriptor_Get_BTCTRL_SRCINC_Bit(const void *const hw)
{
  uint16_t tmp;
  tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
  tmp = (tmp & DMAC_BTCTRL_SRCINC) >> DMAC_BTCTRL_SRCINC_Pos;
  return (bool)tmp;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        static inline bool Dmacdescriptor_Get_BTCTRL_STEPSEL_Bit(const void *const hw)
* Visibility  :        static inline
* Description :        Get BTCTRL STEPSEL Bit   
* Parameters  :        void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static inline bool Dmacdescriptor_Get_BTCTRL_STEPSEL_Bit(const void *const hw)
{
  uint16_t tmp;
  tmp = ((DmacDescriptor *)hw)->BTCTRL.reg;
  tmp = (tmp & DMAC_BTCTRL_STEPSEL) >> DMAC_BTCTRL_STEPSEL_Pos;
  return (bool)tmp; 
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        static inline uint16_t DMAC_Read_INTPEND_ID_Bf(const void *const hw);
* Visibility  :        static inline
* Description :        DMAC read intpend id buffer  
* Parameters  :        void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static inline uint16_t DMAC_Read_INTPEND_ID_Bf(const void *const hw)
{
  uint16_t tmp;
  tmp = ((Dmac *)hw)->INTPEND.reg;
  tmp = (tmp & DMAC_INTPEND_ID_Msk) >> DMAC_INTPEND_ID_Pos;
  return tmp;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        static inline uint16_t DMAC_Read_INTPEND_ID_Bf(const void *const hw);
* Visibility  :        static inline
* Description :        DMAC read intpend id buffer  
* Parameters  :        void *const hw
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
static inline uint8_t  DMAC_Get_CHINTFLAG_Reg(const void *const hw, uint8_t mask)
{
  uint8_t tmp;
  tmp = ((Dmac *)hw)->CHINTFLAG.reg;
  tmp &= mask;
  return tmp;
}
/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Init(void)
* Visibility  :        public
* Description :        Initialize DMA     
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/

void DMAC_Init(void)
{
  uint8_t i=0;
  DMAC->CTRL.reg &= ~DMAC_CTRL_DMAENABLE;  /*disable dmac*/
  DMAC->CTRL.reg &= ~DMAC_CTRL_CRCENABLE;  /*disable crc*/
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_SWRST; /*Reset DMAC*/

  DMAC->CTRL.reg = (CONF_DMAC_LVLEN0 << DMAC_CTRL_LVLEN0_Pos) | (CONF_DMAC_LVLEN1 << DMAC_CTRL_LVLEN1_Pos)
	                            | (CONF_DMAC_LVLEN2 << DMAC_CTRL_LVLEN2_Pos)
	                            | (CONF_DMAC_LVLEN3 << DMAC_CTRL_LVLEN3_Pos);
  uint8_t tmp = DMAC->DBGCTRL.reg;
  tmp &= ~DMAC_DBGCTRL_DBGRUN; 
  tmp |= CONF_DMAC_DBGRUN << DMAC_DBGCTRL_DBGRUN_Pos;  
  DMAC->DBGCTRL.reg = tmp;
/*debug control*/
  /*Priority control*/
  DMAC->PRICTRL0.reg = DMAC_PRICTRL0_LVLPRI0(CONF_DMAC_LVLPRI0) | DMAC_PRICTRL0_LVLPRI1(CONF_DMAC_LVLPRI1)
	        | DMAC_PRICTRL0_LVLPRI2(CONF_DMAC_LVLPRI2) | DMAC_PRICTRL0_LVLPRI3(CONF_DMAC_LVLPRI3)
	        | (CONF_DMAC_RRLVLEN0 << DMAC_PRICTRL0_RRLVLEN0_Pos) | (CONF_DMAC_RRLVLEN1 << DMAC_PRICTRL0_RRLVLEN1_Pos)
	        | (CONF_DMAC_RRLVLEN2 << DMAC_PRICTRL0_RRLVLEN2_Pos) | (CONF_DMAC_RRLVLEN3 << DMAC_PRICTRL0_RRLVLEN3_Pos);
  /* write to descriptor memory section*/
  
  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
  /*write to write-back memory section*/
  DMAC->WRBADDR.reg = (uint32_t)write_back_section; 

  for(; i< DMAC_CH_NUM;i++)
  {
    DMAC->CHID.reg = i; 

    DMAC->CHCTRLA.reg;
    tmp &= ~DMAC_CHCTRLA_RUNSTDBY;
    tmp |= cfgs[i].ctrla & DMAC_CHCTRLA_RUNSTDBY << DMAC_CHCTRLA_RUNSTDBY_Pos;
    DMAC->CHCTRLA.reg = tmp;

    DMAC->CHCTRLB.reg = cfgs[i].ctrlb;   
    ((DmacDescriptor *)&descriptor_section[i])->BTCTRL.reg = cfgs[i].btctrl;  
    ((DmacDescriptor *)&descriptor_section[i])->DESCADDR.reg = 0x0;
  } 
  /*Set interrupt*/
   
   NVIC_DisableIRQ(DMAC_IRQn);
   NVIC_ClearPendingIRQ(DMAC_IRQn);
   NVIC_EnableIRQ(DMAC_IRQn);  
   DMAC->CTRL.reg |= DMAC_CTRL_DMAENABLE;        /*Enable DMAC*/     
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Set_Irq_State
* Visibility  :        public
* Description :        Enable/disable DMA interrupt    
* Parameters  :        channel DMA channel to enable/disable interrupt for
*                      type The type of interrupt to disable/enable if applicable
*                      state Enable or disable
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   
* ===================================================================================================================*/
void DMAC_Set_Irq_State(const uint8_t channel, const enum DMAC_Callback_Type type, const bool state)
{
  DMAC->CHID.reg = channel;
  if (DMA_TRANSFER_COMPLETE_CB == type)
  {
    if (state == 0x0)
    {
      DMAC->CHINTENCLR.reg = DMAC_CHINTENSET_TCMPL;
    } 
    else 
    {
      DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;
    }   
  }
  else if(DMA_TRANSFER_ERROR_CB == type)
  {
    if (state == 0x0)
    {
      DMAC->CHINTENCLR.reg = DMAC_CHINTENSET_TERR;
    } 
    else
    {
      DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TERR;
    }
  }

}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Set_Destination_Address
* Visibility  :        public
* Description :        Set destination address    
* Parameters  :        channel  DMA channel to set destination address for
*                      addr  Destination address
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Set_Destination_Address(const uint8_t channel, const void *const addr)
{
  ((DmacDescriptor *)&descriptor_section[channel])->DSTADDR.reg = (uint32_t)addr; 
} 

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Set_Source_Address
* Visibility  :        public
* Description :        Set source address    
* Parameters  :        channel  DMA channel to set source address for
*                      addr  source address
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Set_Source_Address(const uint8_t channel, const void *const addr)
{
  ((DmacDescriptor *)&descriptor_section[channel])->SRCADDR.reg = (uint32_t)addr; 
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Set_Next_Descriptor
* Visibility  :        public
* Description :        Set next descriptor address    
* Parameters  :        current_channel   Current DMA channel to set next descriptor address
*                      next_channel    Next DMA channel used as next descriptor
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Set_Next_Descriptor(const uint8_t current_channel, const uint8_t next_channel)
{
  ((DmacDescriptor *)&descriptor_section[current_channel])->DESCADDR.reg = (uint32_t)&descriptor_section[next_channel];
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Source_Inc_Enable
* Visibility  :        public
* Description :        Enable/disable source address incrementation during DMA transaction 
* Parameters  :        channel   DMA channel to set destination address for
*                      enable    True to enable, false to disable
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Source_Inc_Enable(const uint8_t channel, const bool enable)
{
  uint16_t tmp = ((DmacDescriptor *)&descriptor_section[channel])->BTCTRL.reg;
  tmp &= ~DMAC_BTCTRL_SRCINC;
  tmp |= enable << DMAC_BTCTRL_SRCINC_Pos;
  ((DmacDescriptor *)&descriptor_section[channel])->BTCTRL.reg = tmp;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Destination_Inc_Enable
* Visibility  :        public
* Description :        Enable/disable source address incrementation during DMA transaction 
* Parameters  :        channel   DMA channel to set destination address for
*                      enable    True to enable, false to disable
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Destination_Inc_Enable(const uint8_t channel, const bool enable)
{
  uint16_t tmp = ((DmacDescriptor *)&descriptor_section[channel])->BTCTRL.reg;
  tmp &= ~DMAC_BTCTRL_DSTINC;
  tmp |= enable << DMAC_BTCTRL_DSTINC_Pos;
  ((DmacDescriptor *)&descriptor_section[channel])->BTCTRL.reg = tmp;
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Set_Data_Amount
* Visibility  :        public
* Description :        channel   DMA channel to trigger transaction on
* Parameters  :        amount Data amount
*                      enable    True to enable, false to disable
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Set_Data_Amount(const uint8_t channel, const uint32_t amount)
{
  uint32_t address = ((DmacDescriptor *)&descriptor_section[channel])->DSTADDR.reg;
  uint16_t tmp = ((DmacDescriptor *)&descriptor_section[channel])->BTCTRL.reg;
  tmp = (tmp & DMAC_BTCTRL_BEATSIZE_Msk) >> DMAC_BTCTRL_BEATSIZE_Pos;
  uint8_t  beat_size = tmp;
/*set the destination address*/
  if(Dmacdescriptor_Get_BTCTRL_DSTINC_Bit(&descriptor_section[channel]))
  {
    ((DmacDescriptor *)&descriptor_section[channel])->DSTADDR.reg =  address + amount * (1 << beat_size);
  }
  
  address =((DmacDescriptor *)&descriptor_section[channel])->SRCADDR.reg ;
 /*set the source address*/
  if(Dmacdescriptor_Get_BTCTRL_SRCINC_Bit(&descriptor_section[channel]))
  {
    ((DmacDescriptor *)&descriptor_section[channel])->SRCADDR.reg = address + amount * (1 << beat_size);
  }
  
  ((DmacDescriptor *)&descriptor_section[channel])->BTCNT.reg = amount;
  
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Enable_Transaction
* Visibility  :        public
* Description :        channel   DMA channel to trigger transaction on
* Parameters  :        software_trigger   software trigger
*                      enable    True to enable, false to disable
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Enable_Transaction(const uint8_t channel, const bool software_trigger)
{
  DMAC->CHID.reg = channel;  /*write CHID registor*/
  ((DmacDescriptor *)&descriptor_section[channel])->BTCTRL.reg |= DMAC_BTCTRL_VALID;  /*Set BTCTRL Valid Bit*/
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;    /*enable DMAC */
  if (software_trigger)
  {
    DMAC->SWTRIGCTRL.reg |= (1 << channel);   /*Software trigger registor control*/
  }
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Enable_Transaction
* Visibility  :        public
* Description :        channel   DMA channel to trigger transaction on
* Parameters  :        software_trigger   software trigger
*                      enable    True to enable, false to disable
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Get_Channel_Resource(struct DMAC_Resource* *resource, const uint8_t channel)
{
  *resource =&resources[channel];
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        _dmac_handler(void)
* Visibility  :        static inline
* Description :        Internal DMAC interrupt handler
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
static inline void _dmac_handler(void)
{
   uint8_t channel = DMAC_Read_INTPEND_ID_Bf(DMAC);
   uint8_t current_channel = (DMAC->CHID.reg);   /*read the current channel*/
   uint8_t flag_status;
   struct DMAC_Resource *tmp_resource = &resources[channel];
   DMAC->CHID.reg = channel;     /*write channel*/
   flag_status = DMAC_Get_CHINTFLAG_Reg(DMAC, DMAC_CHINTFLAG_MASK);  /*Get the CHINT flag*/

   if (flag_status & DMAC_CHINTFLAG_TERR)
   {
     DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TERR;    /*Clear CHINTFLAG TERR Bit*/
     tmp_resource->dma_cb.error(tmp_resource);
   }
   else if(flag_status & DMAC_CHINTFLAG_TCMPL)
   {
     DMAC->CHINTFLAG.reg = DMAC_CHINTFLAG_TCMPL;
   }
   DMAC->CHID.reg = current_channel;  /*write current channel*/
}

/*=====================================================================================================================
* Upward trac.:        DRIVER/dmac
* Method      :        DMAC_Handler(void)
* Visibility  :        public
* Description :        Internal DMAC interrupt handler
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :   void
* ===================================================================================================================*/
void DMAC_Handler(void)
{
  _dmac_handler();
}

#endif
