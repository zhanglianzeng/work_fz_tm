/******************************************************************************
*  Name: led_if.c
*  Description:
*  Project: Wave2
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (LED_MODULE == 1)

#include "led_if.h"
#include <stdint.h>
#include "../../../Device/samc21n18a.h"
#include "../../driver/gpio.h"
#include "../../driver/tc.h"
#include "../../driver/iic_master.h"
#include "../../driver/delay.h"

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
static unBlFlt Bl_Flt_Status = {0};
/*INTERRUPT_STATUS Regsiter offset*/
const uint8_t Bl_Flt_Reg[BL_FLT_REG_NUM] = {BL_INT_FLT_STAT1,BL_INT_FLT_STAT2,BL_INT_FLT_STAT3};
/**********************************************************************************************
* Local functions
**********************************************************************************************/
static void LED_If_Time_Delay(uint16_t ms);
static void LED_If_I2C_Write(uint8_t slave_addr,uint8_t *send_data, uint16_t length);
static void LED_If_I2C_Read(uint8_t slave_addr, uint8_t reg_addr,uint8_t *data, uint16_t length);

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Get_BL_Fault(void)
* Visibility  :        public
* Description :        Get Bl_Flt_Status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
unBlFlt LED_If_Get_BL_Fault(void)
{
    return Bl_Flt_Status;
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Gpio_Init
* Visibility  :        public
* Description :        init led gpio
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Gpio_Init(void)
{
    Delay_Ms(2);  //2ms HSIS20210330
    /*set LED ic enable pin to output high at LCD MODULE according to HW Timing*/
    GPIO_Set_Direction(BL_EN, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(BL_EN, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(BL_EN, true);
   
    GPIO_Set_Direction(BL_FAULT, GPIO_DIRECTION_IN);
    GPIO_Set_Pull_Mode(BL_FAULT, GPIO_PULL_UP);
    GPIO_Set_Function(BL_FAULT, GPIO_PIN_FUNCTION_OFF);
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Gpio_Deinit
* Visibility  :        public
* Description :        disable led gpio
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Gpio_Deinit(void)
{
    Delay_Ms(5);
    /*release led enable gpio*/
    GPIO_Set_Level(BL_EN, false);

    /*release led fault gpio*/          
    GPIO_Set_Direction(BL_FAULT, GPIO_DIRECTION_OFF);
    GPIO_Set_Pull_Mode(BL_FAULT, GPIO_PULL_OFF);
    GPIO_Set_Function(BL_FAULT, GPIO_PIN_FUNCTION_OFF);  

#if (!BL_I2C_CONTROL)
    /*release Tc gpio*/   
    GPIO_Set_Direction(BL_PWM, GPIO_DIRECTION_OFF);
    GPIO_Set_Function(BL_PWM, GPIO_PIN_FUNCTION_OFF); 
#endif 
#if SWITCHPACK_BACKLIGHT_MODULE  
    LED_If_SWP_Port_Deinit();
#endif    
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Set_EN_Level
* Visibility  :        public
* Description :        set led enable pin status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Set_EN_Level(bool level)
{
    if(level)
        GPIO_Set_Level(BL_EN, true);
    else
        GPIO_Set_Level(BL_EN, false);        
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Get_Fault_Level
* Visibility  :        public
* Description :        read lp8863 ic fault pin status
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
bool LED_If_Get_Fault_Level(void)
{
    return GPIO_Get_Level(BL_FAULT);    
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Time_Delay
* Visibility  :        public
* Description :        led time delay
* Parameters  :        uint16_t ms
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void LED_If_Time_Delay(uint16_t ms)
{
    Delay_Ms(ms);
}

#if (!BL_I2C_CONTROL)
/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Pwm_Port_Init
* Visibility  :        public
* Description :        set gpio to Tcc function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Tcc_Port_Init(void)
{
    GPIO_Set_Function(BL_PWM, PINMUX_PB11E_TC5_WO1);
}
#endif /*BL_I2C_CONTROL*/

/*=====================================================================================================================
* Upward trac.:        
* Method      :        LED_If_Pwm_Init
* Visibility  :        public
* Description :        init Tcc hardware module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Pwm_Init(void)
{
#if (!BL_I2C_CONTROL)
    /*enable Tcc bus clock*/
    TC_Clk_Init(TC5);
    /*set gpio to Tcc funcion*/
    LED_If_Tcc_Port_Init();
    /*set Tc pwm work mode*/
    TC_Pwm_Init(TC5);
	/*set Tc pwm freq*/
    TC_Set_Period(TC5,999);
#endif /*BL_I2C_CONTROL*/

#if SWITCHPACK_BACKLIGHT_MODULE
    LED_If_SwitchPack_Pwm_Init();
#endif
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Set_PWM_Duty
* Visibility  :        public
* Description :        set lp8863 input pwm duty
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Set_PWM_Duty(uint16_t duty)
{
#if BL_I2C_CONTROL
    uint8_t readback[2] = {0};
    /*DISPLAY_BRT registers control each led current driver*/
    uint8_t send_data[3] = {BL_DISPLAY_BRT,0x00,0x00};

    send_data[1] = duty & 0xff;     /*8 bit LSB of Data*/
    send_data[2] = (duty>>8) & 0xff;/*8 bit MSB of Data*/

    for (uint8_t i=0; i<BL_READBACK_NUM; i++)
    {
        LED_If_I2C_Write(BL_SLAVE_ADDR,send_data,3);
        /*readback to check whether write correctly*/
        LED_If_I2C_Read(BL_SLAVE_ADDR,BL_DISPLAY_BRT,readback,2);
        if (((readback[0]==send_data[1])&&(readback[1]==send_data[2])))
        {
            break;
        }
    }
#else
    TC_Set_Duty(TC5, duty * 5);
#endif
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_I2C_Init
* Visibility  :        public
* Description :        init i2c module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_I2C_Init(void)
{
    /*init secrom to i2c mode*/
    IIC_Master_Init(BL_I2C, SERCOM_LED);

    /*set gpio to i2c mode function*/
    GPIO_Set_Function(BL_SDA, PINMUX_PB12C_SERCOM4_PAD0);   
    GPIO_Set_Function(BL_SCL, PINMUX_PB13C_SERCOM4_PAD1); 

    /*enable secrom to i2c mode*/
    IIC_Master_Enable_Bit(BL_I2C);   
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_I2c_Deinit
* Visibility  :        public
* Description :        disable i2c module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_I2c_Deinit(void)
{
    /*deinit sercom module*/
    IIC_Master_DeInit(BL_I2C, SERCOM_LED); 

    /*deinit i2c gpio pinmux*/
    GPIO_Set_Direction(BL_SDA, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(BL_SDA, GPIO_PIN_FUNCTION_OFF); 
    GPIO_Set_Level(BL_SDA, false);  
  
    GPIO_Set_Direction(BL_SCL, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(BL_SCL, GPIO_PIN_FUNCTION_OFF);  
    GPIO_Set_Level(BL_SCL, false);   
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Init
* Visibility  :        public
* Description :        init LED hardware module
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Init(void)
{
    /*init hardware*/
	LED_If_Gpio_Init();

    LED_If_Pwm_Init();  
    LED_If_I2C_Init();

    /*init Led ic*/
    LED_If_IC_Config();   
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Standby
* Visibility  :        public
* Description :        enter standby mode
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Standby(void)
{ 
    /*disable hardware*/
    LED_If_I2c_Deinit();   
    LED_If_Gpio_Deinit();   
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_I2C_Write
* Visibility  :        public
* Description :        lp8863 i2c write interface
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void LED_If_I2C_Write(uint8_t slave_addr,uint8_t *send_data, uint16_t length)
{
    IIC_Master_Write(BL_I2C, slave_addr, send_data, length,SEND_STOP);    
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_I2C_Read
* Visibility  :        public
* Description :        lp8863 i2c read interface
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void LED_If_I2C_Read(uint8_t slave_addr, uint8_t reg_addr,uint8_t *data, uint16_t length)
{    
    IIC_Master_Write(BL_I2C, slave_addr, &reg_addr, 1,SEND_NSTOP);
    IIC_Master_Read(BL_I2C,slave_addr,data,2,SEND_STOP);    
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_IC_Config
* Visibility  :        public
* Description :        setting lp8863 reg
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_IC_Config(void)
{
#if BL_I2C_CONTROL
    uint8_t readback[2] = {0};
    /*i2c controll backlight, disable auto detect and set 2 strings*/
    uint8_t send_data[BL_REG_NUM][4]={{BL_SLAVE_ADDR,BL_MODE,0x02,0x03},
                                     {BL_SLAVE_ADDR2,BL_AUTO_DETECT,0x08,0x81}};

    for (uint8_t reg=0; reg<BL_REG_NUM; reg++)
    {
        for (uint8_t i=0;i<BL_READBACK_NUM; i++)
        {
            LED_If_I2C_Write(send_data[reg][0],&send_data[reg][1],3);
            /*readback to check whether write correctly*/
            LED_If_I2C_Read(send_data[reg][0],send_data[reg][1],readback,2);
            if (((readback[0]==send_data[reg][2])&&(readback[1]==send_data[reg][3])))
            {
                break;
            }
        }
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_IC_Config
* Visibility  :        public
* Description :        Clear fault register
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Fault_Write(uint16_t send_value,uint8_t reg)                                                                
{
    uint8_t send_data[3] = {0};
	
    send_data[0] = Bl_Flt_Reg[reg]; 
    send_data[1] = send_value & 0xff; /*8 bit LSB of Data*/
    send_data[2] = send_value >> 8;   /*8 bit MSB of Data*/
	
    LED_If_I2C_Write(BL_SLAVE_ADDR, send_data, 3); 
}

/*=====================================================================================================================
* Upward trac.:        Middle/led/led_if
* Method      :        LED_If_Fault_Read
* Visibility  :        public
* Description :        read led fault reg
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void LED_If_Fault_Read(void)                      
{
    /*read INTERRUPT_STATUS registers from LED IC*/
    for(uint8_t i = 0; i < BL_FLT_REG_NUM; i++)
    {
        LED_If_I2C_Read(BL_SLAVE_ADDR, Bl_Flt_Reg[i], &Bl_Flt_Status.Flt_Status[BL_REG_LEN*i], BL_REG_LEN); 
    }
}

#endif /* LED_MODULE */
