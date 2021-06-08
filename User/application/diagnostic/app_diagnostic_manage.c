/******************************************************************************
*  Name: app_diagnostic_manage.c
*  Description:
*  Project: INFO4 11inch
*  Auther: 
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"
#if (DIAGNOSTIC_MODULE == 1)
#include "app_diagnostic_manage.h"
#include "../../middle/sys/sys_if.h"

#if (LED_MODULE == 1)
#include "../../middle/led/led_if.h"   
#endif

#if (LCD_MODULE == 1)
#include "../../middle/lcd/lcd_if.h"          
#endif

#if (POWER_MODULE == 1)
#include "../../middle/power/power_if.h"
#endif

#if (EEPROM_MODULE == 1)
#include "../eeprom/app_eeprom.h"
#include "../eeprom/eep_data.h"
#endif

#if (POWER_MODULE == 1)
#include "../power/app_power_manage.h"     
#endif

#if (THERMAL_MODULE == 1)
#include "../thermal/app_thermal_manage.h"
#endif

#if (LED_MODULE == 1)
#include "../led/app_led_ctrl.h"
#endif

#if (DESERIALIZER_MODULE == 1)
#include "../../middle/deserializer/deserializer_if.h"
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
#if (LCD_MODULE == 1)
static bool byLcdRecover_Flg = false;
static bool byLcdRefresh_Flg = false;
#endif
/*=====================================================================================================================
* Upward trac.:        APPLICATION/DIAGNOSTIC
* Method      :        CDiagnositcManage_Led_Diag_Resolve(uint8_t *DiagRet,unBlFlt unbl_regflt)
* Visibility  :        private
* Description :        the function BL_diag
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CDiagnositcManage_Led_Diag_Resolve(DIAGNOSTIC_STATUS2_Type *DiagRet,unBlFlt unbl_regflt)
{
#if (LED_MODULE == 1)
    DIAGNOSTIC_STATUS2_Type tem = {0};
    if(DIAG_STATUS2_RESETVALUE!=(unbl_regflt.strReg.unInt_Flt2.hStat_Reg & BL_LED_FLT))
    {
        tem.reg |= DIAG_STATUS2_BL_LED_FAULT;
    }    
    if((DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt1.bStat.BSTOVPH_STATUS)||
        (DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt3.bStat.VINOVP_STATUS)||
        (DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt1.bStat.BSTOVPL_STATUS))
    {
        tem.reg |= DIAG_STATUS2_BL_OVER_VOLTAGE;
    }    
    if(DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt3.bStat.VINUVP_STATUS)
    {
        tem.reg |= DIAG_STATUS2_BL_UNDER_VOLTAGE;
    }    
    if((DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt1.bStat.BSTOCP_STATUS)||
        (DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt3.bStat.VINOCP_STATUS))
    {
        tem.reg |= DIAG_STATUS2_BL_OVER_CURRENT;
    }		    
    if(DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt1.bStat.TEMPHIGH_STATUS)
    {
        tem.reg |= DIAG_STATUS2_BL_TEMP_HIGH;
    }    
    if(DIAG_STATUS2_RESETVALUE!=unbl_regflt.strReg.unInt_Flt1.bStat.TEMPLOW_STATUS)
    {
        tem.reg |= DIAG_STATUS2_BL_TEMP_LOW;
    }
    DiagRet->reg = tem.reg;
#endif
}


/*=====================================================================================================================
* Upward trac.:        APPLICATION/DIAGNOSTIC
* Method      :        CDiagnositcManage_LcdRecover_Manage(void)
* Visibility  :        private
* Description :        Lcd display recover manage
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static void CDiagnositcManage_LcdRecover_Manage(void)
{                                                     
 #if (LCD_MODULE == 1) 
    static uint8_t byGateErr_RecoverCount =0;
    static uint8_t byGateErr_Step = 0;
    if(byLcdRecover_Flg && byLcdRefresh_Flg)
    {       
        if(byGateErr_RecoverCount < 3u)                       /*after 3 times, not deal with it again*/
        {               
            if(0u==byGateErr_Step)
            {
                Lcd_If_Set_Stbyb_Level(false);
                byGateErr_Step ++;  
            }
            else if (byGateErr_Step == 1u)
            {
                Lcd_If_Set_Reset_Level(false);
                byGateErr_Step ++;                                    
            }
            else
            {
                Lcd_If_Power_On();
                Lcd_If_Ic_Init();
                byGateErr_Step = 0;
                byGateErr_RecoverCount ++;
                byLcdRefresh_Flg = false;
            }
        }               
    }   
    else if(!byLcdRecover_Flg)  
    {
        byGateErr_RecoverCount = 0;
    }
    else
    {/*do nothing*/}     
#endif  
}

/*=====================================================================================================================
* Upward trac.:        APPLICATION/DIAGNOSTIC
* Method      :        CDiagnositcManage_SelfDiag(DIAGNOSTIC_STATUS1_Type *DiagRet)
* Visibility  :        private
* Description :        the function selfdiag 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CDiagnositcManage_SelfDiag1(DIAGNOSTIC_STATUS1_Type *DiagRet)
{
/*lcd ic different DA1��diagnositc not sure*/
#if (LCD_MODULE == 2)
    unLcdFlt unlcd_regflt;    
    if (false == Lcd_If_Get_Fault_Level())   
    {
        DiagRet->reg |= DIAG_STATUS1_DISPLAY_ERR;
        Lcd_If_SPI_Read_FltReg();
        unlcd_regflt = Lcd_If_Get_RegFlt();
        if(DIAG_STATUS1_RESETVALUE!=(LCD_OLDI_ERR & unlcd_regflt.strFlt.reg1.byte))
        {
            DiagRet->reg |= DIAG_STATUS1_OLDI_ERR;
        }
        else
        {/*do nothing*/}
        if((DIAG_STATUS1_RESETVALUE!=(LCD_GATE_SOURCE_ERR3 & unlcd_regflt.strFlt.reg3.byte)) || 
            (DIAG_STATUS1_RESETVALUE!=(LCD_GATE_SOURCE_ERR4 & unlcd_regflt.strFlt.reg4.byte)))
        {
            DiagRet->reg |= DIAG_STATUS1_GS_DRIVER_ERR;
            /*need recover three times*/
            byLcdRecover_Flg = true;   
            byLcdRefresh_Flg = true;
         }
         else
         {/*do nothing*/}			
    }
    else
    {
        byLcdRecover_Flg = false;
    } 
#endif
	if((FULLY_OPERATION_LEVEL != CPowerManage_Get_PowerStatus()) || (THERMAL_FULLY_OPERATION_LEVEL != CThermalManage_Get_ThermalStatus()))
        DiagRet->reg |= DIAG_STATUS1_TEMP_SHUT_OFF;                  
    if(Sys_If_IsWatchdog() || Sys_If_IsSWR()||Sys_If_IsHWR())       
        DiagRet->reg |= DIAG_STATUS1_SOFTWARE_RESET_ERR;  				
    if(Sys_If_IsBOD())                              
        DiagRet->reg |= DIAG_STATUS1_POWER_RESET_ERR;
#if (POWER_MODULE == 1)
    if(!Power_If_Get_Fault_Level())
    {
        DiagRet->reg |= DIAG_STATUS1_POWER_IC_ERR;
    }
#endif
#if (DESERIALIZER_MODULE == 1)
    if(!Des_If_Get_Lock_Level())
    {
        DiagRet->reg |= DIAG_STATUS1_LVDS_LOCK_ERR;
    }
#endif
    CDiagnositcManage_LcdRecover_Manage();        
}
/*=====================================================================================================================
* Upward trac.:        APPLICATION/DIAGNOSTIC
* Method      :        CDiagnositcManage_SelfDiag(uint8_t stat_channel,uint8_t *DiagRet)
* Visibility  :        private
* Description :        the function selfdiag 
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :  void 
* ===================================================================================================================*/
static void CDiagnositcManage_SelfDiag2(DIAGNOSTIC_STATUS2_Type *DiagRet)
{
#if (LED_MODULE == 1)
    unBlFlt  unbl_regflt;
    if (false == LED_If_Get_Fault_Level())
    {                   
        LED_If_Fault_Read();   
        unbl_regflt = LED_If_Get_BL_Fault();           
        CDiagnositcManage_Led_Diag_Resolve(DiagRet,unbl_regflt);       		
        if(DIAG_STATUS2_RESETVALUE!=(DiagRet->reg & 0x7Fu))
        {
            DiagRet->reg |= DIAG_STATUS2_BL_ERR_FLAG;            
        }
        else
        {/*do nothing*/}
    }
    else
    {
        DiagRet->reg &= DIAG_STATUS2_BL_ERR_FLAG;
    }
#endif
}
    
/*=====================================================================================================================
* Upward trac.:        APPLICATION/LIFE
* Method      :        CDiagnositcManage_Task(void)
* Visibility  :        public
* Description :        Diagnositc management Task
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void CDiagnositcManage_Task(void)
{
    POWER_ACTION_STATUS power_sta;
    Factory_Config* pConfig_Init ;
    static DIAGNOSTIC_STATUS1_Type Dia_Status1_Reg;
    static DIAGNOSTIC_STATUS2_Type Dia_Status2_Reg;
    pConfig_Init=CEeprom_Get_Factory_Config();
    power_sta = CPowerManage_Get_PowerStatus();  
    Dia_Status1_Reg.reg = pConfig_Init->data.Diagnostic_Status1[3];
    Dia_Status2_Reg.reg = pConfig_Init->data.Diagnostic_Status2[2];
    if((PARTIALLY_OPERATION_LOW == power_sta) || (FULLY_OPERATION_LEVEL == power_sta))
    {
        CDiagnositcManage_SelfDiag1(&Dia_Status1_Reg);
        CDiagnositcManage_SelfDiag2(&Dia_Status2_Reg);
        pConfig_Init->data.Diagnostic_Status1[3] = Dia_Status1_Reg.reg;
        pConfig_Init->data.Diagnostic_Status2[2] = Dia_Status2_Reg.reg;
    }    
}

#endif

