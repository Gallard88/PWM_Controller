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

#define PWM_NUM_CHANELS			16
#define PWM_FIRMWARE_SIZE		128
    typedef struct
    {
        float duty;
        time_t update;
    } PWM_Ch;

    typedef struct
    {
        pthread_mutex_t access;
        pid_t pid;
        int ver_major;
        int ver_mainor;
        unsigned data_ready:1;
        unsigned port_connected:1;
        int updated;
        float voltage;
        float current;
        float temperature;
        char firmware[PWM_FIRMWARE_SIZE];
        PWM_Ch ch[PWM_NUM_CHANELS];
    } Pwm_Shd_Mem;

// *****************
#define PWM_CON_SHM_SIZE		sizeof(Pwm_Shd_Mem)
#define PWM_KEY_FILE	"/tmp/PWM_Controller.shmem"
#define PWM_MEM_KEY 'R'

// *****************
#ifdef __cplusplus
}
#endif
#endif

