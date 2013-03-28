
// *****************
#define _XOPEN_SOURCE

// *****************
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "ShdMem.h"
#include "PWM_Controller.h"


// *****************
// *****************
int main( int argc, char *argv[] )
{
  unsigned int i;
  float duty;

	if ( PWM_Connect() < 0 )
	{
    printf("Couldn't connect\nIs PWM_Controller running?");
		return 0;
	}

	if ( PWM_isConnected() == 0 )
	{
    printf("Hardware Disconnected\n");
		return 0;
	}
	else
	{
    printf("Hardware Connected\n");
	}

  if ( argc == 3 )
  {
    i = atoi(argv[1]);
      duty = atof(argv[2]);
			PWM_SetPWM(i, duty);
      printf("PWM Set: %d = %1.2f\n", i, duty);
  }

  printf("Current %1.2f\n", PWM_GetCurrent());
  printf("Voltage %1.2f\n", PWM_GetVoltage());
  printf("Temp:   %1.2f\n", PWM_GetTemp());

  return 0;
}

// *****************
// *****************
