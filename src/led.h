//-----------------------------------------------------------------------------
/*

LED Driver

Control the SPI based RGB LED Modules

*/
//-----------------------------------------------------------------------------

#ifndef LED_H
#define LED_H

//-----------------------------------------------------------------------------

// number of leds in the chain
// 4 octaves, 7 whites notes per octave, 2 leds per white note = 56 leds
#define NUM_LEDS 56

//-----------------------------------------------------------------------------
// API functions

int led_init(void);
void led_isr(void);
void led_set(int idx, const RGB *rgb);
RGB *led_get(int idx);
void led_all_off(void);

//-----------------------------------------------------------------------------

#endif // LED_H

//-----------------------------------------------------------------------------