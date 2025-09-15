#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H

typedef void(*TimerCallback)();
void timer_start(int frequency);
void timer_set_callback(TimerCallback handler);
void timer_stop();
unsigned long timer_get_elapsed();

#endif // DRIVER_TIMER_H
