/******************************************************************************
*  Name: flash.c
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#if (FLASH_DRIVER == 1)
#include "../../Device/samc21n18a.h"
#include "config/hpl_nvmctrl_config.h"
#include "flash.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define NVM_MEMORY          ((volatile uint16_t *)FLASH_ADDR)
#define RWW_EEPROM_ADDR     0x00400000

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static uint32_t nvm = {(CONF_NVM_CACHE << NVMCTRL_CTRLB_CACHEDIS_Pos) \
                      | (CONF_NVM_READ_MODE << NVMCTRL_CTRLB_READMODE_Pos) | (CONF_NVM_SLEEPPRM << NVMCTRL_CTRLB_SLEEPPRM_Pos)};

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_Init
* Visibility  :        public
* Description :        flash driver init
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_Init(void)
{
    MCLK->APBBMASK.reg |= MCLK_AHBMASK_NVMCTRL;

    uint32_t ctrlb = nvm & ~(NVMCTRL_CTRLB_RWS_Msk | NVMCTRL_CTRLB_MANW);

    /*CTRLB.MANW=1,Write commands must be issued through the CTRLA.CMD register.*/
    ctrlb |= (NVMCTRL->CTRLB.reg & NVMCTRL_CTRLB_RWS_Msk) | NVMCTRL_CTRLB_MANW;
    NVMCTRL->CTRLB.reg = ctrlb; 

    NVIC_DisableIRQ(NVMCTRL_IRQn);
    NVIC_ClearPendingIRQ(NVMCTRL_IRQn);
    NVIC_EnableIRQ(NVMCTRL_IRQn);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_Erase_Row
* Visibility  :        public
* Description :        erase flash row
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_Erase_Row(uint32_t address, uint32_t cmd)
{ 
    /* Wait until this module isn't busy */
    while(!((NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY)>> NVMCTRL_INTFLAG_READY_Pos));
 
  	/* Clear flags */
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK; 

  	/* Set address and command */
    NVMCTRL->ADDR.reg = address / 2; 
    NVMCTRL->CTRLA.reg = cmd | NVMCTRL_CTRLA_CMDEX_KEY; 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_Read
* Visibility  :        public
* Description :        read flash interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_Read(uint32_t address, uint8_t *data, uint16_t length)
{
    uint32_t nvm_address = address / 2;
    uint32_t i;
    uint16_t temp;
  
    /* Check if the module is busy */
    while(!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY)); 
	  
    /* Clear flags */
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

    if(address % 2)
    {
        temp = NVM_MEMORY[nvm_address++];
        data[0] = temp >> 8;
        i = 1;
    }
    else
    {
        i = 0;
    }
    /* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
	   * to ensure alignment */
    while(i < length)
    {  
        temp = NVM_MEMORY[nvm_address++];
        data[i] = (temp & 0xFF);
        if(i < (length - 1))
        {
            data[i + 1] = (temp >> 8);
        }
        i += 2;
    }
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_Program
* Visibility  :        public
* Description :        program flash interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_Program(uint32_t address, uint8_t *data, uint16_t length, uint32_t cmd)
{
    uint32_t nvm_address = address / 2;
    uint16_t i;
    uint16_t temp;
   
    while(!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMD_PBC | NVMCTRL_CTRLA_CMDEX_KEY;
    
    while(!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;
    
   
    
    for(i = 0; i < length; i += 2)
    {
        temp = data[i];
        if(i < NVMCTRL_PAGE_SIZE - 1)
        {
            temp |= (data[i + 1] << 8);
        }
        NVM_MEMORY[nvm_address++] = temp;
    }

    while(!(NVMCTRL->INTFLAG.reg & NVMCTRL_INTFLAG_READY));

    NVMCTRL->ADDR.reg = address / 2;
    NVMCTRL->CTRLA.reg = cmd | NVMCTRL_CTRLA_CMDEX_KEY;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_Write
* Visibility  :        public
* Description :        write flash interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_Write(uint32_t address, uint8_t *data, uint16_t length, uint32_t erase_cmd, uint32_t prog_cmd)
{
    uint8_t  tmp_buffer[NVMCTRL_ROW_PAGES][NVMCTRL_PAGE_SIZE];
    uint32_t row_start_addr;
    uint32_t row_end_addr;
    uint32_t i;
    uint32_t j;
    uint32_t k;
    uint32_t wr_start_addr = address;

    do
    {
        /*row_start_addr = wr_start_addr & (~((NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES) - 1));*/
        row_start_addr = wr_start_addr & (~((uint32_t)(NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES - 1)));  /*negative value implicitly converted to an unsigned type*/
        row_end_addr   = row_start_addr + NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE - 1;

        for(i = 0; i < NVMCTRL_ROW_PAGES; i++)
        {                                  
            Flash_Read(row_start_addr + i * NVMCTRL_PAGE_SIZE, tmp_buffer[i], NVMCTRL_PAGE_SIZE);
        }

        j = (wr_start_addr - row_start_addr) / NVMCTRL_PAGE_SIZE;
        k = wr_start_addr - row_start_addr - j * NVMCTRL_PAGE_SIZE;
        while((wr_start_addr <= row_end_addr) && (length > 0))
        {
            tmp_buffer[j][k] = *data;
            k = (k + 1) % NVMCTRL_PAGE_SIZE;
            if(0 == k)
            {
                j++;
            }
            wr_start_addr++;
            data++;
            length--;
        }

        // ROM: NVMCTRL_CTRLA_CMD_ER;  RWW: NVMCTRL_CTRLA_CMD_RWWEEER;
        Flash_Erase_Row(row_start_addr, erase_cmd);

        for(i = 0; i < NVMCTRL_ROW_PAGES; i++)
        {
            // ROM: NVMCTRL_CTRLA_CMD_WP;  RWW: NVMCTRL_CTRLA_CMD_RWWEEWP;
            Flash_Program(row_start_addr + i * NVMCTRL_PAGE_SIZE,
                          tmp_buffer[i], NVMCTRL_PAGE_SIZE, prog_cmd);
        }

    }
    while(row_end_addr < (wr_start_addr + length - 1));
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_ROM_Write
* Visibility  :        public
* Description :        write flash interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_ROM_Write(uint32_t address, uint8_t *data, uint16_t length)
{
    Flash_Write(address, data, length, NVMCTRL_CTRLA_CMD_ER, NVMCTRL_CTRLA_CMD_WP);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_ROM_Read
* Visibility  :        public
* Description :        read flash interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_ROM_Read(uint32_t address, uint8_t *data, uint16_t length)
{
    Flash_Read(address, data, length);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_RWW_Write
* Visibility  :        public
* Description :        RWW flash write interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_RWW_Write(uint32_t address, uint8_t *data, uint16_t length)
{
    address += RWW_EEPROM_ADDR;
    Flash_Write(address, data, length, NVMCTRL_CTRLA_CMD_RWWEEER, NVMCTRL_CTRLA_CMD_RWWEEWP);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Flash_RWW_Read
* Visibility  :        public
* Description :        RWW flash read interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Flash_RWW_Read(uint32_t address, uint8_t *data, uint16_t length)
{
    address += RWW_EEPROM_ADDR;
    Flash_Read(address, data, length);
}

#endif /*FLASH_DRIVER*/
