/******************************************************************************
*  Name: app_eeprom.h
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (EEPROM_MODULE == 1)

#ifndef APP_EEPROM_H
#define APP_EEPROM_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../device/instance/nvmctrl.h"
#include "./eep_data.h"

enum {
    REC_STRATEGY_SINGLE,       /*single strategy*/
    REC_STRATEGY_DOUBLE,       /*double strategy*/
    REC_STRATEGY_NUMB          /*strategy number*/
};

typedef struct
{
    uint32_t commit_delay: 16; /*commit delay, unit ms*/
    uint32_t rec_strategy:  2; /*operation strategy*/
    uint32_t rec_id:        4; /*group ID*/
    uint32_t start_page:    4; /*first address of eerpom, it's OK to be 4-bit(16 pages max), because is going to be used only for 1k area*/
    uint32_t numb_pages:    4; /*number of pages occupied by data*/
    uint32_t* pData;           /*data*/
    uint32_t* pDefaultData;    /*default data*/
}group_property_t;

extern eep_group1_ID_t CEeprom_GetAddr(void);
extern void  CEeprom_Read_Rww(uint32_t addr, uint8_t *data, uint16_t length);
extern void  CEeprom_Write_Rww(uint32_t addr, uint8_t *data, uint16_t length);
extern void* CEeprom_Get_data_addr(uint32_t group_id);
extern bool  CEeprom_Write_single_group(uint32_t group_id,uint32_t *ptr);
extern void CEeprom_Write_Factory_Config(Factory_Config *pGroup1);
extern Factory_Config CEeprom_Read_Factory_Config(void);
extern Factory_Config* CEeprom_Get_Factory_Config(void);
extern void CEeprom_Power_Counter_Inc(void);
extern void  CEeprom_Init(void);

#endif /* _APP_EEPROM_H_ */
#endif /* EEPROM_MODULE */
