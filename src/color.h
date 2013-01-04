//-----------------------------------------------------------------------------
/*

Color conversion routines

*/
//-----------------------------------------------------------------------------

#ifndef COLOR_H
#define COLOR_H

//-----------------------------------------------------------------------------

typedef struct rgb_color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;

//-----------------------------------------------------------------------------
// pre-defined colors

#define COLOR_BLACK     {0x00,0x00,0x00}
#define COLOR_WHITE     {0xff,0xff,0xff}
#define COLOR_RED       {0xff,0x00,0x00}
#define COLOR_GREEN     {0x00,0xff,0x00}
#define COLOR_BLUE      {0x00,0x00,0xff}
#define COLOR_SILVER    {0xC0,0xC0,0xC0}
#define COLOR_GRAY      {0x80,0x80,0x80}
#define COLOR_MAROON    {0x80,0x00,0x00}
#define COLOR_YELLOW    {0xFF,0xFF,0x00}
#define COLOR_OLIVE     {0x80,0x80,0x00}
#define COLOR_LIME      {0x00,0xFF,0x00}
#define COLOR_AQUA      {0x00,0xFF,0xFF}
#define COLOR_TEAL      {0x00,0x80,0x80}
#define COLOR_NAVY      {0x00,0x00,0x80}
#define COLOR_FUCHSIA   {0xFF,0x00,0xFF}
#define COLOR_PURPLE    {0x80,0x00,0x80}

//-----------------------------------------------------------------------------
// API functions

RGB *u16_to_rgb(RGB *rgb, uint16_t val);
RGB *mag_to_rgb(RGB *rgb, uint8_t val);
RGB *wavelength_to_rgb(RGB *rgb, float w);
RGB *random_rgb(RGB *rgb);
RGB *copy_rgb(RGB *dst, RGB *src);
RGB *zero_rgb(RGB *dst);
RGB *add_rgb(RGB *rgb, RGB *arg);
RGB *scale_rgb(RGB *rgb, float k);

//-----------------------------------------------------------------------------

#endif // COLOR_H

//-----------------------------------------------------------------------------