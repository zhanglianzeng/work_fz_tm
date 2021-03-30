/******************************************************************************
*  Name: app_i2c_etm.c
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#include <string.h>
#include <stdlib.h>

#if ((ETM_MODULE == 1) && (DESERIALIZER_MODULE == 1))
#include "app_i2c_etm.h"
#include "../../middle/sys/sys_if.h"

#if (DESERIALIZER_MODULE == 1)
#include "../../middle/deserializer/deserializer_if.h"
#endif

#if (EEPROM_MODULE == 1)
#include "../eeprom/app_eeprom.h"
#endif

#if (LCD_MODULE == 1)
#include "../../middle/lcd/lcd_if.h"
#endif

#if (LED_MODULE == 1)
#include "../../middle/led/led_if.h"
#include "../led/app_led_ctrl.h"
#endif

#if (THERMAL_MODULE == 1)
#include "../thermal/app_thermal_manage.h"
#endif

#if (POWER_MODULE == 1)
#include "../power/app_power_manage.h"
#endif

#if (CRC_COM == 1)
#include "../../common/crc.h"
#endif

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
static uint8_t  Etm_Read_Data[ETM_READ_LEN] ={0};     /*etm response data buffer*/
static uint8_t  abyLcd_Config_Page[2] = {0};          /*source ic address and page address*/
static uint8_t  byIc_Num;                             /*source ic number*/

ENUM_ETM enumEtm=NORMAL_MODE;                                     /*etm work mode*/

static ETMHANDLE const Etm_Table[ETM_TABLE_LENTH] =     
{
    {0x00, 2, 1, 1,CI2CEtm_GetHardwareVersion},       /*hardware*/
    {0x01, 2, 1, 1,CI2CEtm_GetFirmwareVersion},       /*firmware*/
    {0x05, 16, 17, 1,CI2CEtm_ManufactureTraceability},/*Manufacture Trace ability*/
    {0x06, 4, 5, 1,CI2CEtm_ProgramDate},              /*program date*/
    {0x10, 1, 2, 1,CI2CEtm_ConfigurationFlag},        /*Configuration Flag*/
    {0x14, 2, 1, 1,CI2CEtm_LedDimmingPWMValue},       /*led dimming PWM duty cycle value*/
    {0x15, 8, 1, 1,CI2CEtm_ButtonDimmingPWMValue},    /*button dimming PWM duty cycle value*/
    {0x21, 0, 2, 1,CI2CEtm_DaisyChain},               /*daisy chain*/
    {0x25, 1, 1, 1,CI2CEtm_ClearAllDTCs},             /*clear all dtc*/
    {0x26, 1, 1, 1,CI2CEtm_PCBNTCValue},              /*PCB board temperature value*/
    {0x27, 1, 1, 1,CI2CEtm_LEDNTCValue},              /*LED baklight temperature value*/
    //{0x28, 2, 3, 3,CI2CEtm_LCMConfiguration},         /*LCM configuration*/
    //{0x29, 2, 3, 2,CI2CEtm_LCMConfigDoubleCheck},     /*LCM configuration double check*/
    {0xFE, 1, 2, 1,CI2CEtm_Entry},                    /*ETM entry*/
};
/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/

/*=====================================================================================================================
* Upward trac.:        APP/etm/App_i2c_etm
* Method      :        CI2CEtm_Get_CmdLen(uint8_t cmd)
* Visibility  :        private
* Description :        This function will return etm command data write or read length
* Parameters  :        Elementype cmd
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int 
* ===================================================================================================================*/
static int CI2CEtm_Get_CmdLen(Elementype cmd)   
{
    for (uint8_t index=0; index < ETM_TABLE_LENTH; index++)
    {
        if (Etm_Table[index].command == cmd.data[0])  /*check etm command id*/
        {
            if(ETM_Write_ID == cmd.command_id)
                return Etm_Table[index].data_len_wr;  /*return write data length*/
            else
            {/*do nothing*/}
            
            if(ETM_Read_ID == cmd.command_id)         
                return Etm_Table[index].data_len_response;  /*return read data length*/
            else
            {/*do nothing*/}              
        }
        else
        {/*do nothing*/}
    }

    return -1;
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_Fill_Data(Elementype etm_data, uint8_t index, uint8_t *pdata)
* Visibility  :        private
* Description :        This function fills the response data to the buffer
* Parameters  :        Elementype etm_data, uint8_t index, uint8_t *pdata
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CI2CEtm_Fill_Data(Elementype etm_data, uint8_t index, uint8_t *pdata)
{
#if (DESERIALIZER_MODULE == 1)

    uint8_t  data_index = 0;

    if (ETM_READ_LEN - 2 < Etm_Table[index].data_len_rd)
    {
        return;
    }
    else
    {/*do nothing*/}
    memset(Etm_Read_Data, 0xFF, ETM_READ_LEN);      /*clear Etm_Read_Data to 0xFF*/
    Etm_Read_Data[data_index++] = ETM_RESPONSE_ID;  /*set ETM_RESPONSE_ID*/
    Etm_Read_Data[data_index++] = Etm_Table[index].command;   /*set etm command id*/
    memcpy(&Etm_Read_Data[data_index], pdata, Etm_Table[index].data_len_rd); /*copy response data to Etm_Read_Data*/
#endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_GetHardwareVersion(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response the hardware version to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_GetHardwareVersion(Elementype etm_data, uint8_t index)
{
    if(ETM_RESPONSE_ID == etm_data.command_id)
    {
    #if (EEPROM_MODULE == 1)
        eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
        CI2CEtm_Fill_Data(etm_data, index, (uint8_t*)pGroup1->data.HW_Version);  /*response HW_Version to master*/ 
    #endif
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_GetFirmwareVersion(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response the firmware version to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_GetFirmwareVersion(Elementype etm_data, uint8_t index)
{
    if(ETM_RESPONSE_ID == etm_data.command_id)
    {
    #if (EEPROM_MODULE == 1)
        eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
        CI2CEtm_Fill_Data(etm_data, index, (uint8_t*)pGroup1->data.Firmware_Version);  /*response Firmware_Version to master*/ 
    #endif
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ManufactureTraceability(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, configure the manufacture trace ability
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_ManufactureTraceability(Elementype etm_data, uint8_t index)
{       
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
    if(ETM_REQUEST_ID == etm_data.command_id)
    {
        memcpy(pGroup1->data.Manufacturing_Traceability, &etm_data.data[1], (Etm_Table[index].data_len_wr - 1));
        CEeprom_Write_Factory_Config(pGroup1);  /*write Manufacturing_Traceability data from master to eeprom*/
    }
    else
    {
        CI2CEtm_Fill_Data(etm_data, index, (uint8_t*)pGroup1->data.Manufacturing_Traceability);  /*response Manufacturing_Traceability to master*/
    }
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ProgramDate(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, configure the program date, and write in eeprom
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_ProgramDate(Elementype etm_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
       
    if(ETM_REQUEST_ID == etm_data.command_id)
    {
        memcpy(pGroup1->data.Programing_Date, &etm_data.data[1], (Etm_Table[index].data_len_wr - 1));
        CEeprom_Write_Factory_Config(pGroup1);  /*write Programing_Date data from master to eeprom*/
    }
    else
    {
        CI2CEtm_Fill_Data(etm_data, index, (uint8_t*)pGroup1->data.Programing_Date);   /*response Manufacturing_Traceability to master*/
    }
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ConfigurationFlag(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response the configuration flag to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_ConfigurationFlag(Elementype etm_data, uint8_t index)
{
#if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
       
    if(ETM_REQUEST_ID == etm_data.command_id)
    {
        memcpy(&(pGroup1->data.Configuration_Flag), &etm_data.data[1], (Etm_Table[index].data_len_wr - 1));
        CEeprom_Write_Factory_Config(pGroup1);  /*write Programing_Date data from master to eeprom*/
    }
    else
    {
        CI2CEtm_Fill_Data(etm_data, index, &(pGroup1->data.Configuration_Flag));   
    }
#endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_LedDimmingPWMValue(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response current backlight pwm value to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_LedDimmingPWMValue(Elementype etm_data, uint8_t index)
{
    UNION_AULEXY_TO_HALFWORD datatmp;
    
    #if (DESERIALIZER_MODULE == 1)
    if(ETM_RESPONSE_ID == etm_data.command_id)
    {    
        #if (LED_MODULE == 1)
        datatmp.hdata = CLedCtrl_Get_PWMCurrent_Value();   /*get backlight brightness value*/
        #endif       
        CI2CEtm_Fill_Data(etm_data, index, datatmp.abyte); /*response backlight brightness value to master*/
    }
    else
    {/*do nothing*/}
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ButtonDimmingPWMValue(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response current button backlight pwm value to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_ButtonDimmingPWMValue(Elementype etm_data, uint8_t index)
{  
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ButtonDimmingPWMValue(Elementype cmd_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, configure the sub-display work mode
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_DaisyChain(Elementype etm_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    if(ETM_REQUEST_ID == etm_data.command_id)
    {   
        eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
        pGroup1->data.Daisy_Chain = etm_data.data[1];
        CEeprom_Write_Factory_Config(pGroup1);  /*write request data to eeprom*/
    }
    else
    {/*do nothing*/}
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ClearAllDTCs(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, clear DTC data
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_ClearAllDTCs(Elementype etm_data, uint8_t index)
{
    uint8_t response = 1;
    if(ETM_RESPONSE_ID == etm_data.command_id)
    {
        CI2CEtm_Fill_Data(etm_data, index, &response); /*to be defined*/
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_PCBNTCValue(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response PCB board temperature value to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_PCBNTCValue(Elementype etm_data, uint8_t index)
{
    static uint8_t pcb_ntc_value;
    if(ETM_RESPONSE_ID == etm_data.command_id)
    {
    #if (THERMAL_MODULE == 1)
        pcb_ntc_value = CThermalManage_Get_PCB_NTC_Value(); /*get pcb temperature*/
    #endif
        CI2CEtm_Fill_Data(etm_data, index, &pcb_ntc_value); /*response pcb temperature to master*/
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_LEDNTCValue(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, response LED temperature value to master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_LEDNTCValue(Elementype etm_data, uint8_t index)
{
    static uint8_t led_ntc_value;
    
    if(ETM_RESPONSE_ID == etm_data.command_id)
    {
    #if (THERMAL_MODULE == 1)
        led_ntc_value = CThermalManage_Get_LED_NTC_Value(); /*get backlight brightness temperature*/
        CI2CEtm_Fill_Data(etm_data, index, &led_ntc_value); /*response backlight brightness temperature to master*/
    #endif
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_LCMConfiguration(Elementype cmd_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, configure LCM 
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
#if (LCD_MODULE == 2)
void CI2CEtm_LCMConfiguration(Elementype etm_data, uint8_t index)
{      
    uint8_t read_data = 0;
    uint8_t response[2] = {0};
    uint8_t ic_remainder = 0;
    uint8_t data[2] = {0x1F,0xAA};
    if(ETM_REQUEST_ID == etm_data.command_id)
    {              
        ic_remainder = etm_data.data[1] % ADDR_OFFSET;  /*Take the remainder of address*/
        if(0 == ic_remainder)  /*ic address*/
        {
            byIc_Num = etm_data.data[1] / ADDR_OFFSET; /*get ic number*/
            abyLcd_Config_Page[0] = etm_data.data[1];
            abyLcd_Config_Page[1] = etm_data.data[2];  /*page address*/
        }
        else                   /*register address*/
        {
            Lcd_If_SPI_Select_Page(byIc_Num, abyLcd_Config_Page[1]);
            data[0] |= byIc_Num*ADDR_OFFSET;        
            Lcd_If_SPI_Write_Reg(data,2);    /*write ic and page*/        
            Lcd_If_SPI_Write_Reg(&etm_data.data[1],2);  /*wirte data to specified register*/
        }
    }
    else if(ETM_RESPONSE_ID == etm_data.command_id) 
    {
        Lcd_If_SPI_Select_Page(byIc_Num, abyLcd_Config_Page[1]);  /*choose ic and page*/
        data[0] |= byIc_Num*ADDR_OFFSET;        
        Lcd_If_SPI_Write_Reg(data,2);   /*write ic and page*/  
        Lcd_If_SPI_Read(etm_data.data[1],&read_data);  /*read data from specified register*/
        response[0] = etm_data.data[1];
        if(etm_data.data[2] == read_data)  /*judge write data succeed or not*/
            response[1] = 0;
        else
            response[1] = 1;
        
        CI2CEtm_Fill_Data(etm_data, index, response);
    }
    else
    {/*do nothing*/}      
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_LCMConfigDoubleCheck(Elementype cmd_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, LCM configure data double check
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_LCMConfigDoubleCheck(Elementype etm_data, uint8_t index)
{  
    static uint8_t ic_remainder;
    uint8_t response[2] = {0};
    uint8_t data[2] = {0x1F,0xAA};
    
    if(ETM_REQUEST_ID == etm_data.command_id)
    {       
        ic_remainder = etm_data.data[1] % ADDR_OFFSET;  /*Take the remainder of address*/
        if(0x00 == ic_remainder)  /*ic address*/
        {
            byIc_Num = etm_data.data[1] / ADDR_OFFSET;  /*get ic number*/
            abyLcd_Config_Page[0] = etm_data.data[1];
            abyLcd_Config_Page[1] = etm_data.data[2];   /*page address*/
            Lcd_If_SPI_Select_Page(byIc_Num, abyLcd_Config_Page[1]);  /*choose ic and page*/
        }
        else
        { 
            Lcd_If_SPI_Write_Reg(&etm_data.data[1],2);    /*wirte data to specified register*/          
        }
    }
    else if(ETM_RESPONSE_ID == etm_data.command_id) 
    {      
        Lcd_If_SPI_Select_Page(byIc_Num, abyLcd_Config_Page[1]);  /*choose ic and page*/
        data[0] |= byIc_Num*ADDR_OFFSET;        
        Lcd_If_SPI_Write_Reg(data,2);   /*write ic and page*/
        Lcd_If_SPI_Read(etm_data.data[1],&response[1]);  /*read data from specified register*/
        response[0] = etm_data.data[1];        
        CI2CEtm_Fill_Data(etm_data, index, response);  /*response data to master*/
    }
    else
    {/*do nothing*/}      
    
}
#endif
/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_Entry(Elementype cmd_data, uint8_t index)
* Visibility  :        public
* Description :        In etm mode, configure etm mode according to the data read from master
* Parameters  :        Elementype etm_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_Entry(Elementype etm_data, uint8_t index)
{
    static uint8_t response;        
    if(ETM_REQUEST_ID == etm_data.command_id)
    {          
        if(0x03 == etm_data.data[1])      /*enter etm mode*/
        {            
            enumEtm = ETM_MODE;
            response = 1;
        }
        else if(0x0C == etm_data.data[1])  /*exit etm mode*/
        {
            enumEtm = NORMAL_MODE;
            response = 0;
        }
    }
    else if (ETM_RESPONSE_ID == etm_data.command_id)
    {   
        CI2CEtm_Fill_Data(etm_data, index, &response); /*response data to master in etm mode*/
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_ClearEtmMode(void)
* Visibility  :        public
* Description :        This function will CLear ETM mode to normal mode.
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_ClearEtmMode(void)   
{
    enumEtm = NORMAL_MODE;
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_Init(void)
* Visibility  :        public
* Description :        This function will initialize ETM components.
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CEtm_Init(void)
{
    enumEtm = NORMAL_MODE;
    #if (DESERIALIZER_MODULE == 1)
    Des_If_GetEtm_CallBack(CI2CEtm_Get_CmdLen);
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtm_Handle(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        This function will analyze etm command, and handle it.
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
#if (BUFFER_COM == 1)
/*Execute the corresponding program in Etm_Table according to the etm command ID*/
void CI2CEtm_Handle(Elementype etm_data, uint8_t index)
{   
    POWER_ACTION_STATUS power_sta;
    power_sta=CPowerManage_Get_PowerStatus();
 
    if(FULLY_OPERATION_LEVEL!=power_sta)
    {
        CI2CEtm_ClearEtmMode();
    }

    if(ETM_MODE == enumEtm)
    {
        for (index=0; index < ETM_TABLE_LENTH; index++)
        {
            if (Etm_Table[index].command == etm_data.data[0])    /*check etm command id*/
            {           
                if (NULL != Etm_Table[index].EtmHandle)
                {
                    Etm_Table[index].EtmHandle(etm_data, index); /*call function of etm command id */
                }
                else
                {/*do nothing*/}
                break;
            }
            else
            {/*do nothing*/}              
        }       
    }
    else if(FULLY_OPERATION_LEVEL==power_sta)
    {        
        if (Etm_Table[ETM_TABLE_LENTH - 1].command == etm_data.data[0])  /*executed etm entry function*/
        {
            Etm_Table[ETM_TABLE_LENTH - 1].EtmHandle(etm_data, ETM_TABLE_LENTH - 1);
        }
        else
        {/*do nothing*/}
    }
}
#endif

/*=====================================================================================================================
* Upward trac.:        APP/ETM/App_i2c_etm
* Method      :        CI2CEtmResponse_Handle(Elementype etm_data, uint8_t index)
* Visibility  :        public
* Description :        This function will analyze etm response command, and response data to master.
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
#if (BUFFER_COM == 1)
/*Execute the corresponding program in Etm_Table according to the etm command ID, and response data to master*/
void CI2CEtmResponse_Handle(Elementype etm_data, uint8_t index)
{   
    POWER_ACTION_STATUS power_sta;
    power_sta=CPowerManage_Get_PowerStatus();
 
    if(FULLY_OPERATION_LEVEL!=power_sta)
    {
        CI2CEtm_ClearEtmMode();
    }

    if(ETM_MODE == enumEtm)                                      /*ETM_MODE allow execute etm command*/
    {       
        for (index=0; index < ETM_TABLE_LENTH; index++)
        {
            if (Etm_Table[index].command == etm_data.data[0])    /*check etm command id*/
            {           
                if (NULL != Etm_Table[index].EtmHandle)
                {
                    Etm_Table[index].EtmHandle(etm_data, index); /*call function of etm command id */
                }
                else
                {/*do nothing*/}
                break;
            }
            else
            {/*do nothing*/}              
        }       
        
    #if (DESERIALIZER_MODULE == 1)
        Des_If_Set_Data_Buffer((void *)(Etm_Read_Data), Etm_Table[index].data_len_rd +2);
        Des_If_Set_Ready_Flag();  /*enable send*/
    #endif  
    }

}
#endif
#endif




