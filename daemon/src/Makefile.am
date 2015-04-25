# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall  -std=c99
#LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
PWM_Controllerdir=/usr/sbin/
PWM_Controller_PROGRAMS= PWM_Controller PWM_Test

lib_LIBRARIES = libPWMControl.a
libPWMControl_a_SOURCES = \
        PWM_Lib.c \
	ShdMem.h \
        PWM_Controller.h

include_HEADERS = PWM_Controller.h

# Install that start up script.
PWM_INITdir = /etc/init.d/
PWM_INIT_Data=PWM_Controller.sh

# INstall Log Rotate data
LogRotatedir = /etc/logrotate.d/
LogRotate_Data = PWM_Logrotate

PWM_Controller_SOURCES= \
	CmdParse.c \
	CmdParse.h \
	Cmds_PWM.c \
	Cmds_PWM.h \
	Serial.c \
	Serial.h \
	PWM_Controller.c \
	PWM_Controller.h

PWM_Test_SOURCES= \
        PWM_Test.c \
        PWM_Lib.c \
        PWM_Controller.h



