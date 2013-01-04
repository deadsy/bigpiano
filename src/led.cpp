//-----------------------------------------------------------------------------
/*

LED Driver

Control the SPI based RGB LED Modules

Note:

A string of 20 leds set to white (255,255,255) consumes 2.29A @ 12V.
Or about 1.38 W/module maximum.

*/
//-----------------------------------------------------------------------------

#include <string.h>
#include <stdint.h>
#include <util/atomic.h>

#include "color.h"
#include "led.h"
#include "timer.h"

//-----------------------------------------------------------------------------
// SPI bit assignments on port B

#define SCK  5  // spi clock
#define MISO 4  // master in, slave out
#define MOSI 3  // master out, slave in
#define SS   2  // slave select

//-----------------------------------------------------------------------------

#define SPI_TX(byte) (SPDR = byte)
#define SPI_WAIT() while((SPSR & (1 << SPIF)) == 0)

//-----------------------------------------------------------------------------
// LED Control

static RGB leds[NUM_LEDS];
static int led_dirty;

//-----------------------------------------------------------------------------
// update the led chain

void led_isr(void) {
    if (led_dirty < 0) {
        // no changes since last isr
        return;
    }
    for (int i = 0; i <= led_dirty; i ++) {
        RGB *led = &leds[i];
        SPI_TX(led->b); SPI_WAIT();
        SPI_TX(led->r); SPI_WAIT();
        SPI_TX(led->g); SPI_WAIT();
    }
    led_dirty = -1;
}

//-----------------------------------------------------------------------------
// control an individual led

void led_set(int idx, const RGB *rgb) {
    if ((idx < 0) || (idx >= NUM_LEDS)) {
        return;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (idx > led_dirty) {
            led_dirty = idx;
        }
        leds[idx] = *rgb;
    }
}

RGB *led_get(int idx) {
    return &leds[idx];
}

//-----------------------------------------------------------------------------
// all leds off

void led_all_off(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        memset(leds, 0, sizeof(leds));
        led_dirty = NUM_LEDS - 1;
    }
}

//-----------------------------------------------------------------------------

int led_init(void) {
    // set all leds to off on the first update
    led_all_off();

    // SPI bus setup
    // ss, mosi, sck are outputs, ss is high
    DDRB |= (1 << SCK) | (1 << MOSI) | (1 << SS);
    PORTB |= (1 << SS);
    // enable the spi in master mode - sets MISO to input
    // set sck = fosc/4 (4MHz)
    SPSR = 0;
    SPCR = (1 << SPE) | (1 << MSTR);

    // use the 8 bit timer 0 to drive the led update isr
    // clock the counter at F_CPU / 1024 = 15625 Hz
    // interrupt at the overflow rate 2^8 / 15625 = 16.4 ms
    // gives led update rate around 60Hz
    TCCR0A = 0;
    TCCR0B = DIVIDE_BY_1024;
    OCR0A = 0;
    OCR0B = 0;
    TIMSK0 = (1 << TOIE0);
    TIFR0 = (1 << TOV0);
    return 0;
}

//-----------------------------------------------------------------------------