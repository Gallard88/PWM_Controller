# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall  -std=c99
#LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
PWM_Controllerdir=/usr/bin/
PWM_Controller_PROGRAMS= PWM_Controller PWM_Test

lib_LIBRARIES = libPWMControl.a
libPWMControl_a_SOURCES = \
        PWM_Lib.c \
        PWM_Controller.h

include_HEADERS = PWM_Controller.h

#include_HEADERS= PWM_Controller.h
PWM_Filesdir= /etc
PWM_Files_DATA= PWM_Controller.conf

PWM_Controller_SOURCES= \
	CmdParse.c \
	CmdParse.h \
	Command_List.c \
	Command_List.h \
	parson.c \
	parson.h \
	Serial.c \
	Serial.h \
	PWM_Controller.c \
	PWM_Controller.h

PWM_Test_SOURCES= \
        PWM_Test.c \
        PWM_Lib.c \
        PWM_Controller.h


