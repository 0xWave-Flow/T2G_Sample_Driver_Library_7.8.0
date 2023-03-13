These examples confirm the functionality of PC (Protection context) using SMPU.
CM0+ will take a memory access with violation, and CM7_0/CM7_1 catch the violation details by fault reporting.

1. Setting MSx_CTL (in SMPU) to allow to change PC of MPU to "6"
2. Setting PC of MPU associated to CM0+ to "6"
3. Setting SMPU STRUCTURE2 that only masters who have "6" as PC value can access the area (1).
4. Setting SMPU STRUCTURE3 that only masters who have "5" as PC value can access the area (2).
5. Accessing the area (1) to confirm CM0+  access the area.
6. Accessing the area (2) to confirm CM0+ can't access the area and cause HardFault.
7. CM7_0/CM7_1 was noticed of violation details by the fault reporting system.
