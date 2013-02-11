# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall  -std=c99
#LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
PWM_Controllerdir=../
PWM_Controller_PROGRAMS= PWM_Controller PWM_Test

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
        PWM_Test.c      \
        PWM_Controller.h


