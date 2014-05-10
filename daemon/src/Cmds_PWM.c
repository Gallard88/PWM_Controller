
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
#include "Cmds_PWM.h"

// *****************
static struct PWM_Shared_Mem *PWM_ptr;

// *****************
void PWM_CreateSharedMemory( void )
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
  if (key  == -1) {
    syslog(LOG_EMERG, "ftok() failed");
    perror("ftok() failed");
    exit(1);
  }

  // connect to (and possibly create) the segment:
  if ((shmid = shmget(key, PWM_CON_SHM_SIZE, 0777 | IPC_CREAT)) == -1) {
    syslog(LOG_EMERG, "shmget()");
    perror("shmget() - failed");
    exit(1);
  }

  // attach to the segment to get a pointer to it:
  PWM_ptr = shmat(shmid, (void *)0, 0);
  if ((char *)PWM_ptr == (char *)(-1)) {
    syslog(LOG_EMERG, "shmat()");
    perror("shmat() - failed");
    exit(1);
  }

  memset(PWM_ptr, 0, sizeof(struct PWM_Shared_Mem));
  pthread_mutex_init(&PWM_ptr->access, NULL);
  PWM_ptr->pid = getpid();
  PWM_ptr->ver_major = VER_MAJOR;
  PWM_ptr->ver_minor = VER_MINOR;
}

// *****************
void PWM_ClearSharedMemory(void)
{
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
void PWM_SetConnected(void)
{
  PWM_ptr->port_connected = 1;
}

// *****************
void PWM_SetDisconnected(void)
{
  PWM_ptr->port_connected = 0;
}

// *****************
static const char Get_Time[] = "time\r\n";

// *****************
int PWM_SetStartup(int fd, int update)
{
  char cmd[100];

  sprintf(cmd, "update: temp %d\r\nupdate: volt %d\r\nupdate: current %d\r\n", update, update, update);
  return write (fd, cmd, strlen(cmd));
}

// *****************
static const char Restart_Cmd[] = "restart\r\n";
// *****************
int PWM_Send_Restart(int fd)
{
  return write(fd, Restart_Cmd, strlen(Restart_Cmd));
}

// *****************
static int Check_PWM(int duty)
{
  if ( duty > 100 )
    return 100;
  if ( duty < 0 )
    return 0;
  return duty;
}

// *****************
int PWM_Send_ChanelData(int fd)
{
  int i;
  int newDuty;
  char cmd[32], msg[32*PWM_NUM_CHANELS];
  time_t diff, current = time(NULL);

  msg[0] = 0;
  pthread_mutex_lock( &PWM_ptr->access );
  for ( i = 0; i < PWM_NUM_CHANELS; i ++ ) {

    diff = current - PWM_ptr->ch[i].update;
    
    if ( diff < 5 ) {
      newDuty = Check_PWM(PWM_ptr->ch[i].duty);
    } else {
      newDuty = 0;
    }
    sprintf(cmd, "pwm: %2d %3d\r\n", i, newDuty );
    strcat(msg, cmd);
  }
  pthread_mutex_unlock( &PWM_ptr->access );
  return write(fd, msg, strlen(msg) );
}

// *****************
static int Receive_Current(int fd, char *buf)
{
  char *ptr;

  if ( buf ) {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    PWM_ptr->current = atof(ptr);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
static int Receive_Voltage(int fd, char *buf)
{
  char *ptr;

  if ( buf ) {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    PWM_ptr->voltage = atof(ptr);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
static int Receive_Temp(int fd, char *buf)
{
  char *ptr;

  if ( buf ) {
    ptr = CmdParse_SkipSpace(buf);
    pthread_mutex_lock( &PWM_ptr->access );

    PWM_ptr->temperature = atof(ptr);

    pthread_mutex_unlock( &PWM_ptr->access );
  }
  return 1;
}

// *****************
static int Receive_Firmware(int fd, char *buf)
{
  char *ptr;

  if ( buf ) {
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
const struct CmdFunc Cmd_Table[] = {
  { "volt",	&Receive_Voltage },
  { "current",	&Receive_Current },
  { "temp",	&Receive_Temp },
  { "firmware",	&Receive_Firmware },
  { NULL,	NULL }
};

// *****************
// *****************

