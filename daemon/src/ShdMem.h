/*


*/
// *****************
#ifndef __SHARD_MEMORY_H__
#define __SHARD_MEMORY_H__
// *****************
#ifdef __cplusplus
extern "C" {
#endif
// *****************
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define VER_MAJOR	1
#define VER_MINOR	0

#define PWM_NUM_CHANELS			16
#define PWM_FIRMWARE_SIZE		128
  typedef struct {
    int duty;
    time_t update;
  } PWM_Ch;

  struct PWM_Shared_Mem {
    pthread_mutex_t access;
    pid_t pid;
    int ver_major;
    int ver_minor;
    unsigned data_ready:1;
    unsigned port_connected:1;
    int updated;
    float voltage;
    float current;
    float temperature;
    char firmware[PWM_FIRMWARE_SIZE];
    PWM_Ch ch[PWM_NUM_CHANELS];
  };

// *****************
#define PWM_CON_SHM_SIZE		sizeof(struct PWM_Shared_Mem)
#define PWM_KEY_FILE	"/tmp/PWM_Controller.shmem"
#define PWM_MEM_KEY 'R'

// *****************
#ifdef __cplusplus
}
#endif
#endif

