/******************************************************************************
*  Name: app_iic_comm.c
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (DESERIALIZER_MODULE == 1)
#include <string.h>
#include <stdlib.h>
#include "../../middle/sys/sys_if.h"
#include "app_iic_comm.h"
#include "../../driver/delay.h"

#if (EEPROM_MODULE == 1)
#include "../eeprom/app_eeprom.h"
#endif

#if (ETM_MODULE == 1)
#include "../etm/app_i2c_etm.h"
#endif

#if (SLEEP_MODULE == 1)
#include "../../middle/sleep/sleep_if.h"
#endif

#if (LED_MODULE == 1)
#include "../../middle/led/led_if.h"
#include "../led/app_led_ctrl.h"
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
static uint8_t  Cmd_Read_Data[CMD_READ_LEN] ={0};  /*the buffer of response data to master*/
static STRUDIM_CRC  struDim_CrcCount;
static STRUDIM_DISPLAY_ID struDim_DisplayIdCount;

static COMMANDHANDLE Cmd_Table[] =     /*redtip #bug 168*/
{
    {HWVersion_ID,   2, 0, CI2CComm_GetHardVersion},          /*index 0, hardware version*/
    {FWVersion_ID,   2, 0, CI2CComm_GetFirmwareVersion},      /*index 1, software version*/
    {Touch_ID,       1, 0, CI2CComm_GetTouchId},              /*index 2, TouchId*/
    {Display_ID,     1, 0, CI2CComm_GetDisplayId},            /*index 3, DisplayId*/
    {DiagStatus_ID,  DIAGNOSTIC_STATUS1, 0, CI2CComm_GetDiagnosticStatus},     /*index 4, lcd diagnostic*/
    {DiagStatus1_ID, DIAGNOSTIC_STATUS2, 0, CI2CComm_GetDiagnosticStatus1},    /*index 5, led diagnostic*/
    {PWM_ID,         0, 5, CI2CComm_BacklightSetDuty},        /*index 6, set pwm duty cycle*/
    {ResetCounter_ID,2, 0, CI2CComm_GetResetCounter},         /*index 7, reset counter*/
    {PowerupCounter_ID, 2, 0, CI2CComm_GetPowerupCounter},    /*index 8, powerup counter*/
    {Reset_Mcu_ID,   0, 1, CI2CComm_ResetSystem},             /*index 9, reset system*/
    {Dim_Req_ID,     1, 0, CI2CComm_Dim_Request},             /*index 10, current dimming pwm value*/
    {Boot_ID,        0, 2, NULL},                             /*index 11, null*/
    {SW_Num_ID,      8, 0, CI2CComm_GetSoftwareNumber},       /*index 12, software part number*/
    {HW_Num_ID,      8, 0, CI2CComm_GetHardwareNumber},       /*index 13, hardware part number*/
    {Boot_Status1_ID,1, 0, CI2CComm_GetBootloaderStatus1},    /*index 14, boot status1*/
    {Config_Addr_ID, 0, 3, CI2CComm_ConfigAddr},              /*index 15, eeprom adress config*/
    {ROM_Read_ID,    24, 0, CI2CComm_ReadData},               /*index 16, read factory config data*/
    {ROM_Write_ID,   0, 24, CI2CComm_WriteData},              /*index 17, write factory config data*/
    
    #if (ETM_MODULE == 1)
    {ETM_Write_ID,   0, 1, CI2CEtm_Handle},                   /*index 18, etm request handle*/
    {ETM_Read_ID,    0, 1, CI2CEtmResponse_Handle},           /*index 19, etm response handle*/
    #endif
};

/**********************************************************************************************
* Local functions
**********************************************************************************************/
static CI2CComm_Dimming_Callback I2C_Dimming_Callback = (CI2CComm_Dimming_Callback)(0); 
/**********************************************************************************************
* Global functions
**********************************************************************************************/
/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_PushDim_CallBack(CI2CComm_Dimming_Callback pCallback) 
* Visibility  :        public
* Description :        This function will push dimming value to upper file app_iic_dimming.
* Parameters  :        CI2CComm_Dimming_Callback pCallback
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_PushDim_CallBack(CI2CComm_Dimming_Callback pCallback)  
{
    I2C_Dimming_Callback = pCallback;          /*Assign callback function to I2C_Dimming_Callback*/
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_Get_CmdLen(uint8_t cmd)
* Visibility  :        private
* Description :        This function will return length of response data to master
* Parameters  :        uint8_t cmd
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  int 
* ===================================================================================================================*/
static int CI2CComm_Get_CmdLen(uint8_t cmd)  
{
    uint8_t cmd_len = sizeof(Cmd_Table)/sizeof(COMMANDHANDLE);

    for (uint8_t index=0; index<cmd_len; index++)
    {
        if (Cmd_Table[index].command == cmd)
        {
            return Cmd_Table[index].data_len_wr;  /*return write command length*/
        }
        else
        {/*do nothing*/}
    }

    return -1;
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_Init(void)
* Visibility  :        public
* Description :        This function will initialize I2C etm mode and i2c callback function
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_Init(void)              
{
    #if (ETM_MODULE == 1)
    CI2CEtm_Init();
    #endif
    #if (DESERIALIZER_MODULE == 1)
    Des_If_GetCmd_CallBack(CI2CComm_Get_CmdLen);  /*declare callback function for get commmand write data length*/
    Des_If_I2C_Parsing_CallBack(CI2CComm_Handle); /*declare callback function for iic commnad handle function*/
    #endif
    struDim_CrcCount.byCrcSuc_Count = 0;
    struDim_CrcCount.byCrcErr_Count = 0;
    struDim_CrcCount.enumCrcMode = CRCNORMAL_MODE;
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_Handle(pBuffer buffer)
* Visibility  :        public
* Description :        This function will Analyze i2c command from master, and handle it  
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
#if (BUFFER_COM == 1)
void CI2CComm_Handle(Elementype cmd_data)
{  
    uint8_t  Cmd_Index = 0;  
    uint8_t cmd_len = sizeof(Cmd_Table)/sizeof(COMMANDHANDLE);
    
    for (uint8_t index=0; index < cmd_len; index++)
    {
        if (Cmd_Table[index].command == cmd_data.command_id) /*check command id*/
        {
            Cmd_Index = index;
            if (NULL != Cmd_Table[Cmd_Index].CommandHandle)
            {
                Cmd_Table[Cmd_Index].CommandHandle(cmd_data, Cmd_Index); /*call function*/
            }
            else
            {/*do nothing*/}
            
            break;
        }
        else
        {/*do nothing*/}          
    }
    
    if(CMD_NUM > Cmd_Index)   /*The last two commands do not require a response*/
    {
        Des_If_Set_Data_Buffer((void *)(Cmd_Read_Data), Cmd_Table[Cmd_Index].data_len_rd);
        Des_If_Set_Ready_Flag();  /*enable send*/
    }
    else
    {/*do nothing*/}
}
#endif
/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_Fill_Data(Elementype cmd_data, uint8_t index, uint8_t *pdata)
* Visibility  :        private
* Description :        This function fills the response data to the buffer
* Parameters  :        Elementype cmd_data, uint8_t index, uint8_t *pdata
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CI2CComm_Fill_Data(Elementype cmd_data, uint8_t index, uint8_t *pdata)
{
    uint8_t  data_len = 0;

    data_len = Cmd_Table[index].data_len_rd;  /*get read length*/
    if (CMD_READ_LEN < data_len)
    {
        /* check data len < 68 ;data format : data  */
        return;
    }
    else
    {/*do nothing*/}
    memset(Cmd_Read_Data, 0xFF, CMD_READ_LEN);   /*clear Cmd_Read_Data with 0xFF*/
    memcpy(Cmd_Read_Data, pdata, data_len);      /*copy data to response data buffer*/
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_GetHardVersion(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response hardware version to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetHardVersion(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID);  /*get hard version*/
    CI2CComm_Fill_Data(cmd_data, index, (uint8_t*)pGroup1->data.HW_Version);
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_GetFirmwareVersion(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Firmware version to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetFirmwareVersion(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID);  /*get Firmware version*/
    CI2CComm_Fill_Data(cmd_data, index, (uint8_t*)pGroup1->data.Firmware_Version);   /*write Firmware version to buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_GetTouchId(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Touch Id to master 
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetTouchId(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID);   /*get TouchId*/
    CI2CComm_Fill_Data(cmd_data, index, (uint8_t*)&pGroup1->data.Touch_Controller_ID);/*write TouchId to buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APP/IIC/App_iic_comm
* Method      :        CI2CComm_GetDisplayId(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response DisplayID to master 
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetDisplayId(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID);  /*get DisplayID*/
    CI2CComm_Fill_Data(cmd_data, index, (uint8_t*)&pGroup1->data.Display_ID);        /*write DisplayID to buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetDiagnosticStatus(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Diagnostic Status1 data to master 
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetDiagnosticStatus(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    Factory_Config* pConfig_Init = CEeprom_Get_Factory_Config();
    /*value range is 0xF0-0xFE*/
    if(0xF0 > pConfig_Init->data.Diagnostic_Status1[5])
    {
        pConfig_Init->data.Diagnostic_Status1[5] = 0xF0;
    }
    else
    {/*do nothing*/}
    
    pConfig_Init->data.Diagnostic_Status1[1] = pConfig_Init->data.Touch_Controller_ID;      /*get touch id*/
    pConfig_Init->data.Diagnostic_Status1[2] = pConfig_Init->data.Display_ID;
    pConfig_Init->data.Diagnostic_Status1[4] = 0;
    if(0xFE > pConfig_Init->data.Diagnostic_Status1[5])
    {
        pConfig_Init->data.Diagnostic_Status1[5] ++;
    }
    else
    {
        pConfig_Init->data.Diagnostic_Status1[5] = 0xF0;
    }  
    #if (CRC_COM == 1)
    pConfig_Init->data.Diagnostic_Status1[0] = CRC8_SAEJ1850((uint8_t *)(&pConfig_Init->data.Diagnostic_Status1[1]),(DIAGNOSTIC_STATUS1-1));  /*crc check*/  
    #endif    
    CI2CComm_Fill_Data(cmd_data, index, (uint8_t*)pConfig_Init->data.Diagnostic_Status1);   /*write Diagnostic data to buffer*/
    #endif
    pConfig_Init->data.Diagnostic_Status1[3] = 0;
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetDiagnosticStatus(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Diagnostic Status2 data to master.
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetDiagnosticStatus1(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)   
    Factory_Config* pConfig_Init = CEeprom_Get_Factory_Config();
    /*value range is 0xF0-0xFE*/
    if(0xF0 > pConfig_Init->data.Diagnostic_Status2[3])
    {
        pConfig_Init->data.Diagnostic_Status2[3] = 0xF0;
    }
    else
    {/*do nothing*/}
    
    pConfig_Init->data.Diagnostic_Status2[1] = pConfig_Init->data.Display_ID;
    if(0xFE > pConfig_Init->data.Diagnostic_Status2[3])    
    {
        pConfig_Init->data.Diagnostic_Status2[3] ++;
    }
    else
    {
        pConfig_Init->data.Diagnostic_Status2[3] = 0xF0;
    } 
    /*crc*/
    #if (CRC_COM == 1)
    pConfig_Init->data.Diagnostic_Status2[0] = CRC8_SAEJ1850((uint8_t *)(&pConfig_Init->data.Diagnostic_Status2[1]),(DIAGNOSTIC_STATUS2-1));    /*crc check*/
    #endif    
    CI2CComm_Fill_Data(cmd_data, index, (uint8_t*)pConfig_Init->data.Diagnostic_Status2); /*write Diagnostic data to buffer*/
    #endif
    pConfig_Init->data.Diagnostic_Status2[2] = 0; 
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_BacklightSetDuty(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will set led Backlight PWM Duty cycle value to FIDM
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_BacklightSetDuty(Elementype cmd_data, uint8_t index)
{
    uint16_t pwm_dec;
    #if (EEPROM_MODULE == 1)   
    Factory_Config* pConfig_Init = CEeprom_Get_Factory_Config();
    
    if(cmd_data.data[0] == CRC8_SAEJ1850(&cmd_data.data[1],4))  /*crc check*/
    {
        if(CRCNORMAL_MODE == struDim_CrcCount.enumCrcMode)      /*judge crc error occur 3 times or not*/
        {
            struDim_CrcCount.byCrcErr_Count = 0;
            if(cmd_data.data[1] == pConfig_Init->data.Display_ID) /*judge id error or not*/
            {
                if(DISPLAY_ID_NOMAL_MODE == struDim_DisplayIdCount.enumDisplayIdMode)
                {
                    struDim_DisplayIdCount.byDisplayIdErr_Count = 0;
                    pwm_dec = (cmd_data.data[3] << 8) + cmd_data.data[2];
                    I2C_Dimming_Callback(pwm_dec);
                }
		/*displat id error mode*/
                else
                {
                    if(struDim_DisplayIdCount.byDisplayIdSuc_Count < 2) /*judge displag id succeed 3 time in DISPLAY_ID_ERR_MODE*/
                    {
                        struDim_DisplayIdCount.byDisplayIdSuc_Count ++;
                    }
                    else
                    {
                        struDim_DisplayIdCount.enumDisplayIdMode = DISPLAY_ID_NOMAL_MODE;
                    }
                 }
            }
            /*Display_ID error*/
            else
            {
                /*Display_ID error*/
                if(struDim_DisplayIdCount.byDisplayIdErr_Count < 2)                 /*judge CRC error 3 times or not*/
                {
                    struDim_DisplayIdCount.byDisplayIdErr_Count ++;
                    struDim_DisplayIdCount.byDisplayIdSuc_Count = 0;
                }
                else
                {
                    struDim_DisplayIdCount.enumDisplayIdMode = DISPLAY_ID_ERR_MODE;           
                }          
            }           
         }
         /*CRC error mode*/
        else
        { 
            if(struDim_CrcCount.byCrcSuc_Count < 2)             /*judge crc succeed  3 times in CRCERR_MODE*/
                struDim_CrcCount.byCrcSuc_Count ++;
            else
                struDim_CrcCount.enumCrcMode = CRCNORMAL_MODE;
           }      
    }
    else
    {
        if(struDim_CrcCount.byCrcErr_Count < 2)                 /*judge CRC error 3 times or not*/
        {
            struDim_CrcCount.byCrcErr_Count ++;
            struDim_CrcCount.byCrcSuc_Count = 0;
        }
        else
        {
            struDim_CrcCount.enumCrcMode = CRCERR_MODE;           
        }          
    }  
    
    if ((CRCERR_MODE == struDim_CrcCount.enumCrcMode) || (DISPLAY_ID_ERR_MODE == struDim_DisplayIdCount.enumDisplayIdMode))            /*CRCERR_MODE PWM value is 15%*/
    {
        I2C_Dimming_Callback((uint16_t)((PWM_DEFAULT_CRC_VALUE << 16)/1000));  /*crc error 3 times, PWM value is 15%*/
    }
    else
    {/*do nothing*/}
    #endif    
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_Dim_Request(Elementype cmd_data, uint32_t index)
* Visibility  :        public
* Description :        This function will get current Backlight Duty
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_Dim_Request(Elementype cmd_data, uint8_t index)
{
    #if (LED_MODULE == 1)
    uint8_t datatmp = CLedCtrl_Get_PWMCurrent_Value() >> 8;
    CI2CComm_Fill_Data(cmd_data, index, &datatmp);
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_ResetSystem(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will force software reset
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_ResetSystem(Elementype cmd_data, uint8_t index)
{
    if (0x01 & cmd_data.data[0])
    {
        #if (TOUCH_MODULE == 1)
        Touch_If_Set_Rstn(false);/*touch control*/
        Delay_Ms(2);
        Touch_If_Set_Rstn(true);
        #endif
    }
    else if (0x02 & cmd_data.data[0])
    { 
        Sys_If_Reset();                  /*software reset*/
    }
    else
    {/*do nothing*/}
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetSoftwareNumber(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Software part Number to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetSoftwareNumber(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID); /*get Software_Part number*/
    CI2CComm_Fill_Data(cmd_data, index, pGroup1->data.Software_Part);               /*response Software_Part*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetHardwareNumber(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Hardware part number to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetHardwareNumber(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID);    /*get hardware_Part number*/
    CI2CComm_Fill_Data(cmd_data, index, pGroup1->data.Hardware_Part);                  /*response Hardware_Part*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetResetCounter(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Reset Counter value to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetResetCounter(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    Factory_Config* pConfig_Init = CEeprom_Get_Factory_Config();                            /*get Reset_Counter*/
    CI2CComm_Fill_Data(cmd_data, index, pConfig_Init->data.Reset_Counter);  /*write Reset_Counter to buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetPowerupCounter(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Powerup Counter value to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetPowerupCounter(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    Factory_Config* pConfig_Init = CEeprom_Get_Factory_Config();                               /*get Powerup_Counter*/
    CI2CComm_Fill_Data(cmd_data, index, pConfig_Init->data.Powerup_Counter);   /*write Powerup_Counter to buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_GetBootloaderStatus1(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will response Bootloader Status to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_GetBootloaderStatus1(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    eep_group1_ID_t* pGroup1 = (eep_group1_ID_t*)CEeprom_Get_data_addr(GROUP_1_ID);  /*get Bootloader Status*/  
    CI2CComm_Fill_Data(cmd_data, index, &(pGroup1->data.Bootloader_Status2));        /*write Bootloader Status to buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_ConfigAddr(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        to be defined
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_ConfigAddr(Elementype cmd_data, uint8_t index)
{
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_ReadData(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        response factory data to master
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_ReadData(Elementype cmd_data, uint8_t index)
{
    #if (EEPROM_MODULE == 1)
    uint8_t datatmp[24];
    Factory_Config pConfig_Init = CEeprom_Read_Factory_Config();  /*get factory config data from eeprom*/
    memcpy(datatmp,pConfig_Init.data.HW_Version,2);               /*get hardware version*/
    memcpy(&datatmp[2],pConfig_Init.data.Firmware_Version,2);     /*get firmware version*/
    datatmp[4] = pConfig_Init.data.Touch_Controller_ID;           /*get touchid version*/
    datatmp[5] = pConfig_Init.data.Display_ID;                    /*get displayid version*/
    memcpy(&datatmp[6],pConfig_Init.data.Software_Part,8);        /*get software part*/
    memcpy(&datatmp[14],pConfig_Init.data.Hardware_Part,8);       /*get hardware part*/
    CI2CComm_Fill_Data(cmd_data, index, datatmp);                 /*write data in iic send buffer*/
    #endif
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_WriteData(Elementype cmd_data, uint8_t index)
* Visibility  :        private
* Description :        This function will write factory configuration data recieve from master to eeprom
* Parameters  :        Elementype cmd_data, uint8_t index
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_WriteData(Elementype cmd_data, uint8_t index)
{
    uint16_t CRC = 0;
    uint16_t crc_load = 0;
    /* Data format : len + addr(0-1)+ data[0..len] + crc */    
    CRC = CRC16_CCITT(cmd_data.data, 22);
    crc_load = (cmd_data.data[22] << 8) + cmd_data.data[23];

    if (CRC == crc_load)
    {
        #if (EEPROM_MODULE == 1)
        eep_group1_ID_t* pGroup1 = CEeprom_Get_Factory_Config();
        memcpy(pGroup1->data.HW_Version,cmd_data.data,2);
        memcpy(pGroup1->data.Firmware_Version,&cmd_data.data[2],2);
        pGroup1->data.Touch_Controller_ID = cmd_data.data[4];
        pGroup1->data.Display_ID = cmd_data.data[5];
        memcpy(pGroup1->data.Software_Part,&cmd_data.data[6],8);
        memcpy(pGroup1->data.Hardware_Part,&cmd_data.data[14],8);
        CEeprom_Write_Factory_Config(pGroup1);                /*write data to eeprom, pGroup1 --> factory configuration data*/
        #endif
    }
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/iic/app_iic_comm
* Method      :        CI2CComm_Task(void)
* Visibility  :        public
* Description :        IIC command task function, the execute period is 10ms;
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
void CI2CComm_Task(void)
{
    #if (BUFFER_COM == 1)
    pBuffer buffer;
    buffer = Des_If_Get_Queue_Buffer();
    while (true != Buffer_IsEmpty(buffer))  /*judge buffer is empty or not*/
    {
        Elementype Cmd_data = Buffer_Pop(buffer);  /*get data from buffer*/
        CI2CComm_Handle(Cmd_data);
    }   
    #endif
}

#endif /* DESERIALIZER_MODULE */
