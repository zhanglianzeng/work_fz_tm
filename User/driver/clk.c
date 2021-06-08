/******************************************************************************
*  Name: clk.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../config.h"
#include "../../Device/samc21n18a.h"
#include "./config/hpl_gclk_config.h"
#include "./config/hpl_mclk_config.h"
#include "clk.h"
#define GCLK_GENCTRL_GENEN_Disable (_U_(0x0) << GCLK_GENCTRL_GENEN_Pos)
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
static void OSCCTRL_Initialize(void);
static void GCLK0_Initialize(void);
static void OSC32KCTRL_Initialize(void);
static void GCLK1_Initialize(void);
static void GCLK2_Initialize(void);
static void GCLK3_Initialize(void);
static void GCLK4_Initialize(void);
static void GCLK5_Initialize(void);
static void GCLK6_Initialize(void);
static void GCLK7_Initialize(void);
/**********************************************************************************************
* Global functions
**********************************************************************************************/

/***********************************************************************************************
*
* @brief
* @param    none
* @return   none
*
************************************************************************************************/
void GCLK_Deinit(void)
{

     //GCLK->GENCTRL[0].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_0_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_0_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;
     //GCLK->GENCTRL[1].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_1_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_1_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC | GCLK_GENCTRL_OOV;

     GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_2_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_2_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

     //GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_3_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_3_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE  | GCLK_GENCTRL_IDC;

     GCLK->GENCTRL[4].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_4_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_4_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

     GCLK->GENCTRL[5].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_5_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_5_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

     GCLK->GENCTRL[6].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_6_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_6_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

     GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_7_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_7_SOURCE) | GCLK_GENCTRL_GENEN_Disable | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;
}

void GCLK_Init(void)
{

     GCLK0_Initialize();
     GCLK1_Initialize();
     GCLK2_Initialize();
     GCLK3_Initialize();
     GCLK4_Initialize();
     GCLK5_Initialize();
     GCLK6_Initialize();
     GCLK7_Initialize();
}

/***********************************************************************************************
*
* @brief    Clk_Init
* @param    none
* @return   none
*
************************************************************************************************/
void Clk_Init(void)
{
     NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_RWS(3);
     OSCCTRL_Initialize();
     OSC32KCTRL_Initialize();
     GCLK_Init();
     MCLK->CPUDIV.reg = MCLK_CPUDIV_CPUDIV(CONF_MCLK_CPUDIV);
     MCLK->APBCMASK.reg = MCLK_APBCMASK_EVSYS | MCLK_APBCMASK_SERCOM0 | MCLK_APBCMASK_SERCOM1 | MCLK_APBCMASK_SERCOM2 | MCLK_APBCMASK_SERCOM4 | MCLK_APBCMASK_SERCOM5 | MCLK_APBCMASK_TCC0 | MCLK_APBCMASK_SDADC | MCLK_APBCMASK_EVSYS | MCLK_APBCMASK_ADC0 | MCLK_APBCMASK_PTC;

     /*configure the PTC clock*/
     GCLK->PCHCTRL[37].reg = CONF_GCLK_PTC_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);

     /*for DMAC*/
     MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
     /*for DSU */
     MCLK->APBBMASK.reg |= MCLK_APBBMASK_DSU;
}

/***********************************************************************************************
*
* @brief    OSCCTRL Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void OSCCTRL_Initialize(void)
{
     /****************** OSCCTRL Initialization  ******************************/
     uint32_t calibValue = (uint32_t)(((*(uint64_t *)0x806020) >> 19) & 0x3fffff);
     OSCCTRL->CAL48M.reg = calibValue;
     OSCCTRL->OSC48MDIV.reg = OSCCTRL_OSC48MDIV_DIV(0);
     while ((OSCCTRL->OSC48MSYNCBUSY.reg & OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV) == OSCCTRL_OSC48MSYNCBUSY_OSC48MDIV)
     {
          /* Waiting for the synchronization */
     }
     while (!((OSCCTRL->STATUS.reg & OSCCTRL_STATUS_OSC48MRDY) == OSCCTRL_STATUS_OSC48MRDY))
     {
          /* Waiting for the OSC48M Ready state */
     }
}

/***********************************************************************************************
*
* @brief   OSC32KCTRL Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void OSC32KCTRL_Initialize(void)
{
     /****************** OSC32K Initialization  ******************************/
     uint32_t calibValue = (((*(uint32_t *)0x806020) >> 12) & 0x7f);

     /* Configure 32K RC oscillator */
     OSC32KCTRL->OSC32K.reg = OSC32KCTRL_OSC32K_CALIB(calibValue) | OSC32KCTRL_OSC32K_STARTUP(0) | OSC32KCTRL_OSC32K_ENABLE | OSC32KCTRL_OSC32K_EN1K | OSC32KCTRL_OSC32K_EN32K;

     /*Configure 32KHz OSCULP32K oscillator*/
     OSC32KCTRL->OSCULP32K.reg = OSC32KCTRL_OSCULP32K_CALIB(calibValue) | OSC32KCTRL_OSCULP32K_WRTLOCK;

     while (!((OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_OSC32KRDY) == OSC32KCTRL_STATUS_OSC32KRDY))
     {
          /* Waiting for the OSC32K Ready state */
     }

     OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC32K;
}

/***********************************************************************************************
*
* @brief    GCLK0 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK0_Initialize(void)
{
     if (CONF_GCLK_GEN_0_GENEN)
     {
          GCLK->GENCTRL[0].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_0_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_0_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0) == GCLK_SYNCBUSY_GENCTRL0)
          {
               /* wait for the Generator 0 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK1 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK1_Initialize(void)
{
     if (CONF_GCLK_GEN_1_GENEN)
     {
          GCLK->GENCTRL[1].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_1_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_1_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC | GCLK_GENCTRL_OOV;
          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL1) == GCLK_SYNCBUSY_GENCTRL1)
          {
               /* wait for the Generator 1 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK0 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK2_Initialize(void)
{
     if (CONF_GCLK_GEN_2_GENEN)
     {
          GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_2_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_2_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL2) == GCLK_SYNCBUSY_GENCTRL2)
          {
               /* wait for the Generator 2 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK2 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK3_Initialize(void)
{
     if (CONF_GCLK_GEN_3_GENEN)
     {
          GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_3_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_3_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL3) == GCLK_SYNCBUSY_GENCTRL3)
          {
               /* wait for the Generator 3 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK3 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK4_Initialize(void)
{
     if (CONF_GCLK_GEN_4_GENEN)
     {
          GCLK->GENCTRL[4].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_4_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_4_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL4) == GCLK_SYNCBUSY_GENCTRL4)
          {
               /* wait for the Generator 4 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK5 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK5_Initialize(void)
{
     if (CONF_GCLK_GEN_5_GENEN)
     {
          GCLK->GENCTRL[5].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_5_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_5_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL5) == GCLK_SYNCBUSY_GENCTRL5)
          {
               /* wait for the Generator 5 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK6 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK6_Initialize(void)
{
     if (CONF_GCLK_GEN_6_GENEN)
     {
          GCLK->GENCTRL[6].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_6_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_6_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL6) == GCLK_SYNCBUSY_GENCTRL6)
          {
               /* wait for the Generator 6 synchronization */
          }
     }
}

/***********************************************************************************************
*
* @brief    GCLK7 Initialize
* @param    none
* @return   none
*
************************************************************************************************/
static void GCLK7_Initialize(void)
{
     if (CONF_GCLK_GEN_7_GENEN)
     {
          GCLK->GENCTRL[7].reg = GCLK_GENCTRL_DIV(CONF_GCLK_GEN_7_DIV) | GCLK_GENCTRL_SRC(CONF_GCLK_GEN_7_SOURCE) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_OE | GCLK_GENCTRL_IDC;

          while ((GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL7) == GCLK_SYNCBUSY_GENCTRL7)
          {
               /* wait for the Generator 7 synchronization */
          }
     }
}
