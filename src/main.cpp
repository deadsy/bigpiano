//-----------------------------------------------------------------------------
/*

MIDI Lights

Use MIDI notes to control a string of RGB modules.

*/
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "common.h"
#include "uart.h"
#include "timer.h"
#include "color.h"
#include "led.h"
#include "midi.h"
#include "lcd.h"
#include "key.h"

//-----------------------------------------------------------------------------
// keyboard defines

// 4 octaves 36, 48, 60 (middle c), 72
#define BASE_NOTE 36
#define BASE_OCTAVE 3
#define NOTE_VELOCITY 100 // 0..127

//-----------------------------------------------------------------------------

extern "C" void __cxa_pure_virtual(void);
void __cxa_pure_virtual(void) {}

//-----------------------------------------------------------------------------

static const RGB note2color[WHITE_KEYS_IN_OCTAVE] = {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_AQUA,
    COLOR_FUCHSIA,
    COLOR_WHITE
};

static void led_ctrl(uint8_t note, int on_flag) {
    int white_note = midi_to_white(note);
    int octave = midi_to_octave(note) - BASE_OCTAVE;

    if ((white_note >= 0) && (octave >= 0)) {
        int led_num = ((octave * WHITE_KEYS_IN_OCTAVE) + white_note) * 2;
        if (on_flag) {
            // turn on lights
            const RGB *color = &note2color[white_note];
            led_set(led_num, color);
            led_set(led_num + 1, color);
        } else {
            // turn off lights
            RGB black = COLOR_BLACK;
            led_set(led_num, &black);
            led_set(led_num + 1, &black);
        }
    }
}

static void midi_on(uint8_t note, uint8_t velocity) {
    char tmp[8];
    printf_P(PSTR("\nrx %s %d %d"), midi_full_note_name(tmp, note), note, velocity);
    led_ctrl(note, 1);
}

static void midi_off(uint8_t note, uint8_t velocity) {
    led_ctrl(note, 0);
}

static int downs;

static int key_to_midi(uint8_t key) {
    return white_to_midi(key) + ((key / WHITE_KEYS_IN_OCTAVE) * NOTES_IN_OCTAVE) + BASE_NOTE;
}

static void key_down(uint8_t key) {
    char tmp[8];
    downs += 1;
    uint8_t note = key_to_midi(key);
    printf_P(PSTR("\ndn %d %s %d"), note, midi_full_note_name(tmp, note), downs);
    led_ctrl(note, 1);
    midi_tx(NOTE_ON, note, NOTE_VELOCITY);
}

static void key_up(uint8_t key) {
    char tmp[8];
    uint8_t note = key_to_midi(key);
    printf_P(PSTR("\nup %d %s"), note, midi_full_note_name(tmp, note));
    led_ctrl(note, 0);
    midi_tx(NOTE_OFF, note, NOTE_VELOCITY);
}

static void big_piano(void) {

    printf_P(PSTR("\nThe BFP"));
    printf_P(PSTR("\nVersion 1.0"));

    downs = 0;

    // actions on key presses
    keys.key_down = key_down;
    keys.key_up = key_up;

    // actions on midi rx
    midi.note_on = midi_on;
    midi.note_off = midi_off;

    while(1) {
        key_scan();
        timer_delay_msec_poll(1, midi_rx);
    }
}

//-----------------------------------------------------------------------------
// Use UART for stdio

#if 0

static FILE uart_stream;

static void uart_stdio(void) {
    UART_Init();
    uart_stream.put= UART_putc;
    uart_stream.get = UART_getc;
    uart_stream.flags = _FDEV_SETUP_RW;
    uart_stream.udata = 0;
    stdout = stdin = stderr = &uart_stream;
}

#endif

//-----------------------------------------------------------------------------
// Use LCD for stdio

static FILE lcd_stream;

static void lcd_stdio(void) {
    lcd_init();
    lcd_stream.put= lcd_putc;
    lcd_stream.get = 0;
    lcd_stream.flags = _FDEV_SETUP_RW;
    lcd_stream.udata = 0;
    stdout = stdin = stderr = &lcd_stream;
}

//-----------------------------------------------------------------------------

int main(void)
{
    //uart_stdio();
    lcd_stdio();
    sei();
    putc('\n', stdout);

    // initialisation
    int init_fails = 0;
    INIT(uart_init);
    INIT(timer_init);
    INIT(led_init);
    INIT(midi_init);
    INIT(key_init);
    if (init_fails != 0) {
        // loop forever...
        while(1);
    }

    big_piano();

    // loop forever...
    while(1);
    return 0;
}

//-----------------------------------------------------------------------------
