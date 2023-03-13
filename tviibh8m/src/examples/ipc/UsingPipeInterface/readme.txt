
This example contains how to use the PIPE I/F.
The PIPE I/F uses the IPC channel and it conrols data flow and synchronization between end points.
This example does:
    - data send from one CPU to another.
    - Other CPU receives the data, then changes LEDs ON/OFF with received data.
      
=== IMPORTANT ===

Please edit example_config.h to select the communication flow:
 - CM0+  -> CM7_0
 - CM0+  -> CM7_1
 - CM7_1 -> CM7_0


