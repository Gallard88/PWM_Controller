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

#include "parson.h"
#include "PWM_Controller.h"
#include "Serial.h"

// *****************
const char PWM_Con_Settings_file[] = "PWM_Controller.conf";
#define SHM_SIZE	sizeof(Pwm_Con_Mem)

Pwm_Con_Mem *PWM_ptr;
JSON_Value *JSON_Settings;
JSON_Object *J_Object;

int Serial_fd;
int Send_data;

// *****************
void System_Shutdown(void)
{
  // detach from the segment:
  if (shmdt(PWM_ptr) == -1)
    perror("shmdt");

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
void Write_SystemTime(unsigned long time)
{
	// write the existing time to the controller.
}

void Send_PWMChanelData(void)
{
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
	// send temp limit,
	// send temp update rate
	// send current limit,
	// send current update rate
	// force unit to restart

}

// *****************
int main(int argc, char *argv[])
{
	int write_time = 0;
	struct timeval tv;
  int loop = 1;
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

  printf("Read Settings\n");
	Read_Settings();

	while ( loop > 0 )
	{
		printf("Open Serial port\n");

		Connect_To_Port();
		while ( Serial_fd >= 0 )
		{
			PWM_ptr->port_connected = 1;
			gettimeofday(&tv, NULL);
			// run receiver
			//
			if (( tv.tv_sec - write_time ) > 3600)
			{// here once an hour
				write_time = tv.tv_sec;
				Write_SystemTime(tv.tv_sec);
			}
			else
			if ( Send_data )
			{
				pthread_mutex_lock( &PWM_ptr->access );
				if ( PWM_ptr->data_ready )
				{
					Send_PWMChanelData();
					PWM_ptr->data_ready = 0;
				}
				pthread_mutex_unlock( &PWM_ptr->access );
				Send_data = 0;
			}
		}
		sleep(10);
	}

  return 0;
}


// *****************
// *****************

