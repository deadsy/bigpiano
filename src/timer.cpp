//-----------------------------------------------------------------------------
/*

Timer Functions

Provide a simple timer using the 16-bit timer 1 of the ATmega328P.

*/
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "timer.h"

//-----------------------------------------------------------------------------

static uint32_t timer_ovf_count;

void timer_ovf_isr(void)
{
    timer_ovf_count ++;
}

//-----------------------------------------------------------------------------
// return the time since boot in milliseconds

uint32_t timer_get_msec(void)
{
    // ticks_per_millisecond = F_CPU /(1000 * TIMER_DIV) = 15.625
    // Approximate this as 16 to create a millsecond count using bit shifts.

    uint32_t msec;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        msec = timer_ovf_count << 12;
        msec |= TCNT1 >> 4;
    }
    return msec;
}

//-----------------------------------------------------------------------------
// delay n milliseconds

void timer_delay_msec(int n)
{
    uint32_t timeout = timer_get_msec() + n;
    while(timer_get_msec() < timeout);
}

void timer_delay_msec_poll(int n, void (*poll)(void))
{
    uint32_t timeout = timer_get_msec() + n;
    while(timer_get_msec() < timeout) {
        if (poll) {
            poll();
        }
    }
}

//-----------------------------------------------------------------------------
// delay until a specific time

void timer_delay_until(uint32_t time)
{
    while(timer_get_msec() < time);
}

//-----------------------------------------------------------------------------

int timer_init(void)
{
    timer_ovf_count = 0;

    // using the 16 bit timer 1 for a tick counter
    // clock the counter at F_CPU / 1024 = 15625 Hz
    // increment an overflow counter every 2^16 / 15625 = 4.2 seconds

    TCCR1A = 0;
    TCCR1B = DIVIDE_BY_1024;
    TCCR1C = 0;
    OCR1A = 0;
    OCR1B = 0;
    ICR1 = 0;
    TIMSK1 = (1 << TOIE1);
    TIFR1 = (1 << TOV1);
    return 0;
}

//-----------------------------------------------------------------------------

#if defined(ARDUINO)

unsigned long millis(void)
{
    return timer_get_msec();
}

void delay(unsigned long ms)
{
    timer_delay_msec((int)ms);
}

#endif // ARDUINO

//-----------------------------------------------------------------------------
