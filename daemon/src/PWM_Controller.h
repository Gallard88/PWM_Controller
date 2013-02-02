/*


*/
// *****************
#ifndef __PWM_CONTROLLER_H__
#define __PWM_CONTROLLER_H__
// *****************
/*
#ifdef __cplusplus
extern "C" {
#endif
*/
// *****************
#include <pthread.h>

#define PWM_SHARDED_MEM_NAME "/tmp/PWM_Controller"
#define PWM_SHARDED_MEM_KEY 'P'
#define PWM_NUM_CHANELS	16
#define PWM_CON_SHM_SIZE	1024

typedef struct
{
	pmutex_t access;
	int data_ready;
	float ch[PWM_NUM_CHANELS];
	float current;
	float temperature;
	char firmware[128]
} Pwm_Con_Mem;

// *****************
/*
#ifdef __cplusplus
}
#endif
*/
#endif

