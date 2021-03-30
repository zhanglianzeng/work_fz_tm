/******************************************************************************
*  Name: sleep_if.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (SLEEP_MODULE == 1)

#ifndef SLEEP_IF_H
#define SLEEP_IF_H

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
#define MASTER_WAKE_SIGNAL     GPIO(GPIO_PORTC, 17)
//#define EXTERNAL_WAKE_SIGNAL   GPIO(GPIO_PORTB, 7)
#define SLAVE_REQUIRE_SIGNAL   GPIO(GPIO_PORTC, 16)
//#define NMI_WAKESP_SIGNAL      GPIO(GPIO_PORTA, 8)

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
extern void Sleep_If_Init(void);
extern void Sleep_If_Handle_RCV(void);
extern void Sleep_If_Handle_WakeSP(void);
extern bool Sleep_If_Get_WakeSPSig(void);
extern bool Sleep_If_Get_WakeSP_Flag(void);
extern void Sleep_If_Clear_WakeSP_Flag(void);
extern bool Sleep_If_Get_MasterWakeSig(void);
extern bool Sleep_If_Get_MasterWake_Flag(void);
extern void Sleep_If_Clear_MasterWake_Flag(void);
extern void Sleep_If_Set_SlvReq(void);
extern void Sleep_If_GPIO_Standby(void);
extern void Sleep_If_Module_Standby(void);
extern void Sleep_If_Module_StandbyTowork(void);
#endif /* _SLEEP_IF_H_ */
#endif /* SLEEP_MODULE */
