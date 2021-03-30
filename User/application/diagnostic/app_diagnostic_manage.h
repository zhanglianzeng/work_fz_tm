/******************************************************************************
*  Name: app_diagnostic_manage.h
*  Description:
*  Project: WAVE2 17.7
*  Auther: 
*  MCU: atmel samc211n18
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (DIAGNOSTIC_MODULE == 1)
#ifndef APP_DIAGNOSTIC_MANAGE_H
#define APP_DIAGNOSTIC_MANAGE_H

#include <stdint.h>
#include <string.h>


/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
#if !defined(_UL)
#define _U(x)          x ## U            /**< C code: Unsigned integer literal constant value */
#define _L(x)          x ## L            /**< C code: Long integer literal constant value */
#define _UL(x)         x ## UL           /**< C code: Unsigned Long integer literal constant value */
#endif

typedef union 
{
    struct
    {   
        uint8_t OLDI_ERR                    :1;
        uint8_t DISPLAY_ERR                 :1;
        uint8_t TEMP_SHUT_OFF               :1;
        uint8_t GS_DRIVER_ERR               :1;
        uint8_t SOFTWARE_RESET_ERR          :1;
        uint8_t POWER_RESET_ERR             :1;
        uint8_t POWER_IC_ERR                :1;
        uint8_t LVDS_LOCK_ERR               :1;
     }bit;
    uint8_t reg;   
}DIAGNOSTIC_STATUS1_Type;

typedef union 
{
    struct
    {   
        uint8_t BL_ERR                      :1;
        uint8_t BL_LED_FAULT                :1;
        uint8_t BL_OVER_VOLTAGE             :1;
        uint8_t BL_UNDER_VOLTAGE            :1;
        uint8_t BL_OVER_CURRENT             :1;
        uint8_t BL_TEMP_HIGH                :1;
        uint8_t BL_TEMP_LOW                 :1;
        uint8_t BL_ERR_FLAG                 :1;
     }bit;
    uint8_t reg;   
}DIAGNOSTIC_STATUS2_Type;


/**********************************************************************************************
* Constants and macros
**********************************************************************************************/

#define DIAG_STATUS1_RESETVALUE _U(0x0000)   /**< \brief DIAGNOSTIC_STATUS1 */
    
#define DIAG_STATUS1_OLDI_ERR_Pos           0            /* */
#define DIAG_STATUS1_OLDI_ERR           (_U(0x1) << DIAG_STATUS1_OLDI_ERR_Pos)
#define DIAG_STATUS1_DISPLAY_ERR_Pos        1            
#define DIAG_STATUS1_DISPLAY_ERR        (_U(0x1) << DIAG_STATUS1_DISPLAY_ERR_Pos)
#define DIAG_STATUS1_TEMP_SHUT_OFF_Pos      2            
#define DIAG_STATUS1_TEMP_SHUT_OFF      (_U(0x1) << DIAG_STATUS1_TEMP_SHUT_OFF_Pos)
#define DIAG_STATUS1_GS_DRIVER_ERR_Pos      3            
#define DIAG_STATUS1_GS_DRIVER_ERR      (_U(0x1) << DIAG_STATUS1_GS_DRIVER_ERR_Pos)
#define DIAG_STATUS1_SOFTWARE_RESET_ERR_Pos 4            
#define DIAG_STATUS1_SOFTWARE_RESET_ERR     (_U(0x1) << DIAG_STATUS1_SOFTWARE_RESET_ERR_Pos)
#define DIAG_STATUS1_POWER_RESET_ERR_Pos    5 
#define DIAG_STATUS1_POWER_RESET_ERR    (_U(0x1) << DIAG_STATUS1_POWER_RESET_ERR_Pos)
#define DIAG_STATUS1_POWER_IC_ERR_Pos       6     
#define DIAG_STATUS1_POWER_IC_ERR       (_U(0x1) << DIAG_STATUS1_POWER_IC_ERR_Pos)
#define DIAG_STATUS1_LVDS_LOCK_ERR_Pos      7 
#define DIAG_STATUS1_LVDS_LOCK_ERR      (_U(0x1) << DIAG_STATUS1_LVDS_LOCK_ERR_Pos)

#define DIAG_STATUS2_RESETVALUE _U(0x0000)   /**< \brief DIAGNOSTIC_STATUS1 */
    
#define DIAG_STATUS2_BL_ERR_Pos             0            /* */
#define DIAG_STATUS2_BL_ERR             (_U(0x1) << DIAG_STATUS2_BL_ERR_Pos)
#define DIAG_STATUS2_BL_LED_FAULT_Pos       1            
#define DIAG_STATUS2_BL_LED_FAULT       (_U(0x1) << DIAG_STATUS2_BL_LED_FAULT_Pos)
#define DIAG_STATUS2_BL_OVER_VOLTAGE_Pos    2            
#define DIAG_STATUS2_BL_OVER_VOLTAGE    (_U(0x1) << DIAG_STATUS2_BL_OVER_VOLTAGE_Pos)
#define DIAG_STATUS2_BL_UNDER_VOLTAGE_Pos   3            
#define DIAG_STATUS2_BL_UNDER_VOLTAGE   (_U(0x1) << DIAG_STATUS2_BL_UNDER_VOLTAGE_Pos)
#define DIAG_STATUS2_BL_OVER_CURRENT_Pos    4            
#define DIAG_STATUS2_BL_OVER_CURRENT    (_U(0x1) << DIAG_STATUS2_BL_OVER_CURRENT_Pos)
#define DIAG_STATUS2_BL_TEMP_HIGH_Pos       5 
#define DIAG_STATUS2_BL_TEMP_HIGH       (_U(0x1) << DIAG_STATUS2_BL_TEMP_HIGH_Pos)
#define DIAG_STATUS2_BL_TEMP_LOW_Pos        6     
#define DIAG_STATUS2_BL_TEMP_LOW        (_U(0x1) << DIAG_STATUS2_BL_TEMP_LOW_Pos)
#define DIAG_STATUS2_BL_ERR_FLAG_Pos        7 
#define DIAG_STATUS2_BL_ERR_FLAG        (_U(0x1) << DIAG_STATUS2_BL_ERR_FLAG_Pos)


/**********************************************************************************************
* Local types
**********************************************************************************************/

/**********************************************************************************************
* Local variables
**********************************************************************************************/

/**********************************************************************************************
* Local functions
**********************************************************************************************/

/**********************************************************************************************
* Global functions
**********************************************************************************************/
extern void CDiagnositcManage_Init(void);
extern void CDiagnositcManage_Deinit(void);
extern void CDiagnositcManage_Task(void);
#endif /*APP_DIAGNOSTIC_MANAGE_H*/
#endif /*DIAGNOSTIC_MODULE */

