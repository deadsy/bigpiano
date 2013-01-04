//-----------------------------------------------------------------------------
/*

UART Driver

*/
//-----------------------------------------------------------------------------

#ifndef UART_H
#define UART_H

//-----------------------------------------------------------------------------
// UART baud rate

#define UART_BAUD 31250 // midi

//-----------------------------------------------------------------------------

typedef struct
{
    uint16_t rx_ints;
    uint16_t rx_bytes;
    uint16_t rx_parity_error;
    uint16_t rx_framing_error;
    uint16_t rx_overrun_error;
    uint16_t rx_overflow_error;
    uint16_t tx_ints;
    uint16_t tx_bytes;

} UART_STATS;

//-----------------------------------------------------------------------------

void uart_rx_isr(void);
void uart_tx_isr(void);

// API
int uart_init(void);
void uart_tx(uint8_t c);
uint8_t uart_rx(void);
int uart_test_rx(void);
int uart_test_tx(void);

int uart_putc(char c, FILE *stream);
int uart_getc(FILE *stream);

//-----------------------------------------------------------------------------

#endif // UART_H

//-----------------------------------------------------------------------------