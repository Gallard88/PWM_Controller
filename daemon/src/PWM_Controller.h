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

  typedef struct PWM_Shared_Mem *PWM_Con_t;
// *****************
  PWM_Con_t PWM_Connect(void);

  extern const int PWM_Num_Chanels;
  // *****************
  int PWM_isConnected(PWM_Con_t ptr);

  // *****************
  float PWM_GetTemp(PWM_Con_t ptr);

  // *****************
  float PWM_GetCurrent(PWM_Con_t ptr);

  // *****************
  float PWM_GetVoltage(PWM_Con_t ptr);

  // *****************
  float PWM_GetPWM(PWM_Con_t ptr, int ch);

  // *****************
  int PWM_GetMeasurements(PWM_Con_t ptr, float *volt, float *temp, float *current);
  // *****************
  struct PWM_Update {
    int ch;
    float duty;
  };

  void PWM_SetPWM(PWM_Con_t ptr, int ch, float duty);
  // *****************
  void PWM_SetMultiplePWM(PWM_Con_t ptr, const struct PWM_Update *update, int num_chanels);

  // *****************
  pid_t PWM_GetPid(PWM_Con_t ptr);

  // *****************
  // *****************
#ifdef __cplusplus
}
#endif
#endif

