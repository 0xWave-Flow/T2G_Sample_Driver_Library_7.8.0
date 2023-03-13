This example works only when debugger is NOT connected to the MCU.
When the contents of the infinite loop at bottom of main function are commented out, this example should work as below.

main_cm0plus.c
At first this program turns a LED off.
After 2 seconds, WDT reset will occur. At the top of main code, if WDT reset is detected, CPU is going to blink the LED.
Duration of LED off should be 2 seconds.


main_cm7_x.c
At first this program turns a LED off.
After 1 seconds, WDT reset will occur. At the top of main code, if WDT reset is detected, CPU is going to blink the LED.
Duration of LED off should be 1 seconds.


=== IMPORTANT ===

The example code can only be used on one core at a time!
