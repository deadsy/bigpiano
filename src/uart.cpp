//-----------------------------------------------------------------------------
/*

UART Driver

*/
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "common.h"
#include "uart.h"

//-----------------------------------------------------------------------------
// UART Buffer Sizes (must be a power of 2 and < 256)

#define UART_TX_BUFSIZE 64
#define UART_RX_BUFSIZE 16

//-----------------------------------------------------------------------------

static uint8_t tx_buffer[UART_TX_BUFSIZE];
static uint8_t rx_buffer[UART_RX_BUFSIZE];
static volatile uint8_t tx_rd;
static uint8_t tx_wr;
static uint8_t rx_rd;
static volatile uint8_t rx_wr;
static UART_STATS stats;

//-----------------------------------------------------------------------------

int uart_init(void)
{
    UBRR0L = (F_CPU / (8UL * UART_BAUD)) - 1;
    UBRR0H = 0;
    // double speed
    UCSR0A = _BV(U2X0);
    // 8 data, no parity, 1 stop
    UCSR0C = (3 << 1);
    UCSR0B = _BV(TXEN0)|_BV(RXEN0)|_BV(RXCIE0);
    return 0;
}

//-----------------------------------------------------------------------------
// USART Rx Complete

void uart_rx_isr(void)
{
    uint8_t status;

    stats.rx_ints ++;

    while (((status = UCSR0A) & _BV(RXC0)) != 0)
    {
        uint8_t c = UDR0;

        // Check errors
        if (status & _BV(UPE0))
        {
            stats.rx_parity_error ++;
            break;
        }

        if (status & _BV(FE0))
        {
            stats.rx_framing_error ++;
            break;
        }

        if (status & _BV(DOR0))
        {
            stats.rx_overrun_error ++;
            break;
        }

        // If we have space, put it in the rx buffer.
        if (inc_mod(rx_wr, (UART_RX_BUFSIZE - 1)) != rx_rd)
        {
            rx_buffer[rx_wr] = c;
            rx_wr = inc_mod(rx_wr, (UART_RX_BUFSIZE - 1));
            stats.rx_bytes ++;
        }
        else
        {
            stats.rx_overflow_error ++;
        }
    }
}

//-----------------------------------------------------------------------------
// USART, Data Register Empty

void uart_tx_isr(void)
{
    stats.tx_ints ++;

    if (tx_rd != tx_wr)
    {
        stats.tx_bytes ++;
        UDR0 = tx_buffer[tx_rd];
        tx_rd = inc_mod(tx_rd, (UART_TX_BUFSIZE - 1));
    }
    else
    {
        // No more tx data, disable the tx interrupt.
        UCSR0B &= (uint8_t)~_BV(UDRIE0);
    }
}

//-----------------------------------------------------------------------------
// Receive a character from the serial port. Don't return until we have one.

uint8_t uart_rx(void)
{
    uint8_t c;

    // Wait for a character in the Rx buffer.
    while (rx_rd == rx_wr);

    cli();
    c = rx_buffer[rx_rd];
    rx_rd = inc_mod(rx_rd, (UART_RX_BUFSIZE - 1));
    sei();

    return c;
}

//-----------------------------------------------------------------------------
// Transmit a character on the serial port.

void uart_tx(uint8_t c)
{
    // Wait for a some space in the Tx buffer.
    while (inc_mod(tx_wr, (UART_TX_BUFSIZE - 1)) == tx_rd);

    cli();

    if (tx_wr == tx_rd)
    {
        // Single byte in buffer, turn on tx interrupts.
        UCSR0B |= (uint8_t)_BV(UDRIE0);
    }

    // Put the character into the Tx buffer.
    tx_buffer[tx_wr] = c;
    tx_wr = inc_mod(tx_wr, (UART_TX_BUFSIZE - 1));
    sei();
}

//-----------------------------------------------------------------------------
// stdio compatible putc/getc

int uart_putc(char c, FILE *stream)
{
    if (c == '\n')
        uart_tx('\r');
    uart_tx(c);
    return 0;
}

int uart_getc(FILE *stream)
{
    return (int)uart_rx();
}

//-----------------------------------------------------------------------------
// Return non zero if there is a character waiting in the Rx/Tx buffer

int uart_test_rx(void)
{
    return (rx_rd == rx_wr) ? 0 : 1;
}

int uart_test_tx(void)
{
    return (tx_rd == tx_wr) ? 0 : 1;
}

//-----------------------------------------------------------------------------
