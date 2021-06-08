/******************************************************************************
*  Name: app_data.h
*  Description: Description of eeprom groups of data
*  Project: INFO4 11inch
*  Author: 
*  MCU: atmel samda1j16b
*  Comment: Generated file 
******************************************************************************/

#include <stdint.h>
#include "../../../device/instance/nvmctrl.h"

#ifndef EEP_DATA_H_
#define EEP_DATA_H_

enum {
    GROUP_1_ID,                    /*group1*/
    GROUP_2_CALIB,                 /*group2*/
    GROUP_3_CTRL,                  /*group3*/
    GROUP_4_DTC,                   /*group4*/
    GROUP_5_STATUS,                /*group5*/
    NUMB_GROUPS                    /*group number*/
};

#define GROUP1_START_PAGE   (0u)    /*start page of group1*/
#define GROUP2_START_PAGE   (4u)    /*start page of group2*/
#define GROUP3_START_PAGE   (12u)   /*start page of group3*/
#define GROUP4_START_PAGE   (0u)    /*start page of group4*/
#define GROUP5_START_PAGE   (0u)    /*start page of group5*/

#define HEADER_POSITION     0u  /*header position in pagebuff*/
#define CRC_POSITION        1u  /*crc position in pagebuff*/
#define DATA_POSITION       2u  /*data position in pagebuff*/

#define GROUP1_PAGES        (4u)    /*number of pages occupied by group1*/
#define GROUP2_PAGES        (8u)    /*number of pages occupied by group2*/
#define GROUP3_PAGES        (4u)    /*number of pages occupied by group3*/
#define GROUP4_PAGES        (4u)    /*number of pages occupied by group4*/
#define GROUP5_PAGES        (1u)    /*number of pages occupied by group5*/ 

#define START_ROW_DOUBLE    (4u)    /*start row of double group*/
#define NUMB_ROWS_DOUBLE    (4u)    /*row number of double group*/

#define DIAGNOSTIC_STATUS1  (6u)
#define DIAGNOSTIC_STATUS2  (4u)

typedef union
{
    struct
    {
        uint8_t CheckSum[2];              /*crc data*/
        uint8_t HW_Version[2];            /*hardware version*/ 
        uint8_t Firmware_Version[2];      /*firmware version*/ 
        uint8_t Touch_Controller_ID;      /*touch id*/
        uint8_t Display_ID;               /*display id*/
        uint8_t Tune_Counts;              /*tune knob data*/
        uint8_t Vloume_Counts;            /*volume knob data*/
        uint8_t Debounced_Switch_Status;  /*debounced switch status*/
        uint8_t Debounced_Input_Status;   /*debounced input status*/
        uint8_t Proximity_Zone1;          /*proximity zone1*/
        uint8_t Proximity_Zone2;          /*proximity zone2*/
        uint8_t Proximity_Zone3;          /*proximity zone3*/
        uint8_t Proximity_Zone4;          /*proximity zone4*/
        uint8_t Diagnostic_Status1[DIAGNOSTIC_STATUS1];    /*led diagnostic data*/
        uint8_t Diagnostic_Status2[DIAGNOSTIC_STATUS2];   /*lcd diagnostic data*/
        uint8_t Reset_Counter[2];           /*FIDM reset counter*/
        uint8_t Powerup_Counter[2];         /*FIDM wakeup counter*/
        uint8_t Software_Part[8];         /*software partnumber*/
        uint8_t Hardware_Part[8];         /*hardware partnumber*/
        uint8_t Bootloader_Status1[2];    /*bootloager status1*/
        uint8_t Bootloader_Status2;       /*bootloager status2*/
        
        uint8_t Manufacturing_Traceability[16];    /*Manufacturing Traceability, only in ETM mode*/
        /*ETM*/
        #if(ETM_MODULE == 1)
        uint8_t Programing_Date[4];       /*program date, only in ETM mode*/
        uint8_t Configuration_Flag;       /*eeprom configuration flag, only in ETM mode*/
        uint8_t Daisy_Chain;              /*daisy chain mode, only in ETM mode*/
        #endif
        uint8_t Reserved[];
    } data;
    uint32_t asWord[NVMCTRL_PAGE_W*GROUP1_PAGES-DATA_POSITION];
}Factory_Config,eep_group1_ID_t,*Factory_ConfigPtr;  

typedef union
{
    struct
    {
        uint32_t Display_Backlight_PWM_1;
        uint32_t Display_Backlight_PWM_2;
        uint32_t Display_Backlight_PWM_3;
    }data;
    uint32_t asWord[(NVMCTRL_PAGE_W*GROUP2_PAGES-2)];
}eep_group2_Calibration_t;

typedef union
{
    struct
    {
        uint32_t enable;
    }data;
    uint32_t asWord[(NVMCTRL_PAGE_W*GROUP3_PAGES-2)];
}eep_group3_Control_t;

typedef union
{
    struct
    {
        uint16_t dtc1[20];
        uint16_t dtc2[20];
        uint16_t dtc3[20];
        uint16_t dtc4[20];
        uint16_t dtc5[20];
        uint16_t dtc6[20];
        uint32_t dtc7[2];
    }data;
    uint32_t asWord[(NVMCTRL_PAGE_W*GROUP4_PAGES-2)];
}eep_group4_DTC_t;

typedef union
{
    struct
    {
        uint32_t reset_cnt;
        uint8_t  area1[10];
        uint8_t  area2[10];
        uint8_t  area3[10];
        uint8_t  area4[10];
        uint8_t  area5[10];
        uint16_t status;
    }data;
    uint32_t asWord[(NVMCTRL_PAGE_W*GROUP5_PAGES-2)];
}eep_group5_Status_t;

extern eep_group4_DTC_t    eep_group4_DTC;
extern eep_group5_Status_t eep_group5_Status;
extern const eep_group1_ID_t eep_group1_ID_default;

void* get_group_property(uint32_t idx);


#endif /* EEP_DATA_H_ */
