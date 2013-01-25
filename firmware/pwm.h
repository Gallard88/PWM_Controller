
/******************************************/
#ifndef __PWM_H__
#define __PWM_H__

/******************************************/
#define PWM_NUM_CHANELS	8
#define PWM_TIMER_RES	0xFF

/******************************************/
void PWM_Initialise(void);

void PWM_SetDutyCycle(int ch, int duty);

int PWM_isAlarm(void);
void PWM_SetAlarm(int alm);
void PWM_ClearAlarm(int alm);

/******************************************/
/******************************************/
#endif

