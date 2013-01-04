//-----------------------------------------------------------------------------
/*

Demo Code

*/
//-----------------------------------------------------------------------------

static int demo_done;

static void done_check(uint8_t note, uint8_t velocity) {
    if (note == MODE_NOTE) {
        demo_done = 1;
    }
}

//-----------------------------------------------------------------------------
// demo - all leds set to white

static void demo_all_white(void) {
    printf_P(PSTR("\nall white"));
    midi.note_on = done_check;
    midi.note_off = 0;
    RGB white = COLOR_WHITE;

    for (int i = 0; i < NUM_LEDS; i ++) {
        led_set(i, &white);
    }

    while (!demo_done) {
        timer_delay_msec_poll(20, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------
// demo - all leds at once, spectrum of rgb colors

static void demo_spectrum(void) {
    printf_P(PSTR("\nled spectrum"));
    midi.note_on = done_check;
    midi.note_off = 0;

    float w = 380.0;
    int inc = 1.0;

    while (!demo_done) {
        RGB rgb;
        for (int i = 0; i < NUM_LEDS; i ++) {
            led_set(i, wavelength_to_rgb(&rgb, w));
        }
        w += inc;
        if ((w > 780.0) || (w < 380.0)) {
            inc *= -1;
        }
        if (w > 780.0) {
            w = 780.0;
        }
        if (w < 380.0) {
            w = 380.0;
        }
        timer_delay_msec_poll(10, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------

#define SPEC_PERIOD 150.0
#define SPEC_HI 780.0
#define SPEC_LO 380.0
#define SPEC_AMP (SPEC_HI - SPEC_LO)

static float scroll_function(float x) {
    x = fmod(x, SPEC_PERIOD);
    if (x < SPEC_PERIOD / 2.0) {
        return SPEC_LO + ((x * 2.0 * SPEC_AMP) / SPEC_PERIOD);
    } else {
        return (2.0 * SPEC_AMP) + SPEC_LO + ((x * -2.0 * SPEC_AMP) / SPEC_PERIOD);
    }
}

static void demo_spectrum_scroll(void) {
    printf_P(PSTR("\nspectrum scroll"));
    midi.note_on = done_check;
    midi.note_off = 0;

    float w = 0;
    float inc = 0.2;

    while (!demo_done) {
        RGB rgb;
        for (int i = 0; i < NUM_LEDS; i ++) {
            led_set(i, wavelength_to_rgb(&rgb, scroll_function(w + float(i))));
        }
        w += inc;
        timer_delay_msec_poll(10, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------
// demo - led chase 1

static void demo_chase1(void) {
    printf_P(PSTR("\nled chase 1"));
    midi.note_on = done_check;
    midi.note_off = 0;

    int posn, inc;
    RGB bg = COLOR_BLUE;
    RGB fg = COLOR_RED;
    posn = 1;
    inc = -1;

    while (!demo_done) {
        led_set(posn, &bg);
        posn += inc;
        if (posn == (NUM_LEDS - 1)) {
            // end of the string - go backwards
            inc = -1;
        }
        if (posn == 0) {
            // start of the string - go forward
            inc = 1;
        }
        led_set(posn, &fg);
        timer_delay_msec_poll(40, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------
// demo - led chase 2

static void demo_chase2(void) {
    printf_P(PSTR("\nled chase 2"));
    midi.note_on = done_check;
    midi.note_off = 0;
    int ofs = 0;

    while (!demo_done) {
        for (int i = 0; i < NUM_LEDS; i ++) {
            RGB rgb;
            uint8_t mag = i * (255 / (NUM_LEDS - 1));
            rgb.r = mag;
            rgb.g = (255 - mag);
            rgb.b = mag >> 3;
            led_set((i + ofs) % NUM_LEDS, &rgb);
        }
        ofs += 1;
        timer_delay_msec_poll(40, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------
// demo - led chase 3

static void demo_chase3(void) {
    printf_P(PSTR("\nled chase 3"));
    midi.note_on = done_check;
    midi.note_off = 0;
    int loop = 0;

    while (!demo_done) {
        RGB next[NUM_LEDS];

        zero_rgb(&next[0]);

        // copy all leds to the next position
        for (int i = 1; i < NUM_LEDS; i ++) {
            copy_rgb(&next[i], led_get(i - 1));
        }

        // add an attenuation of the previous value
        for (int i = 0; i < NUM_LEDS; i ++) {
            RGB rgb;
            copy_rgb(&rgb, led_get(i));
            scale_rgb(&rgb, 0.1);
            add_rgb(&next[i], &rgb);
        }

        if ((loop % 30) == 0) {
            random_rgb(&next[0]);
        }

        for (int i = 0; i < NUM_LEDS; i ++) {
            led_set(i, &next[i]);
        }

        loop += 1;
        timer_delay_msec_poll(40, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------

static void demo_random(void) {
    printf_P(PSTR("\nled random"));
    midi.note_on = done_check;
    midi.note_off = 0;

    while (!demo_done) {

        for (int i = 0; i < (NUM_LEDS / 7); i ++) {
            int posn = rand() % NUM_LEDS;
            RGB rgb;
            led_set(posn, random_rgb(&rgb));
        }
        timer_delay_msec_poll(40, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------

static void demo_automata(int rule) {
    printf_P(PSTR("\nled automata %d"), rule);
    midi.note_on = done_check;
    midi.note_off = 0;
    RGB bg;
    RGB fg = COLOR_BLACK;

    random_rgb(&bg);

    uint8_t state[NUM_LEDS];
    memset(state, 0, sizeof(state));
    state[NUM_LEDS/2] = 1;

    while (!demo_done) {

        // work out the next state
        uint8_t next_state[NUM_LEDS];
        for (int i = 0; i < NUM_LEDS; i ++) {
            int n = 0;
            if (i == 0) {
                n = (state[NUM_LEDS - 1] << 2) | (state[0] << 1) | state[1];
            } else if (i == NUM_LEDS - 1) {
                n = (state[NUM_LEDS - 2] << 2) | (state[NUM_LEDS - 1] << 1) | state[0];
            } else {
                n = (state[i -1] << 2) | (state[i] << 1) | state[i + 1];
            }

            if (rule & (1 << n)) {
                next_state[i] = 1;
            } else {
                next_state[i] = 0;
            }
        }
        // copy to current state
        memcpy(state, next_state, sizeof(state));

        // render the state on the leds
        for (int i = 0; i < NUM_LEDS; i ++) {
            if (state[i]) {
                led_set(i, &fg);
            } else {
                led_set(i, &bg);
            }
        }
        timer_delay_msec_poll(500, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------
// demo - color piano

static const RGB note2color[WHITE_KEYS_IN_OCTAVE] = {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_AQUA,
    COLOR_FUCHSIA,
    COLOR_WHITE
};

static void note_ctrl(uint8_t note, uint8_t velocity, int on_flag) {
    int white_note = midi_note_to_white(note);
    int octave = midi_note_to_octave(note);

    if ((white_note >= 0) && (octave >= START_OCTAVE)) {
        octave -= START_OCTAVE;
        int led_num = NUM_LEDS - 1 - (octave * 7) - white_note;

        if (on_flag) {
            // turn on light
            led_set(led_num, (RGB *)&note2color[white_note]);
        } else {
            // turn off light
            RGB black = COLOR_BLACK;
            led_set(led_num, &black);
        }
    }
}

static void light_on(uint8_t note, uint8_t velocity) {
    char tmp[8];
    printf_P(PSTR("\n%s %d %d"), midi_full_note_name(tmp, note), note, velocity);
    done_check(note, velocity);
    note_ctrl(note, velocity, 1);
}

static void light_off(uint8_t note, uint8_t velocity) {
    note_ctrl(note, velocity, 0);
}

static void demo_color_piano(void) {
    printf_P(PSTR("\ncolor piano"));
    midi.note_on = light_on;
    midi.note_off = light_off;

    while (!demo_done) {
        midi_rx();
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------

static void light_drop(uint8_t note, uint8_t velocity) {
    done_check(note, velocity);
    int white_note = midi_note_to_white(note);
    int octave = midi_note_to_octave(note);
    if ((white_note >= 0) && (octave >= START_OCTAVE)) {
        octave -= START_OCTAVE;
        int led_num = NUM_LEDS - 1 - (octave * 7) - white_note;
        led_set(led_num, (RGB *)&note2color[white_note]);
    }
}

static void demo_color_piano2(void) {
    printf_P(PSTR("\ncolor piano2"));
    midi.note_on = light_drop;
    midi.note_off = 0;

    while (!demo_done) {
        RGB next[NUM_LEDS];

        for (int i = 0; i < NUM_LEDS; i ++) {
            int left, right;
            if (i == 0) {
                left = NUM_LEDS - 1;
                right = 1;
            } else if (i == NUM_LEDS - 1) {
                left = NUM_LEDS - 2;
                right = 0;
            } else {
                left = i - 1;
                right = i + 1;
            }

            RGB *l_rgb = led_get(left);
            RGB *m_rgb = led_get(i);
            RGB *r_rgb = led_get(right);

            next[i].r = 332 * (l_rgb->r + m_rgb->r + r_rgb->r) / 1000;
            next[i].g = 332 * (l_rgb->g + m_rgb->g + r_rgb->g) / 1000;
            next[i].b = 332 * (l_rgb->b + m_rgb->b + r_rgb->b) / 1000;
        }

        for (int i = 0; i < NUM_LEDS; i ++) {
            led_set(i, &next[i]);
        }

        timer_delay_msec_poll(40, midi_rx);
    }

    led_all_off();
    demo_done = 0;
}

//-----------------------------------------------------------------------------
