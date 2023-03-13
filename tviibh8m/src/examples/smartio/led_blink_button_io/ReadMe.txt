Please check the base board header.

For 8M 176 A0/B0 CPU boards
1. LED1 is connected to the configured IO (P15.1), short TP202 on the CPU board and JP7_10 on BB.
2. BUTTON1 is connected to the configured IO (P15.0), short TP200 on the CPU board and JP11_14 on BB.

For 8M 176 rev C CPU boards
1. LED: Connect J17_1 to TP202
2. BUTTON: Connect J15_2 to TP200

For 8M 320 CPU boards
1. LED1 is connected to the configured IO (P15.1), short P4_6 and JP7_10.
2. BUTTON1 is connected to the configured IO (P15.0), short P3_6 and JP11_14.

For 4M 272 CPU boards
1. LED1 is connected to the configured IO (P15.1) so please short P14_12 and P4_11.
2. BUTTON1 is connected to the configured IO (P15.0) so please short P14_11 and P13_12.

For C-2D-6M-A0 CPU boards rev B
1. Connect J55_2 (CPU board LED3 input) to P4_17 (CPU board test port)
2. Connect J55_4 (CPU board BUTTON  DOWN output) to P4_13 (CPU board test port)

For C-2D-6M-B0 CPU boards rev A
1. Connect J55_12 (CPU board LD1 input) to P13_11 (CPU board test port)
2. SW7 is connected to Smart IO input pin by default.

For C-2D-4M-216 CPU boards rev A
1. Connect J55_10 (CPU board LED LD3 input) to P9_6 (CPU board test port)
2. Connect J55_8  (CPU board BUTTON SW5 output) to P9_4 (CPU board test port)
