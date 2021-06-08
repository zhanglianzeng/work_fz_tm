/******************************************************************************
*  Name: app_eeprom.c
*  Description:
*  Project: INFO4 11inch
*  Author: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (EEPROM_MODULE == 1)

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "../../../device/samc21n18a.h"
#include "../../../device/instance/nvmctrl.h"
#include "eep_data.h"
#include "app_eeprom.h"

#if (FLASH_MODULE == 1)
#include "../../middle/flash/flash_if.h"
#endif

#if (CRC_COM == 1)
#include "../../common/crc.h"
#endif

#define MAX_PAGE_FLASH_CNT  (1000000u)                                   /*Maximum number of flash operations allowed*/
#define MAX_COMMIT_DELAY    (1000)                                      /*Maximum allowable delay time, unit ms*/
#define RECORD_MARKER       (0xA5u)                                      /*record marker*/
#define INVALID_ROW         (NVMCTRL_RWWEE_PAGES/NVMCTRL_ROW_PAGES)     /*invalid row,  8*/  
#define INVALID_PAGE        (NVMCTRL_PAGE_HW)                           /*invalid page, 32*/
#define FREE_PAGE_BUFFER    (NUMB_GROUPS)                               /*free page buffer*/
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
enum {
    ROW_STATE_SPARE,                    /*indicate the row is in spare state*/
    ROW_STATE_USED,                     /*indicate the row is in used state*/
    ROW_STATE_NUMB                      /*state number*/
};

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef union
{
    struct
    {
        uint32_t flash_cnt: 20;     /*flash operations count, max 0x0F4240 (1000000)*/
        uint32_t rec_id:     4;     /*record ID*/
        uint32_t marker:     8;     /*fixed 0xA5*/
    }asData;
    uint32_t asWord;
}header_t;

typedef struct {
    uint32_t    flash_cnt;          /*flash operations count*/
    uint16_t    timer;              /*delay time*/
    uint8_t     start_page;         /*the first page*/
    bool        postponed;          /*indicate flash operations is postponed or not*/
}group_t;


enum {
    IDLE,                           /*idle state*/
    WRITE_PAGE,                     /*write page state*/
    ERASE_ROW,                      /*erase row state*/
    NUMB_STATE                      /*state number*/
};

typedef struct 
{
    uint8_t state;                  /*current state*/
    uint8_t start_page;             /*the first page*/
    uint8_t numb_pages;             /*page number*/
    uint8_t cnt_page;               /*cnt page*/
    uint8_t row;                    /*row*/
}interrupt_obj_t;

/**********************************************************************************************
* Local variables
**********************************************************************************************/
static Factory_Config pConfig_Init;                                 /*factory config data*/
static uint32_t page_buff[NVMCTRL_ROW_PAGES*2u][NVMCTRL_PAGE_W];     /*eeprom data buffer*/
static group_t groups[NUMB_GROUPS];                                 /*basic operation information of groups*/
static interrupt_obj_t interrupt_obj;                               /*interrupt operation information*/
static uint8_t active_group;                                        /*current active group*/
static uint8_t rows[NUMB_ROWS_DOUBLE];                              /*status of rows (only for double groups)*/

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_GetAddr(void)
* Visibility  :        public
* Description :        get factory data pointer 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  eep_group1_ID_t 
* ===================================================================================================================*/
eep_group1_ID_t CEeprom_GetAddr(void) 
{
    return eep_group1_ID_default;  /*return eeprom group default data pointer*/
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_Read_Rww(uint32_t addr, uint8_t *data, uint16_t length)
* Visibility  :        public
* Description :        Read the assigned length data from the specified address,The read data store in the memory pointed by 
                       the pointer  
* Parameters  :        uint32_t addr, uint8_t *data, uint16_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CEeprom_Read_Rww(uint32_t addr, uint8_t *data, uint16_t length)
{
    #if (FLASH_MODULE == 1)
    Flash_If_Read_RWW(addr, data, length);
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_Write_Rww(uint32_t addr, uint8_t *data, uint16_t length)
* Visibility  :        public
* Description :        Writes assigned length data pointed by a pointer to a specified storage area  
* Parameters  :        uint32_t addr, uint8_t *data, uint16_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CEeprom_Write_Rww(uint32_t addr, uint8_t *data, uint16_t length)
{
    #if (FLASH_MODULE == 1)
    Flash_If_Write_RWW(addr, data, length);
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        crc32_calc(uint32_t *const data, const uint32_t len, uint32_t *pcrc)
* Visibility  :        private
* Description :        32bit CRC check  
* Parameters  :        uint32_t *const data, const uint32_t len, uint32_t *pcrc
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int32_t 
* ===================================================================================================================*/
static int32_t crc32_calc(uint32_t *const data, const uint32_t len, uint32_t *pcrc)/*used to crc check the data of specified length*/
{
    int32_t  rc = 0;
    uint32_t datatmp;
    if ((0x00000003 & ((uint32_t)data))) 
    {
        /* Address must be align with 4 bytes, refer to datasheet */
        return -1;
    }
    else
    {/*do nothing*/}
    /*periph unlock*/
    ((Pac *)PAC)->WRCTRL.reg = PAC_WRCTRL_PERID(33) | PAC_WRCTRL_KEY_CLR;
   
    datatmp = ((Dsu *)DSU)->LENGTH.reg;
    datatmp &= ~DSU_LENGTH_LENGTH_Msk;
    datatmp |= DSU_LENGTH_LENGTH(len);
    ((Dsu *)DSU)->LENGTH.reg = datatmp;
    ((Dsu *)DSU)->ADDR.reg = (uint32_t)data;				
    ((Dsu *)DSU)->DATA.reg = (*pcrc);
    ((Dsu *)DSU)->CTRL.reg = DSU_CTRL_CRC;
		
    while(((((Dsu *)DSU)->STATUSA.reg & DSU_STATUSA_DONE) >> DSU_STATUSA_DONE_Pos) == 0);
    if((((Dsu *)DSU)->STATUSA.reg & DSU_STATUSA_BERR) >> DSU_STATUSA_BERR_Pos)
    {
        ((Dsu *)DSU)->STATUSA.reg = DSU_STATUSA_BERR;
        ((Dsu *)DSU)->STATUSA.reg = DSU_STATUSA_DONE;
        rc = -6;
    }
    else
    {
        *pcrc = (uint32_t)(((Dsu *)DSU)->DATA.reg);
    }
    ((Dsu *)DSU)->STATUSA.reg = DSU_STATUSA_DONE;
    /*periph lock*/ 
    ((Pac *)PAC)->WRCTRL.reg = PAC_WRCTRL_PERID(33) | PAC_WRCTRL_KEY_SET;
 
    return rc;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        memcpy_w(uint32_t* pDst, uint32_t* pSrc, uint32_t size)
* Visibility  :        private
* Description :        32bit word memcpy function 
* Parameters  :        uint32_t* pDst, uint32_t* pSrc, uint32_t size
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void memcpy_w(uint32_t* pDst, uint32_t const * pSrc, uint32_t size)/*Copies the specified length of 32bits source data to the target data*/ 
{
    uint32_t* pd = pDst;
    uint32_t const * ps = pSrc;
    uint32_t  s  = size;

    while(0u != s) /*check size is 0 or not, if 0 ,exit the while loop*/
    {
        *pd = *ps;
        pd++;
        ps++;
        s--;
    }
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        convert_page_addr(uint32_t page)
* Visibility  :        private
* Description :        calculate address of assigned page
* Parameters  :        uint32_t page
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint32_t*  the first address of the page
* ===================================================================================================================*/
static uint32_t* convert_page_addr(uint32_t page)
{
    uint32_t* address = NULL;
    
    if (NVMCTRL_PAGE_HW > page)
    {
        address = (uint32_t*)(NVMCTRL_RWW_EEPROM_ADDR + (page*NVMCTRL_PAGE_SIZE));      /*64 bytes each page*/
    }
    else
    {/*do nothing*/}
    return address; /*return page address*/
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        validate_header(header_t header, uint8_t id)
* Visibility  :        static
* Description :        judge header is validate or not 
* Parameters  :        header_t header, uint8_t id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool  
* ===================================================================================================================*/
static bool validate_header(header_t header, uint8_t id)
{
    return ((RECORD_MARKER == header.asData.marker ) &&
            (header.asData.rec_id == id)             &&
            (0u != header.asData.flash_cnt)           && 
            (MAX_PAGE_FLASH_CNT >= header.asData.flash_cnt));   /*check flash operation reach the upper limit or not*/
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        prepare_header(uint32_t flash_cnt, uint8_t rec_id)
* Visibility  :        private
* Description :        prepare header of assigned rec_id
* Parameters  :        uint32_t flash_cnt, uint8_t rec_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void prepare_header(uint32_t flash_cnt, uint8_t rec_id)
{
    header_t headerL;
    
    headerL.asData.flash_cnt = flash_cnt;
    headerL.asData.marker = RECORD_MARKER;
    headerL.asData.rec_id = rec_id;
    page_buff[0][HEADER_POSITION] = headerL.asWord;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        inc_flash_cnt(uint32_t* cnt)
* Visibility  :        private
* Description :        flash operation count plus 1 function
* Parameters  :        uint32_t* cnt
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void inc_flash_cnt(uint32_t* cnt)
{
    uint32_t flash_cntL = *cnt;
    /*judge flash_cntL over MAX_PAGE_FLASH_CNT or not*/
    flash_cntL++;
    if (MAX_PAGE_FLASH_CNT < flash_cntL)
    {
        flash_cntL = MAX_PAGE_FLASH_CNT;
    }
    else
    {/*do nothing*/}
    *cnt = flash_cntL;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        blank_check_page(uint32_t page)
* Visibility  :        private
* Description :        Check whether the specified page is blank
* Parameters  :        uint32_t page
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool  
* ===================================================================================================================*/
static bool blank_check_page(uint32_t page)
{
    bool blank = true;
    if (NVMCTRL_RWWEE_PAGES > page)
    {
        uint32_t const *rww_addr = convert_page_addr(page);
        uint32_t i=0;
        do {
            if (0xFFFFFFFFu != rww_addr[i]) /*check the data in eeprom of specified page is 0xFFFFFFFF or not*/
            {
                blank = false;
            }
            else                             /*the data is 0xFFFFFFFF, means blank*/
            {/*do nothing*/}
            i++;
        } while((NVMCTRL_PAGE_W > i) && (true == blank));
    }
    else
    {/*do nothing*/}
    return blank;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        blank_check_row(uint32_t row)
* Visibility  :        private
* Description :        Check whether the specified row is blank
* Parameters  :        uint32_t row
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool  
* ===================================================================================================================*/
static bool blank_check_row(uint32_t row)
{
    bool blank = true;
    if (INVALID_ROW > row)
    {
        uint32_t start_page = row*NVMCTRL_ROW_PAGES;
        uint32_t page = 0;
        do {
            blank = blank_check_page(start_page + page);/*check the specified page is blank or not*/
            page++;
        } while((NVMCTRL_ROW_PAGES > page) && (true == blank));
    }
    else
    {/*do nothing*/}
    return blank;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        execute_command(uint32_t command, uint32_t rww_addr)
* Visibility  :        private
* Description :        execute operation command of assigned eeprom address.
* Parameters  :        uint32_t command, uint32_t rww_addr
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void execute_command(uint32_t command, uint32_t rww_addr)
{ 
    NVMCTRL->ADDR.reg = (rww_addr / 2u);     /*calculate ddress*/
    NVMCTRL->CTRLA.reg =(command | NVMCTRL_CTRLA_CMDEX_KEY);   /*operation command*/
    while(!(((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_READY) >> NVMCTRL_INTFLAG_READY_Pos);
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        write_page(uint32_t* buff, uint32_t page) 
* Visibility  :        private
* Description :        write data in buffer to assigned eeprom page address  
* Parameters  :        uint32_t* buff, uint32_t page
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void write_page(uint32_t* buff, uint32_t page)     
{
    if ((NVMCTRL_RWWEE_PAGES > page) && (NULL != buff))
    {
        uint32_t* rww_addr = (uint32_t*)convert_page_addr(page);/*get first address of specified page*/
        
        while(!((((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_READY) >> NVMCTRL_INTFLAG_READY_Pos));
        NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;
        
        memcpy_w(rww_addr, buff, NVMCTRL_PAGE_W); /*copy the data in buff to eeprom*/       
        while(!((((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_READY) >> NVMCTRL_INTFLAG_READY_Pos));			
        execute_command(NVMCTRL_CTRLA_CMD_RWWEEWP, (uint32_t)rww_addr);     /*commit page buffer to rww memory*/  
    }
    else
    {/*do nothing*/}      
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        execute_command_int(uint32_t command, uint32_t rww_addr)   
* Visibility  :        private
* Description :        Starting of command and exit without waiting, end of command is handled in interrupt context
* Parameters  :        uint32_t command, uint32_t rww_addr
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void execute_command_int(uint32_t command, uint32_t rww_addr)   
{
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;
    NVMCTRL->ADDR.reg   = (rww_addr / 2);
    NVMCTRL->CTRLA.reg  = (command | NVMCTRL_CTRLA_CMDEX_KEY);
    if(!((((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_READY) >> NVMCTRL_INTFLAG_READY_Pos))
    {    
        ((Nvmctrl *)NVMCTRL)->INTENSET.reg = NVMCTRL_INTENSET_READY;			
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        fill_page(uint32_t page, uint32_t cnt_page) 
* Visibility  :        private
* Description :        write the data in page_buff to specified page
* Parameters  :        uint32_t page, uint32_t cnt_page
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void fill_page(uint32_t page, uint32_t cnt_page)      
{
    if ((NVMCTRL_RWWEE_PAGES > page) && (NVMCTRL_ROW_PAGES > cnt_page))
    {
        /*get the first address of specified page*/        
        uint32_t* rww_addr = (uint32_t*)(NVMCTRL_RWW_EEPROM_ADDR + (page+cnt_page)*NVMCTRL_PAGE_SIZE);  
        /*copy the data in page_buff to eeprom*/              
        memcpy_w(rww_addr, &page_buff[cnt_page][0], NVMCTRL_PAGE_W);
        execute_command_int(NVMCTRL_CTRLA_CMD_RWWEEWP, (uint32_t)rww_addr);
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        erase_row(const uint32_t row)
* Visibility  :        private
* Description :        erase eeprom of assigned row
* Parameters  :        const uint32_t row
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void erase_row(const uint32_t row)  
{ 
    if (INVALID_ROW > row)
    {
        while(!((((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_READY) >> NVMCTRL_INTFLAG_READY_Pos));
        NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;
        /*get first address of specified row*/
        uint32_t rww_addr = (uint32_t)(NVMCTRL_RWW_EEPROM_ADDR + row*NVMCTRL_ROW_SIZE);
        execute_command(NVMCTRL_CTRLA_CMD_RWWEEER, rww_addr); /*erase eeprom of specified row*/
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        write_group(group_property_t* pGroup) 
* Visibility  :        private
* Description :        Write the data in the array page_buff to specified EEPROM
* Parameters  :        const uint32_t row
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void write_group(group_property_t* pGroup)  
{
    uint32_t sizeL = pGroup->numb_pages * NVMCTRL_PAGE_W - DATA_POSITION; /*get size of specified group*/
    page_buff[0][CRC_POSITION] = 0;
    crc32_calc(&page_buff[0][DATA_POSITION], sizeL, &page_buff[0][CRC_POSITION]);/*crc check*/
    inc_flash_cnt((uint32_t*)&groups[pGroup->rec_id].flash_cnt);
    prepare_header(groups[pGroup->rec_id].flash_cnt, pGroup->rec_id);

    uint32_t numb_rows;
    if (NVMCTRL_ROW_PAGES >= pGroup->numb_pages)
    {
        numb_rows = 1;
    }
    else
    {
        numb_rows = 2;
    }
    uint32_t row = pGroup->start_page/NVMCTRL_ROW_PAGES;
    for (; row < numb_rows; row++)
    {
        if (0 == blank_check_row(row))  /*erase the eeprom of specified row before write*/
        {
            erase_row(row);
        }
        else
        {/*do nothing*/}
    }
    
    /*write pages*/
    for (int i=0; i < pGroup->numb_pages; i++)
    {
        write_page(&page_buff[i][0], pGroup->start_page+i);/*write the data in page_buff to eeprom*/
    }    
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        init_rows(void)
* Visibility  :        private
* Description :        init rows status to spare state.
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint8_t  
* ===================================================================================================================*/
static void init_rows(void)
{
    uint32_t row_id;
    for (row_id=0; row_id < NUMB_ROWS_DOUBLE; row_id++)
    {
        rows[row_id] = ROW_STATE_SPARE;  /*init all rows to spare*/
    }
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        prepare_spare_rows(void)
* Visibility  :        private
* Description :        prepare spare rows, erase the data in spare rows.
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void prepare_spare_rows(void)
{
    uint32_t row_id;
    for (row_id=0; row_id < NUMB_ROWS_DOUBLE; row_id++)
    {
        if (ROW_STATE_SPARE == rows[row_id]) /*find spare row in eeprom*/
        {
            if (0 == blank_check_row(START_ROW_DOUBLE+row_id))
            {
                erase_row(START_ROW_DOUBLE+row_id);/*erase the data in spare row*/
            }
            else
            {/*do nothing*/}
        }
        else
        {/*do nothing*/}
    }
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        load_double_group(group_property_t* pGroup)
* Visibility  :        private
* Description :        load double group data
* Parameters  :        group_property_t* pGroup
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void load_double_group(group_property_t* pGroup)
{
    header_t headerL;
    uint32_t* rww_addr;
    uint32_t flash_cntL = 0;
    uint32_t start_pageL = 0;
    uint32_t crcL;
    uint32_t sizeL;
    
    sizeL = pGroup->numb_pages * NVMCTRL_PAGE_W - DATA_POSITION;
    for (uint32_t row=START_ROW_DOUBLE; row < (START_ROW_DOUBLE+NUMB_ROWS_DOUBLE); row++)
    {
        /*iterate rows*/
        uint32_t next_page = row*NVMCTRL_ROW_PAGES;
        while(row == next_page/NVMCTRL_ROW_PAGES)
        {
            /*iterate records into row*/
            rww_addr = (uint32_t*)convert_page_addr(next_page);
            headerL.asWord = rww_addr[HEADER_POSITION];
            if (validate_header(headerL, pGroup->rec_id))
            {
                if (flash_cntL < headerL.asData.flash_cnt)
                {
                    crcL = 0;
                    crc32_calc(rww_addr+DATA_POSITION, sizeL, &crcL);
                    if (crcL == rww_addr[CRC_POSITION])
                    {
                        flash_cntL = headerL.asData.flash_cnt;
                        start_pageL = next_page;
                    }
                    else
                    {/*do nothing*/}
                }
                else
                {/*do nothing*/}                  
            }
            else
            {
                if (0 == (next_page%NVMCTRL_ROW_PAGES))
                {
                    /*the page is first of the row, no need to check the rest*/
                    break;  /*go to next row*/
                }
                else
                {/*do nothing*/}
            }
            next_page += pGroup->numb_pages;
        }
    }

    uint32_t group_id = pGroup->rec_id;
    groups[group_id].start_page = start_pageL;
    if (0 != start_pageL)
    {
        /*load record into RAM buffer */       
        groups[group_id].postponed = false;
        groups[group_id].start_page = start_pageL;
        groups[group_id].flash_cnt = flash_cntL;
        rww_addr = (uint32_t*)convert_page_addr(start_pageL);
        memcpy_w(pGroup->pData, rww_addr, sizeL);
        rows[(start_pageL/NVMCTRL_ROW_PAGES)-START_ROW_DOUBLE] = ROW_STATE_USED;   /*mark row as used by the group*/
    }
    else
    {
        memcpy_w(pGroup->pData, pGroup->pDefaultData, sizeL);
        groups[group_id].postponed = true;
        groups[group_id].start_page = INVALID_PAGE;
    }
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        verify_single_group(group_property_t* pGroup)
* Visibility  :        private
* Description :        Determine whether the CRC of the read data is valid. If not, write the default data
* Parameters  :        group_property_t* pGroup
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
static void verify_single_group(group_property_t* pGroup)
{
    uint32_t* rww_addr;
    static uint32_t  crc=0;
    header_t  headerL;

    uint32_t sizeL = pGroup->numb_pages * NVMCTRL_PAGE_W - DATA_POSITION;
    rww_addr = convert_page_addr(pGroup->start_page);
    /*do not load data to RAM buffer, just check for validity */
    headerL.asWord = rww_addr[HEADER_POSITION];
    
    crc32_calc(rww_addr+DATA_POSITION, sizeL, &crc);
    
    if ((crc == rww_addr[CRC_POSITION]) && (validate_header(headerL, pGroup->rec_id)))
    {
        /*the group is valid*/
        groups[pGroup->rec_id].flash_cnt = headerL.asData.flash_cnt;
    }
    else
    {
        /*write default data*/
        memcpy_w(&page_buff[0][DATA_POSITION], pGroup->pDefaultData, sizeL);
        groups[pGroup->rec_id].flash_cnt = 0;
        write_group(pGroup);
    }  
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_Write_Factory_Config(Factory_Config *pGroup1)  
* Visibility  :        public 
* Description :        Write factory config data to eeporm
* Parameters  :        Factory_Config *pGroup1
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool  
* ===================================================================================================================*/
void CEeprom_Write_Factory_Config(Factory_Config *pConfig) 
{
    uint16_t crc_result;
    group_property_t* pGroup_property;
    pGroup_property = (group_property_t*)get_group_property(GROUP_1_ID);
    crc_result = CRC16_CCITT(((uint8_t *)pConfig+2), sizeof(*pConfig));   
    pConfig->data.CheckSum[0] = (uint8_t)crc_result;
    pConfig->data.CheckSum[1] = (uint8_t)(crc_result >> 8); 
    
    memcpy_w(&page_buff[0][DATA_POSITION], (uint32_t*)pConfig, sizeof(*pConfig)/4);
   
    write_group(pGroup_property);
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_Read_Factory_Config(void) 
* Visibility  :        public 
* Description :        Read factory config data from eeporm
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  Factory_Config  
* ===================================================================================================================*/
Factory_Config CEeprom_Read_Factory_Config(void)     
{
    Factory_Config pConfig_Init;
    group_property_t* pGroup = (group_property_t*)get_group_property(0);          /*get eeprom address of group1*/
    memcpy_w(pConfig_Init.asWord,pGroup->pData+2,NVMCTRL_PAGE_W*GROUP1_PAGES-2);  /*write group1 data to pConfig_Init*/
    return pConfig_Init;
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        
* Visibility  :        public 
* Description :        Get first address of specified group
* Parameters  :        uint32_t group_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void*  
* ===================================================================================================================*/
void* CEeprom_Get_data_addr(uint32_t group_id)
{
    void* pDataL = NULL;
    group_property_t* pGroup;
    pGroup = (group_property_t*)get_group_property(group_id);
    if (NULL != pGroup)
    {
        pDataL = pGroup->pDefaultData;
    }
    else
    {/*do nothing*/}
    return pDataL;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_Get_Factory_Config(void)
* Visibility  :        public 
* Description :        Get current Factory Config data.
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  Factory_Config*  
* ===================================================================================================================*/
Factory_Config* CEeprom_Get_Factory_Config(void)
{
    return &pConfig_Init;    /*return Factory Config data get form CEeprom_Init function*/
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        
* Visibility  :        
* Description :        Counter plus 1
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void*  
* ===================================================================================================================*/
void CEeprom_Counter_Inc(uint8_t * pr)
{
    /*Value: 0~65535*/
    if(pr[0]==0xFF)
    {
        pr[0]=0;
        if(pr[1]!=0xFF)
        {
           pr[1]++; 
        }
    }
    else
    {
        pr[0]++;
    }  
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        
* Visibility  :        
* Description :        Counter plus 1
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void*  
* ===================================================================================================================*/

void CEeprom_Power_Counter_Inc(void)
{
    CEeprom_Counter_Inc(pConfig_Init.data.Powerup_Counter);
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        
* Visibility  :         
* Description :        Reset_Counter plus 1
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void*  
* ===================================================================================================================*/
void CEeprom_Reset_Counter_Inc(void)
{
    CEeprom_Counter_Inc(pConfig_Init.data.Reset_Counter);
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        CEeprom_Init(void)
* Visibility  :        public 
* Description :        eeprom init function
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
void CEeprom_Init(void)
{
    #if (FLASH_MODULE == 1)
    Flash_If_Init();   /*config eeprom*/
    #endif
    group_property_t* pGroup;
    uint32_t group_id = GROUP_1_ID;
    init_rows();  
    pGroup = (group_property_t*)get_group_property(group_id);
    uint32_t sizeL;
    if (NULL != pGroup)
    {
        sizeL = pGroup->numb_pages * NVMCTRL_PAGE_W - DATA_POSITION;
        if (REC_STRATEGY_SINGLE == pGroup->rec_strategy)
        {
            verify_single_group(pGroup); /*get data with single group from eeprom*/
        }
        else if (REC_STRATEGY_DOUBLE == pGroup->rec_strategy)
        {
            load_double_group(pGroup);   /*get data with double group from eeprom*/
        }
        else
        {/*do nothing*/}
        memcpy_w((uint32_t*)(&pConfig_Init),convert_page_addr(pGroup->start_page)+DATA_POSITION,sizeL);
        eep_group1_ID_t* pr = (eep_group1_ID_t*)(pGroup->pDefaultData);
        for(uint8_t cn=0;cn<2;cn++)
        {
            pConfig_Init.data.HW_Version[cn] =pr->data.HW_Version[cn];
            pConfig_Init.data.Firmware_Version[cn] = pr->data.Firmware_Version[cn];          
        }            
        for(uint8_t cn=0;cn<8;cn++)
        {
    	    pConfig_Init.data.Software_Part[cn] =pr->data.Software_Part[cn];
            pConfig_Init.data.Hardware_Part[cn] = pr->data.Hardware_Part[cn];          
        } 
        pConfig_Init.data.Display_ID = pr->data.Display_ID;
        pConfig_Init.data.Touch_Controller_ID = pr->data.Touch_Controller_ID;
        prepare_spare_rows();   
        CEeprom_Reset_Counter_Inc();
        CEeprom_Write_Factory_Config(&pConfig_Init); 
    }
    else
    {/*do nothing*/}       
 
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/EEPROM/App_eeprom
* Method      :        NVMCTRL_Handler(void)
* Visibility  :        public 
* Description :        NVM Interrupt handler
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void  
* ===================================================================================================================*/
void NVMCTRL_Handler(void)
{
    static interrupt_obj_t interrupt_obj;                               /*interrupt operation information*/
    if((((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_ERROR) >> NVMCTRL_INTFLAG_ERROR_Pos)
    {
        ((Nvmctrl *)NVMCTRL)->INTFLAG.reg = NVMCTRL_INTFLAG_ERROR;
        return;
    }
    else
    {/*do nothgin*/}
    
    if((((Nvmctrl *)NVMCTRL)->INTFLAG.reg & NVMCTRL_INTFLAG_READY) >> NVMCTRL_INTFLAG_READY_Pos)
    {
        ((Nvmctrl *)NVMCTRL)->INTENCLR.reg = NVMCTRL_INTENSET_READY;
        if (IDLE != interrupt_obj.state)
        {
            bool end_activityL = false;
            if (WRITE_PAGE == interrupt_obj.state)
            {
                if (interrupt_obj.cnt_page < interrupt_obj.numb_pages)
                {
                    fill_page(interrupt_obj.start_page, interrupt_obj.cnt_page++);
                }
                else
                {
                    /*all pages are written to RWW*/
                    if (INVALID_ROW != interrupt_obj.row)
                    {
                        uint32_t rww_addr = (uint32_t)(NVMCTRL_RWW_EEPROM_ADDR + (uint32_t)interrupt_obj.row*NVMCTRL_ROW_SIZE);
                        execute_command_int(NVMCTRL_CTRLA_CMD_RWWEEER, rww_addr);
                        interrupt_obj.state = ERASE_ROW;
                    }
                    else
                    {
                        /*no need to erase row*/
                        end_activityL = true;
                    }
                }
            }
            else if (ERASE_ROW == interrupt_obj.state)
            {
                /*erasing is finished*/
                end_activityL = true;
            }
            else
            {
                /*for safety*/
                end_activityL = true;
            }
        
            if (0 != end_activityL)
            {
                interrupt_obj.state = IDLE;
                /*unlock page_buffer*/                   
            }
            else
            {/*do nothing*/}
        }
        else
        {/*do nothing*/}          
    }
    else
    {/*do nothing*/}
}
#endif /*EEPROM_MODULE*/
