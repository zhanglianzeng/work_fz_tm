/******************************************************************************
*  Name: buffer.c
*  Description:
*  Project: INFO4 11inch
*  Auther: LvXuDong
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../config.h"
#if (BUFFER_COM == 1)

#include <stdlib.h>
#include "buffer.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_Init(pBuffer buffer, pBufferData data, uint16_t length)
* Visibility  :        public
* Description :        initialize buffer queue
* Parameters  :        pBuffer buffer, pBufferData data, uint16_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool Buffer_Init(pBuffer buffer, pBufferData data, uint16_t length)
{
    if((NULL == buffer) || (NULL == data))
    {
        return false;
    }
    else
    {
        buffer->data = data;
        buffer->front = buffer->rear = 0;
        buffer->buffer_length = length;
        return true;
    }
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_IsEmpty(pBuffer buffer)
* Visibility  :        public
* Description :        judge buffer queue is empty or not
* Parameters  :        pBuffer buffer
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool Buffer_IsEmpty(pBuffer buffer)
{
    if(buffer->front == buffer->rear)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_IsFull(pBuffer buffer)
* Visibility  :        public
* Description :        judge buffer queue is full or not
* Parameters  :        pBuffer buffer
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool Buffer_IsFull(pBuffer buffer)
{
    if((buffer->rear + 1) % buffer->buffer_length == buffer->front)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_Length(pBuffer buffer)
* Visibility  :        public
* Description :        calculate and return the length of the queue that is not out of the queue
* Parameters  :        pBuffer buffer
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  uint16_t 
* ===================================================================================================================*/
uint16_t Buffer_Length(pBuffer buffer)
{
    if(buffer->front <= buffer->rear)
    {
        return buffer->rear - buffer->front;
    }
    else
    {
        return buffer->buffer_length - (buffer->front - buffer->rear);
    }
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_Insert(pBuffer buffer, BufferData value)
* Visibility  :        public
* Description :        If buffer is not full, insert assigned value to buffer queue; or return false
* Parameters  :        pBuffer buffer, BufferData value
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool Buffer_Insert(pBuffer buffer, BufferData value)
{
    if(Buffer_IsFull(buffer))
    {
        return false;
    }
    else
    {
        buffer->rear = (buffer->rear + 1) % buffer->buffer_length;
        buffer->data[buffer->rear] = value;
        return true;
    }
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_Pop(pBuffer buffer)
* Visibility  :        public
* Description :        Pop data from buffer queue
* Parameters  :        pBuffer buffer
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  BufferData 
* ===================================================================================================================*/
BufferData Buffer_Pop(pBuffer buffer)
{
    buffer->front = (buffer->front + 1) % buffer->buffer_length;
    return buffer->data[buffer->front];
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_Traverse(pBuffer buffer, pBufferData value, uint16_t *length)
* Visibility  :        public
* Description :        
* Parameters  :        pBuffer buffer, pBufferData value, uint16_t *length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  bool 
* ===================================================================================================================*/
bool Buffer_Traverse(pBuffer buffer, pBufferData value, uint16_t *length)
{
    uint16_t current = buffer->front;
    uint16_t index = 0;

    if(0 != Buffer_IsEmpty(buffer))
    {
        return false;
    }
    else
    {/*do nothing*/}

    while(current != buffer->rear)
    {
        current++;
        current = current % buffer->buffer_length;
        value[index++] = buffer->data[current];
    }
    *length = index;

    return true;
}

/*=====================================================================================================================
* Upward trac.:        COMMON/buffer
* Method      :        Buffer_Clear(pBuffer buffer)
* Visibility  :        public
* Description :        Clear buffer index  
* Parameters  :        pBuffer buffer
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void Buffer_Clear(pBuffer buffer)
{
    buffer->front = buffer->rear = 0;
}

#endif /* BUFFER_COM */
