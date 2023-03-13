#ifndef EXAMPLE_CONFIG_H
#define EXAMPLE_CONFIG_H

#define EXAMPLE_CONFIG__CM0P_TO_CM7_0     0  // fixed definition
#define EXAMPLE_CONFIG__CM0P_TO_CM7_1     1  // fixed definition
#define EXAMPLE_CONFIG__CM7_1_TO_CM7_0    2  // fixed definition

#define EXAMPLE_CONFIG_SELECTED           EXAMPLE_CONFIG__CM0P_TO_CM7_0    // Choose one of options above


// Do not change below!

#if   (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_0) || (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM0P_TO_CM7_1)
    #if (CY_CPU_CORTEX_M0P)
        #define EP_INDEX_THIS_CPU         0
        #define EP_INDEX_OTHER_CPU        1
    #else
        #define EP_INDEX_THIS_CPU         1
        #define EP_INDEX_OTHER_CPU        0
    #endif
#elif (EXAMPLE_CONFIG_SELECTED == EXAMPLE_CONFIG__CM7_1_TO_CM7_0)
    #if defined(CY_CORE_CM7_1)
        #define EP_INDEX_THIS_CPU         0
        #define EP_INDEX_OTHER_CPU        1
    #else
        #define EP_INDEX_THIS_CPU         1
        #define EP_INDEX_OTHER_CPU        0
    #endif
#endif    
    

#endif // EXAMPLE_CONFIG_H

