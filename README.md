PWM_Controller
==============

Hardware, firmware, and daemon to control my 16 channel PWM controller.
==============
Firmware:
Program can run all 16 PWM chanels.
Can read voltage, current and temperature, though these need to be calibrated better.

==============
Daemon.
Starts up, connects to serial port.
Can process Temp, Voltage, Current data.
Can send PWM data.
Really needs a signal handler for ^C.


