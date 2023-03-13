CPU BUTTON 1     : DeepSleep <-> Active
CPU LED 1        : On: CM0+ is in Active, Off: CM0+ is in DeepSleep
CPU LED 2        : On: CM7_0 is in Active, Off: CM7_0 is in DeepSleep
Base Board LED 0 : On: CM7_1 is in Active, Off: CM7_1 is in DeepSleep

Please check EXT_CLK port defined in CM0+ main code to ensure the MCU is really going to DeepSleep.

