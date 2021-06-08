/******************************************************************************
*  Name: post_if.h
*  Description:
*  Project: INFO4 11inch
*  Auther: fangsong
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"

#if (POST_MODULE == 1)
#ifndef POST_IF_H
#define POST_IF_H

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
//   <o>        CPU: CPU family selection
//                      <0=> SAMD20J18
//                      <1=> SAMD21J18
//                      <2=> SAMC20J18
//                      <3=> SAMC21J18
#define CPU_SELECTION   3

#define LoopForever() { \
                       for(;;) {} \
} /*!< macro enables loop for ever */

#define ClassB_TEST_POST_RAM                1       /*!< RAM test: 1=Enabled, 0=Disabled */
#define ClassB_TEST_POST_STACK          1       /*!< RAM test on Stack area: 1=Enabled, 0=Disabled */
#define ClassB_TEST_POST_WDT                0       /*!< WatchDog test: 1=Enabled, 0=Disabled */
#define ClassB_TEST_POST_WDT_PERIOD    11   /*!< timeout value for the WDT in the test; must be > ClassB_TEST_POST_WDOG_WLB */

/* do not change this 20 lines */
#define ClassB_Enabled          1U                      /*!< enabled features of the ClassB library */
#define ClassB_Disabled         0U                      /*!< disabled features of the ClassB library */

/* System clock bus configuration */
#define CONF_WDT_FLASH_WAIT_STATES  (0u)

#if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
    #define CONF_WDT_CPU_DIV            (0u)
#else /* CPU_SELECTION == 2, 3 */
    #define CONF_WDT_CPU_DIV            (1u)
#endif /* CPU_SELECTION */

/* register bit definitions */
#define SYSCTRL_INTFLAG_DFLLRDY_    (0x01u << 4u)
#define SYSCTRL_INTFLAG_BOD33RDY_   (0x01u << 9u)
#define SYSCTRL_INTFLAG_BOD33DET_   (0x01u << 10u)

#define PM_APBAMASK_WDT_            (0x01u << 4u)   /*!< AP Mask: WDT module gate */
#define PM_RCAUSE_POR_              (0x01u)         /*!< Reset cause: Power-On reset */
#define PM_RCAUSE_EXT_              (0x01u << 4u)   /*!< Reset cause: Xternal reset */
#define PM_RCAUSE_WDT_              (0x01u << 5u)   /*!< Reset cause: Watchdog reset */
#define PM_RCAUSE_SYST_             (0x01u << 6u)   /*!< Reset cause: System reset */

#define WDT_CTRL_ALWAYSON_          (0x01u << 7u)   /*!< WDT status: "always on" bit */
#define WDT_CTRL_ENABLE_            (0x01u << 1u)   /*!< WDT configuration: "Enable" bit */
#define WDT_CTRL_WEN_               (0x01u << 2u)   /*!< WDT configuration: "Window Mode Enable" bit */
/*!< WDT configuration: period */
#define WDT_NEW_CONFIG_             ((ClassB_TEST_POST_WDT_PERIOD - 1u) & 0x0Fu)

/*! @endcond */

#define TESSY_SYNC()
#define WDT_Sync()  while (WDT->SYNCBUSY.reg) { }   /* Wait for sync */

/* configuration for the debugging GPIOs */
#if (CLASSB_DBG_GPIO == ClassB_Enabled)
    /* port IO defines for LED access PB.30 on the board*/
    #define __DBG_PIN_OUTPUT() PORT->Group[1].DIRSET.reg = PORT_PB05    /*!< Setup Pin for LED */

    #define __DBG_GET_IO0()    !(PORT->Group[1].OUT.reg & PORT_PB05)    /*!< mask LED pin value */ 

    #define __DBG_SET_IO0()    PORT->Group[1].OUT.reg |= PORT_PB05      /*!< reset LED */

    #define __DBG_RESET_IO0()  PORT->Group[1].OUT.reg &= ~PORT_PB05     /*!< set LED */

    #define __DBG_SET_IO1()                                 /*!< macro defined empty to switch off */

    #define __DBG_RESET_IO1()                               /*!< macro defined empty to switch off */
    
    #define __DBG_WAIT(x)       {uint32_t i; for (i=0; i<x; i++) __NOP();}
#else
/* defined dummy function to replace if de-activated feature */
    #define __DBG_PIN_OUTPUT()  __NOP()   /*PRQA S 0602*//*PRQA S 0603*/
    #define __DBG_GET_IO0()     false     /*PRQA S 0602*//*PRQA S 0603*/
    #define __DBG_SET_IO0()     __NOP()   /*PRQA S 0602*//*PRQA S 0603*/
    #define __DBG_RESET_IO0()   __NOP()   /*PRQA S 0602*//*PRQA S 0603*/
    #define __DBG_SET_IO1()     __NOP()   /*PRQA S 0602*//*PRQA S 0603*/
    #define __DBG_RESET_IO1()   __NOP()   /*PRQA S 0602*//*PRQA S 0603*/
    #define __DBG_WAIT(x)       __NOP()   /*PRQA S 0602*//*PRQA S 0603*/
#endif

/* ************************ Critical Section Mcros ***************************** */
#define EnterCriticalSection()          __disable_irq();\
                                        __DMB()             /*!< enable atomic instructions */
#define ExitCriticalSection()           __DMB();        \
                                        __enable_irq()      /*!< disable atomic instructions */

#define getSP()                         __get_MSP()
#define setMSP(x)                        __set_MSP(x)


typedef struct _SystemVectors    /*PRQA S 0602*/
{
    /* Stack pointer */
    uint32_t pStackPtr;                      /*!< 0 stack pointer */
    /* Cortex-M system handlers */
    uint32_t pReset_Vector;                  /*!< 0 reset vector */
    uint32_t pNMI_Vector;                    /*!< 0 NMI vector */
    uint32_t pHardFault_Vector;              /*!< 0 hard fault vector */
    uint32_t pReserved_12_Vector;
    uint32_t pReserved_11_Vector;
    uint32_t pReserved_10_Vector;
    uint32_t pReserved_9_Vector;
    uint32_t pReserved_8_Vector;
    uint32_t pReserved_7_Vector;
    uint32_t pReserved_6_Vector;
    uint32_t pSVC_Handler;                   /*!< 0 SVC vector */
    uint32_t pReserved_4_Vector;
    uint32_t pReserved_3_Vector;
    uint32_t pPendSV_Vector;                 /*!< 0 PendSV vector */
    uint32_t pSysTick_Vector;                /*!< 0 SysTick vector */
} _SystemVectors;                            /*PRQA S 0602*/

#define SystemVectors           ((_SystemVectors *)0x00000000UL) /**< \brief Vector Base Address */

#define ClassB_RAM_START                (0x20000000uL)  /*!< start address of the RAM memory */

#define SIZE2K                          (0x00000500UL)  /*!< definition for 2k area */
#define SIZE4K                          (0x00001000UL)  /*!< definition for 4k area */
#define SIZE8K                          (0x00002000UL)  /*!< definition for 8k area */
#define SIZE16K                         (0x00004000UL)  /*!< definition for 16k area */
#define SIZE32K                         (0x00008000UL)  /*!< definition for 32k area */
#define SIZE64K                         (0x00010000UL)  /*!< definition for 64k area */
#define SIZE128K                        (0x00020000UL)  /*!< definition for 128k area */
#define SIZE256K                        (0x00040000UL)  /*!< definition for 256k area */

#define ClassB_RAM_SIZE             (SIZE32K)
/*!< Estimated size of a buffer to the next stack frame */
#define ClassB_RAM_SP_Offset        (0x10U)

/*!
 * brief offset value to place a new intermediate stack during the test
 */
#define StackOffset                 (0x100U)
#define StackNegOffset              (0x20U)
#define CURRENT_SP_CORRECTION       (0x40u)

#define RAMTEST_W_SIZE              (4u)
    
/*!
 * brief RAM test control states
 */
#define RAM_TEST_PENDING            (0x11U) /* test is pending, no started */
#define RAM_TEST_DONE               (0x10U) /* test is complete */

/*!
 * brief Stack test watermark value
 */
#define CLASSB_STACK_WATERMARK_VAL  (0xDEADDEAD)

/*!
 * @}
 */
/* Local Definitions ----------------------------------------------------------- */

/* ------------------------ */
/* RAM test suite status definitions */
/* RAM algo constants */
/*!< macro to access the initial stack pointer on the Flash */
#define initialSP                   (SystemVectors->pStackPtr)

//(*(uint32_t *) ClassB_FLASH_START)

/*!< content of R0 expected */
#define CORE_RAM_R0_EXP             (0x00000000L)
/*!< content of R0 expected (inverse) */
#define CORE_RAM_R1_EXP             (~CORE_RAM_R0_EXP)

/** MSB Bit is active */
#define MSB_BIT_1 0x80000000

/* correction for last address access */
#define LASTADDR_OFFSET             (0x01u)

/**********************************************************************************************
* Local types
**********************************************************************************************/

/*! generic type for ClassB functional results */
typedef enum ClassB_testResult_enum
{
    ClassB_testFailed     = 0U,             /*!< test result failed replacement */
    ClassB_testPassed     = 1U,             /*!< test result passed replacement */
    ClassB_testInProgress = 2U              /*!< test is still in progress replacement */
} ClassB_testResult_t;

/* RAM test modes */
typedef enum ClassB_RAMTest_Mode_enum
{
    ClassB_RAMtestNormal   = 0U,            /*!< test mode for RAM test without stack test */
    ClassB_RAMtestStack    = 1U             /*!< test mode for RAM test explicit stack test */
} ClassB_RAMTest_Mode_t;
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void ClassB_POST(void);
extern void Post_RamInit(void);
extern bool Post_RamTest(void);

#endif /* _POST_IF_H_ */
#endif /*POST_MODULE */
