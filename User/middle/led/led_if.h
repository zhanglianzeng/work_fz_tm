/******************************************************************************
*  Name: led_if.h
*  Description:
*  Project: Wave2
*  Auther: 
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LED_MODULE == 1)

#ifndef LED_IF_H
#define LED_IF_H

#include <stdbool.h>
#include <stdint.h>
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define BL_EN                   GPIO(GPIO_PORTB, 10)   
#define BL_FAULT                GPIO(GPIO_PORTB, 14)
#define BL_PWM                  GPIO(GPIO_PORTB, 11)
#define BL_SDA                  GPIO(GPIO_PORTB, 12)
#define BL_SCL                  GPIO(GPIO_PORTB, 13)

//#if (SWITCHPACK_BACKLIGHT_MODULE == 1)
//#define BL_PWM1                 GPIO(GPIO_PORTA, 5)   
//#define BL_PWM2                 GPIO(GPIO_PORTA, 6)   
//#define BL_PWM3                 GPIO(GPIO_PORTA, 12)   
//#define BL_PWM4                 GPIO(GPIO_PORTA, 13)
//#endif

#define BL_I2C                  SERCOM4
#define BL_TCC_ID               1      /*TCC1*/
#define BL_I2C_CONTROL          1      /*pwm is controled by i2c register*/

#define BL_REG_NUM              2      /*nums of inited register*/
#define BL_READBACK_NUM         3      /*readback times when init led registers*/

#define BL_I2C_BITS             16     /*lp8863 bit 15-0*/
#define BL_MODE                 0x20   /*BL_MODE Register Offset = 20h*/
#define BL_DISPLAY_BRT          0x28
#define BL_AUTO_DETECT          0x8C   /*0x8C is LSB of 0x28C*/ 



#define BL_SLAVE_ADDR           0x2C   /*access to 0x000 to 0x0FF*/
#define BL_SLAVE_ADDR1          0x2D   /*access to 0x100 to 0x1FF*/
#define BL_SLAVE_ADDR2          0x2E   /*access to 0x200 to 0x2FF*/
#define BL_INT_FLT_STAT1        0x54
#define BL_INT_FLT_STAT2        0x56
#define BL_INT_FLT_STAT3        0x58

#define BL_FLT_REG_NUM          3
#define BL_REG_LEN              2

#define BL_LED_FLT              0x0F80     //stat2  LED0/LED1/LEDOPEN/LEDSHORT/LED_STATUS    
#define BL_LED_FLT_CLR          0x00C0 

#define BL_OV_BOOST_FLT         0x8080     //stat1  BSTOVPH_STATUS/BSTOVPL_STATUS
#define BL_OV_BOOST_FLT_CLR     0xC0C0
#define BL_OV_VIN_FLT           0x0008     //stat3  VINOVP_STATUS
#define BL_OV_VIN_FLT_CLR       0x000C

#define BL_UV_VDO_FLT           0x0008     //stat1  VDDUVLO_STATUS
#define BL_UV_VDO_FLT_CLR       0x000C
#define BL_UV_VIN_FLT           0x0002     //stat3  VINUVP_STATUS
#define BL_UV_VIN_FLT_CLR       0x0003

#define BL_OC_BOOST_FLT         0x0020     //stat1  BSTOCP_STATUS
#define BL_OC_BOOST_FLT_CLR     0x0030 
#define BL_OC_VIN_FLT           0x2000     //stat3  VINOCP_STATUS
#define BL_OC_VIN_FLT_CLR       0x3000

#define BL_OT_FLT               0x2000     //stat1  TEMPHIGH_STATUS
#define BL_OT_FLT_CLR           0x3000

#define BL_UT_FLT               0x0800     //stat1  TEMPLOW_STATUS
#define BL_UT_FLT_CLR           0x0C00
/**********************************************************************************************
* Local types
**********************************************************************************************/
typedef union
{
    struct
    {
        /*INTERRUPT_STATUS_1 Register bit map*/
        union
        {
            struct
            {
                uint16_t TSD_CLEAR       :      1;
                uint16_t TSD_STATUS      :      1;
                uint16_t VDDUVLO_CLEAR   :      1;
                uint16_t VDDUVLO_STATUS  :      1;
                uint16_t BSTOCP_CLEAR    :      1;
                uint16_t BSTOCP_STATUS   :      1;
                uint16_t BSTOVPL_CLEAR   :      1;
                uint16_t BSTOVPL_STATUS  :      1;
                uint16_t RESERVED        :      2;
                uint16_t TEMPLOW_CLEAR   :      1;
                uint16_t TEMPLOW_STATUS  :      1;
                uint16_t TEMPHIGH_CLEAR  :      1;
                uint16_t TEMPHIGH_STATUS :      1;
                uint16_t BSTOVPH_CLEAR   :      1;
                uint16_t BSTOVPH_STATUS  :      1;
            }bStat;
            uint16_t  hStat_Reg;
        }unInt_Flt1;

        union
        {
            /*INTERRUPT_STATUS_2 Register bit map*/
            struct
            {
                uint16_t RESERVED        :      6;
                uint16_t LED_CLEAR       :      1;
                uint16_t LED_STATUS      :      1;
                uint16_t SHORT_LED       :      1;
                uint16_t OPEN_LED        :      1;
                uint16_t LED0_FAULT      :      1;
                uint16_t LED1_FAULT      :      1;
                uint16_t LED2_FAULT      :      1;
                uint16_t LED3_FAULT      :      1;
                uint16_t LED4_FAULT      :      1;
                uint16_t LED5_FAULT      :      1;
            }bStat;
            uint16_t  hStat_Reg;
        }unInt_Flt2;

        union
        {
            /*INTERRUPT_STATUS_3 Register bit map*/
            struct
            {
                uint16_t VINUVP_CLEAR     :      1;
                uint16_t VINUVP_STATUS    :      1;
                uint16_t VINOVP_CLEAR     :      1;
                uint16_t VINOVP_STATUS    :      1;
                uint16_t INVSTRING_CLEAR  :      1;
                uint16_t INVSTRING_STATUS :      1;
                uint16_t FSET_CLEAR       :      1;
                uint16_t FSET_STATUS      :      1;
                uint16_t CP_CLEAR         :      1;
                uint16_t CP_STATUS        :      1;
                uint16_t CPCAP_CLEAR      :      1;
                uint16_t CPCAP_STATUS     :      1;
                uint16_t VINOCP_CLEAR     :      1;
                uint16_t VINOCP_STATUS    :      1;
                uint16_t BSTSYNC_CLEAR    :      1;
                uint16_t BSTSYNC_STATUS   :      1;
            }bStat;
            uint16_t  hStat_Reg;
        }unInt_Flt3;
    }strReg;
    uint8_t Flt_Status[BL_FLT_REG_NUM*BL_REG_LEN];
}unBlFlt;

#if (SWITCHPACK_BACKLIGHT_MODULE == 1)
typedef enum 
{
    PWM1_ID,
    PWM2_ID,
    PWM3_ID,
    PWM4_ID
}PWM_CTL_ID;
#endif

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void LED_If_Init(void);
extern void LED_If_I2C_Init(void);
extern void LED_If_I2c_Deinit(void);
extern void LED_If_Standby(void);
extern unBlFlt LED_If_Get_BL_Fault(void);
extern void LED_If_Set_EN_Level(bool level);  
extern bool LED_If_Get_Fault_Level(void);
extern void LED_If_Set_PWM_Duty(uint16_t duty);
extern void LED_If_Fault_Read(void);
extern void LED_If_Fault_Write(uint16_t send_value,uint8_t reg);
extern void LED_If_Gpio_Deinit(void);
extern void LED_If_IC_Config(void);
#if (SWITCHPACK_BACKLIGHT_MODULE == 1)
extern void LED_If_Set_SwitchPack_Duty(PWM_CTL_ID pwm_id, uint16_t duty);
#endif
#endif /* LED_IF_H */
#endif /* LED_MODULE */
