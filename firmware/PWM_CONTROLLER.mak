CC = iccavr
LIB = ilibw
CFLAGS =  -e -D__ICC_VERSION=722 -D_EE_EXTIO -DATMega2560  -l -g -MExtended -MLongJump -MHasMul -MEnhanced -Wf-str_in_flash -Wf-use_elpm -Wf-const_is_flash -DCONST="" 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -g -e:0x40000 -xcall -ucrtatmega.o -bfunc_lit:0xe4.0x40000 -dram_end:0x21ff -bdata:0x200.0x21ff -dhwstk_size:30 -beeprom:0.4096 -fihx_coff -S2
FILES = adc.o CmdProcessor.o eeprom.o pwm.o PWM_Cmds.o timer.o uart1.o uart3.o main.o 

PWM_CONTROLLER:	PWM_CONTROLLER_pre $(FILES)
	$(CC) -o PWM_CONTROLLER $(LFLAGS) @PWM_CONTROLLER.lk   -llpatm256 -lcatm256
PWM_CONTROLLER_pre:
	touch main.c
adc.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h .\adc.h
adc.o:	adc.c
	$(CC) -c $(CFLAGS) adc.c
CmdProcessor.o: C:\iccv7avr\include\string.h C:\iccv7avr\include\_const.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h C:\iccv7avr\include\ctype.h .\CmdProcessor.h
CmdProcessor.o:	CmdProcessor.c
	$(CC) -c $(CFLAGS) CmdProcessor.c
eeprom.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h .\eeprom.h
eeprom.o:	eeprom.c
	$(CC) -c $(CFLAGS) eeprom.c
pwm.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h .\pwm.h
pwm.o:	pwm.c
	$(CC) -c $(CFLAGS) pwm.c
PWM_Cmds.o: C:\iccv7avr\include\stdlib.h C:\iccv7avr\include\_const.h C:\iccv7avr\include\limits.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h C:\iccv7avr\include\string.h C:\iccv7avr\include\ctype.h .\adc.h .\CmdProcessor.h .\PWM_Cmds.h .\pwm.h .\uart1.h .\uart3.h .\eeprom.h .\conf_eeprom.h
PWM_Cmds.o:	PWM_Cmds.c
	$(CC) -c $(CFLAGS) PWM_Cmds.c
timer.o: .\timer.h C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h
timer.o:	timer.c
	$(CC) -c $(CFLAGS) timer.c
uart1.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h C:\iccv7avr\include\_const.h .\uart1.h .\uart.c
uart1.o:	uart1.c
	$(CC) -c $(CFLAGS) uart1.c
uart3.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h C:\iccv7avr\include\_const.h .\uart3.h .\uart.c
uart3.o:	uart3.c
	$(CC) -c $(CFLAGS) uart3.c
main.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h C:\iccv7avr\include\string.h C:\iccv7avr\include\_const.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h .\Adc.h .\conf_eeprom.h .\eeprom.h .\CmdProcessor.h .\pwm.h .\PWM_Cmds.h .\timer.h .\uart1.h .\uart3.h
main.o:	main.c
	$(CC) -c $(CFLAGS) main.c
