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

// *****************
#include "ShdMem.h"
#include "PWM_Controller.h"

// *****************
const int PWM_Num_Chanels = PWM_NUM_CHANELS;

// *****************
PWM_Con_t PWM_Connect(void)
{
  key_t key;
  int shmid;
  int fd;
  PWM_Con_t ptr;

  // Make sure the file exists.
  fd = open(PWM_KEY_FILE, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
  /* Only wanted to make sure that the file exists. */
  close(fd);

  // Generate memory key. */
  key = ftok(PWM_KEY_FILE, PWM_MEM_KEY);
  if (key  == -1) {
    perror("ftok");
    return NULL;
  }

  // connect to (and possibly create) the segment:
  if ((shmid = shmget(key, PWM_CON_SHM_SIZE, O_RDWR)) == -1) {
    perror("shmget");
    return NULL;
  }

  // attach to the segment to get a pointer to it:
  ptr = shmat(shmid, (void *)0, 0);
  if ((char *)ptr == (char *)(-1)) {
    perror("shmat");
    return NULL;
  }
  if (( ptr->ver_major != VER_MAJOR ) ||
      ( ptr->ver_minor != VER_MINOR )) {
    printf("Library is not compatible with this version\n");
    printf("Library Ver %d.%d, Prog Ver %d.%d\n", VER_MAJOR,VER_MINOR, ptr->ver_major, ptr->ver_minor);
    return NULL;
  }
  return ptr;
}

// *****************
int PWM_isConnected(PWM_Con_t ptr)
{
  int con;
  if ( ptr == NULL )
    return 0;
  pthread_mutex_lock( &ptr->access );
  con = ptr->port_connected;
  pthread_mutex_unlock( &ptr->access );

  return con;
}

// *****************
float PWM_GetTemp(PWM_Con_t ptr)
{
  float value;

  if ( ptr == NULL )
    return 0.0;
  pthread_mutex_lock( &ptr->access );
  value = ptr->temperature;
  pthread_mutex_unlock( &ptr->access );

  return value;
}

// *****************
float PWM_GetCurrent(PWM_Con_t ptr)
{
  float value;

  if ( ptr == NULL )
    return 0.0;
  pthread_mutex_lock( &ptr->access );
  value = ptr->current;
  pthread_mutex_unlock( &ptr->access );

  return value;
}

// *****************
float PWM_GetVoltage(PWM_Con_t ptr)
{
  float value;

  if ( ptr == NULL )
    return 0.0;
  pthread_mutex_lock( &ptr->access );
  value = ptr->voltage;
  pthread_mutex_unlock( &ptr->access );

  return value;
}

// *****************
float PWM_GetPWM(PWM_Con_t ptr, int ch)
{
  float value;

  if ( ptr == NULL )
    return 0.0;
  if ( ch >= PWM_NUM_CHANELS )
    return 0;

  pthread_mutex_lock( &ptr->access );
  value = (float)ptr->ch[ch].duty / 100.0;
  pthread_mutex_unlock( &ptr->access );

  return value;
}

// *****************
static int Verify_PWM(float value)
{
  if ( value > 1.0 )
    return 100;
  if ( value < 0.0 )
    return 0;
  return value * 100;
}

// *****************
void PWM_SetPWM(PWM_Con_t ptr, int ch, float duty)
{
  if ( ptr == NULL )
    return;
  if ( ptr->port_connected == 0 )
    return;
  if ( ch >= PWM_NUM_CHANELS )
    return;

  pthread_mutex_lock( &ptr->access );
  ptr->ch[ch].duty = Verify_PWM(duty);
  ptr->ch[ch].update = time(NULL);
  ptr->data_ready = 1;
  ptr->updated |= (1<<ch);

  pthread_mutex_unlock( &ptr->access );
  kill( ptr->pid, SIGUSR1);
}

// *****************
void PWM_SetMultiplePWM(PWM_Con_t ptr, const struct PWM_Update *update, int num_chanels)
{
  time_t current_time= time(NULL);

  if ( ptr == NULL )
    return ;
  if ( ptr->port_connected == 0 )
    return;
  pthread_mutex_lock( &ptr->access );
  for ( int i = 0; i < num_chanels; i ++ ) {
    int ch;

    ch = update[i].ch;
    if ( ch >= PWM_NUM_CHANELS )
      continue;

    ptr->ch[ch].duty = Verify_PWM(update[i].duty);
    ptr->ch[ch].update = current_time;
    ptr->updated |= (1<<ch);
  }
  ptr->data_ready = 1;
  pthread_mutex_unlock( &ptr->access );
  kill( ptr->pid, SIGUSR1);
}

// *****************
pid_t PWM_GetPid(PWM_Con_t ptr)
{
  if ( ptr == NULL )
    return 0;
  return ptr->pid;
}

// *****************
// *****************
