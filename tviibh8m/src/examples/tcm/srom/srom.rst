SROM: SROM 
==========
Description
^^^^^^^^^^^
- This example executes three SROM API. “silicon id”, “read fuse byte”, and “read unique id”. 

Target Device
^^^^^^^^^^^^^
- Traveo-II CYT2B7x devices

CPU Board
^^^^^^^^^
- CYTVII-B-E-1M-176-CPU Rev. C Board

Dependency
^^^^^^^^^^
- None.

Expectation
^^^^^^^^^^^
- Read value will be stored into global variables “SiIdReadValues”, “readEfuseByteMap”, and 
  “UnIdReadValues”. The value depends on CPU revision and its life stage.