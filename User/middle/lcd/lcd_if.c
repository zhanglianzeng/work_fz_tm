/******************************************************************************
*  Name: lcd_if.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LCD_MODULE == 1)

#include "lcd_if.h"
#include "../../driver/gpio.h"
#include "../../driver/spi.h"
#include "../../driver/delay.h"
#include "../../../Device/samc21n18a.h"
#include <string.h>
#include "./lcd_parameter.h"
#include "../deserializer/deserializer_if.h"
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/

/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#define LCD_BRIDGE_MODULE    1
#define LCD_TD7800_MODULE    0  /*source ic OTP: 1 NOT OTP : 0*/
/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/
const uint8_t Lcd_Bridge_Write_Enable = 0x11;
const uint8_t Lcd_Bridge_WR_REG  = 0xAA;
const uint8_t Lcd_Bridge_AROF = 0x55;

const uint16_t Lcd_TD_Enter_Read_Mode = 0x038; //enter read mode commd 
const uint16_t Lcd_TD_Exit_Read_Mode = 0x039;
const uint16_t Lcd_TD_Read_Display_Status = 0x00A;


const uint8_t Lcd_Bridge_Default = 0x00;
const uint16_t Lcd_TD_Default = 0x00;


/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_SPI_Port_Init
* Visibility  :        public
* Description :        set gpio to spi function
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Port_Init(void)
{  
#if (1 == LCD_TD7800_MODULE)
    /*set SCL gpio to clk mode*/
    GPIO_Set_Direction(SPI_SCL, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SPI_SCL, false);
    GPIO_Set_Function(SPI_SCL, PINMUX_PB25C_SERCOM0_PAD1);
                               
    /*set SDO gpio to mosi mode*/
    GPIO_Set_Direction(SPI_SDO, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SPI_SDO, false);
    GPIO_Set_Function(SPI_SDO, PINMUX_PC25C_SERCOM0_PAD3);
                                  
    /*set SDI gpio to miso mode*/
    GPIO_Set_Direction(SPI_SDI, GPIO_DIRECTION_IN);
    GPIO_Set_Pull_Mode(SPI_SDI, GPIO_PULL_OFF);
    GPIO_Set_Function(SPI_SDI, PINMUX_PB24C_SERCOM0_PAD0);

    /*set CS gpio to gpio mode*/
    GPIO_Set_Direction(SPI_CSB, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SPI_CSB, true);
    GPIO_Set_Function(SPI_CSB, GPIO_PIN_FUNCTION_OFF);   
#endif
	
#if (1 == LCD_BRIDGE_MODULE)
    /*set SCL gpio to clk mode*/
    GPIO_Set_Direction(SPI_BRIDGE_SCL, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SPI_BRIDGE_SCL, false);
    GPIO_Set_Function(SPI_BRIDGE_SCL, PINMUX_PC13C_SERCOM7_PAD1);
                               
    /*set SDO gpio to mosi mode*/
    GPIO_Set_Direction(SPI_BRIDGE_SDO, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SPI_BRIDGE_SDO, false);
    GPIO_Set_Function(SPI_BRIDGE_SDO, PINMUX_PC15C_SERCOM7_PAD3);
                                  
    /*set SDI gpio to miso mode*/
    GPIO_Set_Direction(SPI_BRIDGE_SDI, GPIO_DIRECTION_IN);
    GPIO_Set_Pull_Mode(SPI_BRIDGE_SDI, GPIO_PULL_OFF);
    GPIO_Set_Function(SPI_BRIDGE_SDI, PINMUX_PC12C_SERCOM7_PAD0);

    /*set CS gpio to gpio mode*/
    GPIO_Set_Direction(SPI_BRIDGE_CSB, GPIO_DIRECTION_OUT);
    GPIO_Set_Level(SPI_BRIDGE_CSB, true);
    GPIO_Set_Function(SPI_BRIDGE_CSB, GPIO_PIN_FUNCTION_OFF); 
#endif		
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_SPI_Set_CSB
* Visibility  :        public
* Description :        set gpio to control cs function
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Set_CSB(bool level)
{
    GPIO_Set_Level(SPI_CSB, level); 
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_SPI_Bridge_Set_CSB
* Visibility  :        public
* Description :        set gpio to control cs function
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Bridge_Set_CSB(bool level)
{
    GPIO_Set_Level(SPI_BRIDGE_CSB, level); 
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Gpio_Init
* Visibility  :        public
* Description :        init reset, stbyb, fault pin of lcd ic 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Gpio_Init(void)
{
    /*set reset pin to output mode*/
    GPIO_Set_Direction(LCM_RESX, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(LCM_RESX, GPIO_PIN_FUNCTION_OFF);

    /*set stbyb pin to output mode*/
    GPIO_Set_Direction(LCM_DSTB, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(LCM_DSTB, GPIO_PIN_FUNCTION_OFF);

    /*set fault pin to input mode*/
    GPIO_Set_Direction(LCM_FAILT, GPIO_DIRECTION_IN);
    GPIO_Set_Function(LCM_FAILT, GPIO_PIN_FUNCTION_OFF);   
	
    /*set PON pin to output mode*/
    GPIO_Set_Direction(LCM_PON, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(LCM_PON, GPIO_PIN_FUNCTION_OFF);
	
    /*set Display control pin to output mode*/
    GPIO_Set_Direction(LCM_DISP, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(LCM_DISP, GPIO_PIN_FUNCTION_OFF);
	
    /*set RL pin to output mode*/
    //GPIO_Set_Direction(LCM_RL, GPIO_DIRECTION_OUT); //Disabled accouding to new HSIS
    //GPIO_Set_Function(LCM_RL, GPIO_PIN_FUNCTION_OFF);
		
    /*set TB pin to output mode*/
    GPIO_Set_Direction(LCM_TB, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(LCM_TB, GPIO_PIN_FUNCTION_OFF);
	  
    /*set bridge RSTN pin to output mode*/
    GPIO_Set_Direction(TP_EXT_RSTN, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(TP_EXT_RSTN, GPIO_PIN_FUNCTION_OFF);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Gpio_Deinit
* Visibility  :        public
* Description :        release lcd gpio pin 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Gpio_Deinit(void)
{
    /*release lcd fault pin*/
    GPIO_Set_Direction(LCM_FAILT, GPIO_DIRECTION_OFF);      
    GPIO_Set_Function(LCM_FAILT, GPIO_PIN_FUNCTION_OFF);
#if (1 == LCD_TD7800_MODULE)
    /*release SCL pin and output low*/
    GPIO_Set_Direction(SPI_SCL, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SPI_SCL, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SPI_SCL, false);
    
    /*release SDO pin and output low*/
    GPIO_Set_Direction(SPI_SDO, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SPI_SDO, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SPI_SDO, false);

    /*release SDI pin to input depend on hardware request*/
    GPIO_Set_Direction(SPI_SDI, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(SPI_SDI, GPIO_PIN_FUNCTION_OFF);

    /*release CSB pin and output low*/
    GPIO_Set_Direction(SPI_CSB, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SPI_CSB, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SPI_CSB, false);    
#endif
		
#if (1 == LCD_BRIDGE_MODULE)
    /*release SCL pin and output low*/
    GPIO_Set_Direction(SPI_BRIDGE_SCL, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SPI_BRIDGE_SCL, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SPI_BRIDGE_SCL, false);
    
    /*release SDO pin and output low*/
    GPIO_Set_Direction(SPI_BRIDGE_SDO, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SPI_BRIDGE_SDO, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SPI_BRIDGE_SDO, false);

    /*release SDI pin to input depend on hardware request*/
    GPIO_Set_Direction(SPI_BRIDGE_SDI, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(SPI_BRIDGE_SDI, GPIO_PIN_FUNCTION_OFF);

    /*release CSB pin and output low*/
    GPIO_Set_Direction(SPI_BRIDGE_CSB, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(SPI_BRIDGE_CSB, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(SPI_BRIDGE_CSB, false);    
#endif		  
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Set_Reset_Level
* Visibility  :        public
* Description :        set lcd reset pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Set_Reset_Level(bool level)
{
    if(level)
        GPIO_Set_Level(LCM_RESX, true);
    else
        GPIO_Set_Level(LCM_RESX, false);        
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Set_Disp_Level
* Visibility  :        public
* Description :        set lcd disp pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Set_Disp_Level(bool level)
{
    if(level)
        GPIO_Set_Level(LCM_DISP, true);
    else
        GPIO_Set_Level(LCM_DISP, false);        
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Set_Stbyb_Level
* Visibility  :        public
* Description :        set lcd standby pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Set_Stbyb_Level(bool level)
{
    if(level)
        GPIO_Set_Level(LCM_DSTB, true);
    else
        GPIO_Set_Level(LCM_DSTB, false);       
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Set_Pon_Level
* Visibility  :        public
* Description :        set lcd pon pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Set_Pon_Level(bool level)
{
    if(level)
        GPIO_Set_Level(LCM_PON, true);
    else
        GPIO_Set_Level(LCM_PON, false);       
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Set_Tb_Level
* Visibility  :        public
* Description :        set lcd tb pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Set_Tb_Level(bool level)
{
    if(level)
        GPIO_Set_Level(LCM_TB, true);
    else
        GPIO_Set_Level(LCM_TB, false);       
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Set_Rl_Level
* Visibility  :        public
* Description :        set lcd rl pin status
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Set_Rl_Level(bool level)
{
    if(level)
        GPIO_Set_Level(LCM_RL, true);
    else
        GPIO_Set_Level(LCM_RL, false);       
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_SPI_Init
* Visibility  :        public
* Description :        init spi master module 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Init(void)
{
#if (1 == LCD_TD7800_MODULE)
    /*set spi work and transfer mode*/
    SPI_Master_Init(LCD_SERCOM, SERCOM_LCD); 
    /*enable spi module*/
    SPI_Master_Set_Enable_Bit(LCD_SERCOM); 
#endif
	
#if (1 == LCD_BRIDGE_MODULE)
    SPI_Master_Init(LCD_BRIDGE_SERCOM, SERCOM_LCD_BRIDGE);
    /*enable spi module*/
    SPI_Master_Set_Enable_Bit(LCD_BRIDGE_SERCOM); 
#endif 
	
    Lcd_If_SPI_Port_Init();
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_SPI_Deinit
* Visibility  :        public
* Description :        deinitial spi module 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Deinit(void)
{
#if (1 == LCD_TD7800_MODULE)
    SPI_Master_Deinit(LCD_SERCOM);   
#endif  
	
#if (1 == LCD_BRIDGE_MODULE)
    SPI_Master_Deinit(LCD_BRIDGE_SERCOM);   
#endif
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Delay_Us
* Visibility  :        public
* Description :        us delay 
* Parameters  :        uint16_t time
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Delay_Us(uint16_t time)
{
    Delay_Us(time);   
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Delay_Ms
* Visibility  :        public
* Description :        ms delay 
* Parameters  :        uint16_t time
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Delay_Ms(uint16_t time)
{
    Delay_Ms(time);   
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Power_On
* Visibility  :        public
* Description :        lcd ic power on 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Power_On(void)
{
    /*depend on hardware requirement when power on*/
    Lcd_If_Set_Reset_Level(true);
    Lcd_If_Delay_Us(500);
    Lcd_If_Set_Stbyb_Level(true);
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        LCD_Bridge_Force_Set_Done
* Visibility  :        public
* Description :        lcd bridge IC Force Set Done 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void LCD_Bridge_Force_Set_Done(void)
{
    uint8_t i;
	
    for(i = 0; i < 4; i++)
    {
        Lcd_If_SPI_Bridge_Set_CSB(false);
        Delay_Us(5);
        Lcd_If_SPI_Bridge_Write(&Force_Set_Done_Data[i][0],&Lcd_TD_Default,5);
        Delay_Us(10);
        Lcd_If_SPI_Bridge_Set_CSB(true);
        Delay_Us(20);
    }
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Ic_Init
* Visibility  :        public
* Description :        lcd ic Ic init 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Ic_Init(void)
{
#if LCD_INIT
    Lcd_If_Step1_Init();
    Lcd_If_Step2_Init();
#endif
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Power_Down
* Visibility  :        public
* Description :        lcd ic power down 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Power_Down(void)
{
    /*depend on hardware requirement when power down*/
    Lcd_If_Set_Pon_Level(false);
    Lcd_If_Set_Disp_Level(false);
    /*SHUT*/
    Lcd_If_Set_Tb_Level(false);
    Lcd_If_Set_Stbyb_Level(false);
   	
    Lcd_If_Delay_Ms(70);      /*HSIS 20210309*/

    Lcd_If_Set_Reset_Level(false);		
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Step1_Init
* Visibility  :        public
* Description :        init lcd hardwre 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Step1_Init(void)
{
    Des_If_Set_PDB_Level(false);
    /*init hardware*/ 
    Lcd_If_Gpio_Init();
    Lcd_If_SPI_Init();
    Delay_Ms(5);
    Lcd_If_Set_Tb_Level(true);
    Lcd_If_Set_Reset_Level(true);
    Lcd_If_Set_Stbyb_Level(true);
    Delay_Us(2600);
#if LCD_INIT 
    /*bridge for EDP config*/
    #if (1 == LCD_BRIDGE_MODULE)
    Delay_Ms(10);
    LCD_Bridge_Init(0,3);       //about 2.4ms
    LCD_Bridge_Force_Set_Done();
    #endif  
#endif	

}


/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Step2_Init
* Visibility  :        public
* Description :        init lcd hardwre 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/

void Lcd_If_Step2_Init(void)
{
#if LCD_INIT
    Delay_Ms(3);//HSIS 20210330

    /*init td7800 ic*/
    #if (1 == LCD_TD7800_MODULE)
    /*MASTER TDDI CODE*/
    LCD_TD7800_Init(TD7830_MASTER_ET1_InitCode);
    /*SLAVE1 TDDI CODE*/
    LCD_TD7800_Init(TD7830_SLAVE1_ET1_InitCode);
    /*SLAVE2 TDDI CODE*/	
    LCD_TD7800_Init(TD7830_SLAVE2_ET1_InitCode);
    #endif  
    Lcd_If_Set_Pon_Level(true); 
    Lcd_If_Set_Disp_Level(true);		

    /*bridge for LUT setting*/
    #if (1 == LCD_BRIDGE_MODULE)
    LCD_Bridge_Init(3,28);
    #endif  
#endif

#if (1 == LCD_READ_TEST)
    Lcd_If_Read_Test();
#endif

}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Init_Setting
* Visibility  :        public
* Description :        init lcd hardwre 
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Init_Setting(void)
{
    LCD_Bridge_Init(0,3);//about 2.4ms
    LCD_Bridge_Force_Set_Done();
    LCD_Bridge_Init(3,28);
}
/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Standby
* Visibility  :        public
* Description :        lcd module enter standby mode
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Standby(void)
{
    /*spi peripheral enter sleep mode when mcu sleep*/
    Lcd_If_Power_Down();
    Lcd_If_SPI_Deinit();
    Lcd_If_Gpio_Deinit();
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Standby
* Visibility  :        public
* Description :        lcd IC standby mode
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Stbyb_Mode(void)
{
    /*lcd ic enter standby mode when power is abnormal mode*/
    Lcd_If_Set_Stbyb_Level(false);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Normal_Mode
* Visibility  :        public
* Description :        normal operation
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Normal_Mode(void)
{
    /*lcd ic enter normal mode when power recover from abnormal mode*/
    Lcd_If_Set_Stbyb_Level(true);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Lcd_If_Get_Fault_Level(void)
* Visibility  :        public
* Description :        Read lcd fault pin status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
bool Lcd_If_Get_Fault_Level(void)
{
    return GPIO_Get_Level(LCM_FAILT);
}

/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if 
* Method      :        Lcd_If_SPI_Bridge_Write
* Visibility  :        public
* Description :        write data to addressed register
* Parameters  :        uint8_t *write_reg, uint8_t write_len
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Bridge_Write(const uint8_t *data,const uint16_t *td_data, uint16_t length)
{
    SPI_Master_Write(LCD_BRIDGE_SERCOM, data, length,td_data);  
}
/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if 
* Method      :        Lcd_If_SPI_TD_Write
* Visibility  :        public
* Description :        write data to addressed register
* Parameters  :        uint8_t *write_reg, uint8_t write_len
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_TD7800_Write(const uint8_t *data, const uint16_t *td_data,uint16_t length)
{
    Lcd_If_SPI_Set_CSB(false);
    Lcd_If_Delay_Us(5);
    SPI_Master_Write(LCD_SERCOM, data, length,td_data);
    Delay_Us(10);
    Lcd_If_SPI_Set_CSB(true);   
}
/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if 
* Method      :        Lcd_If_SPI_Bridge_Read
* Visibility  :        public
* Description :        read data from addressed register
* Parameters  :        uint8_t *read_comd,uint8_t *data, uint16_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_Bridge_Read(const uint8_t *read_comd,uint8_t *data, uint16_t length, uint8_t commmand_length)
{
    Lcd_If_SPI_Bridge_Set_CSB(false);
    Lcd_If_Delay_Us(20);
    SPI_Master_Write(LCD_BRIDGE_SERCOM, read_comd, commmand_length,&Lcd_TD_Default);
    SPI_Master_Read(LCD_BRIDGE_SERCOM,data,length);
    Lcd_If_Delay_Us(50);	
    Lcd_If_SPI_Bridge_Set_CSB(true); 	
}
/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if 
* Method      :        Lcd_If_SPI_TD_Read
* Visibility  :        public
* Description :        read data from addressed register
* Parameters  :        const uint16_t *read_comd,uint8_t *data, uint16_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_SPI_TD7800_Read(const uint16_t *read_comd,uint8_t *data, uint16_t length)
{
    Lcd_If_SPI_Set_CSB(false);
    Lcd_If_Delay_Us(20);
    /*enter read mode and read commond*/
    SPI_Master_Write(LCD_SERCOM, &Lcd_Bridge_Default, 1,&Lcd_TD_Enter_Read_Mode);

    SPI_Master_Write(LCD_SERCOM, &Lcd_Bridge_Default, 1,read_comd);
    /*read parameter*/
    SPI_Master_Read(LCD_SERCOM,data,length);
	 
    /*exit read mode*/
    SPI_Master_Write(LCD_SERCOM, &Lcd_Bridge_Default, 1,&Lcd_TD_Exit_Read_Mode);
	
    Lcd_If_Delay_Us(50);	
    Lcd_If_SPI_Set_CSB(true); 	
}
/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if 
* Method      :        LCD_TD7800_Init
* Visibility  :        public
* Description :        TD7800 IC Init
* Parameters  :        const uint16_t *TD7830_ET1
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void LCD_TD7800_Init(const uint16_t *TD7830_ET1)
{
    uint16_t num,i,commond;
	uint8_t length,j;
	uint16_t parameter[235] = {0};
	num = TD_CODE_SIZE;
	
    for(i = 0; i < (num-1);)
	{
	    if(0xC == (TD7830_ET1[i] >> 8) )
        {
            length = TD7830_ET1[i] & 0xff;
            commond = TD7830_ET1[i+1] & 0xff;
            i += 2;
            Lcd_If_SPI_TD7800_Write(&Lcd_Bridge_Default,&commond,1);
				
            for( j = 0; j < (length-1); j++ ,i++)
            {
                parameter[j] = (TD7830_ET1[i] & 0xff) | 0x100;
            }
            Lcd_If_SPI_TD7800_Write(&Lcd_Bridge_Default,parameter,length-1);				
        }		
	}
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        LCD_Bridge_Write_Internal_Registers
* Visibility  :        public
* Description :        lcd bridge IC write internal register
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void LCD_Bridge_Write_Internal_Registers(uint16_t start_num, uint16_t end_num)
{
    uint16_t i;
	
    for(i = start_num; i < end_num; i++)
    {
        Lcd_If_SPI_Bridge_Set_CSB(false);
        Delay_Us(5);
        Lcd_If_SPI_Bridge_Write(&BU92RE44M_InitCode_Addr[i][0],&Lcd_TD_Default,4);		
        Lcd_If_SPI_Bridge_Write(&BU92RE44M_InitCode_Data[i][0],&Lcd_TD_Default,256);
			  
        Delay_Us(2);
        Lcd_If_SPI_Bridge_Set_CSB(true);
        Delay_Us(2);	
			
    }
}
/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if 
* Method      :        LCD_Bridge_Init
* Visibility  :        public
* Description :        Bridge IC Init
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
/*LCD BRIDGE Init*/
void LCD_Bridge_Init(uint16_t start_num,uint16_t end_num)
{	  
    /*Write enable for internal registers and LUT*/
    Lcd_If_SPI_Bridge_Set_CSB(false);
    Lcd_If_Delay_Us(5);
    Lcd_If_SPI_Bridge_Write(&Lcd_Bridge_Write_Enable,&Lcd_TD_Default,1);
    Delay_Us(10);
    Lcd_If_SPI_Bridge_Set_CSB(true);
	
    /*Write internal registers and LUT  */
    LCD_Bridge_Write_Internal_Registers(start_num,end_num);

    /*Force Set Done*/
   // LCD_Bridge_Force_Set_Done();    
}
#if LCD_READ_TEST
/*=====================================================================================================================
* Upward trac.:        Middle/lcd/lcd_if
* Method      :        Lcd_If_Read_Test(void)
* Visibility  :        public
* Description :        setting lcd page and read register
* Parameters  :        LCD_CONFIG_P data, uint8_t length
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* =====================================================================================================================*/
void Lcd_If_Read_Test(void)
{
#if (1 == LCD_TD7800_MODULE)
    uint8_t read_buf[2] = {0};
    const uint16_t write_en[4] = {0x039,0x0B0,0x100,0x100};
	 
    Lcd_If_SPI_TD7800_Write(&Lcd_Bridge_Default,write_en,4);
    Lcd_If_SPI_TD7800_Read(&Lcd_TD_Read_Display_Status,read_buf,2);
#endif
	 
#if (1 == LCD_BRIDGE_MODULE)
    const uint8_t read_comd[5] = {0x99,0x00,0x00,0x00,0xff}; 
    uint8_t read_data[256] = {0};
	 
    Lcd_If_SPI_Bridge_Read(read_comd,read_data,256,5);
#endif 
}
#endif /*LCD_READ_TEST*/


#endif /* LCD_MODULE */
