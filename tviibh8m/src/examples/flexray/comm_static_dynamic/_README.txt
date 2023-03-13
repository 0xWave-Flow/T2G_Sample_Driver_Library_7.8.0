1) Copy all files (not folders!) in this folder to the src folder (you can omit files with _PSVP suffix if building for silicon and vice versa).
2) In order to run a FlexRay network with 2 nodes, you will need 2 separate TVII SDL projects:
   - for the "Node1" project, copy the files within the "Node1" subfolder to the src folder (this can be skipped as it is the default)
   - for the "Node2" project, copy the files within the "Node2" subfolder to the src folder

3) Files in the cfg subfolder are just for reference and contain the FlexConfig project, FIBEX export and CANoe configuration

Example description:
====================

FlexRay communication using static and dynamic segment

Node status:
------------

If the node is in the NORMAL_ACTIVE state, the Adapter/CPU board LED is periodically toggling.


Functional behavior:
--------------------

Every FlexRay cycle a byte value is incremented by a certain value X (see further below) and send in the node's static slot.

Every FlexRay cycle the byte value which has been received from the other node's static slot will be displayed as a "bargraph" value using the 10 base board LEDs.

The increment value X can be controlled by the other node and is received from the other node's dynamic slot.

PSVP:
    By pressing button 1 on the base board (SWITCH_1) the value X for the other node can be decreased
    By pressing button 2 on the base board (SWITCH_2) the value X for the other node can be increased
176-pin CPU-Board (silicon):
    By pressing button SW2 on the CPU board the value X for the other node can be decreased
    By pressing button SW4 on the CPU board the value X for the other node can be increased

Or in other words:

A node can control its own bargraph "display speed" (which is based on the received increment value from the other node)
by pressing the buttons (which will send a dynamic message with the new increment value to the other node)



