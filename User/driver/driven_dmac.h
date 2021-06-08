/******************************************************************************
*  Name: dmac.h
*  Description:
*  Project: Wave 2
*  Auther: Jun Gao
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#ifndef DRIVEN_DMAC_H
#define DRIVEN_DMAC_H

#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"

/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
typedef void (*dam_memory_cb_t)(void);
/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
enum DMAC_Callback_Type {DMA_TRANSFER_COMPLETE_CB, DMA_TRANSFER_ERROR_CB};
/**********************************************************************************************
* Local types
**********************************************************************************************/
struct DMAC_Resource;
/*DMAC Interrupt Callbacks*/
struct DMAC_Callbacks
{
  void (*transfer_done)(struct DMAC_Resource *resource);
  void (*error)(struct DMAC_Resource *resource);
};

/*DMAC resource structure*/
struct DMAC_Resource
{
  struct DMAC_Callbacks dma_cb;
  void *                 back;
};

/*memory with dma callback type*/
typedef void (*dma_memory_cb_t)(void);

/*memory with DMA descriptor */
struct dma_memory_descriptor;

/*memory with DMA callback types*/
enum dma_memory_callback_type {DMA_MEMORY_COMPLETE_CB, DMA_MEMORY_ERROR_CB};

/*memory with DMA callback*/
struct dma_memory_callbacks
{
  dma_memory_cb_t complete;
  dma_memory_cb_t error;
};

/*memory with DMA descriptor*/
struct dma_memory_descriptor
{
  struct DMAC_Resource *   resource;
  struct dma_memory_callbacks  memory_cb;
};

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void DMAC_Init(void);  
extern void DMAC_Set_Irq_State(const uint8_t channel, const enum DMAC_Callback_Type type, const bool state);
extern void DMAC_Set_Destination_Address(const uint8_t channel, const void *const addr);
extern void DMAC_Set_Source_Address(const uint8_t channel, const void *const addr);
extern void DMAC_Set_Next_Descriptor(const uint8_t current_channel, const uint8_t next_channel);
extern void DMAC_Source_Inc_Enable(const uint8_t channel, const bool enable);
extern void DMAC_Destination_Inc_Enable(const uint8_t channel, const bool enable);
extern void DMAC_Set_Data_Amount(const uint8_t channel, const uint32_t amount);
extern void DMAC_Enable_Transaction(const uint8_t channel, const bool software_trigger);
extern void DMAC_Get_Channel_Resource(struct DMAC_Resource* *resource, const uint8_t channel);

#endif/*_DRIVEN_DMAC_H_*/
