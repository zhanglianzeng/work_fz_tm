/******************************************************************************
*  Name: deserializer_if.h
*  Description:
*  Project: INFO4 11inch
*  Auther: LvXuDong
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (DESERIALIZER_MODULE == 1)

#ifndef DESERIALIZER_IF_H
#define DESERIALIZER_IF_H

#include <stdbool.h>
#include <stdint.h>
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
#define PDB_DES             GPIO(GPIO_PORTC, 27)   
#define LOCK_DES            GPIO(GPIO_PORTC, 26)

#define DES_SDA1            GPIO(GPIO_PORTB, 16)     
#define DES_SCL1            GPIO(GPIO_PORTB, 17)
#define CTP_SDA             GPIO(GPIO_PORTA, 16)     
#define CTP_SDL             GPIO(GPIO_PORTA, 17)
#define DES_SERCOM          SERCOM5
#define CTP_SERCOM          SERCOM1
#define DES_ADDRESS         0X2D

#if (ETM_MODULE == 1)
#define ETM_WRITE_CMD       0x3B
#define ETM_READ_CMD        0x7B
#endif

#if (ETM_MODULE == 1)
#define COMMAND_TABLE_LENTH 20
#else
#define COMMAND_TABLE_LENTH 18
#endif

#define COMMAND_LENGTH      20u
#define BUFFER_LENGTH       10u

#define BOOT_ACCESS_CMD     0x44
#define EEPROM_WRITE_CMD    0xB2
#define EEPROM_CMD_HEAD     5

/*deserializer 988 patgen macros*/
#define INIT_LEN            2
#define PATGEN_IA           0x02
#define PATGEN_ID           0x03
#define RETRY_NUM           0x03
#define DES_I2C_TEST        0     /*1: open readback test, 0: closed readback test*/
//#define DES_LOCAL_CONFIG    1     /*1: 984 is initialized locally, 0: 983 send param to init 988 remotely*/
#define DAISY_CHAIN         0     /*1: open daisy chain when DES_LOCAL_CONFIG is 1, 0: closed*/
#define DES_PATTERN_MODE    0     /*1: 984 self generate pattern picture, 0: 983 remotely generate pattern picture*/
#define TIME_DELAY          0x1ff /*time delay flag, indicate followed data is delay time*/
#define REV_ID              0

/*988 self pattern generate*/
#if (REV_ID == 1)
/*Video parameters - 2000x810 when Revision ID = 0(REV_ID)*/
#define THW                 (2084*2)
#define TVW                 842
#define AHW                 (2000*2)
#define AVW                 810
#define HBP                 (32*2)
#define VBP                 24
#define HSW                 (16*2)
#define VSW                 2
#else
/*Video parameters - 2000x810 when Revision ID = 1(Production A0)*/
#define THW                 2084
#define TVW                 842
#define AHW                 2000
#define AVW                 810
#define HBP                 32
#define VBP                 24
#define HSW                 16
#define VSW                 2
#endif

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/  
typedef int (*IIC_Slave_Command_Callback)(uint8_t);		/*!< I2C call back function */
typedef int (*IIC_Slave_Etm_Callback)(Elementype);		/*!< I2C call back function */
typedef void (*IIC_Command_Parsing_Callback)(Elementype cmd_data);

typedef struct
{
    uint16_t cmd_id;
    uint8_t  cmd_data_len;
} CMD_DATA;

typedef struct
{
    uint8_t *   pbuffer;
    uint16_t    buffer_len;
    uint16_t    por;
}SComm_Data;

typedef enum I2C_CMD_ID {
    HWVersion_ID=0x00,
    FWVersion_ID=0x02,
    Touch_ID=0x04,
    Display_ID,
    SwitchPack_ID,
    Vol_Data_ID=0x10,
    Tune_Data_ID,
    SwitchPack_Press_ID,
    Power_Press_ID,
    DiagStatus_ID=0x16,
    DiagStatus1_ID=0x1C,
    PWM_ID=0x20,
    ResetCounter_ID=0x2D,
    PowerupCounter_ID=0x2F,
    Reset_Mcu_ID=0x31,
    Boot_ID=0x44,
    Dim_Req_ID=0x56,
    SW_Num_ID=0x60,
    HW_Num_ID=0x68,
    Boot_Status1_ID=0x81,
    Config_Addr_ID=0xB0,
    ROM_Read_ID,
    ROM_Write_ID,
    ETM_Write_ID=0x3B,
    ETM_Mode_ID=0x70,
    ETM_Read_ID=0x7B,
    I2C_CMD_MAX,
}VCU_CMD;

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Des_If_Init(void);
extern void Des_If_GPIO_Init(void);
extern void Des_If_Slave_Init(void);
extern void Des_If_Standby(void);
extern void Des_If_Power_Off(void);
extern void Des_If_Power_On(void);
extern void Des_If_Set_PDB_Level(bool level);
extern bool Des_If_Get_Lock_Level(void);
extern void Des_If_Set_Ready_Flag(void);
extern uint8_t Des_If_Get_Key_Flag(void);
extern void Des_If_Clear_Key_Flag(void);
extern uint16_t Des_If_Get_Key_Value(void);
extern bool Des_If_Get_Lock_Level(void);
extern Buffer *Des_If_Get_Queue_Buffer(void);
extern void Des_If_Set_Data_Buffer(void *pBuffer, uint16_t Buffer_Len);
extern void Des_If_GetCmd_CallBack(IIC_Slave_Command_Callback pCallback); 
extern void Des_If_GetEtm_CallBack(IIC_Slave_Etm_Callback pCallback);
extern void Des_If_Slave_DeInit(void);
extern void Des_If_Master_Init(void);
extern void Des_If_Master_DeInit(void);
extern void Des_If_Register_Config(void);
extern void Des_If_I2C_Parsing_CallBack(IIC_Command_Parsing_Callback pCallback);
extern void Des_If_Lock_TimeCheck(void);


#endif /* DESERIALIZER_IF_H */
#endif /* DESERIALIZER_MODULE */
