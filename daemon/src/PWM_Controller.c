
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

#include "parson.h"
#include "PWM_Controller.h"
#include "Serial.h"
#include "Cmds_PWM.h"

// *****************
// Options
#define __DAEMONISE__		1

// *****************
const char PWM_Con_Settings_file[] = "/etc/PWM_Controller.conf";
static int Serial_fd;

#define SYSTEM_DELAY		 10000	// ms
#define TIMER_TICK			100000  // ms
static const struct timeval system_time = {0,SYSTEM_DELAY};

static char *PortName;

// *****************
static void Setup_Timer(int start);
static void System_Shutdown(void);
static void Check_Serial(int rv);
static void Read_Settings(void);
static void Fork_LogModule(void);

// *****************
static void Read_Settings(void)
{
  JSON_Value *JSON_Settings;
  JSON_Object *J_Object;
  int rv;
  int length;
  const char *ptr;

  JSON_Settings = json_parse_file(PWM_Con_Settings_file);
  rv = json_value_get_type(JSON_Settings);

  if ( rv != JSONObject ) {
    syslog(LOG_EMERG, "JSON data incorrect, %d\n", rv );
    exit( -1);
  }

  J_Object = json_value_get_object(JSON_Settings);
  if ( J_Object == NULL ) {
    syslog(LOG_EMERG, "JSON: Failed to get object" );
    exit( -1);
  }

  ptr = json_object_get_string(J_Object, "Serial_Port");
  if ( ptr != NULL ) {
    length = strlen(ptr);
    PortName = malloc(length+1);
    strncpy(PortName, ptr, length);
    PortName[length] = 0;
  } else {
    syslog(LOG_NOTICE, "Com Port name not set");
    exit(-1);
  }
  json_value_free(JSON_Settings);
}

// *****************
void Connect_To_Port(void)
{
  Serial_fd = Serial_Openport(PortName, 115200, 0,0 );
  if ( Serial_fd < 0 )
    return;
  syslog(LOG_NOTICE, "Com Port %s connected", PortName);
  PWM_SetConnected();
  Setup_Timer(1);

  // send start up commands.
  // send temp update rate
  // send current update rate
  // send voltage update rate
  PWM_SetStartup(Serial_fd);

  // force unit to restart
  PWM_Send_Restart(Serial_fd);
}

// *****************
void RunTimer(int sig)
{ // receive signal, set up next tick.

  if ( Serial_fd >= 0 )
	{
		int rv = PWM_Send_ChanelData( Serial_fd );
		Check_Serial(rv);
	}
}

// *****************
void Setup_Timer(int start)
{
  struct itimerval timer;

	memset(&timer, 0, sizeof(struct itimerval));

  if ( start > 0 ) {
    // Configure the timer to expire after 25 msec...
    // ... and every 25 msec after that.
    timer.it_value.tv_usec = TIMER_TICK;
    timer.it_interval.tv_usec = TIMER_TICK;
  }
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

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &RunTimer;
  sigaction (SIGALRM , &sig, NULL);
  sigaction (SIGUSR1 , &sig, NULL);
  sigaction (SIGUSR2 , &sig, NULL);
}

// *****************
static char *args[] = {(char *) 0};		// for when we fork the log process.
// *****************
int main(int argc, char *argv[])
{
  int loop = 1;
  int rv;
  fd_set readfds;
  struct timeval select_time;

  openlog("PWM_Controller", LOG_PID , LOG_USER );
  syslog(LOG_NOTICE, "PWM_Controller Startup");

  // register shutdown function.
  atexit(System_Shutdown);

  Read_Settings();
	Fork_LogModule();
  PWM_CreateSharedMemory();
  Setup_SignalHandler();

	#ifdef __DAEMONISE__
  rv = daemon( 0, 0 );
  if ( rv < 0 ) {
    syslog(LOG_EMERG, "Daemonise failed" );
    exit(-1);
  }
#else
  printf("System starting - Debug mode\n");
#endif

  while ( loop > 0 ) {
    Connect_To_Port();
    while ( Serial_fd >= 0 ) {
      FD_ZERO(&readfds);
      FD_SET(Serial_fd, &readfds);
      select_time = system_time;

      rv = select(Serial_fd+1, &readfds, NULL, NULL, &select_time);	// wait indefinately
      if ( rv < 0 )
        continue;

      if ( FD_ISSET(Serial_fd, &readfds) ) {
        // run receiver
				char readBuf[4096];

        int length = strlen(readBuf);
        rv = read(Serial_fd, readBuf + length, sizeof(readBuf) - length);
        Check_Serial(rv);

        if ( rv > 0 ) {
          readBuf[length + rv] = 0;

          while ( rv >= 0 ) {
            rv = CmdParse_ProcessString(Cmd_Table, readBuf, Serial_fd);
          }
        }
      }
    }
    sleep(60);
  }
  return 0;
}

// *****************
static void Fork_LogModule(void)
{
  pid_t pid = fork();
  if ( pid == 0 ) { // child
    syslog(LOG_NOTICE,"Forking Log module");
    execv("/usr/sbin/PWM_Log", args);

  } else if ( pid < 0 ) { // error
    syslog(LOG_ERR,"Failed t fork log module");
  }
}

// *****************
void Check_Serial(int rv)
{
  if ( rv < 0 ) {
    Setup_Timer(0);
    PWM_ClearSharedMemory();
    Serial_fd = Serial_ClosePort(Serial_fd);
    syslog(LOG_EMERG, "Serial coms lost, %d", errno);
  }
}

// *****************
static void System_Shutdown(void)
{
  PWM_ClearSharedMemory();
  syslog(LOG_NOTICE, "System shutting down");
  closelog();
}

// *****************
// *****************

