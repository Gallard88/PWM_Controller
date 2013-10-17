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
Pwm_Shd_Mem *PWM_ptr;
const int PWM_Num_Chanels = PWM_NUM_CHANELS;

// *****************
int PWM_Connect(void)
{
    key_t key;
    int shmid;
    int fd;

    // Make sure the file exists.
    fd = open(PWM_KEY_FILE, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    /* Only wanted to make sure that the file exists. */
    close(fd);

    // Generate memory key. */
    key = ftok(PWM_KEY_FILE, PWM_MEM_KEY);
    if (key  == -1)
    {
        perror("ftok");
        return -1;
    }

    // connect to (and possibly create) the segment:
    if ((shmid = shmget(key, PWM_CON_SHM_SIZE, O_RDWR)) == -1)
    {
        perror("shmget");
        return -1;
    }

    // attach to the segment to get a pointer to it:
    PWM_ptr = shmat(shmid, (void *)0, 0);
    if ((char *)PWM_ptr == (char *)(-1))
    {
        perror("shmat");
        return -1;
    }
    if (( PWM_ptr->ver_major != VER_MAJOR ) ||
        ( PWM_ptr->ver_minor != VER_MINOR ))
    {
      printf("Library is not compatible with this version\n");
      printf("Library Ver %d.%d, Prog Ver %d.%d\n", VER_MAJOR,VER_MINOR, PWM_ptr->ver_major, PWM_ptr->ver_minor);
      return -1;
    }
    return 0;
}

// *****************
int PWM_isConnected(void)
{
    int con;
    pthread_mutex_lock( &PWM_ptr->access );
    con = PWM_ptr->port_connected;
    pthread_mutex_unlock( &PWM_ptr->access );

    return con;
}

// *****************
float PWM_GetTemp(void)
{
    float value;

    pthread_mutex_lock( &PWM_ptr->access );
    value = PWM_ptr->temperature;
    pthread_mutex_unlock( &PWM_ptr->access );

    return value;
}

// *****************
float PWM_GetCurrent(void)
{
    float value;

    pthread_mutex_lock( &PWM_ptr->access );
    value = PWM_ptr->current;
    pthread_mutex_unlock( &PWM_ptr->access );

    return value;
}

// *****************
float PWM_GetVoltage(void)
{
    float value;

    pthread_mutex_lock( &PWM_ptr->access );
    value = PWM_ptr->voltage;
    pthread_mutex_unlock( &PWM_ptr->access );

    return value;
}

// *****************
float PWM_GetPWM(int ch)
{
    float value;

    if ( ch >= PWM_NUM_CHANELS )
        return 0;

    pthread_mutex_lock( &PWM_ptr->access );
    value = PWM_ptr->ch[ch].duty;
    pthread_mutex_unlock( &PWM_ptr->access );

    return value;
}

// *****************
static float Verify_PWM(float value)
{
  if ( value > 1.0 )
    return 1.0;
  if ( value < 0.0 )
    return 0.0;
  return value;
}

// *****************
void PWM_SetPWM(int ch, float duty)
{
    if ( PWM_ptr->port_connected == 0 )
        return;
    if ( ch >= PWM_NUM_CHANELS )
        return;

    pthread_mutex_lock( &PWM_ptr->access );
    PWM_ptr->ch[ch].duty = Verify_PWM(duty);
    PWM_ptr->ch[ch].update = time(NULL);
    PWM_ptr->data_ready = 1;
    PWM_ptr->updated |= (1<<ch);

    pthread_mutex_unlock( &PWM_ptr->access );
    kill( PWM_ptr->pid, SIGUSR1);
}

// *****************
void PWM_SetMultiplePWM(const struct PWM_Update *update, int num_chanels)
{
  time_t current_time= time(NULL);

	if ( PWM_ptr->port_connected == 0 )
			return;
  pthread_mutex_lock( &PWM_ptr->access );
  for ( int i = 0; i < num_chanels; i ++ )
  {
		int ch;

		ch = update[i].ch;
    if ( ch >= PWM_NUM_CHANELS )
        continue;

    PWM_ptr->ch[ch].duty = Verify_PWM(update[i].duty);
    PWM_ptr->ch[ch].update = current_time;
    PWM_ptr->updated |= (1<<ch);
  }
  PWM_ptr->data_ready = 1;
  pthread_mutex_unlock( &PWM_ptr->access );
  kill( PWM_ptr->pid, SIGUSR1);
}

// *****************
pid_t PWM_GetPid(void)
{
  return PWM_ptr->pid;
}

// *****************
// *****************
