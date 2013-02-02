// *****************
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "PWM_Controller.h"

// *****************
Pwm_Con_Mem *PWM_ptr;

// *****************
int Create_Shared_Memory( void )
{
  key_t key;
  int shmid;

  // make the key:
  if ((key = ftok(PWM_SHARDED_MEM_NAME, PWM_SHARDED_MEM_KEY)) == -1)
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
  if (PWM_ptr == (char *)(-1))
  {
    perror("shmat");
    exit(1);
  }
  return 0;
}

// *****************
void shutdown(int rv)
{
  // detach from the segment:
  if (shmdt(data) == -1)
  {
    perror("shmdt");
    exit(1);
  }
  closelog();
  exit(rv);
}

// *****************

// *****************
int main(int argc, char *argv[])
{
  int loop = 1;
  openlog("PWM_Controller", LOG_PID | LOG_NDELY , LOG_USER );
  syslog(LOG_NOTICE, "PWM_Controller Startup");

//  printf("become a daemon\n");
//  daemon( 0, 0 );

  printf("create shared memory\n");
  if ( Create_Shared_Memory() < 0 )
  {
    printf("Shared Memory failed\n");
    shutdown(-1);
  }

  printf("Read Settings\n");
  printf("Open Serial Port\n");
  printf("Send Startup commands\n");
  sleep(5);
  printf("\n");


  // main loop
  while ( loop > 0 )
  {
  }
  shutdown(0);
  return 0;
}


// *****************
// *****************

