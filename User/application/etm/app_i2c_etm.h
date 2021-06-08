/******************************************************************************
*  Name: app_i2c_etm.h
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (ETM_MODULE == 1)

#ifndef APP_I2C_ETM_H
#define APP_I2C_ETM_H

#include <stdint.h>
#include <stddef.h>
#include "../../common/buffer.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define ETM_TABLE_LENTH  12u   //14u             /*etm command number*/
#define ETM_READ_LEN     32u             /*etm response data buffer length*/

#define ETM_REQUEST_ID   0x3Bu           /*etm request command id*/
#define ETM_RESPONSE_ID  0x7Bu           /*etm response command id*/

#define ETM_ENTRY        0x03u           /*command data for enter etm mode*/
#define ETM_DROPOUT      0x0Cu           /*command data for exit etm mode*/

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef struct
{
    uint8_t command;                                          /*etm command id*/
    uint8_t data_len_rd;                                      /*request command data length read from master*/
    uint8_t data_len_wr;                                      /*the length of data response to master*/
    uint8_t data_len_response;                                /*response command data length read from master*/
    void (*EtmHandle)(Elementype etm_data,uint8_t index);     /*executed function pointer*/
}ETMHANDLE,*ETMHANDLE_P;

typedef enum 
{
    NORMAL_MODE,                                              /*normal work mode, etm command invalid*/
    ETM_MODE,                                                 /*etm work mode, etm command valid*/
}ENUM_ETM;

typedef union                                                 /*union type between half word and two bytes*/
{
    uint8_t  abyte[2];
    uint16_t hdata;
}UNION_AULEXY_TO_HALFWORD;
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
void CI2CEtm_GetHardwareVersion(Elementype etm_data, uint8_t index);
void CI2CEtm_GetFirmwareVersion(Elementype etm_data, uint8_t index);
void CI2CEtm_ManufactureTraceability(Elementype etm_data, uint8_t index);
void CI2CEtm_ProgramDate(Elementype etm_data, uint8_t index);
void CI2CEtm_ConfigurationFlag(Elementype etm_data, uint8_t index);
void CI2CEtm_LedDimmingPWMValue(Elementype etm_data, uint8_t index);
void CI2CEtm_ButtonDimmingPWMValue(Elementype etm_data, uint8_t index);
void CI2CEtm_DaisyChain(Elementype etm_data, uint8_t index);
void CI2CEtm_ClearAllDTCs(Elementype etm_data, uint8_t index);
void CI2CEtm_PCBNTCValue(Elementype etm_data, uint8_t index);
void CI2CEtm_LEDNTCValue(Elementype etm_data, uint8_t index);
void CI2CEtm_LCMConfiguration(Elementype etm_data, uint8_t index);
void CI2CEtm_LCMConfigDoubleCheck(Elementype etm_data, uint8_t index);
void CI2CEtm_VersionConfiguration(Elementype etm_data, uint8_t index);
void CI2CEtm_Entry(Elementype etm_data, uint8_t index);
extern void CI2CEtm_ClearEtmMode(void);
extern void CI2CEtm_Init(void);
extern void CI2CEtm_Handle(Elementype etm_data, uint8_t index);
extern void CI2CEtmResponse_Handle(Elementype etm_data, uint8_t index);

#endif /*APP_I2C_ETM_H*/
#endif /*ETM_MODULE */




















