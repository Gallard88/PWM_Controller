
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
  PWM_Con_t ptr;
  unsigned int i;
  float duty;

  ptr = PWM_Connect();
  if ( ptr == NULL ) {
    printf("Couldn't connect\nIs PWM_Controller running?");
    return 0;
  }

  if ( PWM_isConnected(ptr) == 0 ) {
    printf("Hardware Disconnected\n");
    return 0;
  } else {
    printf("Hardware Connected\n");
    printf("PID: %d\n", (int) PWM_GetPid(ptr));
  }

  if ( argc == 3 ) {
    i = atoi(argv[1]);
    duty = atof(argv[2]);
    PWM_SetPWM(ptr, i, duty);
    printf("PWM Set: %d = %1.2f\n", i, duty);
    return 0;
  }

  printf("Current %1.2f\n", PWM_GetCurrent(ptr));
  printf("Voltage %1.2f\n", PWM_GetVoltage(ptr));
  printf("Temp:   %1.2f\n", PWM_GetTemp(ptr));

  for ( i = 0; i < PWM_Num_Chanels; i ++) {
    printf("PWM %d: %1.2f\n", i, PWM_GetPWM(ptr, i));
  }
  return 0;
}

// *****************
// *****************
