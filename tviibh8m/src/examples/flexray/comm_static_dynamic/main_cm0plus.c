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
#include "flexray_port_config.h"
#include "flexray_common_config.h"
#include "flexray_node_config.h"

#define ENABLE_VARIABLE_DEFINITION
#include "cy_bargraph_definitions.h"
#undef ENABLE_VARIABLE_DEFINITION

//===============================
// Defines/Macros

#define FLEXRAY0_LINE0_CPU_IRQ   CPUIntIdx2_IRQn
#define FLEXRAY0_LINE1_CPU_IRQ   CPUIntIdx3_IRQn
#define BUTTON_CPU_IRQ           CPUIntIdx3_IRQn

#if (CY_USE_PSVP == 1)
    #define SYNC_ATTEMPT_DELAY     1000000UL
    #define LED_TOGGLE_DELAY        100000UL
#else
    #define SYNC_ATTEMPT_DELAY    (1000000UL * 14)
    #define LED_TOGGLE_DELAY       (100000UL * 14)
#endif


#if (CY_USE_PSVP == 1)
    #define USER_LED_PORT                CY_AB_LED_PORT
    #define USER_LED_PIN                 CY_AB_LED_PIN
    #define USER_LED_PIN_MUX             CY_AB_LED_PIN_MUX
    
    #define USER_BUTTON_MINUS_PORT       CY_BUTTON1_PORT   
    #define USER_BUTTON_MINUS_PIN        CY_BUTTON1_PIN    
    #define USER_BUTTON_MINUS_PIN_MUX    CY_BUTTON1_PIN_MUX
    #define USER_BUTTON_MINUS_IRQN       CY_BUTTON1_IRQN
    #define USER_BUTTON_PLUS_PORT        CY_BUTTON2_PORT   
    #define USER_BUTTON_PLUS_PIN         CY_BUTTON2_PIN    
    #define USER_BUTTON_PLUS_PIN_MUX     CY_BUTTON2_PIN_MUX
    #define USER_BUTTON_PLUS_IRQN        CY_BUTTON2_IRQN
#else
    #define USER_LED_PORT                CY_CB_USER_LED1_PORT
    #define USER_LED_PIN                 CY_CB_USER_LED1_PIN
    #define USER_LED_PIN_MUX             CY_CB_USER_LED1_PIN_MUX 
    
    #define USER_BUTTON_MINUS_PORT       CY_CB_USER_BUTTON1_PORT   
    #define USER_BUTTON_MINUS_PIN        CY_CB_USER_BUTTON1_PIN    
    #define USER_BUTTON_MINUS_PIN_MUX    CY_CB_USER_BUTTON1_PIN_MUX
    #define USER_BUTTON_MINUS_IRQN       CY_CB_USER_BUTTON1_IRQN
    #define USER_BUTTON_PLUS_PORT        CY_CB_USER_BUTTON2_PORT   
    #define USER_BUTTON_PLUS_PIN         CY_CB_USER_BUTTON2_PIN    
    #define USER_BUTTON_PLUS_PIN_MUX     CY_CB_USER_BUTTON2_PIN_MUX    
    #define USER_BUTTON_PLUS_IRQN        CY_CB_USER_BUTTON2_IRQN
#endif


//===============================
// Variables

cy_stc_gpio_pin_config_t user_led_port_pin_cfg =             
{                                                  
    .outVal = 0x00,                                
    .driveMode = CY_GPIO_DM_STRONG_IN_OFF,    
    .hsiom = USER_LED_PIN_MUX,                           
};

cy_stc_gpio_pin_config_t user_button_minus_port_pin_cfg = 
{
    .outVal = 0x00,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = USER_BUTTON_MINUS_PIN_MUX,
    .intEdge = CY_GPIO_INTR_FALLING,
    .intMask = 1,    
};
cy_stc_gpio_pin_config_t user_button_plus_port_pin_cfg = 
{
    .outVal = 0x00,
    .driveMode = CY_GPIO_DM_HIGHZ,
    .hsiom = USER_BUTTON_PLUS_PIN_MUX,
    .intEdge = CY_GPIO_INTR_FALLING,
    .intMask = 1,    
};

cy_stc_sysint_irq_t button_minus_irq_cfg =
{
    .sysIntSrc  = USER_BUTTON_MINUS_IRQN,
    .intIdx     = BUTTON_CPU_IRQ,
    .isEnabled  = true,
};
cy_stc_sysint_irq_t button_plus_irq_cfg =
{
    .sysIntSrc  = USER_BUTTON_PLUS_IRQN,
    .intIdx     = BUTTON_CPU_IRQ,
    .isEnabled  = true,
};
cy_stc_sysint_irq_t flexray0_line0_irq_cfg = 
{
    .sysIntSrc  = flexray_0_interrupt0_IRQn,
    .intIdx     = FLEXRAY0_LINE0_CPU_IRQ,
    .isEnabled  = true,
};
cy_stc_sysint_irq_t flexray0_line1_irq_cfg = 
{
    .sysIntSrc  = flexray_0_interrupt1_IRQn,
    .intIdx     = FLEXRAY0_LINE1_CPU_IRQ,
    .isEnabled  = true,
};

static bool is_in_normal_active  = false;
static bool is_new_cycle_started = false;
static bool is_dyn_tx_requested  = false;
static PAYLOAD_STATIC_STRUCT  pl_stat_tx;
static PAYLOAD_STATIC_STRUCT  pl_stat_rx;
static PAYLOAD_DYNAMIC_STRUCT pl_dyn_tx;
static PAYLOAD_DYNAMIC_STRUCT pl_dyn_rx;


//===============================
// Static function prototypes
static void fr_task(void);
static void button_irq_handler(void);
static void flexray0_line0_irq_handler(void);
static void flexray0_line1_irq_handler(void);
static void flexray0_line0_callback(volatile stc_FLEXRAY_t * base, CY_FR_INTERRUPT_TYPE cy_fr_int);
static void flexray0_line1_callback(volatile stc_FLEXRAY_t * base, CY_FR_INTERRUPT_TYPE cy_fr_int);
static void fr_enter_normal_active_state(void);
static void fr_chi_include_function(volatile stc_FLEXRAY_t * base);



//===============================
// Function definitions


int main(void)
{
    SystemInit();
  
    __enable_irq();
    
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    // Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    // Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);
   
    /** ============= Port Setup ====================== */
    
    // User LED on AB/CB (indicates FlexRay NORMAL_ACTIVE state by toggling)
    Cy_GPIO_Pin_Init(USER_LED_PORT, USER_LED_PIN, &user_led_port_pin_cfg);
    
    // User buttons on baseboard used to control TX frequency of the other(!) node ("minus" button -> less frequent, "plus" button -> more frequent)
    Cy_GPIO_Pin_Init(USER_BUTTON_MINUS_PORT, USER_BUTTON_MINUS_PIN, &user_button_minus_port_pin_cfg);
    Cy_GPIO_Pin_Init(USER_BUTTON_PLUS_PORT,  USER_BUTTON_PLUS_PIN,  &user_button_plus_port_pin_cfg);
    
    // Baseboard LEDs (indicates received data from the other node)
    // Note: For correct bargraph operation please ensure that the other cores do not access one of the LEDs as well
    Cy_GPIO_Pin_Init(BARGRAPH_LED_A_PORT, BARGRAPH_LED_A_PIN, &bargraph_led_a_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_B_PORT, BARGRAPH_LED_B_PIN, &bargraph_led_b_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_C_PORT, BARGRAPH_LED_C_PIN, &bargraph_led_c_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_D_PORT, BARGRAPH_LED_D_PIN, &bargraph_led_d_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_E_PORT, BARGRAPH_LED_E_PIN, &bargraph_led_e_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_F_PORT, BARGRAPH_LED_F_PIN, &bargraph_led_f_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_G_PORT, BARGRAPH_LED_G_PIN, &bargraph_led_g_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_H_PORT, BARGRAPH_LED_H_PIN, &bargraph_led_h_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_I_PORT, BARGRAPH_LED_I_PIN, &bargraph_led_i_port_pin_cfg);
    Cy_GPIO_Pin_Init(BARGRAPH_LED_J_PORT, BARGRAPH_LED_J_PIN, &bargraph_led_j_port_pin_cfg); 
    
    // Initialize all FlexRay port pins
    for (uint8_t i = 0; i < (sizeof(flexray_pin_cfg) / sizeof(flexray_pin_cfg[0])); i++)
    {
        Cy_GPIO_Pin_Init(flexray_pin_cfg[i].portReg, flexray_pin_cfg[i].pinNum, &flexray_pin_cfg[i].cfg);
    }
    
    /** ============= Clock Setup ====================== */
    
    // FlexRay peripheral clock is derived from CLK_HF2, so it needs to be enabled
    Cy_SysClk_HfClkEnable(CY_SYSCLK_HFCLK_2);
    
  #if (CY_USE_PSVP == 1)
    // For PSVP we need to use the fractional divider to get to one of the standard bitrates e.g. supported by Vector CANoe (i.e. 1.25, 2.5, 5, 10 MBit/s)
    // If no such analyzer shall be used and user can control the frequency of the other network nodes, also the integer divider (e.g. 24 MHz) can be used
    // For 2.5MBit/s operation we need 20 MHz, there are 2 fractional dividers possible which result in a frequency slightly lower and higher.
    // The higher frequency turned out to be working if the other nodes run at nominal 20 MHz. But maybe for different setups/tolerances this may change
    
    Cy_SysClk_PeriphAssignDivider(PCLK_FLEXRAY0_CLK_FLEXRAY, CY_SYSCLK_DIV_24_5_BIT, 0u);    
    // Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_24_5_BIT, 0u, 0u, 7u);  // divider 1+(7/32) = 1.21875 --> 24 MHz / 1.21875 = 19.69MHz -> ~2.500 MBit/s at 8 times oversampling
    Cy_SysClk_PeriphSetFracDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_24_5_BIT, 0u, 0u, 6u);  // divider 1+(6/32) = 1.1875 --> 24 MHz / 1.1875 = 20.21MHz -> ~2.500 MBit/s at 8 times oversampling
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_24_5_BIT, 0u);
  #else
    
    // CLK_HF2 is configured to 80 MHz by SystemInit

    Cy_SysClk_PeriphAssignDivider(PCLK_FLEXRAY0_CLK_FLEXRAY, CY_SYSCLK_DIV_8_BIT, 0u);
    Cy_SysClk_PeriphSetDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_8_BIT, 0u, 0u); // divide-by-1 --> 80 MHz
    Cy_SysClk_PeriphEnableDivider(Cy_SysClk_GetClockGroup(PCLK_FLEXRAY0_CLK_FLEXRAY), CY_SYSCLK_DIV_8_BIT, 0u);
  #endif
	
    /** ============= Interrupt Setup ====================== */
    
    Cy_SysInt_InitIRQ(&button_minus_irq_cfg);
    Cy_SysInt_InitIRQ(&button_plus_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(button_minus_irq_cfg.sysIntSrc, button_irq_handler);
    Cy_SysInt_SetSystemIrqVector(button_plus_irq_cfg.sysIntSrc, button_irq_handler);    
    NVIC_SetPriority(BUTTON_CPU_IRQ, 3);
    NVIC_ClearPendingIRQ(BUTTON_CPU_IRQ);
    NVIC_EnableIRQ(BUTTON_CPU_IRQ);
    
    Cy_SysInt_InitIRQ(&flexray0_line0_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(flexray_0_interrupt0_IRQn, flexray0_line0_irq_handler);
    NVIC_SetPriority(FLEXRAY0_LINE0_CPU_IRQ, 0);
    NVIC_ClearPendingIRQ(FLEXRAY0_LINE0_CPU_IRQ);
    NVIC_EnableIRQ(FLEXRAY0_LINE0_CPU_IRQ);
    
    Cy_SysInt_InitIRQ(&flexray0_line1_irq_cfg);
    Cy_SysInt_SetSystemIrqVector(flexray_0_interrupt1_IRQn, flexray0_line1_irq_handler);
    NVIC_SetPriority(FLEXRAY0_LINE1_CPU_IRQ, 0);
    NVIC_ClearPendingIRQ(FLEXRAY0_LINE1_CPU_IRQ);
    NVIC_EnableIRQ(FLEXRAY0_LINE1_CPU_IRQ);
    
    
    /** ============= FlexRay Controller Setup ====================== */
    
    // Will set global enable bit of FlexRay IP
    cy_fr_api_enable_controller(FLEXRAY0);
    
    // FlexRay Controller must be in DEFAULT_CONFIG state to start configuration
    if(cy_fr_api_pocs_is_default_config(FLEXRAY0) == CY_FR_FALSE)
    {
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_FREEZE);    // enter HALT state
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_CONFIG);    // enter DEFAULT_CONFIG state
    }

    // Configure FlexRay Controller with the CHI file
    if (cy_fr_api_init_chi(FLEXRAY0, fr_chi_include_function) != CY_FR_OKAY)
    {
        while(1) // error
        {
        }
    }
    
    // Assign interrupt callback functions and enable all required FlexRay interrupts and interrupt lines
    cy_fr_api_interrupt_set_callback(FLEXRAY0, CY_FR_INT_LINE_0, flexray0_line0_callback);
    cy_fr_api_interrupt_set_callback(FLEXRAY0, CY_FR_INT_LINE_1, flexray0_line1_callback);
    cy_fr_api_interrupt_line_set(FLEXRAY0, CY_FR_SIR_CYCLE_STARTUP_INTERRUPT, CY_FR_INT_LINE_0);
    cy_fr_api_interrupt_enable  (FLEXRAY0, CY_FR_SIR_CYCLE_STARTUP_INTERRUPT);
    cy_fr_api_interrupt_line_set(FLEXRAY0, CY_FR_EIR_POC_ERROR_MODE_CHANGED, CY_FR_INT_LINE_1);
    cy_fr_api_interrupt_enable  (FLEXRAY0, CY_FR_EIR_POC_ERROR_MODE_CHANGED);
    cy_fr_api_interrupt_line_set(FLEXRAY0, CY_FR_SIR_STARTUP_COMPLETED, CY_FR_INT_LINE_1);
    cy_fr_api_interrupt_enable  (FLEXRAY0, CY_FR_SIR_STARTUP_COMPLETED);
    cy_fr_api_interrupt_line_enable(FLEXRAY0, CY_FR_INT_LINE_0);
    cy_fr_api_interrupt_line_enable(FLEXRAY0, CY_FR_INT_LINE_1);
    
    cy_fr_api_go_active(FLEXRAY0);     
    
    /** ============= FlexRay Sample Application ====================== */
    
    // Initially request sending of one dynamic message automatically (further dynamic messages only on button press)
    pl_dyn_tx.remote_bargraph_incr_val = BARGRAPH_INCR_INIT;    
    is_dyn_tx_requested = true;
    
    uint32_t sync_attempt_delay_cnt = 0;
    uint32_t led_toggle_delay_cnt   = 0;
    
    for(;;)
    {
        // If node is not in NORMAL_ACTIVE state, a periodic attempt to (re-)enter NORMAL_ACTIVE state is made
        if(is_in_normal_active == false)
        {
            sync_attempt_delay_cnt++;
            if(sync_attempt_delay_cnt > SYNC_ATTEMPT_DELAY)            
            {
                sync_attempt_delay_cnt = 0;
                fr_enter_normal_active_state();
            }
        }
        else
        {
            // Node is in NORMAL_ACTIVE state
            
            if(is_new_cycle_started == true)
            {
                // New cycle has started, so we execute the FlexRay task function
                fr_task();
                is_new_cycle_started = false;
            }
            
            // Periodically toggle an LED to indicate that node is in NORMAL_ACTIVE state
            led_toggle_delay_cnt++;
            if(led_toggle_delay_cnt > LED_TOGGLE_DELAY)            
            {
                led_toggle_delay_cnt = 0;
                Cy_GPIO_Inv(USER_LED_PORT, USER_LED_PIN);
            }
        }
    }
}


// Executed every time a new FlexRay cycle starts
static void fr_task(void)
{
    CY_FR_RX_BUFFER_HEADER_STRUCT rx_buf_head;
    CY_FR_RETURN_TYPE             rx_status;
    static uint8_t                bargraph_increment = BARGRAPH_INCR_INIT;
    
    // Prepare static frame payload
    pl_stat_tx.bargraph_slider_val += bargraph_increment;

    // Send static frame in every cycle
    cy_fr_api_tx_handler_buffer(FLEXRAY0, (uint32_t) &pl_stat_tx, PAYLOAD_STATIC_BYTES, STATIC_TX_BUF_AB, CY_FR_CHANNEL_A_B);

    // Send dynamic frame only when it has been requested (by a button press in this case)
    if(is_dyn_tx_requested == true)
    {
        cy_fr_api_tx_handler_buffer(FLEXRAY0, (uint32_t) &pl_dyn_tx, PAYLOAD_DYNAMIC_BYTES, DYNAMIC_TX_BUF_A, CY_FR_CHANNEL_A);
        is_dyn_tx_requested = false;
    }

    // Check for static frame received from the other node
    rx_status = cy_fr_api_rx_handler_buffer(FLEXRAY0, (uint32_t) &pl_stat_rx, &rx_buf_head, PAYLOAD_STATIC_BYTES, STATIC_RX_BUF_A, CY_FR_CHANNEL_A, cy_fr_api_new_rx_data_buffer(FLEXRAY0, STATIC_RX_BUF_A));
    // If Channel A failed, try Channel B
    // This is a simple handling of redundant data. No comparison of both channels, no error handling
    if(rx_status != CY_FR_OKAY)
    {
        rx_status = cy_fr_api_rx_handler_buffer(FLEXRAY0, (uint32_t) &pl_stat_rx, &rx_buf_head, PAYLOAD_STATIC_BYTES, STATIC_RX_BUF_B, CY_FR_CHANNEL_B, cy_fr_api_new_rx_data_buffer(FLEXRAY0, STATIC_RX_BUF_B));
    }
    // If reception was okay on one channel, update the bargraph
    if(rx_status == CY_FR_OKAY)
    {
        uint8_t u8Percent = (pl_stat_rx.bargraph_slider_val * 100UL) / 255UL;
        SET_BARGRAPH_PERCENT(u8Percent);
    }
    // Check for dynamic frame received from the other node
    rx_status = cy_fr_api_rx_handler_buffer(FLEXRAY0, (uint32_t) &pl_dyn_rx, &rx_buf_head, PAYLOAD_DYNAMIC_BYTES, DYNAMIC_RX_BUF_A, CY_FR_CHANNEL_A, cy_fr_api_new_rx_data_buffer(FLEXRAY0, DYNAMIC_RX_BUF_A));  

    // If a dynamic frame was received, update the bargraph increment value
    if(rx_status == CY_FR_OKAY)
    {
        // remote_bargraph_incr_val range is [0:15] because it also serves as "hexchar_val" depending on what is connected to this node
        // -> +1 to ensure there is at least some bargraph update
        bargraph_increment = pl_dyn_rx.remote_bargraph_incr_val + 1;
    }

}

static void button_irq_handler(void)
{
    uint32_t int_status;

    // Decrease bargraph increment value if "minus" button is pressed
    int_status = Cy_GPIO_GetInterruptStatusMasked(USER_BUTTON_MINUS_PORT, USER_BUTTON_MINUS_PIN);
    if (int_status != 0)
    {
        Cy_GPIO_ClearInterrupt(USER_BUTTON_MINUS_PORT, USER_BUTTON_MINUS_PIN);
        if(pl_dyn_tx.remote_bargraph_incr_val >= (BARGRAPH_INCR_MIN + BARGRAPH_INCR_STEP))
        {
            pl_dyn_tx.remote_bargraph_incr_val -= BARGRAPH_INCR_STEP;
        }

    }
    
    // Increase bargraph increment value if "plus" button is pressed
    int_status = Cy_GPIO_GetInterruptStatusMasked(USER_BUTTON_PLUS_PORT, USER_BUTTON_PLUS_PIN);
    if (int_status != 0)
    {
        Cy_GPIO_ClearInterrupt(USER_BUTTON_PLUS_PORT, USER_BUTTON_PLUS_PIN);
        if(pl_dyn_tx.remote_bargraph_incr_val <= (BARGRAPH_INCR_MAX - BARGRAPH_INCR_STEP))
        {
            pl_dyn_tx.remote_bargraph_incr_val += BARGRAPH_INCR_STEP;
        }
    }
    
    // Value has changed therefore request sending of dynamic frame
    is_dyn_tx_requested = true;
}  


// This function has been assigned as callback function by cy_fr_api_interrupt_set_callback() and 
// will be called by the dispatcher for active Line #0 interrupts of FlexRay #0
static void flexray0_line0_callback(volatile stc_FLEXRAY_t * base, CY_FR_INTERRUPT_TYPE cy_fr_int)
{
    switch(cy_fr_int)
    {
        // This status IRQ is set when a new FlexRay cycle starts
        case CY_FR_SIR_CYCLE_STARTUP_INTERRUPT:
            is_new_cycle_started = true;
            break;        
        default:
            break;
    }
}

// This function has been assigned as callback function by cy_fr_api_interrupt_set_callback() and 
// will be called by the dispatcher for active Line #1 interrupts of FlexRay #0
static void flexray0_line1_callback(volatile stc_FLEXRAY_t * base, CY_FR_INTERRUPT_TYPE cy_fr_int)
{
    switch(cy_fr_int)
    {
        // This error IRQ is set when the error mode of the POC has changed
        case CY_FR_EIR_POC_ERROR_MODE_CHANGED:
        {
            CY_FR_ERROR_MODE_TYPE current_error_mode;
            current_error_mode = cy_fr_api_get_error_mode(base);
            if(current_error_mode != CY_FR_ACTIVE)
            {
                is_in_normal_active = false;
            }
            break;
        }
        // This status IRQ is set when NORMAL_ACTIVE is entered
        case CY_FR_SIR_STARTUP_COMPLETED:
            is_in_normal_active = true;
            break;
        default:
            break;
    }
}

// This is the actual interrupt handler for the Line #0 IRQ of FlexRay #0
static void flexray0_line0_irq_handler(void)
{
    // Call dispatcher for IRQs enabled and assigned to this line
    cy_fr_api_interrupt_dispatcher(FLEXRAY0, CY_FR_INT_LINE_0);
}  

// This is the actual interrupt handler for the Line #1 IRQ of FlexRay #0
static void flexray0_line1_irq_handler(void)
{
    // Call dispatcher for IRQs enabled and assigned to this line
    cy_fr_api_interrupt_dispatcher(FLEXRAY0, CY_FR_INT_LINE_1);
}  

// This function tries to (re-)enter NORMAL_ACTIVE state
static void fr_enter_normal_active_state(void)
{
    CY_FR_POC_STATUS_TYPE poc_status;  

    // Check if FlexRay CC is not sync
    poc_status = cy_fr_api_get_poc_status(FLEXRAY0);
    if (poc_status != CY_FR_POCS_NORMAL_ACTIVE)
    {
        // Enter HALT state if not already in it
        if(cy_fr_api_pocs_is_halt(FLEXRAY0) == CY_FR_FALSE)
        {
            cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_FREEZE);
        }
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_CONFIG);                   // enter DEFAULT_CONFIG state 
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_CONFIG);                   // enter CONFIG state 
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_READY);                    // enter READY state 
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_RUN);                      // enter RUN state 
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_RESET_STATUS_INDICATORS);
        cy_fr_api_poc_command(FLEXRAY0, CY_FR_POCC_ALLOW_COLDSTART);          // do a coldstart or integration start 
    }
}



// This function includes the node specific FlexRay controller settings which are contained in the node specific CHI file
// It will be called within cy_fr_api_init_chi()
static void fr_chi_include_function(volatile stc_FLEXRAY_t * base)
{    
  #if   (NODE == NODE_1)
    #if (CY_USE_PSVP == 1)
      #include "Node1_PSVP.chi"
    #else
      #include "Node1.chi"
    #endif
  #elif (NODE == NODE_2)
    #if (CY_USE_PSVP == 1)
      #include "Node2_PSVP.chi"
    #else
      #include "Node2.chi"
    #endif
  #else
      #error "NODE is not defined or value is not supported!"
  #endif
}  



/* [] END OF FILE */
