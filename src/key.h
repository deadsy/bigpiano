//-----------------------------------------------------------------------------
/*

Key Scanning

*/
//-----------------------------------------------------------------------------

#ifndef KEY_H
#define KEY_H

//-----------------------------------------------------------------------------
// 4 octaves x 7 whites keys per octave = 28 keys

#define KEY_ROWS 7
#define KEY_COLS 4
#define NUM_KEYS (KEY_ROWS * KEY_COLS)

//-----------------------------------------------------------------------------

typedef struct key_control {

    uint8_t row;
    uint8_t state[NUM_KEYS];
    void (*key_down)(uint8_t key);
    void (*key_up)(uint8_t key);

} KEY_CTRL;

extern KEY_CTRL keys;

//-----------------------------------------------------------------------------
// API functions

void key_scan(void);
int key_init(void);

//-----------------------------------------------------------------------------

#endif // KEY_H

//-----------------------------------------------------------------------------