
The examples are common across cores, and only application core example will be made available.
i.e., main_cm7_0/1.c only for lot of features.
In case if there is any dependency on CM0+ core, respective source (main_cm0plus.c) will be available.

How to convert main_cm7_0/1.c into main_cm0plus.c ?
1. Rename the file from main_cm0plus.c
2. Add the API "Cy_SysEnableApplCore" into the example source, sample is shown below
    SystemInit();
    __enable_irq();
    /* Enable CM7_0/1. CY_CORTEX_M7_APPL_ADDR is calculated in linker script, check it in case of problems. */
    Cy_SysEnableApplCore(CORE_CM7_0, CY_CORTEX_M7_0_APPL_ADDR);
    Cy_SysEnableApplCore(CORE_CM7_1, CY_CORTEX_M7_1_APPL_ADDR);
3. This will enable the application core and its respective power, otherwise not possible.

How do you use the example source ?
1. Copy the source file above into the src folder after renaming them as "main_cm0plus.c" or "main_cm7_0/1.c"
2. Open the respective workspace 
3. Rebuild, run or debug the sample