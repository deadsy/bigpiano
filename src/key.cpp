//-----------------------------------------------------------------------------
/*

Key Scanning

Driver for a key matrix.

7 rows - rows 0..6 = notes C,D,E,F,G,A,B
4 cols - cols 0..3 = octaves 0..3

Note rows are selected 1 at a time.
Octave columns are read to determine which switch is closed.

Note:
The 3 row select lines drive the rows via a 3 to 8 decoder (74ls138).
The decoder has active low outputs. ie- A closed switch will show as
a low on the input when it is scanned.

*/
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#include "key.h"

//-----------------------------------------------------------------------------

KEY_CTRL keys;

//-----------------------------------------------------------------------------
// low-level keyboard matrix rd/wr

// read all column lines
static uint8_t key_rd(void) {
    return ~PINC & 15;
}

// select a row line
static void key_wr(uint8_t row) {
    PORTB = (PORTB & ~7) | row;
}

static void key_io_init(void) {
    // rows: portb 0..2 set as outputs
    DDRB |= 7;
    // columns: portc 0..3 set as inputs with pullup resistors
    DDRC &= ~15;
    PORTC |= 15;
}

//-----------------------------------------------------------------------------
// Scan the keys. Call provided key up/down functions.

// key state - 8 bits
// the top 3 bits indicate the key state
// the bottom 5 bits are a debounce counter

#define KEY_STATE_UNKNOWN       (0 << 5)
#define KEY_STATE_WAIT4_DOWN    (1 << 5)
#define KEY_STATE_DOWN          (2 << 5)
#define KEY_STATE_WAIT4_UP      (3 << 5)
#define KEY_STATE_UP            (4 << 5)

#define KEY_COUNT(x)            (keys.state[x] & (31 << 0))
#define KEY_STATE(x)            (keys.state[x] & (7 << 5))

#define DEBOUNCE_COUNT_DOWN     2
#define DEBOUNCE_COUNT_UP       4

void key_scan(void) {

    // read the column lines
    int col = key_rd();
    int key = keys.row;

    for (int i = 0; i < KEY_COLS; i ++) {
        int down = col & 1;

        switch (KEY_STATE(key)) {
            case KEY_STATE_WAIT4_DOWN: {
                // wait for n successive key down conditions
                if (!down) {
                    keys.state[key] = KEY_STATE_UP;
                } else {
                    int n = KEY_COUNT(key);
                    if (n >= DEBOUNCE_COUNT_DOWN) {
                        keys.state[key] = KEY_STATE_DOWN;
                        if (keys.key_down) {
                            keys.key_down(key);
                        }
                    } else {
                        keys.state[key] = KEY_STATE_WAIT4_DOWN | (n + 1);
                    }
                }
                break;
            }
            case KEY_STATE_DOWN: {
                // the key is down
                if (!down) {
                    keys.state[key] = KEY_STATE_WAIT4_UP;
                }
                break;
            }
            case KEY_STATE_WAIT4_UP: {
                // wait for n successive key up conditions
                if (down) {
                    keys.state[key] = KEY_STATE_DOWN;
                } else {
                    int n = KEY_COUNT(key);
                    if (n >= DEBOUNCE_COUNT_UP) {
                        keys.state[key] = KEY_STATE_UP;
                        if (keys.key_up) {
                            keys.key_up(key);
                        }
                    } else {
                        keys.state[key] = KEY_STATE_WAIT4_UP | (n + 1);
                    }
                }
                break;
            }
            case KEY_STATE_UP: {
                // the key is up
                if (down) {
                    keys.state[key] = KEY_STATE_WAIT4_DOWN;
                }
                break;
            }
            default: {
                keys.state[key] = down ? KEY_STATE_DOWN : KEY_STATE_UP;
                break;
            }
        }
        col >>= 1;
        key += KEY_ROWS; 
    }

    // Set the next row line
    keys.row ++;
    if (keys.row == KEY_ROWS) {
        keys.row = 0;
    }
    key_wr(keys.row);
}

//-----------------------------------------------------------------------------

int key_init(void) {
    key_io_init();
    memset(&keys, 0, sizeof(keys));
    key_wr(keys.row);
    return 0;
}

//-----------------------------------------------------------------------------
