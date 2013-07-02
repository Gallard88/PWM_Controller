// *****************
#ifndef __PWM_CONTROLLER__
#define __PWM_CONTROLLER__
// *****************
#ifdef __cplusplus
extern "C" {
#endif

// *****************
#include <unistd.h>
#include <sys/types.h>

// *****************
    int PWM_Connect(void);

    extern const int PWM_Num_Chanels;
// *****************
    int PWM_isConnected(void);

// *****************
    float PWM_GetTemp(void);

// *****************
    float PWM_GetCurrent(void);

// *****************
    float PWM_GetVoltage(void);

// *****************
    void PWM_SetPWM(int ch, float duty);

// *****************
    float PWM_GetPWM(int ch);

    // *****************
    pid_t PWM_GetPid(void);

    // *****************
    // *****************
#ifdef __cplusplus
}
#endif
#endif

