//-----------------------------------------------------------------------------
/*

LCD Driver for HD44780 Style Devices

This driver supports the 4 bit data, write only style of operation for minimal
io pin usage.

*/
//-----------------------------------------------------------------------------

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"

//-----------------------------------------------------------------------------

#define LCD_ROWS 2
#define LCD_COLS 16

static struct lcd_shadow {
    uint8_t row[LCD_COLS];
    uint8_t col;
} lcd;

//-----------------------------------------------------------------------------

#define LCD_FUNCTION_SET    (0x20 | (0 << 4 /*DL*/) | (1 << 3 /*N*/) | (0 << 2 /*F*/))
#define LCD_DISPLAY_ON      (0x08 | (1 << 2 /*D*/) | (0 << 1 /*C*/) | (0 << 0 /*B*/))
#define LCD_DISPLAY_CLEAR   (0x01)
#define LCD_ENTRY_MODE_SET  (0x04 | (1 << 1 /*I/D*/) | (0 << 0 /*S*/))
#define LCD_DDRAM_ADR(x)    (0x80 | x)
#define LCD_HOME            (0x02)
#define LCD_ROW0            LCD_DDRAM_ADR(0)
#define LCD_ROW1            LCD_DDRAM_ADR(0x40)

//-----------------------------------------------------------------------------
// Low Level LCD Control

// D0 - serial rx
// D1 - serial tx
// D2 - lcd register select
// D3 - lcd enable
// D4 - lcd d4
// D5 - lcd d5
// D6 - lcd d6
// D7 - lcd d7

#define LCD_DATA PORTD
#define LCD_CTRL PORTD
#define LCD_EN (1 << PORTD3)
#define LCD_RS (1 << PORTD2)

#define LCD_RS_HI() (LCD_CTRL |= LCD_RS)
#define LCD_RS_LO() (LCD_CTRL &= ~LCD_RS)
#define LCD_EN_HI() (LCD_CTRL |= LCD_EN)
#define LCD_EN_LO() (LCD_CTRL &= ~LCD_EN)

static void lcd_wr(uint8_t val) {
    LCD_EN_HI();
    LCD_DATA = (LCD_DATA & 0x0f) | (val & 0xf0);
    _delay_us(5);
    LCD_EN_LO();
    _delay_us(50);
}

//-----------------------------------------------------------------------------
// send a command

// write 8 bits to the instruction/command register
static void lcd_cmd(uint8_t cmd) {
    LCD_RS_LO();
    _delay_us(5);
    lcd_wr(cmd);
    lcd_wr(cmd << 4);
}

// write 8 bits to the data register
static void lcd_char(uint8_t ch) {
    LCD_RS_HI();
    _delay_us(5);
    lcd_wr(ch);
    lcd_wr(ch << 4);
}

//-----------------------------------------------------------------------------

static void lcd_io_init(void) {
    // set the portd data and ctrl pins as outputs
    DDRD |= (0xf0 | LCD_EN | LCD_RS);
    LCD_EN_LO();
}

//-----------------------------------------------------------------------------
// stdio compatible putc

static void lcd_shift_up(void) {
    int i;
    // copy row 1 onto row 0 of the lcd
    lcd_cmd(LCD_ROW0);
    for (i = 0; i < LCD_COLS; i ++) {
        lcd_char(lcd.row[i]);
    }
    // clear and display row 1
    lcd_cmd(LCD_ROW1);
    for (i = 0; i < LCD_COLS; i ++) {
        lcd.row[i] = ' ';
        lcd_char(' ');
    }
    // back to row1, col0
    lcd_cmd(LCD_ROW1);
    lcd.col = 0;
}

int lcd_putc(char c, FILE *stream)
{
    if (c == '\n') {
        lcd_shift_up();
    } else {
        if (lcd.col < LCD_COLS) {
            lcd.row[lcd.col] = c;
            lcd_char(c);
            lcd.col += 1;
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------

int lcd_init(void) {
    lcd_io_init();

    // 4 bit setup as per hd44780 datasheet
    _delay_ms(20);
    lcd_wr(0x30);
    _delay_ms(10);
    lcd_wr(0x30);
    _delay_ms(1);
    lcd_wr(0x30);
    lcd_wr(0x20);

    lcd_cmd(LCD_FUNCTION_SET);
    lcd_cmd(LCD_DISPLAY_ON);
    lcd_cmd(LCD_DISPLAY_CLEAR);
    _delay_ms(2);
    lcd_cmd(LCD_ENTRY_MODE_SET);

    // initialise the shadow display
    memset(lcd.row, ' ', LCD_COLS);
    lcd_shift_up();
    return 0;
}

//-----------------------------------------------------------------------------