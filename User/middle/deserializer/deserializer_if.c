/******************************************************************************
*  Name: deserializer_if.c
*  Description:
*  Project: WAVE2
*  Auther: Li Yanfeng (Ported from INFO4)
*  MCU: atmel samc21n18a
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (DESERIALIZER_MODULE == 1)

#include "deserializer_if.h"
#include "../../driver/delay.h"
#include "../../driver/gpio.h"
#include <string.h>
#include <stdlib.h>
#include "../Device/samc21n18a.h"
#include "../../driver/iic_slave.h"
#include "../../driver/iic_master.h"
#include "../../driver/tc.h"
#include "../../common/buffer.h"

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

static uint16_t BlKeyData = 0;
static uint8_t BlKeyFlag = 0;

static uint16_t Link_Time = 0;
static Elementype Cmd_Data;
static Buffer Comm_Data_Buffer;
static Elementype Data_Buffer[BUFFER_LENGTH] = {0};
volatile static SComm_Data Send_Buffer = {NULL, 0, 0};

volatile static uint8_t Cmd_Data_Ready = 0;
volatile static uint8_t Command_Start = 1;
volatile static uint8_t Command_End = 0;
volatile static uint8_t Command_Index = 0;
volatile static uint8_t Command_Error = 0;


/*988 self pattern generate*/

uint8_t Deser_Init_Data[][INIT_LEN] = {
{0x46,0x08},
{0x0E,0x02},
{0x46,0x00},
{0x0E,0x01},
{0x40,0x2C},
{0x41,0xB3},
{0x42,0x22},
{0x41,0x81},
{0x42,0x60},
{0x41,0x82},
{0x42,0x02},
{0x41,0x82},
{0x42,0x5E},
{0x40,0x3C},
{0x41,0x00},
{0x42,0x80},
{0x41,0x01},
{0x42,0x09},
{0x41,0x02},
{0x42,0x07},
{0x41,0x03},
{TIME_DELAY,0x06},
{0x0E,0x01},
{0xB1,0x01},
{0xB2,0xF6},
{0xB3,0x46},
{0xB4,0x04},
{0xB5,0xC0},
{0xB6,0x7A},
{0xB7,0x10},
{0x0E,0x01},
{0x40,0x50},
{0x41,0x01},
{0x42,0x0C},
{0x41,0x02},
{0x42,0x86},
{0x41,0x03},
{0x42,0xDE},
{0x42,0x0D},
{0x42,0x24},
{0x42,0x05},
{0x42,0x48},
{0x42,0x0D},
{0x42,0x14},
{0x42,0x05},
{0x42,0x1E},
{0x42,0x00},
{0x42,0x01},
{0x42,0x00},
{0x42,0x69},
{0x42,0x00},
{0x42,0x08},
{0x42,0x00},
{0x42,0x00},
{0x41,0x00},
{0x42,0x95},
{0x49,0xA4},
{0x4A,0x01},
{0x48,0x03},
{TIME_DELAY,0x06},
{0x49,0xA4},
{0x4A,0x01},
{0x4B,0x20},
{0x4C,0x00},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xB8},
{0x4A,0x01},
{0x48,0x03},
{TIME_DELAY,0x06},
{0x49,0xB8},
{0x4A,0x01},
{0x4B,0x04},
{0x4C,0x00},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xAC},
{0x4A,0x01},
{0x4B,0xE3},
{0x4C,0x84},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xB4},
{0x4A,0x01},
{0x4B,0x00},
{0x4C,0x80},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xAC},
{0x4A,0x01},
{0x48,0x03},
{TIME_DELAY,0x06},
{0x49,0xC8},
{0x4A,0x01},
{0x4B,0x00},
{0x4C,0x00},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xB0},
{0x4A,0x01},
{0x4B,0x40},
{0x4C,0x00},
{0x4D,0x32},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xC8},
{0x4A,0x00},
{0x4B,0x05},
{0x4C,0x40},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xBC},
{0x4A,0x01},
{0x4B,0xF6},
{0x4C,0x09},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xC0},
{0x4A,0x01},
{0x4B,0x00},
{0x4C,0x00},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0xC4},
{0x4A,0x01},
{0x4B,0x0F},
{0x4C,0x00},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
{0x49,0x84},
{0x4A,0x00},
{0x4B,0x01},
{0x4C,0x00},
{0x4D,0x00},
{0x4E,0x00},
{0x48,0x01},
};

#if DAISY_CHAIN
uint8_t Daisy_Chain_PLL[][INIT_LEN] = {
    {0x40, 0x0C}, /*Page 3 on 988*/
    {0x41, 0x05},
    {0x42, 0x7D}, /*NCount [7:0] = 6.75G*/
    {0x41, 0x13},
    {0x42, 0x90}, /*Post Divider = 2*/
    {0x0E, 0x01},
    {0x90, 0x0A}, /*BC Oversampling to 20*/

    /*Setting Daisy PLL_TX_1 to 6.75GHz*/
    {0x40, 0x0C}, /*Page 3 on 988*/
    {0x41, 0x45},
    {0x42, 0x7D}, /*NCount [7:0]*/
    {0x41, 0x53},
    {0x42, 0x90}, /*Post Divider = 2*/

    {0x0E, 0x12},
    {0x90, 0x0A}, /*BC Oversampling to 20*/
    {0x0E, 0x01},

    /*soft reset PLL_TX*/
    {0x01, 0x30},
};
uint8_t Daisy_Chain_LinkLayer[][INIT_LEN] = {
    /*Enabling Daisy Chain Video to Stream_0*/
    {0x0E, 0x01}, /*Set TX Port select*/
    {0xD1, 0x3F}, /*Set stream mapping*/

    /*page 17*/
    {0x40, 0x44}, /*TX Link Layer*/
    {0x41, 0x20},
    {0x42, 0x55}, /*Set 24 bit mode*/
    {0x41, 0x06},
    {0x42, 0x3C}, /*Set number of slots to 60 for video stream 0*/
    {0x41, 0x01},
    {0x42, 0x01}, /*Enable stream 0 to link layer 0*/
    {0x41, 0x00},
    {0x42, 0x00},
    {0x41, 0x00},
    {0x42, 0x02}, /*Enable link layer and en_new_t slot0*/
};
uint8_t Reset_LinkLayer[][INIT_LEN] = {
    /*page17,LINK_LAYER_0_EN*/
    {0x41, 0x00},
    {0x42, 0x01},
};
#endif
/**********************************************************************************************
* Local functions
**********************************************************************************************/
static IIC_Slave_Command_Callback IIC_Cmd_Callback = (IIC_Slave_Command_Callback)(0); /*redtip #bug 168*/
static IIC_Slave_Etm_Callback ETM_CMD_CALLBACK = (IIC_Slave_Etm_Callback)(0);
static IIC_Command_Parsing_Callback IIC_Parsing_CallBack = (IIC_Command_Parsing_Callback)(0);

/*i2c slave interrupt funtion which deal with received command from vcu*/
static void Des_If_Interrupt_Handle(uint16_t status_reg, uint8_t intflag_reg);

/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Slave_Init(void)
* Visibility  :        public
* Description :        deserializer is set as i2c slave
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Slave_Init(void)
{
    /*init slave driver and set irq callback funtion*/
    IIC_Slave_SetCallBack(Des_If_Interrupt_Handle);
//    GPIO_Set_Function(CTP_SDA, PINMUX_PA16D_SERCOM3_PAD0);
//    GPIO_Set_Function(CTP_SDL, PINMUX_PA17D_SERCOM3_PAD1);

    IIC_Slave_Init(CTP_SERCOM, 1);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Master_Init(void)
* Visibility  :        public
* Description :        disable deserializer i2c slave mode
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Slave_DeInit(void)
{
    GPIO_Set_Direction(CTP_SDA, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(CTP_SDA, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(CTP_SDA, false);
    GPIO_Set_Direction(CTP_SDL, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(CTP_SDL, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(CTP_SDL, false);

    IIC_Slave_DeInit(CTP_SERCOM, 1);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Iic_On(void)
* Visibility  :        public
* Description :        set sercom to i2c master mode
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Master_Init(void)
{
    /*init secrom i2c module*/
    IIC_Master_Init(DES_SERCOM, SERCOM_DES);

    /*set gpio to i2c function*/
    GPIO_Set_Function(DES_SDA1, PINMUX_PB16C_SERCOM5_PAD0);
    GPIO_Set_Function(DES_SCL1, PINMUX_PB17C_SERCOM5_PAD1);

    /*enable secrom i2c module*/
    IIC_Master_Enable_Bit(DES_SERCOM);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Init(void)
* Visibility  :        public
* Description :        Deinitialize sercom4 i2c module
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Master_DeInit(void)
{
    /*disable i2c of secrom*/
    IIC_Master_DeInit(DES_SERCOM, SERCOM_DES);

    /*release SDA pin*/
    GPIO_Set_Direction(DES_SDA1, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(DES_SDA1, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(DES_SDA1, false);

    /*release SCL pin*/
    GPIO_Set_Direction(DES_SCL1, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(DES_SCL1, GPIO_PIN_FUNCTION_OFF);
    GPIO_Set_Level(DES_SCL1, false);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Standby(void)
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
static void Des_If_Time_Delay(uint16_t ms)
{
    Delay_Ms(ms);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_TurnOff(void)
* Visibility  :        public
* Description :        power off deserializer hardware
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Power_Off(void)
{
    Des_If_Set_PDB_Level(false);
    Des_If_Slave_DeInit();
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_TurnOn(void)
* Visibility  :        public
* Description :        enable deserializer hardware when power on
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Power_On(void)
{
    Des_If_Set_PDB_Level(true);
    Des_If_Slave_Init();
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_GPIO_Init(void)
* Visibility  :        public
* Description :        init deserializer gpio pin
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_GPIO_Init(void)
{
    /*PDB is enabled at power_if.c because Hw timing request*/

    /*set LOCK pin,input function, pull up*/
    GPIO_Set_Direction(LOCK_DES, GPIO_DIRECTION_IN);
    GPIO_Set_Pull_Mode(LOCK_DES, GPIO_PULL_UP);
    GPIO_Set_Function(LOCK_DES, GPIO_PIN_FUNCTION_OFF);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_GPIO_DeInit(void)
* Visibility  :        public
* Description :        release deserializer gpio pin
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_GPIO_DeInit(void)
{
    GPIO_Set_Level(PDB_DES, false);
    GPIO_Set_Direction(PDB_DES, GPIO_DIRECTION_OFF);
    GPIO_Set_Pull_Mode(PDB_DES, GPIO_PULL_OFF); /*according to HSIS20200330  Hiz*/
    GPIO_Set_Function(PDB_DES, GPIO_PIN_FUNCTION_OFF);
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Des_If_Buffer_Init
* Visibility  :        public
* Description :        init command buffer
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Buffer_Init(void)
{
#if (BUFFER_COM == 1)
    Buffer_Init(&Comm_Data_Buffer, (pBufferData)Data_Buffer, BUFFER_LENGTH);
#endif
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Lock_TimeCheck
* Visibility  :        public
* Description :        record link lock time
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Lock_TimeCheck(void)
{
    if (Link_Time < 65535) /*uint16_t type*/
    {
        /*unit is 1ms*/
        Link_Time++;
    }
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Timer_Begin
* Visibility  :        public
* Description :        enable link lock time check
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Timer_Begin(void)
{
    /*record time*/
    Link_Time = 0;
    /*link timeout handler, period is 1ms*/
   // TC4_Set_Callback(Des_If_Lock_TimeCheck); //port it back later
   // TC_Timer_Enable(TC4);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Timer_End
* Visibility  :        public
* Description :        disable link lock time check
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Timer_End(void)
{
    TC_DeInit(TC4);
    Link_Time = 0;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Des_If_Init
* Visibility  :        public
* Description :        init deserializer module
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Init(void)
{
    Des_If_Set_PDB_Level(true);
    Des_If_Buffer_Init();

    /*init i2c module*/
    Des_If_Slave_Init();
    Des_If_Master_Init();

    /*init deserializer gpio*/
    Des_If_GPIO_Init();
    /* init 988 register*/
    Des_If_Register_Config();

    
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Des_If_Standby
* Visibility  :        public
* Description :        enter standby mode
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Standby(void)
{
    /*release i2c and gpio*/
    Des_If_Slave_DeInit();
    Des_If_Master_DeInit();
    Des_If_GPIO_DeInit();
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_I2c_Write(uint8_t *data, uint16_t length,uint8_t send_stop)
* Visibility  :        static
* Description :        deserializer i2c write interface
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static enum I2c_Status Des_If_I2c_Write(uint8_t *data, uint16_t length)
{
    return IIC_Master_Write(DES_SERCOM, DES_ADDRESS, data, length, SEND_STOP);
}

#if DES_I2C_TEST
/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_I2C_Read(uint8_t page_num,uint8_t reg_addr, uint8_t *buff, uint16_t length)
* Visibility  :        static
* Description :        deserializer Indirect page i2c read interface
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void Des_If_I2C_Indirect_Read(uint8_t page_num, uint8_t reg_addr, uint8_t *buff, uint16_t length)
{
    uint8_t data[2] = {0};

#define IND_ACC_CTL 0x40
#define IND_ACC_ADDR 0x41
#define IND_ACC_DATA 0x42

    /*select the desired register block*/
    data[0] = IND_ACC_CTL;
    data[1] = page_num | 0x3; /*read enable and auto increasing address*/
    Des_If_I2c_Write(data, 2);

    /*set the register offset*/
    data[0] = IND_ACC_ADDR;
    data[1] = reg_addr;
    Des_If_I2c_Write(data, 2);

    /*Read from the IND_ACC_DATA register*/
    data[0] = IND_ACC_DATA;
    for (uint8_t i = 0; i < length; i++)
    {
        IIC_Master_Write(DES_SERCOM, DES_ADDRESS, data, 1, SEND_NSTOP);
        IIC_Master_Read(DES_SERCOM, DES_ADDRESS, &buff[i], 1, SEND_STOP);
    }
}
#endif /*DES_I2C_TEST*/

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_I2C_Read
* Visibility  :        public
* Description :        deserializer main page i2c read interface
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static enum I2c_Status Des_If_I2C_Read(uint8_t reg_addr, uint8_t *buff, uint16_t length)
{
    IIC_Master_Write(DES_SERCOM, DES_ADDRESS, &reg_addr, 1, SEND_NSTOP);
    return IIC_Master_Read(DES_SERCOM, DES_ADDRESS, buff, length, SEND_STOP);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Wait_Link_Lock
* Visibility  :        public
* Description :        wait link lock to reset digital block except
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
static void Des_If_Wait_Link_Lock(void)
{
    uint8_t lock_status = 0;
    uint16_t Link_Timeout = 5000; /*timeout is 5000ms*/
    /*reset ctrl Register*/
    uint8_t soft_reset[2] = {0x01, 0x01};
    enum I2c_Status i2c_status = STATUS_NULL;

    /*init timeout check begin*/
    Des_If_Timer_Begin();

    /*digital reset when link lock or end when timeout*/
    while (Link_Time < Link_Timeout)
    {
        /*read Deserializer Lock Status from 0x54*/
        i2c_status = Des_If_I2C_Read(0x54, &lock_status, 1);
        if (STATUS_NULL == i2c_status)
        {
            if (lock_status & 0x01)
            {
                /*Resets the entire digital block except registers when lock*/
                i2c_status = Des_If_I2c_Write(soft_reset, 2);
                if (STATUS_NULL == i2c_status)
                    break;
            }
        }
        Des_If_Time_Delay(1);
    }

    /*restart to record time*/
    Link_Time = 0;

    /*wait init success until 45ms timeout*/
    while (Link_Time < 45)
    {
        /*reseverd. read 988 reg to check whether init success*/
        Des_If_Time_Delay(1);
    }
    /*init timeout check end*/
    Des_If_Timer_End();
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Register_Config(void)
* Visibility  :        public
* Description :        config 988 chip register
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Register_Config(void)
{
#if DES_I2C_TEST
#define DES_READ_LEN 5
    uint8_t buff[DES_READ_LEN] = {0};
#endif
#if DES_PATTERN_MODE
    /*Digital Reset All*/
    uint8_t send_data[2] = {0x01, 0x02};
    enum I2c_Status status = STATUS_NULL;
    Des_If_I2c_Write(send_data, 2);
    Des_If_Time_Delay(6);
    /*Deserializer 984 init */
    for(uint8_t i = 0; i < sizeof(Deser_Init_Data)/sizeof(Deser_Init_Data[0]); i++)
    {
        
        /*followed data is 988 reg values*/
        if (Deser_Init_Data[i][0] != TIME_DELAY)
        {
            send_data[0] = Deser_Init_Data[i][0] & 0xff;
            send_data[1] = Deser_Init_Data[i][1] & 0xff;
            /*retry init 3 times when write fail*/
            for (uint8_t retry = 0; retry<RETRY_NUM; retry++)
            {
                status = Des_If_I2c_Write(send_data, 2);
                if (STATUS_NULL == status)
                    break;
            }
        }else
        {   /*followed data is delay time*/
            Des_If_Time_Delay(Deser_Init_Data[i][1]);
        }
    }
#if DAISY_CHAIN
    Des_If_Time_Delay(1);
    for (uint8_t i = 0; i < sizeof(Daisy_Chain_PLL) / sizeof(Daisy_Chain_PLL[0]); i++)
    {
        Des_If_I2c_Write(Daisy_Chain_PLL[i], 2);
    }
    Des_If_Time_Delay(1);
    for (uint8_t i = 0; i < sizeof(Daisy_Chain_LinkLayer) / sizeof(Daisy_Chain_LinkLayer[0]); i++)
    {
        Des_If_I2c_Write(Daisy_Chain_LinkLayer[i], 2);
    }
    Des_If_Time_Delay(1);
    for (uint8_t i = 0; i < sizeof(Reset_LinkLayer) / sizeof(Reset_LinkLayer[0]); i++)
    {
        Des_If_I2c_Write(Reset_LinkLayer[i], 2);
    }
#endif
#endif /*DES_LOCAL_CONFIG*/

    /*wait until link lock*/
    //Des_If_Wait_Link_Lock();

#if DES_I2C_TEST
    Des_If_Time_Delay(5);
    /*readback test*/
    Des_If_I2C_Indirect_Read(0x2c, 0x08, buff, DES_READ_LEN);
#endif
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Set_PDB_Level(bool level)
* Visibility  :        public
* Description :        set PDB pin level
* Parameters  :        bool level
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Set_PDB_Level(bool level)
{
    GPIO_Set_Direction(PDB_DES, GPIO_DIRECTION_OUT);
    GPIO_Set_Function(PDB_DES, GPIO_PIN_FUNCTION_OFF);

    if (level)
        GPIO_Set_Level(PDB_DES, true);
    else
        GPIO_Set_Level(PDB_DES, false);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Get_Lock_Level(bool level)
* Visibility  :        public
* Description :        get deserializer lock pin status
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
bool Des_If_Get_Lock_Level(void)
{
    return GPIO_Get_Level(LOCK_DES);
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Set_Data_Buffer(void *pBuffer, uint16_t Buffer_Len)
* Visibility  :        public
* Description :        init send buffer
* Parameters  :        
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Set_Data_Buffer(void *pBuffer, uint16_t Buffer_Len)
{
    if (pBuffer == NULL)
    {
        return;
    }

    Send_Buffer.buffer_len = Buffer_Len;
    Send_Buffer.pbuffer = pBuffer;
    Send_Buffer.por = 0;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Get_Queue_Buffer(void)
* Visibility  :        public
* Description :        get command buffer
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
Buffer *Des_If_Get_Queue_Buffer(void)
{
    return &Comm_Data_Buffer;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Set_Ready_Flag(void)
* Visibility  :        public
* Description :        set global ready flag
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Set_Ready_Flag(void)
{
    Cmd_Data_Ready = 1;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Get_Key_Flag(void)
* Visibility  :        public
* Description :        Get Bl Key flag
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
uint8_t Des_If_Get_Key_Flag(void)
{
    return BlKeyFlag;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Clear_Key_Flag(void)
* Visibility  :        public
* Description :        Clear Bl Key flag
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Clear_Key_Flag(void)
{
    BlKeyFlag = 0;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Get_Key_Value(void)
* Visibility  :        public
* Description :        Get Bl Key value
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
uint16_t Des_If_Get_Key_Value(void)
{
    return BlKeyData;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_GetCmd_CallBack(IIC_Slave_Command_Callback pCallback)
* Visibility  :        public
* Description :        Find write Cmd dataLen,find the cmd from table
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int 
=====================================================================================================================*/
void Des_If_GetCmd_CallBack(IIC_Slave_Command_Callback pCallback) /*redtip #bug 168*/
{
    IIC_Cmd_Callback = pCallback;
}

/*=====================================================================================================================
* Upward trac.:        
* Method      :        Des_If_I2C_Parsing_CallBack
* Visibility  :        public
* Description :        Parsing callback to deal with i2c command
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int 
=====================================================================================================================*/
void Des_If_I2C_Parsing_CallBack(IIC_Command_Parsing_Callback pCallback)
{
    IIC_Parsing_CallBack = pCallback;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_GetCmd_CallBack(IIC_Slave_Command_Callback pCallback)
* Visibility  :        public
* Description :        Find write Cmd dataLen,find the cmd from table
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int 
=====================================================================================================================*/
void Des_If_GetEtm_CallBack(IIC_Slave_Etm_Callback pCallback) /*redtip #bug 168*/
{
    ETM_CMD_CALLBACK = pCallback;
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Send_Data(void)
* Visibility  :        public
* Description :        send data to master
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Send_Data(void)
{
    uint8_t Dummy_Data = 0xff;
    uint16_t datatmp = Send_Buffer.por;

    if (Cmd_Data_Ready == 1)
    {
        IIC_Slave_WriteOneByte(CTP_SERCOM, Send_Buffer.pbuffer[datatmp]); /*QAC more than one read access to volatile objects between sequence points*/
    }
    else
    {
        IIC_Slave_WriteOneByte(CTP_SERCOM, Dummy_Data); /*dummy data*/
    }
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Async_Tx(void)
* Visibility  :        public
* Description :        send other data to master
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Async_Tx(void)
{
    uint16_t datatmp;

    datatmp = ++Send_Buffer.por;
    if ((Send_Buffer.buffer_len) && (datatmp != Send_Buffer.buffer_len)) /*QAC more than one read access to volatile objects between sequence points*/
    {
        IIC_Slave_WriteOneByte(CTP_SERCOM, Send_Buffer.pbuffer[datatmp]); /*QAC more than one read access to volatile objects between sequence points*/
    }
    else
    {
        Send_Buffer.por = 0;
        Send_Buffer.buffer_len = 0;
        IIC_Slave_WriteOneByte(CTP_SERCOM, 0xff); /*dummy data*/
    }
}
/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_I2C_handing
* Visibility  :        public
* Description :        deal with i2c command recevied from VCU
* Parameters  :        Elementype data
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_I2C_handling(Elementype data)
{
    /*consuming time is little, deal with it immediately*/
    if (ROM_Write_ID != data.command_id)
    {
        IIC_Parsing_CallBack(data);
    }
    else
    {
        /*write rom will consuming more time, so delay to deal with it when sched command task is running*/
        if (((Comm_Data_Buffer.rear + 1) % Comm_Data_Buffer.buffer_length) != Comm_Data_Buffer.front)
        {
            Comm_Data_Buffer.rear = (Comm_Data_Buffer.rear + 1) % Comm_Data_Buffer.buffer_length;
            Comm_Data_Buffer.data[Comm_Data_Buffer.rear] = data;
        }
    }
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Receive_Data(void)
* Visibility  :        public
* Description :        receive data from master
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Receive_Data(void)
{
    uint8_t Cmd = 0;
    uint8_t Rxdata = 0;
    int Cmd_Len = -1;

    if ((Command_Error == 0) && (Command_End == 0))
    {
        if (Command_Start == 1)
        {
            Command_Index = 0;
            IIC_Slave_ReadOneByte(CTP_SERCOM, &Cmd);

            Cmd_Len = IIC_Cmd_Callback(Cmd); /*redtip #bug 168 */
            if (Cmd_Len < 0)
            {
                Command_Error = 1; /*not find command*/
            }
            else
            {
                /* read comm cmd */
                Command_Error = 0;
                memset(&Cmd_Data, 0x00, sizeof(Elementype));
                Cmd_Data.command_id = Cmd;
                Cmd_Data.data_len = Cmd_Len;
                Command_Start = 0;
                BlKeyFlag = 0;
                if (Cmd_Len == 0)
                {
                    Des_If_I2C_handling(Cmd_Data);
                    memset(&Cmd_Data, 0x00, sizeof(Elementype));
                    Command_End = 1;
                }
            }
        }
        else
        {
            /* read comm data */
            IIC_Slave_ReadOneByte(CTP_SERCOM, &Rxdata);
            Cmd_Data.data[Command_Index] = Rxdata;
/* EEPROM_WRITE_CMD command first byte(length), update data length */
#if (ETM_MODULE == 1)
            if (((ETM_WRITE_CMD == Cmd_Data.command_id) || (ETM_READ_CMD == Cmd_Data.command_id)) && (!Command_Index))
            {
                Cmd_Len = ETM_CMD_CALLBACK(Cmd_Data);
                if (Cmd_Len > 0)
                {
                    Cmd_Data.data_len = Cmd_Len;
                }
                else
                { /*do nothing*/
                }
            }
            else
            { /*do nothing*/
            }
#endif

            if (Command_Index == (Cmd_Data.data_len - 1))
            {
                /* boot command */
                if (Cmd_Data.command_id == BOOT_ACCESS_CMD)
                {
                    BlKeyData = (Cmd_Data.data[1] << 8) + Cmd_Data.data[0];
                    BlKeyFlag = 1;
                }
                else
                {
                    Des_If_I2C_handling(Cmd_Data);
                }
                memset(&Cmd_Data, 0x00, sizeof(Elementype));
                Command_End = 1;
            }

            Command_Index++;
        }
    }
    else
    {
        /*dummy read*/
        IIC_Slave_ReadOneByte(CTP_SERCOM, &Rxdata);
    }
}

/*=====================================================================================================================
* Upward trac.:        middle/deserializer/deserializer_if
* Method      :        Des_If_Interrupt_Handle(uint16_t status_reg, uint8_t intflag_reg)
* Visibility  :        public
* Description :        recevied or send command from host
* Parameters  :        void
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
=====================================================================================================================*/
void Des_If_Interrupt_Handle(uint16_t status_reg, uint8_t intflag_reg)
{
    if (intflag_reg & ERROR_FLAG)
    {
        /* TD */
        return;
    }
    else if (intflag_reg & AMATCH_FLAG)
    {
        /* slave writes to Master */
        if (status_reg & DIR_STATUS)
        {
            Des_If_Send_Data();
        }
        /* slave  read from Master */
        else
        {
            Command_Start = 1;
            Command_End = 0;
            Command_Error = 0;
            if (Cmd_Data_Ready == 1)
            {
                Send_Buffer.buffer_len = 0;
                Cmd_Data_Ready = 0;
            }
        }
    }
    else if (intflag_reg & DRDY_FLAG)
    {
        /*slave receive data from master*/
        if (0 == (status_reg & DIR_STATUS))
        {
            Des_If_Receive_Data();
        }
        else
        {
            Des_If_Async_Tx();
        }
    }
    else
    {
        /* do nothing */
    }
}
#endif /* DESERIALIZER_MODULE */
