/******************************************************************************
*  Name: lcd_if.h
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LCD_MODULE == 1)

#ifndef LCD_IF_H
#define LCD_IF_H

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
#define SPI_SCL                GPIO(GPIO_PORTB, 25)
#define SPI_SDO                GPIO(GPIO_PORTC, 25)
#define SPI_CSB                GPIO(GPIO_PORTC, 24)
#define SPI_SDI                GPIO(GPIO_PORTB, 24)

#define SPI_BRIDGE_SCL          GPIO(GPIO_PORTC, 13)
#define SPI_BRIDGE_SDO          GPIO(GPIO_PORTC, 12)
#define SPI_BRIDGE_CSB          GPIO(GPIO_PORTC, 14)
#define SPI_BRIDGE_SDI          GPIO(GPIO_PORTC, 15)


#define LCM_DSTB               GPIO(GPIO_PORTC, 19)  /*Deep standby mode control 0:on 1:off*/ 
#define LCM_PON                GPIO(GPIO_PORTC, 20)
#define LCM_DISP               GPIO(GPIO_PORTC, 21)  /*Display control 0:off 1:on*/
#define LCM_RESX               GPIO(GPIO_PORTB, 20)  
#define LCM_RL                 GPIO(GPIO_PORTB, 19)  
#define LCM_TB                 GPIO(GPIO_PORTB, 18)
#define LCM_FAILT              GPIO(GPIO_PORTA, 25)

#define TP_EXT_RSTN            GPIO(GPIO_PORTA, 24)

#define LCD_INIT               1        /*1: software init. 0: lcd otp*/
#define LCD_READ_TEST          0        /*1: open lcd readback test. 0: closed*/

#define LCD_SERCOM             SERCOM0
#define LCD_BRIDGE_SERCOM      SERCOM7

/*Bridge IC automatic Screen cutting 1.AGING_PATTEM_MODULE = 1, 2.0x65->0x67, 0x0130 - 0x013F*/
#define AGING_PATTEM_MODULE  0
/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void Lcd_If_Step1_Init(void);
extern void Lcd_If_Step2_Init(void);
extern void Lcd_If_Standby(void);
extern void Lcd_If_Normal_Mode(void);
extern void Lcd_If_Stbyb_Mode(void);
extern bool Lcd_If_Get_Fault_Level(void);
extern void Lcd_If_SPI_Read_FltReg(void);
extern void Lcd_If_Bridge_Set_Rstn(bool level);
extern void Lcd_If_Set_Disp_Level(bool level);
extern void Lcd_If_Set_Pon_Level(bool level);
extern void Lcd_If_Set_Disp_Level(bool level);
extern void Lcd_If_Set_Reset_Level(bool level);
extern void Lcd_If_Set_Stbyb_Level(bool level);
extern void Lcd_If_Power_On(void);
extern void Lcd_If_Gpio_Init(void);
extern void Lcd_If_Gpio_Deinit(void);


extern void Lcd_If_Ic_Init(void);
extern void Lcd_If_SPI_Init(void);
extern void Lcd_If_SPI_Bridge_Write(const uint8_t *data,const uint16_t *td_data,uint16_t length);
extern void Lcd_If_SPI_Bridge_Read(const uint8_t *read_comd,uint8_t *data, uint16_t length, uint8_t commmand_length);
extern void LCD_Bridge_Init(uint16_t start_num,uint16_t end_num);
extern void LCD_TD7800_Init(const uint16_t *TD7830_ET1);
extern void Lcd_If_Init_Setting(void);



//extern void Lcd_If_Read_Test(void);
#endif /* LCD_IF_H */
#endif /* LCD_MODULE */
