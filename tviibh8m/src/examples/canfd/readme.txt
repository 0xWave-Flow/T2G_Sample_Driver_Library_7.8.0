Please refer to base board schematics and put in the jumper settings.

Say for CANFD0/CANFD0_0 following settings will have to be done
Short - J70/72 for CAN TXD/RXD lines
Short - J71 if wakeup is needed (currently in the sample not used)
Short - J69 (terminal resistor)

* Copy main_cm0plus.c or main_cm4.c (or main_cm7_0.c) into your workspace and re-build.
  Both file's behavior is same. Only one of those can be used at once.

* If you have CANoe, you can use CANFD_CFG.cfg for it.
  It's configured simple test for this example.
  Just run and monitor using trace, statistics window.

* Usage:
  [1] Copy one of the main file to the src folder.
  [2] Rebuild the project.
  [3] Start debug and run.
  [4] Start CANoe with the configuration.
      That is configured:
        - CANFD mode (500kHz for nominal bitrate, 1MHz for fast bitrate. Samping point of both are 75%)
        - 4 message is sent from periodically. (100ms)
          2 standard IDs and 2 extended IDs.
        - All ID filters are configured as the clasical filter,
          only following IDs are accepted by the MCU:
                STD : 0x010, 0x020
                EXT : 0x10010, 0x10020
          At the receive call back CanfdRxCallback in the main.c,
          received message is sent back with received ID +1.
          You can check the behavior on the CANoe trace window.

