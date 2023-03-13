
This example contains how to use the IPC low level I/F.
The low level I/F directly controls the IPC.
This example does:
    - data send from CM7_0/CM7_1 to CM0+.
    - CM0+ receive the data, then change LED ON/OFF with received data.
      LED0-2 are used.
      
=== IMPORTANT ===

The IPC example code in main_cm7_0.c and main_cm7_1.c cannot be used at the same time!



