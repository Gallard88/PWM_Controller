/*


*/
// *****************
#ifndef __PWM_CONTROLLER_H__
#define __PWM_CONTROLLER_H__
// *****************
#ifdef __cplusplus
extern "C" {
#endif
// *****************
#include <pthread.h>

#define PWM_KEY_FILE	"/tmp/PWM_Controller.shmem"
#define PWM_MEM_KEY 'R'

#define PWM_NUM_CHANELS	16
#define PWM_CON_SHM_SIZE	1024

typedef struct
{
	pthread_mutex_t access;
	unsigned data_ready:1;
	unsigned port_connected:1;
	float ch[PWM_NUM_CHANELS];
	float current;
	float temperature;
	char firmware[128];
} Pwm_Con_Mem;

// *****************
#ifdef __cplusplus
}
#endif
#endif

