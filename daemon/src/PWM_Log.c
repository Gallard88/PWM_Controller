
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
const char Filename[] = "/var/log/PWM_Log";
#define TIMER_TICK		(60) // sec

// *****************
PWM_Con_t PWM_Ptr;

// *****************
void RunTimer(int sig)
{
	FILE *fp;
	struct tm *loc_time;
	time_t current;

  if ( PWM_isConnected(PWM_Ptr) == 0 )
		return;
	current = time(NULL);
	loc_time = localtime(&current);

	fp = fopen(Filename, "a+");
  if ( fp == NULL )
		return;

  fprintf(fp, "%04d/%02d/%02d, ", loc_time->tm_year+1900, loc_time->tm_mon+1, loc_time->tm_mday);
  fprintf(fp, "%02d:%02d:%02d, ", loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec);

	fprintf(fp, "%1.2f, ", PWM_GetVoltage(PWM_Ptr));
  fprintf(fp, "%1.2f, ", PWM_GetCurrent(PWM_Ptr));
	fprintf(fp, "%1.2f\n", PWM_GetTemp(PWM_Ptr));
	fclose(fp);
}

// *****************
void Setup_Timer(void)
{
  struct itimerval timer;

	// Configure the timer to expire after 25 msec...
	timer.it_value.tv_sec = TIMER_TICK;
	timer.it_value.tv_usec = 0;
	// ... and every 25 msec after that.
	timer.it_interval.tv_sec = TIMER_TICK;
	timer.it_interval.tv_usec = 0;

		// Start a virtual timer.
  setitimer (ITIMER_REAL, &timer, NULL);
}

// *****************
void Run_CtrlC(int sig)
{
  exit(0);
}

// *****************
void Setup_SignalHandler(void)
{
  struct sigaction sig;

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &Run_CtrlC;
  sigaction (SIGINT , &sig, NULL);
  sigaction (SIGTERM , &sig, NULL);
  sigaction (SIGABRT , &sig, NULL);
  sigaction (SIGUSR1 , &sig, NULL);
  sigaction (SIGUSR2 , &sig, NULL);

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &RunTimer;
  sigaction (SIGALRM , &sig, NULL);
}

// *****************
int main( int argc, char *argv[] )
{
  PWM_Ptr = PWM_Connect();
  if ( PWM_Ptr == NULL ) {
    printf("Couldn't connect\nIs PWM_Controller running?\n");
    return 0;
  }

  Setup_Timer();
	Setup_SignalHandler();
	RunTimer(0);
	while (1)
		sleep(600);

  return 0;
}

// *****************
// *****************
