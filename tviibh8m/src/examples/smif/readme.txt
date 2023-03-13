/************** General *****************/

1. Change value of USED_SMIF_CHANNEL defined in "tvii_series_smif_ex_adopter.h" in accordance with your hardware.
2. Check the "MemoryDeviceToBoardGuide.xls" to identify memories supported by the particular CPU board. "Supported CPU Board" column shows the particular memory part (listed in "Memory" column) tested with these many boards.


/****************************************/
/********* For Body High Series *********/
/****************************************/

/** 1. Using quad SPI **/

  - Hardware settings 

    + If using PSVP adopterboad
      1. Short R205
      2. Short R208
      3. Detach R210
      4. Detach R217
      5. Attach pull up register at a pin of R210 which is connected to Hyper bus device's chip select.
      6. Attach pull up register at a pin of R217 which is connected to Hyper bus device's chip select.

    + If using CYTVII-B-H-176-CPU boad
      1. Close R169, R170, R221, R223, R247, R250, R252, R254, R256
      2. Open R155, R157

    + If using CYTVII-B-H-272-CPU boad
      1. Close R190, R191, J57(1-2), J58(1-2), J59(1-2), J60(1-2), J61(1-2), J62(1-2), J64(1-2)
      2. Open R189, R193

    + If using CYTVII-B-H-320-CPU boad
      1. Close R190, R191
      2. Open R189, R193

  -Software settings

    + Copy example code "main_XXXX.c" in working directory similarly to other examples.
    + Copy also "SMIF_TestData.h" in working directory.
    + Copy also "tvii_series_smif_ex_adopter.h" in working directory.
    + Compile it. if you hit the compile error which says memory is lacking, please secure space by modifying .ld file.

/** 2. Using quad Hyper bus device **/

  - Hardware settings 

    + If using PSVP adopterboad
      1. Short R210
      2. Short R217
      3. Detach R205
      4. Detach R208
      5. Attach pull up register at a pin of R205 which is connected to Hyper bus device's chip select.
      6. Attach pull up register at a pin of R208 which is connected to Hyper bus device's chip select.
      7. Detach R211 and put lowpass filter between the pins to delay RWDS. (See HyperBusHWSettingForPSVP.png)

    + If using CYTVII-B-H-176-CPU boad
      1. Close R155, R157, R221, R223, R247, R250, R252, R254, R256
      2. Open R169, R170

    + If using CYTVII-B-H-272-CPU boad
      1. Close R189, R193, J57(1-2), J58(1-2), J59(1-2), J60(1-2), J61(1-2), J62(1-2), J64(1-2)
      2. Open R190, R191

    + If using CYTVII-B-H-320-CPU boad
      1. Close R189, R193
      2. Open R190, R191

  -Software settings

    + Copy example code "main_XXXX.c" in working directory similarly to other examples.
    + Copy also "SMIF_TestData.h" in working directory.
    + Copy also "tvii_series_smif_ex_adopter.h" in working directory.
    + Compile it. if you hit the compile error which says memory is lacking, please secure space by modifying .ld file.


/****************************************/
/******** For Cluster 2D Series *********/
/****************************************/

/** 1. Using quad SPI SMIF0 **/

  - Hardware settings 

    + If using PSVP adopterboad
      1. Short R205
      2. Short R208
      3. Detach R210
      4. Detach R217
      5. Attach pull up register at a pin of R210 which is connected to Hyper bus device's chip select.
      6. Attach pull up register at a pin of R217 which is connected to Hyper bus device's chip select.

    + If using CYTVII-C-2D-6M-500-CPU boad
      Not supported by the Cluster CPU Boards.

    + If using CYTVII-C-2D-4M-216-CPU boad
      Not supported by the Cluster CPU Boards.

  -Software settings

    + Copy example code "main_XXXX.c" in working directory similarly to other examples.
    + Copy also "SMIF_TestData.h" in working directory.
    + Copy also "tvii_series_smif_ex_adopter.h" in working directory.
    + Compile it. if you hit the compile error which says memory is lacking, please secure space by modifying .ld file.

/** 2. Using quad Hyper bus device SMIF0 **/

  - Hardware settings 

    + If using PSVP adopterboad
      1. Short R210
      2. Short R217
      3. Detach R205
      4. Detach R208
      5. Attach pull up register at a pin of R205 which is connected to Hyper bus device's chip select.
      6. Attach pull up register at a pin of R208 which is connected to Hyper bus device's chip select.

    + If using CYTVII-C-2D-6M-500-CPU boad
      For HyperFlash device R199 should be closed on SMIF0 bus.

    + If using CYTVII-C-2D-4M-216-CPU boad
      For HyperFlash device R199 should be closed on SMIF0 bus.

  -Software settings

    + Copy example code "main_XXXX.c" in working directory similarly to other examples.
    + Copy also "SMIF_TestData.h" in working directory.
    + Copy also "tvii_series_smif_ex_adopter.h" in working directory.
    + Compile it. if you hit the compile error which says memory is lacking, please secure space by modifying .ld file.

/** 3. Using quad SPI SMIF1 **/

  - Hardware settings 

    + If using PSVP Extension board for Cluster
      1. Detach R744
      2. Detach R745
      3. Short R746
      4. Short R747
      5. Replace R135 (1[Kƒ¶] originally) with a 430[ƒ¶] register

    + If using CYTVII-C-2D-6M-500-CPU boad
      Not supported by the Cluster CPU Boards.

    + If using CYTVII-C-2D-4M-216-CPU boad
      Not supported by the Cluster CPU Boards.

  -Software settings

    + Copy example code "main_XXXX.c" in working directory similarly to other examples.
    + Copy also "SMIF_TestData.h" in working directory.
    + Copy also "tvii_series_smif_ex_adopter.h" in working directory.
    + Compile it. if you hit the compile error which says memory is lacking, please secure space by modifying .ld file.

/** 4. Using quad Hyper bus device SMIF1 **/

  - Hardware settings 

    + If using PSVP Extension board for Cluster
      1. Short R744
      2. Short R745
      3. Detach R746
      4. Detach R747
      5. Replace R121 (0[ƒ¶] originally) with a 20[ƒ¶] register

    + If using CYTVII-C-2D-6M-500-CPU boad
      For HyperRAM device R514 should be closed on SMIF1 bus and for maximum frequency please remove 
      R544, R545, R546, R547, R548, R549, R550, R552, R555, R556, R557, R558, R559, R560, R561, R562,
      R563, R565, R567, R568, R569, R570.

    + If using CYTVII-C-2D-4M-216-CPU boad
      For HyperRAM device R514 should be closed on SMIF1 bus.

  -Software settings

    + Copy example code "main_XXXX.c" in working directory similarly to other examples.
    + Copy also "SMIF_TestData.h" in working directory.
    + Copy also "tvii_series_smif_ex_adopter.h" in working directory.
    + Compile it. if you hit the compile error which says memory is lacking, please secure space by modifying .ld file.


