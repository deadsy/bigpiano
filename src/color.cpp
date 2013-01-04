//-----------------------------------------------------------------------------
/*

Color conversion routines

*/
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>

#include "common.h"
#include "color.h"

//-----------------------------------------------------------------------------
// unpack a 16 bit color to rgb: xrrrrrgggggbbbbb

RGB *u16_to_rgb(RGB *rgb, uint16_t val) {
    rgb->r = (val >> 7) & 0xf8;
    rgb->g = (val >> 2) & 0xf8;
    rgb->b = (val << 3) & 0xf8;
    return rgb;
}

//-----------------------------------------------------------------------------
// convert a magnitude (0-255) to a color

RGB *mag_to_rgb(RGB *rgb, uint8_t val) {
    rgb->r = min(max((val - 64) << 2, 0), 255);
    rgb->g = min(max((abs(val - 128) << 2) - 255, 0), 255);
    rgb->b = min(max((192 - val) << 2, 0), 255);
    return rgb;
}

//-----------------------------------------------------------------------------
// convert a wavelength (in nm) to an rgb approximation
// see - http://www.physics.sfasu.edu/astro/color/spectra.html
// 380 - 780 nm

RGB *wavelength_to_rgb(RGB *rgb, float w) {
    float r, g, b, i;
    r = g = b = i = 0;

    // rgb color
    if (w >= 380 && w < 440) {
        r = -(w - 440) / (440 - 350);
        b = 1;
    } else if (w >= 440 && w < 490) {
        g = (w - 440) / (490 - 440);
        b = 1;
    } else if (w >= 490 && w < 510) {
        g = 1;
        b = -(w - 510) / (510 - 490);
    } else if (w >= 510 && w < 580) {
        r = (w - 510) / (580 - 510);
        g = 1;
    } else if (w >= 580 && w < 645) {
        r = 1;
        g = -(w - 645) / (645 - 580);
    } else if (w >= 645 && w <= 780) {
        r = 1;
    }
    // intensity correction
    if (w >= 380 && w < 420) {
        i = 0.3 + 0.7 * (w - 350) / (420 - 350);
    } else if (w >= 420 && w <= 700) {
        i = 1;
    } else if (w > 700 && w <= 780) {
        i = 0.3 + 0.7 * (780 - w) / (780 - 700);
    }
    i *= 255;

    rgb->r = (uint8_t)(r * i);
    rgb->g = (uint8_t)(g * i);
    rgb->b = (uint8_t)(b * i);
    return rgb;
}

//-----------------------------------------------------------------------------
// color operations

RGB *random_rgb(RGB *rgb) {
    rgb->r = rand() & (7 << 5);
    rgb->g = rand() & (7 << 5);
    rgb->b = rand() & (7 << 5);
    return rgb;
}

RGB *add_rgb(RGB *rgb, RGB *arg) {
    int r, g, b;
    r = rgb->r + arg->r;
    g = rgb->g + arg->g;
    b = rgb->b + arg->b;
    rgb->r = min(r, 255);
    rgb->g = min(g, 255);
    rgb->b = min(b, 255);
    return rgb;
}

RGB *scale_rgb(RGB *rgb, float k) {
    rgb->r *= k;
    rgb->g *= k;
    rgb->b *= k;
    return rgb;
}

RGB *copy_rgb(RGB *dst, RGB *src) {
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    return dst;
}

RGB *zero_rgb(RGB *dst) {
    dst->r = 0;
    dst->g = 0;
    dst->b = 0;
    return dst;
}

//-----------------------------------------------------------------------------




