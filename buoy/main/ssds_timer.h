
#ifndef __SSDS_TIMER
#define __SSDS_TIMER


#define TIMER_NUM       0
#define TIMER_INTERVAL  5 // Pause in seconds
#define TIMER_DIVIDER   16
#define TIMER_SCALE     (TIMER_BASE_CLK / TIMER_DIVIDER)

#define TIMER "TIMER"

/* Defines January 1st, 2015. If the RTC is below this, we can
 * be confident that our timer hasn't been intialized
 */
#define RTC_THRESHOLD 1420070400

void init_thread_timer();
void check_if_day();
void pause_thread_timer();
void resume_thread_timer();


#endif