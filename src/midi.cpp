//-----------------------------------------------------------------------------
/*

Read MIDI stream and make note on/off calls.

*/
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"
#include "midi.h"

//-----------------------------------------------------------------------------

#define MIDDLE_C 60

//-----------------------------------------------------------------------------

MIDI_CTRL midi;

//-----------------------------------------------------------------------------
// note to name conversion
// TODO - put strings into program space

static const char *sharps[NOTES_IN_OCTAVE] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static const char *flats[NOTES_IN_OCTAVE] = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};

// convert a midi note to a name
const char *midi_note_name(uint8_t note, char mode) {
    note %= NOTES_IN_OCTAVE;
    return (mode == '#') ? sharps[note] : flats[note];
}

// return a note name with sharp and flat forms
char *midi_full_note_name(char *str, uint8_t note) {
    const char *s_name = midi_note_name(note, '#');
    const char *f_name = midi_note_name(note, 'b');
    strcpy(str, s_name);
    if (strcmp(s_name, f_name)) {
        strcat(str, "/");
        strcat(str, f_name);
    }
    return str;
}

//-----------------------------------------------------------------------------
// note to ordinals

// return an octave number
int midi_to_octave(uint8_t note) {
    return (note / NOTES_IN_OCTAVE);
}

// return 0 - 6 for the white key notes, -1 for a black key
int midi_to_white(uint8_t note) {
    static const int8_t convert[NOTES_IN_OCTAVE] = {0,-1,1,-1,2,3,-1,4,-1,5,-1,6};
    note %= NOTES_IN_OCTAVE;
    return convert[note];
}

// convert 0-6 white key notes into midi notes
int white_to_midi(uint8_t white) {
    static const int8_t convert[WHITE_KEYS_IN_OCTAVE] = {0,2,4,5,7,9,11};
    white %= WHITE_KEYS_IN_OCTAVE;
    return convert[white];
}

//-----------------------------------------------------------------------------
// Transmit midi note commands

void midi_tx(uint8_t cmd, uint8_t note, uint8_t velocity) {
    uart_tx(cmd);
    uart_tx(note & 0x7f);
    uart_tx(velocity & 0x7f);
}

//-----------------------------------------------------------------------------
// Receive midi note commands. Call provided note on/off functions.

enum {
    MIDI_STATE_COMMAND,
    MIDI_STATE_NOTE,
    MIDI_STATE_VELOCITY,
};

void midi_rx(void) {

    if (uart_test_rx() == 0) {
        return;
    }

    uint8_t rx = uart_rx();

    switch (midi.state) {
        case MIDI_STATE_COMMAND: {
            uint8_t cmd = rx & 0xf0;
            if ((cmd == NOTE_ON) || (cmd == NOTE_OFF)) {
                midi.command = cmd;
                midi.state = MIDI_STATE_NOTE;
            }
            break;
        }
        case MIDI_STATE_NOTE: {
            if ((rx & 0x80) == 0) {
                midi.note = rx;
                midi.state = MIDI_STATE_VELOCITY;
            } else {
                // not a valid note
                midi.state = MIDI_STATE_COMMAND;
            }
            break;
        }
        case MIDI_STATE_VELOCITY: {
            if ((rx & 0x80) == 0) {
                if ((midi.command == NOTE_ON) && midi.note_on) {
                    midi.note_on(midi.note, rx);
                }
                if ((midi.command == NOTE_OFF) && midi.note_off) {
                    midi.note_off(midi.note, rx);
                }
            }
            midi.state = MIDI_STATE_COMMAND;
            break;
        }
        default: {
            // ?
            midi.state = MIDI_STATE_COMMAND;
            break;
        }
    }
}

//-----------------------------------------------------------------------------

int midi_init(void) {
    memset(&midi, 0, sizeof(midi));
    midi.state = MIDI_STATE_COMMAND;
    return 0;
}

//-----------------------------------------------------------------------------
