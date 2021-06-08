/******************************************************************************
*  Name: app_iic_comm.h
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (DESERIALIZER_MODULE == 1)

#ifndef APP_IIC_COMM_H
#define APP_IIC_COMM_H

#include <stdint.h>
#include "../../common/buffer.h"
#include "../../common/crc.h"

#if (EEPROM_MODULE == 1)
#include "../eeprom/app_eeprom.h"
#endif

#if (DESERIALIZER_MODULE == 1)
#include "../../middle/deserializer/deserializer_if.h"
#endif
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define CMD_READ_LEN                  70      /*The maximum length of the read command data buffer*/
#define CMD_NUM                       18      /*deduct 2 ETM commands*/

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef struct
{
    VCU_CMD command;                                            /*i2c command id*/
    uint8_t data_len_rd;                                        /*request command data length read from master*/
    uint8_t data_len_wr;                                        /*the length of data response to master*/
    void (*CommandHandle)(Elementype cmd_data,uint8_t index);   /*executed function pointer*/
}COMMANDHANDLE,*COMMANDHANDLE_P;

typedef void (*CI2CComm_Dimming_Callback)(uint16_t);

typedef enum
{
    CRCNORMAL_MODE,
    CRCERR_MODE
}ENUM_CRCMODE;

typedef struct
{
    ENUM_CRCMODE enumCrcMode;
    uint8_t byCrcErr_Count;
    uint8_t byCrcSuc_Count;
}STRUDIM_CRC;

typedef enum
{
    DISPLAY_ID_NOMAL_MODE,
	DISPLAY_ID_ERR_MODE
}ENUM_DISPLAY_ID_MODE;

typedef struct
{
   ENUM_DISPLAY_ID_MODE enumDisplayIdMode;
   uint8_t byDisplayIdErr_Count;
   uint8_t byDisplayIdSuc_Count;   
}STRUDIM_DISPLAY_ID;
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
void CI2CComm_Handle(Elementype cmd_data);
void CI2CComm_GetHardVersion(Elementype cmd_data, uint8_t index);
void CI2CComm_GetFirmwareVersion(Elementype cmd_data, uint8_t index);
void CI2CComm_BacklightSetDuty(Elementype cmd_data,uint8_t index);
void CI2CComm_GetTouchId(Elementype cmd_data,uint8_t index);
void CI2CComm_GetDisplayId(Elementype cmd_data,uint8_t index);
void CI2CComm_GetPowerupCounter(Elementype cmd_data, uint8_t index);
void CI2CComm_GetResetCounter(Elementype cmd_data, uint8_t index);
void CI2CComm_GetBootloaderStatus1(Elementype cmd_data, uint8_t index);
void CI2CComm_GetDiagnosticStatus(Elementype cmd_data, uint8_t index);
void CI2CComm_GetDiagnosticStatus1(Elementype cmd_data, uint8_t index);
void CI2CComm_ResetSystem(Elementype cmd_data, uint8_t index);
void CI2CComm_Dim_Request(Elementype cmd_data, uint8_t index);
void CI2CComm_GetHardwareNumber(Elementype cmd_data, uint8_t index);
void CI2CComm_GetSoftwareNumber(Elementype cmd_data, uint8_t index);
#if (EEPROM_MODULE == 1)
void CI2CComm_GetConfig(Factory_Config ptemp);
#endif
void CI2CComm_WriteData(Elementype cmd_data, uint8_t index);
void CI2CComm_ReadData(Elementype cmd_data, uint8_t index);
void CI2CComm_ConfigAddr(Elementype cmd_data, uint8_t index);

extern void CI2CComm_Init(void);
extern void CI2CComm_Task(void);
extern void CI2CComm_PushDim_CallBack(CI2CComm_Dimming_Callback pCallback); 

#endif /* _APP_IIC_COMM_H_ */
#endif /* DESERIALIZER_MODULE */
