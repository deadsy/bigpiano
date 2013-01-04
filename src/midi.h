//-----------------------------------------------------------------------------
/*

MIDI Processing

*/
//-----------------------------------------------------------------------------

#ifndef MIDI_H
#define MIDI_H

//-----------------------------------------------------------------------------

#define NOTES_IN_OCTAVE 12
#define WHITE_KEYS_IN_OCTAVE 7
#define BLACK_KEYS_IN_OCTAVE 5

//-----------------------------------------------------------------------------
// midi commands

#define NOTE_OFF 0x80
#define NOTE_ON  0x90

//-----------------------------------------------------------------------------

typedef struct midi_control {

    uint8_t state;
    uint8_t command;
    uint8_t note;
    void (*note_on)(uint8_t note, uint8_t velocity);
    void (*note_off)(uint8_t note, uint8_t velocity);

} MIDI_CTRL;

extern MIDI_CTRL midi;

//-----------------------------------------------------------------------------
// API functions

const char *midi_note_name(uint8_t note, char mode);
char *midi_full_note_name(char *str, uint8_t note);

int midi_to_octave(uint8_t note);
int midi_to_white(uint8_t note);
int white_to_midi(uint8_t white);

int midi_init(void);
void midi_rx(void);
void midi_tx(uint8_t cmd, uint8_t note, uint8_t velocity);

//-----------------------------------------------------------------------------

#endif // MIDI_H

//-----------------------------------------------------------------------------