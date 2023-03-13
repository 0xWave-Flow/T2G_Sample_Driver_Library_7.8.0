This example works only when debugger is NOT connected to the MCU.
When the contents of the infinite loop at bottom of main function are commented out, this example should work as below.

main_cm0plus.c
At first this program turns a LED off.
After 2 seconds, MCWDT0 reset will occur. At the top of main code, if MCWDT0 reset is detected, CPU is going to blink the LED on the adapter/CPU board.
Duration of LED off should be 2 seconds.


main_cm7_0.c
At first this program turns a LED off.
After 2 seconds, MCWDT1 reset will occur. At the top of main code, if MCWDT1 reset is detected, CPU is going to blink the LED on the adapter/CPU board.
Duration of LED off should be 2 seconds.

main_cm7_1.c
At first this program turns a LED off.
After 2 seconds, MCWDT1 reset will occur. At the top of main code, if MCWDT2 reset is detected, CPU is going to blink the LED on the adapter/CPU board.
Duration of LED off should be 2 seconds.


=== IMPORTANT ===

The example code can only be used on one core at a time!
