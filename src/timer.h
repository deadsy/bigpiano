//-----------------------------------------------------------------------------
/*

Timer Functions

*/
//-----------------------------------------------------------------------------

#ifndef TIMER_H
#define TIMER_H

//-----------------------------------------------------------------------------
// timer prescaler control

#define DIVIDE_BY_1     (1 << 0)
#define DIVIDE_BY_8     (2 << 0)
#define DIVIDE_BY_64    (3 << 0)
#define DIVIDE_BY_256   (4 << 0)
#define DIVIDE_BY_1024  (5 << 0)

//-----------------------------------------------------------------------------
// API functions

void timer_ovf_isr(void);
int timer_init(void);
void timer_delay_msec(int n);
void timer_delay_msec_poll(int n, void (*poll)(void));
void timer_delay_until(uint32_t time);
uint32_t timer_get_msec(void);

//-----------------------------------------------------------------------------

#endif // TIMER_H

//-----------------------------------------------------------------------------