
// *****************************************************************************
#ifndef __TIMER_H__
#define __TIMER_H__
// *****************************************************************************

/************************************
 * Timer_Init()
 *
 * initailises heart beat timer.
 */
void Timer_Init(void);

/************************************
 * Timer_Is1ms()
 *
 * returns 1 once every 1 ms
 */
int Timer_Is1ms (void);

/************************************
 * Timer_Is10ms()
 *
 * returns 1 once every 10ms
 */
int Timer_Is10ms (void);

/************************************
 * Timer_Is100ms()
 *
 * returns 1 once every 100ms
 */
int Timer_Is100ms (void);

/************************************
 * Timer_Is1s()
 *
 * returns 1 once every second
 */
int Timer_Is1s (void);

// *****************************************************************************
// *****************************************************************************
#endif

