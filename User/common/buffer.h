/******************************************************************************
*  Name: buffer.h
*  Description:
*  Project: INFO4 11inch
*  Auther: LvXuDong
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../config.h"
#if (BUFFER_COM == 1)

#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define CMD_LEN    70

/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef struct
{
    uint8_t data[CMD_LEN];
    uint8_t command_id;
    uint8_t data_len;
}Elementype, *PElementype;

typedef Elementype* pBufferData;
typedef Elementype BufferData;

typedef struct BufferStruct
{
    pBufferData data;
    uint16_t    buffer_length;
    uint16_t    front;
    uint16_t    rear;
} Buffer, *pBuffer;

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern bool Buffer_Init(pBuffer buffer, pBufferData data, uint16_t length);
extern bool Buffer_IsEmpty(pBuffer buffer);
extern bool Buffer_IsFull(pBuffer buffer);
extern bool Buffer_Insert(pBuffer buffer, BufferData value);
extern BufferData Buffer_Pop(pBuffer buffer);

#endif /* _BUFFER_H_ */
#endif /* BUFFER_COM */
