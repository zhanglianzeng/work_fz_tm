/******************************************************************************
*  Name: app_data.c
*  Description: Description of eeprom groups of data
*  Project: INFO4 11inch
*  Author: 
*  MCU: atmel samda1j16b
*  Comment: Generated file 
******************************************************************************/
#include "../../config.h"
#if (EEPROM_MODULE == 1)

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../../device/instance/nvmctrl.h"
#include "./app_eeprom.h"
#include "./eep_data.h"

static eep_group4_DTC_t    eep_group4_DTC;
static eep_group5_Status_t eep_group5_Status;

const eep_group1_ID_t eep_group1_ID_default = {  
        .data.CheckSum = {0x34, 0x65},              /*crc data*/
        .data.HW_Version = {0x01,0x01},             /*hardware version*/
        .data.Firmware_Version = {0x53,0x22},       /*firmware version*/
        #if (TOUCH_MODULE == 1)
        .data.Touch_Controller_ID = 0X02,              
        #else
        .data.Touch_Controller_ID = 0xFF,           /*touch id for IPC*/
        #endif
        
        .data.Display_ID = 0x0C,                    /*display id*/
        .data.Tune_Counts = 0,                      /*tune knob data, no use*/
        .data.Vloume_Counts = 0,                    /*volume knob data, no use*/
        #if (SWITCHPACK_MODULE == 1)
        .data.Debounced_Switch_Status = 1,          /*debounced switch status*/
        #else
        .data.Debounced_Switch_Status = 0,
        #endif
        .data.Debounced_Input_Status = 0,           /*debounced input status*/
        .data.Proximity_Zone1 = 1,                  /*proximity zone1, no use*/
        .data.Proximity_Zone2 = 2,                  /*proximity zone2, no use*/
        .data.Proximity_Zone3 = 3,                  /*proximity zone3, no use*/
        .data.Proximity_Zone4 = 4,                  /*proximity zone4, no use*/
        .data.Diagnostic_Status1 = {0,0,0,0,0,0},       /*lcd diagnostic data*/
        .data.Diagnostic_Status2 = {0,0,0,0},         /*led diagnostic data*/
        .data.Reset_Counter = 0,                    /*FIDM reset counter*/
        .data.Powerup_Counter = 0,                  /*FIDM wakeup counter*/
        .data.Software_Part = {0x38,0x34,0x39,0x30,0x31,0x37,0x33,0x31},   /*software partnumber*/
        .data.Hardware_Part = {0x38,0x34,0x39,0x30,0x31,0x37,0x33,0x31},   /*hardware partnumber*/
        .data.Bootloader_Status1 = {0x00,0x04},                            /*bootloager status1*/
        .data.Bootloader_Status2 = 1,                                      /*bootloager status2*/
        .data.Manufacturing_Traceability = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,\
                                            0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30},
        /*ETM*/
        #if(ETM_MODULE == 1)
        .data.Programing_Date = {20,20,1,6},       /*program date*/
        .data.Configuration_Flag = 1,              /*eeprom configuration flag*/
        .data.Daisy_Chain = 0,                     /*daisy chain mode*/
        #endif                                    
                                                    
};      /*Factory configuration*/

static const eep_group2_Calibration_t eep_group2_Calibration_default = {
        .data.Display_Backlight_PWM_1 = 1,
        .data.Display_Backlight_PWM_2 = 2,
        .data.Display_Backlight_PWM_3 = 3
};     /* LM8863 */

static const eep_group3_Control_t eep_group3_Control_default = {
    .data.enable = 1,
};    

static const eep_group4_DTC_t eep_group4_DTC_default = {
    .data.dtc1 = {0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0x11,0x22,0x33,0x44},
    .data.dtc2 = {0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0x11,0x22,0x33,0x44},
    .data.dtc3 = {0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0x11,0x22,0x33,0x44},
    .data.dtc4 = {0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0x11,0x22,0x33,0x44},
    .data.dtc5 = {0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0x11,0x22,0x33,0x44},
    .data.dtc6 = {0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0x11,0x22,0x33,0x44},
    .data.dtc7 = {0xAAFFAAFFu,0x11111111u}
};    

static const eep_group5_Status_t eep_group5_Status_default = {
    .data.reset_cnt = 0xFAFBFCFDu,
    .data.area1 = {0,1,2,3,4,5,6,7,8,9},
    .data.area2 = {0,1,2,3,4,5,6,7,8,9},
    .data.area3 = {0,1,2,3,4,5,6,7,8,9},
    .data.area4 = {0,1,2,3,4,5,6,7,8,9},
    .data.area5 = {0,1,2,3,4,5,6,7,8,9},
    .data.status = 0x2222
};   

static const group_property_t groups_property[NUMB_GROUPS] = {
    {
        .commit_delay = 0,                       /*write delay time allowable*/
        .rec_strategy = REC_STRATEGY_SINGLE,     /*operation strategy*/
        .rec_id = GROUP_1_ID,                    /*group id*/
        .start_page = GROUP1_START_PAGE,         /*start page*/
        .numb_pages = GROUP1_PAGES,              /*number of pages occupied */
        .pData = (uint32_t*)(NVMCTRL_RWW_EEPROM_ADDR + (GROUP1_START_PAGE*NVMCTRL_PAGE_SIZE)), /*the first address of eeprom data*/
        .pDefaultData = (uint32_t*)&eep_group1_ID_default  /*the first address of default data*/
    },
    {
        .commit_delay = 0,                       /*write delay time allowable*/
        .rec_strategy = REC_STRATEGY_SINGLE,     /*operation strategy*/
        .rec_id = GROUP_2_CALIB,                 /*group id*/
        .start_page = GROUP2_START_PAGE,         /*start page*/
        .numb_pages = GROUP2_PAGES,              /*number of pages occupied */
        .pData = NULL,                           /*the first address of eeprom data*/
        .pDefaultData = (uint32_t*)&eep_group2_Calibration_default  /*the first address of default data*/
    },
    {
        .commit_delay = 0,                       /*write delay time allowable*/
        .rec_strategy = REC_STRATEGY_SINGLE,     /*operation strategy*/
        .rec_id = GROUP_3_CTRL,                  /*group id*/
        .start_page = GROUP3_START_PAGE,         /*start page*/
        .numb_pages = GROUP3_PAGES,              /*number of pages occupied */
        .pData = NULL,                           /*the first address of eeprom data*/
        .pDefaultData = (uint32_t*)&eep_group3_Control_default  /*the first address of default data*/
    },
    {
        .commit_delay = 0,                      /*write delay time allowable*/
        .rec_strategy = REC_STRATEGY_DOUBLE,    /*operation strategy*/
        .rec_id = GROUP_4_DTC,                  /*group id*/
        .start_page = GROUP4_START_PAGE,        /*start page*/
        .numb_pages = GROUP4_PAGES,             /*number of pages occupied */
        .pData = (uint32_t*)&eep_group4_DTC,    /*the first address of eeprom data*/
        .pDefaultData = (uint32_t*)&eep_group4_DTC_default  /*the first address of default data*/
    },
    {
        .commit_delay = 0,                     /*write delay time allowable*/
        .rec_strategy = REC_STRATEGY_DOUBLE,   /*operation strategy*/
        .rec_id = GROUP_5_STATUS,              /*group id*/
        .start_page = GROUP5_START_PAGE,       /*start page*/
        .numb_pages = GROUP5_PAGES,            /*number of pages occupied */
        .pData = (uint32_t*)&eep_group5_Status,/*the first address of eeprom data*/
        .pDefaultData = (uint32_t*)&eep_group5_Status_default  /*the first address of default data*/
    }
};

/*=====================================================================================================================
* Upward trac.:        App/eeprom/eep_data
* Method      :        get_group_property(uint32_t group_id)
* Visibility  :        public
* Description :        Get pointer of group according to the assigned group_id
* Parameters  :        uint32_t group_id
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void*
* ===================================================================================================================*/
void* get_group_property(uint32_t group_id)
{
    void* ptr = NULL;
    if (NUMB_GROUPS > group_id)
    {
        ptr = (void*)&groups_property[group_id];        
    }
    else
    {/*do nothing*/}      
    return ptr;
}

#endif
