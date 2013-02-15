
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

#include "PWM_Controller.h"

// *****************
Pwm_Con_Mem *PWM_ptr;

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
  if ((shmid = shmget(key, PWM_CON_SHM_SIZE, O_RDWR)) == -1)
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
int main( int argc, char *argv[] )
{
	int i;

	Create_Shared_Memory();


  pthread_mutex_lock( &PWM_ptr->access );
	printf("Current %2.2f\n", PWM_ptr->current);
	printf("Voltage %2.2f\n", PWM_ptr->voltage);
	printf("Temp:   %2.2f\n", PWM_ptr->temperature);

	for ( i = 0; i < PWM_NUM_CHANELS; i ++)
	{
		printf("Ch %d: %f\n", i, PWM_ptr->ch[i]);
	}
  pthread_mutex_unlock( &PWM_ptr->access );

	return 0;
}

// *****************
// *****************
