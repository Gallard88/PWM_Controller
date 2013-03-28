
// *****************
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
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

// *****************
#include "ShdMem.h"
#include "Command_List.h"

// *****************
Pwm_Shd_Mem *PWM_ptr;

// *****************
void CL_Create_Shared_Memory( void )
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
    syslog(LOG_EMERG, "ftok() failed");
    exit(1);
  }

  // connect to (and possibly create) the segment:
  if ((shmid = shmget(key, PWM_CON_SHM_SIZE, 0644 | IPC_CREAT)) == -1)
  {
    syslog(LOG_EMERG, "shmget()");
    exit(1);
  }

  // attach to the segment to get a pointer to it:
  PWM_ptr = shmat(shmid, (void *)0, 0);
  if ((char *)PWM_ptr == (char *)(-1))
  {
    syslog(LOG_EMERG, "shmat()");
    exit(1);
  }

  pthread_mutex_init(&PWM_ptr->access, NULL);
}

// *****************
void CL_CLearSharedMemory(void)
{
  PWM_ptr->data_ready = 0;
  PWM_ptr->port_connected = 0;
  PWM_ptr->voltage = 0;
  PWM_ptr->current = 0;
  PWM_ptr->temperature = 0;
  PWM_ptr->firmware[0] = 0;

  // detach from the segment:
  if (shmdt(PWM_ptr) == -1)
    syslog(LOG_EMERG, "shmdt()");
}

// *****************
void CL_SetConnected(void)
{
  PWM_ptr->port_connected = 1;
}

// *****************
void CL_SetDisconnected(void)
{
  PWM_ptr->port_connected = 0;
}

// *****************
const char Restart_Cmd[] = "restart\r\n";
const char Get_Time[] = "time\r\n";

// *****************
int Send_TempData(int fd, JSON_Object *J_Object)
{
  char cmd[100];
  int rv;
  int value;

  // send temp limit,
//	value = (int)json_object_get_number(J_Object, "Temp_Limit");
  sprintf(cmd, "temp: %d\r\n", 50);
  rv = write (fd, cmd, strlen(cmd));

  // send temp update rate
  value = (int)json_object_get_number(J_Object, "Temp_Update");
  sprintf(cmd, "update: temp %d\r\n", value);
  rv = write (fd, cmd, strlen(cmd));

  return rv;
}

// *****************
int Send_VoltData(int fd, JSON_Object *J_Object)
{
  char cmd[100];
  int rv;
  int value;

  // send voolt update rate
  value = (int)json_object_get_number(J_Object, "Volt_Update");
  sprintf(cmd, "update: volt %d\r\n", value);
  rv = write (fd, cmd, strlen(cmd));

  return rv;
}

// *****************
int Send_CurrentData(int fd, JSON_Object *J_Object)
{
  char cmd[100];
  int rv;
  int value;

  // send current limit,
//	value = (int)json_object_get_number(J_Object, "Current_Limit");
  sprintf(cmd, "current: %d\r\n", 50);
  rv = write (fd, cmd, strlen(cmd));

  // send current update rate
  value = (int)json_object_get_number(J_Object, "Current_Update");
  sprintf(cmd, "update: current %d\r\n", value);
  rv = write (fd, cmd, strlen(cmd));

  return rv;
}

// *****************
int Send_SystemTime(int fd, unsigned long time)
{
  int rv = 0;
  // write the existing time to the controller.

  return rv;
}

// *****************
int Send_Restart(int fd)
{
  return write(fd, Restart_Cmd, strlen(Restart_Cmd));
}

// *****************
int Send_PWMChanelData(int fd)
{
  PWM_Ch *ch;
  int i;
  float duty;
  char cmd[20], msg[20*PWM_NUM_CHANELS];

//  if ( PWM_ptr->data_ready != 0)

  msg[0] = 0;
  pthread_mutex_lock( &PWM_ptr->access );
  for ( i = 0; i < PWM_NUM_CHANELS; i ++ )
  {
    ch = &PWM_ptr->ch[i];
    duty = ch->duty;
    if ( duty > 1.0 )
      duty = 1.0;
    if ( duty < 0 )
      duty = 0;
    sprintf(cmd, "pwm: %d %f\n\n", i, duty*100);
    strcat(msg, cmd);
  }
  pthread_mutex_unlock( &PWM_ptr->access );

  return write(fd, msg, strlen(msg) );
}

// *****************
int Send_GetTime(int fd)
{
  return write(fd, Get_Time, strlen(Get_Time));
}

// *****************
int Receive_Time(int fd, char *buf)
{
  // To Do.
  return 1;
}

// *****************
int Receive_Current(int fd, char *buf)
{
  char *ptr;

  if ( buf )
  {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    PWM_ptr->current = atof(ptr);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
int Receive_Voltage(int fd, char *buf)
{
  char *ptr;

  if ( buf )
  {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    PWM_ptr->voltage = atof(ptr);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
int Receive_Temp(int fd, char *buf)
{
  char *ptr;

  if ( buf )
  {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    PWM_ptr->temperature = atof(ptr);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
int Receive_Firmware(int fd, char *buf)
{
  char *ptr;

  if ( buf )
  {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    strncpy(PWM_ptr->firmware, ptr, PWM_FIRMWARE_SIZE);
    printf("Firmware %s\n", PWM_ptr->firmware );
    syslog(LOG_NOTICE, "Firmware detected: %s", PWM_ptr->firmware);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
const struct CmdFunc Cmd_Table[] =
{
  { "time",				&Receive_Time },
  { "volt",				&Receive_Voltage },
  { "current",		&Receive_Current },
  { "temp",				&Receive_Temp },
  { "firmware",		&Receive_Firmware },
  { NULL,					NULL }
};

// *****************
void Build_CmdList(CmdFuncList_t list)
{
  /*
  	printf("CL_Time\n");

  	CmdParse_AddCallback( list, "time", &Receive_Time);
  	printf("CL_Current\n");

  	CmdParse_AddCallback( list, "current", &Receive_Current);
  	printf("CL_Temp\n");

  	CmdParse_AddCallback( list, "temp", &Receive_Temp);

  	printf("CL_Firmware\n");
  	CmdParse_AddCallback( list, "firmware", &Receive_Firmware);
  	*/
}

// *****************
// *****************

