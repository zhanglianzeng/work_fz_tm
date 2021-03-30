/******************************************************************************
*  Name: post_if.c
*  Description:
*  Project: INFO4 11inch
*  Auther: fangsong
*  MCU: atmel samda1j16b
*  Comment:
******************************************************************************/
#include "../../config.h"

#if (POST_MODULE == 1)
#include "../../../Device/samc21n18a.h"
#include "post_if.h"

#if (CRC_COM == 1)
#include "../../common/crc.h"
#endif
/**********************************************************************************************
* External objects
**********************************************************************************************/

/**********************************************************************************************
* Global variables
**********************************************************************************************/
uint8_t RamTest[8][16] = {0};
/**********************************************************************************************
* Constants and macros
**********************************************************************************************/
#ifndef NULL  
#define NULL   0
#endif

#define WordAcc(ptr)    (*((volatile uint32_t *) ((uint32_t)(ptr))))    /*!< macro to access the data of a pointer 32 bit wide */
/* local macros */
/* March memory operations */
#define st(x) {       \
            do { x; }   \
            while (0);  \
            }

#define March_R0() {                            \
            st(if ((WordAcc(ptr) != r0_exp))    \
            {mresult = ClassB_testFailed;})     \
            }

#define March_R1() {                            \
            st(if ((WordAcc(ptr) != r1_exp))    \
            {mresult = ClassB_testFailed;})     \
            }

#define March_W0() {                            \
            st(WordAcc(ptr) = r0_exp);          \
            }

#define March_W1() {                            \
            st(WordAcc(ptr) = r1_exp;)          \
            }


#define March_DEL()     __NOP()
#define ASM_NOP()       __NOP()
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

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        ClassB_testResult_t ClassB_WDTTest_ForceReset(void)
* Visibility  :        static
* Description :        WDT test;Force uC reset
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static ClassB_testResult_t ClassB_WDTTest_ForceReset(void);
static ClassB_testResult_t ClassB_WDTTest_ForceReset(void)
{
    ClassB_testResult_t result = ClassB_testFailed;
    uint8_t             resetcause;

    /* save the reset cause */
#if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
    resetcause = PM->RCAUSE.reg;
#else /* CPU_SELECTION == 3 */
    resetcause = RSTC->RCAUSE.reg;
#endif /* CPU_SELECTION */

    /* debug IO action see ClassB_config.h */
    // __DBG_PIN_OUTPUT();
    // __DBG_RESET_IO0();

    /* Basic Setup */
    /* Various bits in the INTFLAG register can be set to one at startup.
       This will ensure that these bits are cleared */
#if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
    SYSCTRL->INTFLAG.reg = (SYSCTRL_INTFLAG_DFLLRDY_   |
                            SYSCTRL_INTFLAG_BOD33RDY_) |
                            SYSCTRL_INTFLAG_BOD33DET_;
#else /* CPU_SELECTION == 2 */
    SUPC->INTFLAG.reg = SUPC_INTFLAG_BODVDDDET |
                        SUPC_INTFLAG_BODVDDRDY;
#endif /* CPU_SELECTION */

    /* Set flash controller wait states */
    NVMCTRL->CTRLB.bit.RWS = CONF_WDT_FLASH_WAIT_STATES;

    /* reset CPU clock divider */
#if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
    PM->CPUSEL.reg = CONF_WDT_CPU_DIV;
#else /* CPU_SELECTION == 2 */
    MCLK->CPUDIV.reg = CONF_WDT_CPU_DIV;
#endif /* CPU_SELECTION */

    /* WDT init */
    /* Turn on the WDT digital interface clock */
#if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
    PM->APBAMASK.reg |= PM_APBAMASK_WDT_;
#else /* CPU_SELECTION == 2 */
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_WDT;
#endif /* CPU_SELECTION */
    WDT_Sync();

    /* Check of the Watchdog has been locked to be always on, if so, abort */
#if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
    if (WDT->CTRL.reg & WDT_CTRL_ALWAYSON_)
#else /* CPU_SELECTION == 2 */
    if (WDT->CTRLA.reg & WDT_CTRLA_ALWAYSON)
#endif /* CPU_SELECTION */
    {
        // exit here!
    }
    else
    {
        /* check POR reset as last reset event, system for debugging, ext for RST pin
           Attention: more than one flag is possible */
        if (((resetcause & PM_RCAUSE_POR_)  == PM_RCAUSE_POR_)||
            ((resetcause & PM_RCAUSE_SYST_) == PM_RCAUSE_SYST_)||
            ((resetcause & PM_RCAUSE_EXT_)  == PM_RCAUSE_EXT_))
        {
            /* Disable the Watchdog module */
            #if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
            WDT->CTRL.reg &= ~WDT_CTRL_ENABLE_;
            #else /* CPU_SELECTION == 2, 3 */
            WDT->CTRLA.reg &= ~WDT_CTRLA_ENABLE;
            #endif /* CPU_SELECTION */
            TESSY_SYNC();
            WDT_Sync();

            /* Configure WDT-GCLK channel and enable clock */
            EnterCriticalSection();

            /* Ensure the window enable control flag is cleared */
            #if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
            WDT->CTRL.reg &= ~WDT_CTRL_WEN_;
            #else /* CPU_SELECTION == 2, 3 */
            WDT->CTRLA.reg &= ~WDT_CTRLA_WEN;
            #endif /* CPU_SELECTION */
            TESSY_SYNC();
            WDT_Sync();
            TESSY_SYNC();
            /* Write the new Watchdog configuration */
            WDT->CONFIG.reg = WDT_NEW_CONFIG_;
            WDT_Sync();

            /* enable the WDT */
            #if (CPU_SELECTION==0) || (CPU_SELECTION == 1)
            WDT->CTRL.reg |= WDT_CTRL_ENABLE_;
            #else /* CPU_SELECTION == 2, 3 */
            WDT->CTRLA.reg |= WDT_CTRLA_ENABLE;
            #endif /* CPU_SELECTION */

            ExitCriticalSection();
            TESSY_SYNC();
            WDT_Sync();
            TESSY_SYNC();
            /*! @endcond */

            result = ClassB_testInProgress;  
        }
        /* check for Watchdog timer reset */
        else if ((resetcause & PM_RCAUSE_WDT_) == PM_RCAUSE_WDT_)
        {
            /* test has been successful */
            result = ClassB_testPassed;
            #if (CLASSB_DBG_GPIO == ClassB_Enabled)
            /* debug IO action see ClassB_config.h */
            #ifdef TESSY
                __DBG_SET_IO0();
                __DBG_WAIT(100);
            #else
                __DBG_SET_IO0();
                __DBG_WAIT(100000);
                __DBG_RESET_IO0();
                __DBG_WAIT(100000);
                __DBG_SET_IO0();
                __DBG_WAIT(100000);
                __DBG_RESET_IO0();
                __DBG_WAIT(100000);
                __DBG_SET_IO0();
            #endif
            #endif /* CLASSB_DBG_GPIO */
        }
        else /* BOD33, BOD12 */
        {
            /* TODO UserCode: insert some functions to cover other reset causes */
            __NOP();
        }
    }        /* reset detected (other cause) */

    return(result);
}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        ClassB_testResult_t ClassB_WDTTest_POST(void)
* Visibility  :        public
* Description :        WDT selftest task
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
ClassB_testResult_t ClassB_WDTTest_POST(void)
{
    ClassB_testResult_t result;
    /* no debugger allowed here */
    result = ClassB_WDTTest_ForceReset();

    if (result == ClassB_testInProgress)
    {
        /*lint -save -e527 MISRA 2004 Rule 14.1 accepted */
        /** wait for reset */
        LoopForever();   /*PRQA S 2880*/
        /*lint -restore */
    }

    return(result);
}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        ClassB_testResult_t ClassB_RAMTest_MarchC(uint32_t startAddr, uint32_t size, ClassB_RAMTest_Mode_t mode)
* Visibility  :        static
* Description :        RAM selftest task
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static ClassB_testResult_t ClassB_RAMTest_MarchC(uint32_t startAddr, uint32_t size, ClassB_RAMTest_Mode_t mode) /*PRQA S 2755*/
{
    ClassB_testResult_t       mresult = ClassB_testInProgress;

    /* Note that function calls cannot be used from this function as the stack is destroyed. */

    /* Use a byte pointer to increment through the RAM blocks.
     * The byte pointer is cast into 32-bit word accesses to generate both
     * aligned and unaligned accesses. The unaligned accesses are split into
     * multiple 8- and 16-bit accesses by the CM3 bus matrix. That way, we are
     * also testing for individual byte WE faults in the RAM IP and address
     * decoder faults. */
    register volatile uint8_t *ptr;
    volatile uint8_t          *ram_base_init;
    volatile uint8_t          *ram_end_init;
    register volatile uint8_t *ram_base;
    register volatile uint8_t *ram_end;
    register uint32_t         r0_exp;
    register uint32_t         r1_exp;
    register uint32_t         segment_size2;
    uint32_t                  currentSP;

    /* Set RAM block boundary pointers */
    ram_base_init = (volatile uint8_t *) startAddr;
    ram_end_init  = ram_base_init + size;
    r0_exp        = CORE_RAM_R0_EXP;
    r1_exp        = (uint32_t)CORE_RAM_R1_EXP;       /* QAC */
    currentSP     = getSP();                         /*get current main stack pointer    gongxl*/            
    /*if(SystemVectors == NULL)
        init_Sp       = (SystemVectors->pStackPtr);*/

    do {
        segment_size2    = 0U;
        if (ClassB_RAMtestNormal == mode)           /*test without stack*/
        {
            /* test boundaries */
            /* test start address smaller end address
             * xxxxxxxxxxxxxxxxxxxx
             *     rei   rbi
             */
            if (ram_base_init >= ram_end_init)
            {
                mresult = ClassB_testFailed;
            }
            /*lint -save -e506 accepted bitwise operations */
            /* test stack issues
             * SP is under the startaddr AND the initial SP is above the startaddr means the
             * stack begins in the tested area and goes down under the tested area
             *       xxxxxxxxxxxxxxxxxxxx
             * cSP     iSP
             */
            if ((currentSP < startAddr)&&(initialSP > startAddr))  /*PRQA S 2810*/
            {
                startAddr = initialSP + RAMTEST_W_SIZE;            /*PRQA S 2810*/
            }

            /*lint -restore */
            /* test alignment */
            if ((size%4 != 0U) || (size == 0U))
            {
                mresult = ClassB_testFailed;
            }

            /** check stack end in tested RAM area */
            if (((uint32_t)ram_base_init <= currentSP) && ((uint32_t)ram_end_init >= currentSP))
            {
                /*lint -save -e574 accepted Signed-unsigned mix with relational */
                /** check endaddr is in the stack */
                if ((uint32_t)ram_end_init <= initialSP)           /*PRQA S 2810*/
                {
                    /*lint -e734 accepted Loss of precision (assignment) (32 bits to 8 bits) */
                    /* move endaddr below stack */
                    ram_end_init = (uint8_t*)(currentSP - ClassB_RAM_SP_Offset);   /* some distance required */
                    segment_size2 = 0U;
                    /* check size to test */
                    if (ram_end_init <= ram_base_init)
                    {
                        mresult = ClassB_testFailed;
                    }
                }
                else /** two sections are required */
                {
                    /* set two sections */
                    segment_size2 = (uint32_t)ram_end_init - initialSP;   /*PRQA S 2810*/ /* next segment to check */
                    ram_end_init = (uint8_t*)(currentSP - ClassB_RAM_SP_Offset);
                }
                /*lint -restore */
            }
            /*lint -save -e574 accepted Signed-unsigned mix with relational */
            /** check stack start in tested RAM area */
            if (((uint32_t)ram_base_init <= initialSP) && ((uint32_t)ram_end_init) >= initialSP)  /*PRQA S 2810*/
            {
                /*lint -restore */
                /** stack is at the start of the tested area */
                ram_base_init = (uint8_t*)initialSP + ClassB_RAM_SP_Offset;  /*PRQA S 2810*/
                segment_size2 = 0;
                /** check sufficient size of tested memory */
                if (ram_end_init <= ram_base_init)
                {
                    mresult = ClassB_testFailed;
                }
            }
        }
        else /* mode */
        {
            segment_size2 = 0;
            /*lint -save -e506 accepted bitwise operations */
            /* limit addresses to the stack as maximum address */
            if ((currentSP < (uint32_t) ram_end_init) && (initialSP < currentSP))  /*PRQA S 2810*/
            {
                ram_end_init = (uint8_t*)(currentSP - RAMTEST_W_SIZE);
            }
            /*lint -restore */

            /* check for unaligned access */
            ram_base_init = ram_base_init - ((uint32_t)ram_base_init%RAMTEST_W_SIZE);
        }
        if (mresult == ClassB_testInProgress)
        {
            ram_base = ram_base_init;
            ram_end  = ram_end_init;

            /* March element M0: <->(w0), using -> address order */
            ptr = ram_base;
            while (ptr < ram_end)
            {
                March_W0();
                ptr = ptr + sizeof(uint32_t);
            }

            /* March element M1: ->(r0,w1) */
            ptr = ram_base;
            while (ptr < ram_end)
            {
                March_R0();
                March_W1();
                ptr = ptr + sizeof(uint32_t);
            }
            /* March element M2: <-(r1,w0,r0,w1) */
            if (mresult == ClassB_testInProgress)
            {
                ptr = (ram_end - sizeof(uint32_t));
                while (ptr >= ram_base)
                {
                    March_R1();
                    March_W0();
                    March_R0();
                    March_W1();
                    ptr = ptr - sizeof(uint32_t);
                }
            }
            /* March element M3: <-(r1,w0) */
            if (mresult == ClassB_testInProgress)
            {
                ptr = (ram_end - sizeof(uint32_t));
                while (ptr >= ram_base)
                {
                    March_R1();
                    March_W0();
                    ptr = ptr - sizeof(uint32_t);
                }
            }
            /* March element M4: <-(r0,w1,r1,w0) */
            if (mresult == ClassB_testInProgress)
            {
                ptr = (ram_end - sizeof(uint32_t));
                while (ptr >= ram_base)
                {
                    March_R0();
                    March_W1();
                    March_R1();
                    March_W0();
                    ptr = ptr - sizeof(uint32_t);
                }
            }
            /* March element M5: <-(r0) */
            if (mresult == ClassB_testInProgress)
            {
                ptr = (ram_end - sizeof(uint32_t));
                while (ptr >= ram_base)
                {
                    March_R0();
                    ptr = ptr - sizeof(uint32_t);
                }
            }
            /* March element M6: Del */
            if (mresult == ClassB_testInProgress)
            {
                March_DEL();
            }

            /* March element M7: <->(r0,w1,r1), using -> address order */
            if (mresult == ClassB_testInProgress)
            {
                ptr = ram_base;
                while (ptr < ram_end)
                {
                    March_R0();
                    March_W1();
                    March_R1();
                    ptr = ptr + sizeof(uint32_t);
                }
            }
            /* March element M8: Del */
            if (mresult == ClassB_testInProgress)
            {
                March_DEL();
            }

            /* March element M9: <->(r1), using <- address order */
            if (mresult == ClassB_testInProgress)
            {
                ptr = (ram_end - sizeof(uint32_t));
                while (ptr >= ram_base)
                {
                    March_R1();
                    ptr = ptr - sizeof(uint32_t);
                }
            }
            if (segment_size2 != 0U)
            {
                ram_base_init = (volatile uint8_t *) initialSP + RAMTEST_W_SIZE;  /*PRQA S 2810*/
                ram_end_init  = (volatile uint8_t *) initialSP + segment_size2;   /*PRQA S 2810*/
            }
        }
    } while ((segment_size2 != 0U) && (mresult == ClassB_testInProgress));
    /* No faults found, report OK */
    if (mresult == ClassB_testInProgress)
    {
        mresult = ClassB_testPassed;
    }

    return(mresult);
}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        ClassB_testResult_t ClassB_RAMTest_Stack(void)
* Visibility  :        static
* Description :        RAM selftest with stack
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
static ClassB_testResult_t ClassB_RAMTest_Stack(void)
{
    register volatile uint8_t* originalStack;
    register volatile uint8_t* newStack = 0U;
    ClassB_testResult_t        result   = ClassB_testFailed;
    register volatile uint8_t  i;
    uint32_t                   currentSP;
    uint32_t                   size;
    /*int32_t                    SPDelta;*/  /*QAC*/
    uint32_t                   RamEndAddress;
    RamEndAddress = (ClassB_RAM_START + ClassB_RAM_SIZE);

    /*lint -save -e50 -e58 -e438 -e501 -e506 -e732 accepted signed type */
    /* get the current stack placement and save this information */
    currentSP     = getSP();
    originalStack = (uint8_t*)currentSP;

    /* calculate the stack size (byte) */
    size = initialSP - currentSP;    /*PRQA S 2810*/
    /* replace the stack */
    /* find a sufficient area */
  
    if (ClassB_RAM_START == (initialSP & ClassB_RAM_START))   /*PRQA S 2810*/
    {
        /* check SP exceeds the end of the RAM */
        if ((initialSP + size) >= RamEndAddress)              /*PRQA S 2810*/
        {
            /*lint -e737 -e648 accepted Overflow in computing constant, MISRA 2004 Rule 12.11 */
            /* use the RAM before the current SP to save the stack */
            newStack = (uint8_t*)(currentSP - StackNegOffset);  /*QAC*/
            result   = ClassB_testInProgress;
        }
        else
        {   /* check for enough place for a copy */
            if ((initialSP + size + StackOffset) < RamEndAddress)   /*PRQA S 2810*/
            {
                /*lint -e713 accepted Loss of precision */
                newStack = (uint8_t*)(initialSP + StackOffset);     /*PRQA S 2810*/
                result   = ClassB_testInProgress;
            }
        }
    }
    /*lint -restore */
    else
    {
        result = ClassB_testFailed;
    }

    if (ClassB_testInProgress == result)
    {
        /*lint -save -e613 accepted Possible use of null pointer */
        /* copy the data */
        size += 1U;        /* the last bytes in the stack are located after the SP */
        newStack--;
        for (i = 1U; i < size; i++)
        {
            *newStack-- = *(uint8_t*)(initialSP - i);           /*PRQA S 2810*/
        }
        /* correct the stack position */
        newStack++;
        /*lint -restore */
        /* move the stack pointer */
        setMSP((uint32_t)newStack);
        /* test the stack area */
        /*lint -save -e501 -e641 -e834 -e928 -e947 accepted cast from pointer to pointer MISRA 2004 Rule 11.4 */
        result = ClassB_RAMTest_MarchC((initialSP - size), size, ClassB_RAMtestStack);    /*PRQA S 2810*/
        /* save the result to a source location */
        *(uint8_t*)((uint32_t)(uint8_t*)&result - (uint32_t)originalStack + (uint32_t)newStack) = result; /*QAC*/
        /* *((uint8_t*)&result - originalStack + newStack) = result;*/  /*QAC*/
        /* calculate the stack size (byte) */
        size = initialSP - (uint32_t)originalStack;                     /*PRQA S 2810*/
        newStack += size-1u;
        /* recover the data */
        for (i = 1U; i < size; i++)
        {
            #if (defined (__ICCARM__))
                #pragma diag_suppress=Pa082 /* disable warning */
            #endif
            *(uint8_t*)(initialSP - i) = *newStack--;                   /*PRQA S 2810*/
            #if (defined (__ICCARM__))
                #pragma diag_default=Pa082  /* redefine warnings */
            #endif
        }
        /* recover the stack area */
        setMSP((uint32_t)originalStack);
        /*lint -restore */
    }
    return(result);
}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        ClassB_testResult_t ClassB_RAMTest_POST(void)
* Visibility  :        static
* Description :        RAM selftest without stack
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
ClassB_testResult_t ClassB_RAMTest_POST(void)
{
    ClassB_testResult_t result;

    /** Select according SAM_D20 derivate */
    result = ClassB_RAMTest_MarchC(ClassB_RAM_START, ClassB_RAM_SIZE/sizeof(uint32_t), ClassB_RAMtestNormal);

#if (ClassB_TEST_POST_STACK == ClassB_Enabled)
    /* continue if primary test passed */
    if (result == ClassB_testPassed)
    {
        result = ClassB_RAMTest_Stack();
    }
#else   /* ClassB_TEST_POST_STACK */
    #warning "(ClassB library) ram_test.c: RAMTest_Stack (POST): unused feature"
#endif  /* ClassB_TEST_POST_STACK */
    return(result);
}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        ClassB_POST(void)
* Visibility  :        public
* Description :        uC selftest task
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void ClassB_POST(void)
{
    volatile ClassB_testResult_t result;                        /* variable is located in the stack */

    /* fill to prevent warnings */
    result = ClassB_testFailed;

#if (ClassB_TEST_POST_WDT == ClassB_Enabled)
    result = ClassB_WDTTest_POST();                             /* test WDT with reset */

    if (ClassB_testFailed == result)
    {
        LoopForever();                                          /* remains until WDT test passes */
    }
#endif /* ClassB_TEST_POST_WDT */

#if (ClassB_TEST_POST_RAM == ClassB_Enabled)
/*! @cond ClassB_TEST_POST_RAM */
    result = ClassB_RAMTest_POST();                             /* stack is not tested here */

    if (ClassB_testFailed == result)
    {
        LoopForever();                                          /*PRQA S 2880*//* remains if RAM test fails */
    }
/*! @endcond */
#endif

}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        Post_RamInit(void)
* Visibility  :        public
* Description :        RAM init
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
void Post_RamInit(void)
{
    uint8_t cnt = 0;
    uint8_t i = 0;
    for (i=0; i<8; i++)
    {
        for (cnt=0; cnt < 16; cnt++)
        {
            if (cnt == 15)
            {
                #if (CRC_COM == 1)
                RamTest[i][cnt] = CRC8(RamTest[i], 15);
                #endif
            }
            else
               RamTest[i][cnt] = i*10 + cnt;
        }
    }
}

/*=====================================================================================================================
* Upward trac.:        Middle/post/post_if
* Method      :        Post_RamTest(void)
* Visibility  :        public
* Description :        RAM test
* Parameters  :
* ---------------------------------------------------------------------------------------------------------------------
*     Type      | Name(Direction) |                                      Description
* ---------------------------------------------------------------------------------------------------------------------
* Status_Ref  | Status   | Status reference
* ---------------------------------------------------------------------------------------------------------------------
* Return type :        void
* ===================================================================================================================*/
bool Post_RamTest(void)
{
    uint8_t cnt = 0;
    for (cnt=0; cnt<8; cnt++)
    {   
        #if (CRC_COM == 1)
        if (RamTest[cnt][15] != CRC8(RamTest[cnt], 15))
        #endif
        {
            return false;
        }
    }

    return true;
}
#endif /*POST_MODULE */
