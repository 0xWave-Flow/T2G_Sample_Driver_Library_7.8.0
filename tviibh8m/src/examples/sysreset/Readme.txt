**To be used without Debugger**

Use Buttons as per the Package/PSVP, to trigger different types of Reset.
The Reset cause is displayed using the Base Board USER_LED 0,1,2. 
If buttons on CPU board exist, those buttons can be used for checking CY_SYSRESET_WDT and CY_SYSRESET_ACT_FAULT

BUTTON                   LED0 LED1 LED2 ResetCause
------------------------------------------------------------
BB_BUTTON1 or CB_BUTTON1 1    0    0    CY_SYSRESET_WDT
BB_BUTTON2 or CB_BUTTON2 0    1    0    CY_SYSRESET_ACT_FAULT
BB_BUTTON3               1    1    0    CY_SYSRESET_SOFT
BB_BUTTON4               0    0    1    CY_SYSRESET_MCWDT0
CB_XRES_BUTTON           1    0    1    CY_SYSRESET_XRES

