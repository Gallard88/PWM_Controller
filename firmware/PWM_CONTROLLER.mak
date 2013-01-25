CC = iccavr
LIB = ilibw
CFLAGS =  -e -D__ICC_VERSION=722 -D_EE_EXTIO -DATMega2560  -l -g -MExtended -MLongJump -MHasMul -MEnhanced -Wf-str_in_flash -Wf-use_elpm -Wf-const_is_flash -DCONST="" 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -g -e:0x40000 -xcall -ucrtatmega.o -bfunc_lit:0xe4.0x40000 -dram_end:0x21ff -bdata:0x200.0x21ff -dhwstk_size:30 -beeprom:0.4096 -fihx_coff -S2
FILES = pwm.o timer.o uart1.o uart3.o main.o 

PWM_CONTROLLER:	$(FILES)
	$(CC) -o PWM_CONTROLLER $(LFLAGS) @PWM_CONTROLLER.lk   -llpatm256 -lcatm256
pwm.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h .\pwm.h
pwm.o:	pwm.c
	$(CC) -c $(CFLAGS) pwm.c
timer.o: .\timer.h C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h
timer.o:	timer.c
	$(CC) -c $(CFLAGS) timer.c
uart1.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h C:\iccv7avr\include\_const.h .\uart1.h .\uart.c
uart1.o:	uart1.c
	$(CC) -c $(CFLAGS) uart1.c
uart3.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h C:\iccv7avr\include\stdio.h C:\iccv7avr\include\stdarg.h C:\iccv7avr\include\_const.h .\uart3.h .\uart.c
uart3.o:	uart3.c
	$(CC) -c $(CFLAGS) uart3.c
main.o: C:\iccv7avr\include\iom2560v.h C:\iccv7avr\include\_iom640to2561v.h C:\iccv7avr\include\string.h C:\iccv7avr\include\_const.h .\pwm.h .\timer.h .\uart1.h .\uart3.h
main.o:	main.c
	$(CC) -c $(CFLAGS) main.c
