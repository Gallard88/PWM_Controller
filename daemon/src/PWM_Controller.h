// *****************
#ifndef __PWM_CONTROLLER__
#define __PWM_CONTROLLER__
// *****************
#ifdef __cplusplus
extern "C" {
#endif

// *****************
  int PWM_Connect(void);

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
// *****************
#ifdef __cplusplus
}
#endif
#endif

