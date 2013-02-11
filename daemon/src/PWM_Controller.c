
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
#include "Command_List.h"

// *****************
const char PWM_Con_Settings_file[] = "PWM_Controller.conf";
#define SHM_SIZE	sizeof(Pwm_Con_Mem)

Pwm_Con_Mem *PWM_ptr;
JSON_Value *JSON_Settings;
JSON_Object *J_Object;

int Serial_fd;

#define SERIAL_BUF_SIZE 4096
char SerialRead_Buf[SERIAL_BUF_SIZE];

//CmdFuncList_t Cmd_List;


// *****************
void System_Shutdown(void)
{
  // detach from the segment:
  if (shmdt(PWM_ptr) == -1)
    perror("shmdt");
  syslog(LOG_EMERG, "System shutting down");
	closelog();
}

// *****************
int Create_Shared_Memory( void )
{
  key_t key;
  int shmid;
	int fd;

  // Make sure the file exists.
  fd = open(PWM_KEY_FILE, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
	/* Only wanted to make sure that the file exists. */
  close(fd);

  // Generate memory key. */
  key = ftok(PWM_KEY_FILE, PWM_MEM_KEY);
  if (key  == -1)
  {
    perror("ftok");
    exit(1);
  }

  // connect to (and possibly create) the segment:
  if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1)
  {
    perror("shmget");
    exit(1);
  }

  // attach to the segment to get a pointer to it:
  PWM_ptr = shmat(shmid, (void *)0, 0);
  if ((char *)PWM_ptr == (char *)(-1))
  {
    perror("shmat");
    exit(1);
  }

  pthread_mutex_init(&PWM_ptr->access, NULL);
  return 0;
}

// *****************
void Check_Serial(int rv)
{
  if ( rv < 0 )
  {
		perror("Serial");
    PWM_ptr->port_connected = 0;
    Serial_fd = Serial_ClosePort(Serial_fd);
    syslog(LOG_EMERG, "Serial coms lost, %d", errno);
  }
}

// *****************
void Read_Settings(void)
{
	int rv;

  JSON_Settings = json_parse_file(PWM_Con_Settings_file);
	rv = json_value_get_type(JSON_Settings);

  if ( rv != JSONObject )
	{
		printf("System didn't work, %d\n", rv );
    exit( -1);
  }

  J_Object = json_value_get_object(JSON_Settings);
	if ( J_Object == NULL )
	{
		printf("JSON_Object == NULL\n");
		exit( -1);
	}
	printf("Opening Serial Port: %s\n", json_object_get_string(J_Object, "Serial_Port"));
}

// *****************
void Connect_To_Port(void)
{
	const char *name = json_object_get_string(J_Object, "Serial_Port");

	if ( name == NULL )
	{
		syslog(LOG_NOTICE, "Com Port name not set");
		exit(-1);
	}

 	Serial_fd = Serial_Openport((char *)name, 115200, 0,0 );
	if ( Serial_fd < 0 )
		return;
	syslog(LOG_NOTICE, "Com Port %s connected", name);
	// send start up commands.
	// read time
	Send_GetTime(Serial_fd);
	// send temp limit,
	// send temp update rate
	Send_TempData(Serial_fd, J_Object);

	// send current limit,
	// send current update rate
	Send_CurrentData(Serial_fd, J_Object);

	// force unit to restart
	Send_Restart(Serial_fd);
}

// *****************
void RunTimer(int sig)
{// receive signal, set up next tick.
	static int write_time;
  struct timeval tv;
  int rv = 1;
	int i;

  if ( Serial_fd < 0 )
    return ;	// serial port not configured so don't bother

  gettimeofday(&tv, NULL);
  if (( tv.tv_sec - write_time ) > 3600)
  {// here once an hour
    write_time = tv.tv_sec;
		printf("sending system time\n");
		rv = Send_SystemTime(Serial_fd, write_time);
    Check_Serial(rv);
  }

  pthread_mutex_lock( &PWM_ptr->access );
  if ( PWM_ptr->data_ready )
  {
		printf("sending PWM data\n");

		for ( i = 0; i < PWM_NUM_CHANELS; i++ )
		{
			rv = Send_PWMChanelData(Serial_fd, i, PWM_ptr->ch[i]);
			Check_Serial(rv);
		}
    PWM_ptr->data_ready = 0;
  }
  pthread_mutex_unlock( &PWM_ptr->access );
}

// *****************
void Setup_Timer(void)
{
  struct itimerval timer;
  struct sigaction sig;

  // Install timer_handler as the signal handler for SIGVTALRM.
  memset (&sig, 0, sizeof (struct sigaction));
  sig.sa_handler = &RunTimer;
  sigaction (SIGALRM , &sig, NULL);

  // Configure the timer to expire after 25 msec...
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 25000;
  // ... and every 25 msec after that.
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 25000;

  // Start a virtual timer.
  setitimer (ITIMER_REAL, &timer, NULL);
}

// *****************
int main(int argc, char *argv[])
{
  int loop = 1;
	int rv;
	int length;

  openlog("PWM_Controller", LOG_PID , LOG_USER );
  syslog(LOG_NOTICE, "PWM_Controller Startup");


	// register shutdown function.
	atexit(System_Shutdown);

//  printf("become a daemon\n");
//  daemon( 0, 0 );

  printf("create shared memory\n");
  if ( Create_Shared_Memory() < 0 )
  {
    printf("Shared Memory failed\n");
    exit(-1);
  }

//  Cmd_List = CmdParse_CreateFuncList();
//	Build_CmdList(Cmd_List);

  printf("Read Settings\n");
	Read_Settings();

	Setup_Timer();

	while ( loop > 0 )
	{
		printf("Open Serial port\n");

		Connect_To_Port();
		while ( Serial_fd >= 0 )
		{
			PWM_ptr->port_connected = 1;

			// run receiver
			length = strlen(SerialRead_Buf);
			rv = read(Serial_fd, SerialRead_Buf + length, SERIAL_BUF_SIZE - length);
			Check_Serial(rv);

			if ( rv > 0 )
			{
				SerialRead_Buf[length + rv] = 0;
				printf("CR %s\n",	strchr(SerialRead_Buf, '\n')== NULL? "Found":"Missing");
//				printf("Raw: %s\n", SerialRead_Buf);
//				rv = CmdParse_ProcessString(Cmd_Table, SerialRead_Buf, Serial_fd);
//				Check_Serial(rv);
			}
		}
		printf("Port Coms lost\n");
		break;
//		sleep(10);
	}

  return 0;
}


// *****************
// *****************

