/***************************************************************************//**
* \file main_cm0plus.c
*
* \version 1.0
*
* \brief Main example file for CM0+
*
********************************************************************************
* \copyright
* Copyright 2016-2020, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"  

// Static Function Declarations
static void Sample_WDTInitForReset(void);
static void Sample_MCWDT0InitForReset(void);
static void Sample_ButtonInitForReset(void);
static void Sample_ActiveMPUAccessFaultInitForReset(void);

typedef struct
{
    volatile stc_GPIO_PRT_t* port;
    uint32_t                 pin;
    cy_stc_gpio_pin_config_t cfg;
} stc_gpio_setting_t;

// GPIO configuration for LEDs
#define BB_LED0 0
#define BB_LED1 1
#define BB_LED2 2

const stc_gpio_setting_t user_led_cfg[] =
{
//  {         port,         pin, outVal,                driveMode,           hsiom, intEdge, intMask, vtrip, slewRate, driveSel, },
    { CY_LED0_PORT, CY_LED0_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF, CY_LED0_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul, },
    { CY_LED1_PORT, CY_LED1_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF, CY_LED1_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul, },
    { CY_LED2_PORT, CY_LED2_PIN,    0ul, CY_GPIO_DM_STRONG_IN_OFF, CY_LED2_PIN_MUX,     0ul,     0ul,   0ul,      0ul,      0ul, },
};
#define SIZE_LED_PORT_NUM (sizeof(user_led_cfg)/sizeof(stc_gpio_setting_t))

// GPIO configuration for Buttons
typedef enum
{
    SYSRESET_TEST_WDT       = 0, 
    SYSRESET_TEST_ACT_FAULT,
    SYSRESET_TEST_SOFT,
    SYSRESET_TEST_MCWDT0,
    SYSRESET_TEST_INVALID,
} en_reset_test_case_t;

en_reset_test_case_t btnNo2TestCaseTable[] = 
{
#ifdef CY_BUTTON1_PORT
    SYSRESET_TEST_WDT,
#endif
#ifdef CY_BUTTON2_PORT
    SYSRESET_TEST_ACT_FAULT,
#endif
#ifdef CY_BUTTON3_PORT
    SYSRESET_TEST_SOFT,
#endif
#ifdef CY_BUTTON4_PORT
    SYSRESET_TEST_MCWDT0,
#endif
#ifdef CY_CB_USER_BUTTON1_PORT
    SYSRESET_TEST_WDT,
#endif
#ifdef CY_CB_USER_BUTTON2_PORT
    SYSRESET_TEST_ACT_FAULT,
#endif
};

const stc_gpio_setting_t user_button_cfg[] =
{
//  {                    port,                    pin, outVal,        driveMode,                      hsiom,              intEdge, intMask, vtrip, slewRate, driveSel,  }
#ifdef CY_BUTTON1_PORT
    {         CY_BUTTON1_PORT,         CY_BUTTON1_PIN,    0ul, CY_GPIO_DM_HIGHZ,         CY_BUTTON1_PIN_MUX, CY_GPIO_INTR_FALLING,     1ul,   0ul,      0ul,      0ul, },
#endif
#ifdef CY_BUTTON2_PORT
    {         CY_BUTTON2_PORT,         CY_BUTTON2_PIN,    0ul, CY_GPIO_DM_HIGHZ,         CY_BUTTON2_PIN_MUX, CY_GPIO_INTR_FALLING,     1ul,   0ul,      0ul,      0ul, },
#endif
#ifdef CY_BUTTON3_PORT
    {         CY_BUTTON3_PORT,         CY_BUTTON3_PIN,    0ul, CY_GPIO_DM_HIGHZ,         CY_BUTTON3_PIN_MUX, CY_GPIO_INTR_FALLING,     1ul,   0ul,      0ul,      0ul, },
#endif
#ifdef CY_BUTTON4_PORT
    {         CY_BUTTON4_PORT,         CY_BUTTON4_PIN,    0ul, CY_GPIO_DM_HIGHZ,         CY_BUTTON4_PIN_MUX, CY_GPIO_INTR_FALLING,     1ul,   0ul,      0ul,      0ul, },
#endif
#ifdef CY_CB_USER_BUTTON1_PORT
    { CY_CB_USER_BUTTON1_PORT, CY_CB_USER_BUTTON1_PIN,    0ul, CY_GPIO_DM_HIGHZ, CY_CB_USER_BUTTON1_PIN_MUX, CY_GPIO_INTR_FALLING,     1ul,   0ul,      0ul,      0ul, },
#endif
#ifdef CY_CB_USER_BUTTON2_PORT
    { CY_CB_USER_BUTTON2_PORT, CY_CB_USER_BUTTON2_PIN,    0ul, CY_GPIO_DM_HIGHZ, CY_CB_USER_BUTTON2_PIN_MUX, CY_GPIO_INTR_FALLING,     1ul,   0ul,      0ul,      0ul, },
#endif
};
#define SIZE_BUTTON_PORT_NUM (sizeof(user_button_cfg)/sizeof(stc_gpio_setting_t))

const cy_stc_sysint_irq_t button_irq_cfg[] =
{
//  {               sysIntSrc,          intIdx, isEnabled }
#ifdef CY_BUTTON1_PORT
    {         CY_BUTTON1_IRQN, CPUIntIdx7_IRQn,      true },
#endif
#ifdef CY_BUTTON2_PORT
    {         CY_BUTTON2_IRQN, CPUIntIdx7_IRQn,      true },
#endif
#ifdef CY_BUTTON3_PORT
    {         CY_BUTTON3_IRQN, CPUIntIdx7_IRQn,      true },
#endif
#ifdef CY_BUTTON4_PORT
    {         CY_BUTTON4_IRQN, CPUIntIdx7_IRQn,      true },
#endif
#ifdef CY_CB_USER_BUTTON1_PORT
    { CY_CB_USER_BUTTON1_IRQN, CPUIntIdx7_IRQn,      true },
#endif
#ifdef CY_CB_USER_BUTTON2_PORT
    { CY_CB_USER_BUTTON2_IRQN, CPUIntIdx7_IRQn,      true },
#endif
};

en_reset_test_case_t g_testCase = SYSRESET_TEST_INVALID;
volatile uint32_t testRead0;
volatile uint32_t testRead1;

// Button Interrupt Handler
void ButtonIntHandler(void)
{
    /* If button falling edge detected */
    for(uint32_t i = 0; i < SIZE_BUTTON_PORT_NUM; i++)
    {
        uint32_t intStatus;
        intStatus = Cy_GPIO_GetInterruptStatusMasked(user_button_cfg[i].port, user_button_cfg[i].pin);
        if (intStatus != 0uL)
        {
            Cy_GPIO_ClearInterrupt(user_button_cfg[i].port, user_button_cfg[i].pin);
            g_testCase = btnNo2TestCaseTable[i];
        }
    }
}

int main(void)
{
    SystemInit();

    uint32_t tRstReason = 0u;
    
    __enable_irq(); /* Enable global interrupts. */
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR); 
    
    /* Configure the LEDs */
    for(uint32_t i = 0; i < SIZE_LED_PORT_NUM; i++)
    {
        Cy_GPIO_Pin_Init(user_led_cfg[i].port, user_led_cfg[i].pin, &user_led_cfg[i].cfg);
    }

    // Buttons Init
    Sample_ButtonInitForReset();

    /* Read the RESET reason */
    tRstReason = Cy_SysReset_GetResetReason();
    
    /* activate LEDs as a combination based on type of a reset and lock in 
       a while loop */
    if( ( tRstReason & CY_SYSRESET_WDT ) == CY_SYSRESET_WDT )
    {
        // '001'
        Cy_GPIO_Write(user_led_cfg[BB_LED0].port, user_led_cfg[BB_LED0].pin, 1);
        Cy_GPIO_Write(user_led_cfg[BB_LED1].port, user_led_cfg[BB_LED1].pin, 0);
        Cy_GPIO_Write(user_led_cfg[BB_LED2].port, user_led_cfg[BB_LED2].pin, 0);
    }
    else if( ( tRstReason & CY_SYSRESET_ACT_FAULT ) == CY_SYSRESET_ACT_FAULT )
    {
        // '010'
        Cy_GPIO_Write(user_led_cfg[BB_LED0].port, user_led_cfg[BB_LED0].pin, 0);
        Cy_GPIO_Write(user_led_cfg[BB_LED1].port, user_led_cfg[BB_LED1].pin, 1);
        Cy_GPIO_Write(user_led_cfg[BB_LED2].port, user_led_cfg[BB_LED2].pin, 0);
    }
    else if( ( tRstReason & CY_SYSRESET_SOFT ) == CY_SYSRESET_SOFT )
    {
        // '011'
        Cy_GPIO_Write(user_led_cfg[BB_LED0].port, user_led_cfg[BB_LED0].pin, 1);
        Cy_GPIO_Write(user_led_cfg[BB_LED1].port, user_led_cfg[BB_LED1].pin, 1);
        Cy_GPIO_Write(user_led_cfg[BB_LED2].port, user_led_cfg[BB_LED2].pin, 0);
    }
    else if( ( tRstReason & CY_SYSRESET_MCWDT0 ) == CY_SYSRESET_MCWDT0 )
    {
        // '100'
        Cy_GPIO_Write(user_led_cfg[BB_LED0].port, user_led_cfg[BB_LED0].pin, 0);
        Cy_GPIO_Write(user_led_cfg[BB_LED1].port, user_led_cfg[BB_LED1].pin, 0);
        Cy_GPIO_Write(user_led_cfg[BB_LED2].port, user_led_cfg[BB_LED2].pin, 1);
    }
    else if( ( tRstReason & CY_SYSRESET_XRES ) == CY_SYSRESET_XRES )
    {
        // '101'
        Cy_GPIO_Write(user_led_cfg[BB_LED0].port, user_led_cfg[BB_LED0].pin, 1);
        Cy_GPIO_Write(user_led_cfg[BB_LED1].port, user_led_cfg[BB_LED1].pin, 0);
        Cy_GPIO_Write(user_led_cfg[BB_LED2].port, user_led_cfg[BB_LED2].pin, 1);
    }

    // Clear all reset reasons
    Cy_SysReset_ClearAllResetReasons();

    for(;;)
    {
        switch(g_testCase)
        {
        case SYSRESET_TEST_WDT:
            // WDT Init for Reset
            Sample_WDTInitForReset();
            // Reset the button state
            g_testCase = SYSRESET_TEST_INVALID;
            break;

        case SYSRESET_TEST_ACT_FAULT:
            Sample_ActiveMPUAccessFaultInitForReset();
            // Reset the button state
            g_testCase = SYSRESET_TEST_INVALID;
            break;

        case SYSRESET_TEST_SOFT: // Soft Reset
            NVIC_SystemReset();
            // Reset the button state
            g_testCase = SYSRESET_TEST_INVALID;
            break;

        case SYSRESET_TEST_MCWDT0:
            // MCWDT Init for Reset
            Sample_MCWDT0InitForReset();
            // Reset the button state
            g_testCase = SYSRESET_TEST_INVALID;
            break;

        default: // Do Nothing
            break;
        }

    }
}

static void Sample_WDTInitForReset(void)
{
    /*-----------------------*/
    /* Configuration for WDT */
    /*-----------------------*/
    Cy_WDT_Init();                      /* Upper Limit: 1sec and reset */
    Cy_WDT_Unlock();
    Cy_WDT_SetUpperLimit(64000);        /* Upper Limit: 2sec (override) */
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);  /* This is necessary when using debugger */
    Cy_WDT_Lock();
    Cy_WDT_Enable();
}

static void Sample_MCWDT0InitForReset(void)
{
    /*********************************************************************/
    /*****      Set Core DeepSleep of which can pause counter        *****/
    /*********************************************************************/
    Cy_MCWDT_CpuSelectForDpSlpPauseAction(MCWDT0, CY_MCWDT_PAUSED_BY_DPSLP_CM0);

    /*********************************************************************/
    /*****                        Set actions                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER0, CY_MCWDT_WARN_ACTION_NONE);

    Cy_MCWDT_SetLowerAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_NONE);
    Cy_MCWDT_SetUpperAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_ACTION_FAULT_THEN_RESET); // sub counter 1 upper limit causes to reset
    Cy_MCWDT_SetWarnAction(MCWDT0, CY_MCWDT_COUNTER1, CY_MCWDT_WARN_ACTION_NONE);

    Cy_MCWDT_SetSubCounter2Action(MCWDT0, CY_MCWDT_CNT2_ACTION_NONE);

    /*********************************************************************/
    /*****                      Set limit values                     *****/
    /*********************************************************************/
    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER0, 0, 0);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER0, 0, 0);
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER0, 100, 0);

    Cy_MCWDT_SetLowerLimit(MCWDT0, CY_MCWDT_COUNTER1, 0, 0);
    Cy_MCWDT_SetWarnLimit(MCWDT0, CY_MCWDT_COUNTER1, 0, 0);
    Cy_MCWDT_SetUpperLimit(MCWDT0, CY_MCWDT_COUNTER1, 64000, 0);  /* 2 sec when clk_lf = 32KHz */

    Cy_MCWDT_SetToggleBit(MCWDT0, CY_MCWDT_CNT2_MONITORED_BIT4); // means 32768 count period


    /*********************************************************************/
    /*****                        Set options                        *****/
    /*********************************************************************/
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER0, 0u); // disable
    Cy_MCWDT_SetAutoService(MCWDT0, CY_MCWDT_COUNTER1, 0u); // disable

    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER0, 1u); // enable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER1, 1u); // enable
    Cy_MCWDT_SetSleepDeepPause(MCWDT0, CY_MCWDT_COUNTER2, 1u); // enable

    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER0, 1u); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER1, 1u); // enable
    Cy_MCWDT_SetDebugRun(MCWDT0, CY_MCWDT_COUNTER2, 1u); // enable

    // Enable all the counters
    Cy_MCWDT_Enable(MCWDT0, CY_MCWDT_CTR_Msk, 0u);

    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER0) != 1u);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER1) != 1u);
    while(Cy_MCWDT_GetEnabledStatus(MCWDT0, CY_MCWDT_COUNTER2) != 1u);
}

static void Sample_ButtonInitForReset(void)
{
    // Mapping Buttons for a reset case
    for(uint32_t i = 0; i < SIZE_BUTTON_PORT_NUM; i++)
    {
        Cy_GPIO_Pin_Init(user_button_cfg[i].port, user_button_cfg[i].pin, &user_button_cfg[i].cfg);
    }
    
    /* Setup GPIO for BUTTON interrupt */
    for(uint32_t i = 0; i < SIZE_BUTTON_PORT_NUM; i++)
    {
        Cy_SysInt_InitIRQ(&button_irq_cfg[i]);
        Cy_SysInt_SetSystemIrqVector(button_irq_cfg[i].sysIntSrc, ButtonIntHandler);
        NVIC_SetPriority(button_irq_cfg[i].intIdx, 3);
        NVIC_EnableIRQ(button_irq_cfg[i].intIdx);
    }
}

static void Sample_ActiveMPUAccessFaultInitForReset(void)
{
    cy_stc_smpu_cfg_t smpuStructTemp = { 0 };
    cy_stc_sysflt_t tFlt_Temp = {0};
   
    /*********************************************************************/
    /*****                 Fault report settings                     *****/
    /*********************************************************************/
    {
        Cy_SysFlt_ClearStatus(FAULT_STRUCT0); // clear status (typically this process is done by boot code)
        Cy_SysFlt_SetMaskByIdx(FAULT_STRUCT0, CY_SYSFLT_MPU_0); // enalbe Fault Master 0 (CM0+) MPU
        Cy_SysFlt_SetInterruptMask(FAULT_STRUCT0);
    }
    
    tFlt_Temp.ResetEnable = true;
    tFlt_Temp.OutputEnable = false;
    tFlt_Temp.TriggerEnable = false;
    Cy_SysFlt_Init(FAULT_STRUCT0, &tFlt_Temp);

    {
        cy_stc_sysint_irq_t irq_cfg;
        irq_cfg = (cy_stc_sysint_irq_t){
            .sysIntSrc  = cpuss_interrupts_fault_0_IRQn,
            .intIdx     = CPUIntIdx2_IRQn,
            .isEnabled  = true,
        };
        irq_cfg.sysIntSrc = cpuss_interrupts_fault_0_IRQn;
        Cy_SysInt_InitIRQ(&irq_cfg);
        NVIC_SetPriority(CPUIntIdx2_IRQn, 0);
        NVIC_ClearPendingIRQ(CPUIntIdx2_IRQn);
        NVIC_EnableIRQ(CPUIntIdx2_IRQn);
    }

    /* 1. Setting for MSx_CTL */
    if(Cy_Prot_ConfigBusMaster(CPUSS_MS_ID_CM0, 1 /* privireged */, 0 /* secure */, 1 << (6-1) /* enable context 6 */ )
                               != CY_PROT_SUCCESS)
    {
        while(1); // Setting error
    }

    /* 2. Setting for MPU PC = "6" */
    /* This time MPU_PC can be set to only "6" because only SMPU_MS_CTL_PC_MASK[6] = "1"*/
    if(Cy_Prot_SetActivePC(CPUSS_MS_ID_CM0, 6) != CY_PROT_SUCCESS)
    {
        while(1); // Setting error
    }

    /* 3. Setting for SMPU_STRUCT (using STRUCT 2 and 3 for this time )*/
    /* 0x28020000 ~ 0x28027FFF: Only masters which have "5" as PC value can access this area */
    /* 0x28028000 ~ 0x2802FFFF: Only masters which have "6" as PC value can access this area */
    smpuStructTemp.regionSize = CY_PROT_SIZE_32KB; // 32KB: 0x8000 Byte
    smpuStructTemp.subregions = 0x00;
    smpuStructTemp.userPermission = CY_PROT_PERM_RWX;
    smpuStructTemp.privPermission = CY_PROT_PERM_RWX;
    smpuStructTemp.secure = 0; // Non secure
    smpuStructTemp.pcMatch = 0;

    smpuStructTemp.address = (uint32_t*)(0x28020000);
    smpuStructTemp.pcMask = 1 << (5 - 1); // enable context 5
    if(Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT2, &smpuStructTemp) != CY_PROT_SUCCESS)
    {
        while(1); // Setting error
    }

    smpuStructTemp.address = (uint32_t*)(0x28028000);
    smpuStructTemp.pcMask = 1 << (6 - 1); // enable context 6
    if(Cy_Prot_ConfigSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT3, &smpuStructTemp) != CY_PROT_SUCCESS)
    {
        while(1); // Setting error
    }

    /* 4. Enable SMPU_STRUCT */
    if(Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT2) != CY_PROT_SUCCESS)
    {
        while(1); // Setting error
    }
    if(Cy_Prot_EnableSmpuSlaveStruct(PROT_SMPU_SMPU_STRUCT3) != CY_PROT_SUCCESS)
    {
        while(1); // Setting error
    }

    /* 5. Memory access test1. This area can be accessed by masters which have "6" as PC value */
    /* CM0+ has "6" as PC value, then these accesses are expected to be allowed */
    testRead0 = 0;
    *(uint32_t*)(0x28028000) = 0xA5A5A5A5;
    testRead0 = *(uint32_t*)(0x28028000);

    /* 6. Memory access test2. This area can be accessed by masters which have "5" as PC value */
    /* CM0+ has "6" as PC value, then these accesses are expected to be restricted and cause HardFault */
    testRead1 = 0;
    *(uint32_t*)(0x28020000) = 0xA5A5A5A5;
    testRead1 = *(uint32_t*)(0x28020000);
	
}


/* [] END OF FILE */

