PWM_Controller
==============

A 3 prong project.
Hardware, Firmware, and a Linux based daemon.

Hardware is based around an Atmel ATMega2560.
Communication is via USB.
Can control upto 16 PWM chanels. Each chanel times out  and disables
itself 7 seconds after it's last updated.
The system can measure it's current consumption, ambient temperature, and input voltage.
Library supplied to control via other software.


